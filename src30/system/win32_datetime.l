#define system.

#symbol DATE_SHORTDATE  = 00000001h.
#symbol DATE_LONGDATE   = 00000002h.

// --- SystemTime ---

#class Win32SystemTime
{
    #field(size:16)theData.
    
    #constructor new
    [
    ]
    
    #constructor new &type'short:aYear &type'short:aMonth &type'short:aDay &type'short:anHour &type'short:aMinute &type'short:aSecond &type'short:aMillisecond
    [
        system'external'CoreAPI write &type'bytearray:theData &type'index:0 &type'short:aYear.
        system'external'CoreAPI write &type'bytearray:theData &type'index:2 &type'short:aMonth.
        system'external'CoreAPI write &type'bytearray:theData &type'index:6 &type'short:aDay.
        system'external'CoreAPI write &type'bytearray:theData &type'index:8 &type'short:anHour.
        system'external'CoreAPI write &type'bytearray:theData &type'index:10 &type'short:aMinute.
        system'external'CoreAPI write &type'bytearray:theData &type'index:12 &type'short:aSecond.
        system'external'CoreAPI write &type'bytearray:theData &type'index:14 &type'short:aMillisecond.
    ]
    
    #method read &type'short &type'index:anIndex
        = IntNumber new &type'int:(system'external'CoreAPI read &type'bytearray:theData &type'index:anIndex) and &type'int:0FFFFh.

    #method win32'FileTime
    [
        #var aRetVal := LongNumber new.
        
        LongNumber new &type'long:(system'external'KERNEL32 SystemTimeToFileTime &type'bytearray:theData &out'type'bytearray:aRetVal).
        
        ^ aRetVal.
    ]    
    
    #method read &date &type'widestr &type'int:Flags
    [
        #var aLength := IntNumber new &type'int:(system'external'KERNEL32 GetDateFormatEx
            &type'int:0
            &type'int:Flags
            &type'bytearray:theData
            &type'int:0
            &type'int:0
            &type'int:0
            &type'int:0
            | raise:(system'FormatException new:"Cannot format date")).
            
        #var aLiteral := LiteralValue new &type'length:aLength.
        
        system'external'KERNEL32 GetDateFormatEx
            &type'int:0
            &type'int:Flags
            &type'bytearray:theData
            &type'int:0
            &out'type'widestr:aLiteral
            &type'int:aLength
            &type'int:0
            | raise:(system'FormatException new:"Cannot format date").
            
        ^ aLiteral.
    ]
    
    #method read &time &type'widestr &type'int:Flags
    [
        #var aLength := IntNumber new &type'int:(system'external'KERNEL32 GetTimeFormatEx
            &type'int:0
            &type'int:Flags
            &type'bytearray:theData
            &type'int:0
            &type'int:0
            &type'int:0
            &type'int:0
            | raise:(system'FormatException new:"Cannot format time")).
                        
        #var aLiteral := LiteralValue new &type'length:aLength.
        
        system'external'KERNEL32 GetTimeFormatEx
            &type'int:0
            &type'int:Flags
            &type'bytearray:theData
            &type'int:0
            &out'type'widestr:aLiteral
            &type'int:aLength
            &type'int:0
            | raise:(system'FormatException new:"Cannot format date").
            
        ^ aLiteral.
    ]
}

// --- EWin32FileTime ---

#class(role)EWin32FileTime
{
    #method win32'SystemTime
    [
        #var aSystemStruct := Win32SystemTime new.
        
        system'external'KERNEL32 FileTimeToSystemTime &type'bytearray:(self type'long) &out'type'bytearray:aSystemStruct.
        
        ^ aSystemStruct.
    ]
    
    #method win32'LocalTime
    [
        #var aRetVal := LongNumber new.
        
        system'external'KERNEL32 FileTimeToLocalFileTime &type'bytearray:(self type'long) &out'type'bytearray:aRetVal.
        
        ^ aRetVal.
    ]
}

// --- dateTimeControl ---

#symbol dateTimeControl =
{
    CurrentUTCTime 
    [
        #var aRetVal := LongNumber new.
        
        system'external'KERNEL32 GetSystemTimeAsFileTime &out'type'bytearray:aRetVal.
        
        ^ aRetVal.
    ]
    
    CurrentLocalTime 
    [
        #var aRetVal := $self CurrentUTCTime.
        
        ^ aRetVal~EWin32FileTime win32'LocalTime.
    ]
    
    readYear:aValue
        = aValue~EWin32FileTime win32'SystemTime read &type'short &type'index:0.
    
    readMonth:aValue
        = aValue~EWin32FileTime win32'SystemTime read &type'short &type'index:2.
    
    readDay:aValue
        = aValue~EWin32FileTime win32'SystemTime read &type'short &type'index:6.
    
    readHour:aValue
        = aValue~EWin32FileTime win32'SystemTime read &type'short &type'index:8.
    
    readMinute:aValue
        = aValue~EWin32FileTime win32'SystemTime read &type'short &type'index:10.
    
    readSecond:aValue
        = aValue~EWin32FileTime win32'SystemTime read &type'short &type'index:12.
    
    readMillisecond:aValue
        = aValue~EWin32FileTime win32'SystemTime read &type'short &type'index:14.
        
    new &Year:aYear &Month:aMonth &Day:aDay &Hour:anHour &Minute:aMinute &Second:aSecond
    [
        #var aSystemStruct := Win32SystemTime new 
            &type'short:(aYear type'int)
            &type'short:(aMonth type'int)
            &type'short:(aDay type'int)
            &type'short:(anHour type'int)
            &type'short:(aMinute type'int)
            &type'short:(aSecond type'int)
            &type'short:0.
        
        ^ aSystemStruct win32'FileTime.
    ]
    
    toShortDateTime:aValue
    [
        #var aSystemStruct := aValue~EWin32FileTime win32'SystemTime.

        #var aDate := aSystemStruct read &date &type'widestr &type'int:DATE_SHORTDATE.
        #var aTime := aSystemStruct read &time &type'widestr &type'int:0.

        ^ aDate + " " + aTime.
    ]
    
    toShortDate:aValue
    [
        #var aSystemStruct := aValue~EWin32FileTime win32'SystemTime.

        ^ aSystemStruct read &date &type'widestr &type'int:DATE_SHORTDATE.
    ]
    
    toShortTime:aValue
    [
        #var aSystemStruct := aValue~EWin32FileTime win32'SystemTime.

        ^ aSystemStruct read &time &type'widestr &type'int:0.
    ]
}.

#symbol timeSpanControl =
{
    readYears:aValue
        = aValue~EWin32FileTime win32'SystemTime read &type'short &type'index:0 - 1601.
    
    readMonths:aValue
        = aValue~EWin32FileTime win32'SystemTime read &type'short &type'index:2 - 1.
    
    readDays:aValue
        = aValue~EWin32FileTime win32'SystemTime read &type'short &type'index:6 - 1.
    
    readHours:aValue
        = aValue~EWin32FileTime win32'SystemTime read &type'short &type'index:8.
    
    readMinutes:aValue
        = aValue~EWin32FileTime win32'SystemTime read &type'short &type'index:10.
    
    readSeconds:aValue
        = aValue~EWin32FileTime win32'SystemTime read &type'short &type'index:12.
        
    readMilliseconds:aValue = LongNumber new &type'long:(aValue type'long / 10000l).
        
    new &Years:aYear &Months:aMonth &Days:aDay &Hours:anHour &Minutes:aMinute &Seconds:aSecond
    [
        #var aSystemStruct := Win32SystemTime new
            &type'short:(1601 + aYear type'int)
            &type'short:(1 + aMonth type'int)
            &type'short:(1 + aDay type'int)
            &type'short:(anHour type'int)
            &type'short:(aMinute type'int)
            &type'short:(aSecond type'int)
            &type'short:0.
                    
        ^ aSystemStruct win32'FileTime.
    ]
}.
