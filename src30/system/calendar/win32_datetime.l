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
    
    #constructor new &short:aYear &short:aMonth &short:aDay &short:anHour &short:aMinute &short:aSecond &short:aMillisecond
    [
        primitive write &bytearray:theData &index:0 &short:aYear.
        primitive write &bytearray:theData &index:2 &short:aMonth.
        primitive write &bytearray:theData &index:6 &short:aDay.
        primitive write &bytearray:theData &index:8 &short:anHour.
        primitive write &bytearray:theData &index:10 &short:aMinute.
        primitive write &bytearray:theData &index:12 &short:aSecond.
        primitive write &bytearray:theData &index:14 &short:aMillisecond.
    ]
    
    #method read &short &index:anIndex
    [
        #var(type:short)aValue.
        primitive read &bytearray:theData &index:anIndex &out'short:aValue.
        
        ^ IntNumber new &int:aValue.
    ]

    #method win32'FileTime
    [
        #var aRetVal := LongNumber new.
        
        system'external'KERNEL32 SystemTimeToFileTime &bytearray:theData &out'bytearray:aRetVal.
        
        ^ aRetVal.
    ]    
    
    #method read &date &literal &int:Flags
    [
        #var(type:int) aLength := system'external'KERNEL32 GetDateFormatEx
            &int:0
            &int:Flags
            &bytearray:theData
            &int:0
            &int:0
            &int:0
            &int:0
            | raise:(system'FormatException new:"Cannot format date").
            
        #var aLiteral := LiteralValue new &length:(aLength - 1).
        
        system'external'KERNEL32 GetDateFormatEx
            &int:0
            &int:Flags
            &bytearray:theData
            &int:0
            &out'literal:aLiteral
            &int:aLength
            &int:0
            | raise:(system'FormatException new:"Cannot format date").
            
        ^ aLiteral.
    ]
    
    #method read &time &literal &int:Flags
    [
        #var(type:int) aLength := system'external'KERNEL32 GetTimeFormatEx
            &int:0
            &int:Flags
            &bytearray:theData
            &int:0
            &int:0
            &int:0
            &int:0
            | raise:(system'FormatException new:"Cannot format time").
                        
        #var aLiteral := LiteralValue new &length:(aLength - 1).
        
        system'external'KERNEL32 GetTimeFormatEx
            &int:0
            &int:Flags
            &bytearray:theData
            &int:0
            &out'literal:aLiteral
            &int:aLength
            &int:0
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
        
        system'external'KERNEL32 FileTimeToSystemTime &bytearray:(self long) &out'bytearray:aSystemStruct.
        
        ^ aSystemStruct.
    ]
    
    #method win32'LocalTime
    [
        #var aRetVal := LongNumber new.
        
        system'external'KERNEL32 FileTimeToLocalFileTime &bytearray:(self long) &out'bytearray:aRetVal.
        
        ^ aRetVal.
    ]
}

// --- dateTimeControl ---

#symbol dateTimeControl =
{
    CurrentUTCTime 
    [
        #var aRetVal := LongNumber new.
        
        system'external'KERNEL32 GetSystemTimeAsFileTime &out'bytearray:aRetVal.
        
        ^ aRetVal.
    ]
    
    CurrentLocalTime 
    [
        #var aRetVal := $self CurrentUTCTime.
        
        ^ aRetVal~EWin32FileTime win32'LocalTime.
    ]
    
    readYear:aValue
        = aValue~EWin32FileTime win32'SystemTime read &short &index:0.
    
    readMonth:aValue
        = aValue~EWin32FileTime win32'SystemTime read &short &index:2.
    
    readDayOfWeek:aValue
        = aValue~EWin32FileTime win32'SystemTime read &short &index:4.
    
    readDay:aValue
        = aValue~EWin32FileTime win32'SystemTime read &short &index:6.
    
    readHour:aValue
        = aValue~EWin32FileTime win32'SystemTime read &short &index:8.
    
    readMinute:aValue
        = aValue~EWin32FileTime win32'SystemTime read &short &index:10.
    
    readSecond:aValue
        = aValue~EWin32FileTime win32'SystemTime read &short &index:12.
    
    readMillisecond:aValue
        = aValue~EWin32FileTime win32'SystemTime read &short &index:14.
        
    new &Year:aYear &Month:aMonth &Day:aDay &Hour:anHour &Minute:aMinute &Second:aSecond
    [
        #var aSystemStruct := Win32SystemTime new 
            &short:(aYear int)
            &short:(aMonth int)
            &short:(aDay int)
            &short:(anHour int)
            &short:(aMinute int)
            &short:(aSecond int)
            &short:0.
        
        ^ aSystemStruct win32'FileTime.
    ]
    
    toShortDateTime:aValue
    [
        #var aSystemStruct := aValue~EWin32FileTime win32'SystemTime.

        #var aDate := aSystemStruct read &date &literal &int:DATE_SHORTDATE.
        #var aTime := aSystemStruct read &time &literal &int:0.

        ^ aDate + " " + aTime.
    ]
    
    toShortDate:aValue
    [
        #var aSystemStruct := aValue~EWin32FileTime win32'SystemTime.

        ^ aSystemStruct read &date &literal &int:DATE_SHORTDATE.
    ]
    
    toShortTime:aValue
    [
        #var aSystemStruct := aValue~EWin32FileTime win32'SystemTime.

        ^ aSystemStruct read &time &literal &int:0.
    ]
}.

#symbol timeSpanControl =
{
    readYears:aValue
        = aValue~EWin32FileTime win32'SystemTime read &short &index:0 - 1601.
    
    readMonths:aValue
        = aValue~EWin32FileTime win32'SystemTime read &short &index:2 - 1.
    
    readDays:aValue
        = aValue~EWin32FileTime win32'SystemTime read &short &index:6 - 1.
    
    readHours:aValue
        = aValue~EWin32FileTime win32'SystemTime read &short &index:8.
    
    readMinutes:aValue
        = aValue~EWin32FileTime win32'SystemTime read &short &index:10.
    
    readSeconds:aValue
        = aValue~EWin32FileTime win32'SystemTime read &short &index:12.
        
    readMilliseconds:aValue = LongNumber new &long:(aValue long / 10000l).
        
    new &Years:aYear &Months:aMonth &Days:aDay &Hours:anHour &Minutes:aMinute &Seconds:aSecond
    [
        // HOTFIX: temporally, due to current implementation
        #var(type:int)aYearWord := 1601 + aYear int.
        #var(type:int)aMonthWord := 1 + aMonth int.
        #var(type:int)aDayWord := 1 + aDay int.

        #var aSystemStruct := Win32SystemTime new
            &short:aYearWord
            &short:aMonthWord
            &short:aDayWord
            &short:(anHour int)
            &short:(aMinute int)
            &short:(aSecond int)
            &short:0.
                                                
//        #var aSystemStruct := Win32SystemTime new
//            &type'short:(1601 + aYear type'int)
//            &type'short:(1 + aMonth type'int)
//            &type'short:(1 + aDay type'int)
//            &type'short:(anHour type'int)
//            &type'short:(aMinute type'int)
//            &type'short:(aSecond type'int)
//            &type'short:0.

        ^ aSystemStruct win32'FileTime.
    ]
}.
