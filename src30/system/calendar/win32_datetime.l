#define system.

#symbol(const) DATE_SHORTDATE  = 00000001h.
#symbol(const) DATE_LONGDATE   = 00000002h.

// --- SystemTime ---

#type(size:16,wrapper:Win32SystemTime)__win_systemtime.

#class(type:__win_systemtime) Win32SystemTime
{
    #field(type:short)theYear.
    #field(type:short)theMonth.
    #field(type:short)theDayOfWeek.
    #field(type:short)theDay.
    #field(type:short)theHour.
    #field(type:short)theMinute.
    #field(type:short)theSecond.
    #field(type:short)theMillisecond.
    
    #constructor new
    [
    ]
    
//    #constructor new &short:aYear &short:aMonth &short:aDay &short:anHour &short:aMinute &short:aSecond &short:aMillisecond
//    [
//        byteArrayPrimitiveOp write &bytearray:theData &index:0 &short:aYear.
//        byteArrayPrimitiveOp write &bytearray:theData &index:2 &short:aMonth.
//        byteArrayPrimitiveOp write &bytearray:theData &index:6 &short:aDay.
//        byteArrayPrimitiveOp write &bytearray:theData &index:8 &short:anHour.
//        byteArrayPrimitiveOp write &bytearray:theData &index:10 &short:aMinute.
//        byteArrayPrimitiveOp write &bytearray:theData &index:12 &short:aSecond.
//        byteArrayPrimitiveOp write &bytearray:theData &index:14 &short:aMillisecond.
//    ]

    #method Year = IntNumber new &short:theYear.

    #method Month = IntNumber new &short:theMonth.

    #method DayOfWeek = IntNumber new &short:theDayOfWeek.

    #method Day = IntNumber new &short:theDay.

    #method Hour = IntNumber new &short:theHour.

    #method Minute = IntNumber new &short:theMinute.

    #method Second = IntNumber new &short:theSecond.

    #method Millisecond = IntNumber new &short:theMillisecond.

//    #method win32'FileTime
//    [
//        #var aRetVal := LongNumber new.
//        
//        system'external'KERNEL32 SystemTimeToFileTime &bytearray:theData &out'bytearray:aRetVal.
//        
//        ^ aRetVal.
//    ]    
    
    #method get &literal &dateFlags:Flags
    [
        #var(type:shortarray,size:127)aBuffer.

        #var(type:int) aLength := system'external'KERNEL32 GetDateFormatEx
            &int:0
            &int::Flags
            &bytearray:$self
            &int:0
            &shortarray:aBuffer
            &int:255
            &int:0
            | raise:(system'FormatException new:"Invalid date").

        ^ LiteralValue new &length:(aLength - 1) &shortarray:aBuffer.                
    ]
    
    #method get &literal &timeFlags:Flags
    [
        #var(type:shortarray,size:127)aBuffer.

        #var(type:int) aLength := system'external'KERNEL32 GetTimeFormatEx
            &int:0
            &int::Flags
            &bytearray:$self
            &int:0
            &shortarray:aBuffer
            &int:255
            &int:0
            | raise:(system'FormatException new:"Invalid time").

        ^ LiteralValue new &length:(aLength - 1) &shortarray:aBuffer.                
    ]
}

// --- dateTimeControl ---

#symbol dateTimeControl =
{
    read &CurrentUTCTime &out'long:aRetVal
    [
        system'external'KERNEL32 GetSystemTimeAsFileTime &out'long:aRetVal.
    ]

    read &CurrentLocalTime &out'long:aRetVal
    [
        #var(type:long)aSrcVal.
        
        $self read &CurrentUTCTime &out'long:aSrcVal.

        system'external'KERNEL32 FileTimeToLocalFileTime &out'long:aSrcVal &out'long:aRetVal.
    ]

    readSystemTime &long:aFileTime &out'__win_systemtime:aRetVal
    [
        system'external'KERNEL32 FileTimeToSystemTime &bytearray:aFileTime &out'__win_systemtime:aRetVal.
    ]

    readYear &long:aDateValue
    [
        #var(type:__win_systemtime)aSystemTime.

        $self readSystemTime &long:aDateValue &out'__win_systemtime:aSystemTime.
                                
        ^ aSystemTime Year.
    ]

    readMonth &long:aDateValue
    [
        #var(type:__win_systemtime)aSystemTime.

        $self readSystemTime &long:aDateValue &out'__win_systemtime:aSystemTime.
                                
        ^ aSystemTime Month.
    ]
    
    readDayOfWeek &long:aDateValue
    [
        #var(type:__win_systemtime)aSystemTime.

        $self readSystemTime &long:aDateValue &out'__win_systemtime:aSystemTime.
                                
        ^ aSystemTime DayOfWeek.
    ]
    
    readDay &long:aDateValue
    [
        #var(type:__win_systemtime)aSystemTime.

        $self readSystemTime &long:aDateValue &out'__win_systemtime:aSystemTime.
                                
        ^ aSystemTime Day.
    ]
    
    readHour &long:aDateValue
    [
        #var(type:__win_systemtime)aSystemTime.

        $self readSystemTime &long:aDateValue &out'__win_systemtime:aSystemTime.
                                
        ^ aSystemTime Hour.
    ]
    
    readMinute &long:aDateValue
    [
        #var(type:__win_systemtime)aSystemTime.

        $self readSystemTime &long:aDateValue &out'__win_systemtime:aSystemTime.
                                
        ^ aSystemTime Minute.
    ]
    
    readSecond &long:aDateValue
    [
        #var(type:__win_systemtime)aSystemTime.

        $self readSystemTime &long:aDateValue &out'__win_systemtime:aSystemTime.
                                
        ^ aSystemTime Second.
    ]
    
    readMillisecond &long:aDateValue
    [
        #var(type:__win_systemtime)aSystemTime.

        $self readSystemTime &long:aDateValue &out'__win_systemtime:aSystemTime.
                                
        ^ aSystemTime Millisecond.
    ]
        
//    new &Year:aYear &Month:aMonth &Day:aDay &Hour:anHour &Minute:aMinute &Second:aSecond
//    [
//        #var aSystemStruct := Win32SystemTime new 
//            &short:(aYear int)
//            &short:(aMonth int)
//            &short:(aDay int)
//            &short:(anHour int)
//            &short:(aMinute int)
//            &short:(aSecond int)
//            &short:0.
//        
//        ^ aSystemStruct win32'FileTime.
//    ]
    
    toShortDateTime &long:aDateValue
    [
        #var(type:__win_systemtime)aSystemTime.

        $self readSystemTime &long:aDateValue &out'__win_systemtime:aSystemTime.

        #var aDate := aSystemTime get &literal &dateFlags:DATE_SHORTDATE.
        #var aTime := aSystemTime get &literal &timeFlags:0.

        ^ aDate + " " + aTime.
    ]
    
//    toShortDate:aValue
//    [
//        #var aSystemStruct := aValue~EWin32FileTime win32'SystemTime.
//
//        ^ aSystemStruct read &date &literal &int:DATE_SHORTDATE.
//    ]
//    
//    toShortTime:aValue
//    [
//        #var aSystemStruct := aValue~EWin32FileTime win32'SystemTime.
//
//        ^ aSystemStruct read &time &literal &int:0.
//    ]
}.

#symbol timeSpanControl =
{
    readYears &long:aValue
    [
        #var(type:__win_systemtime)aSystemTime.

        $self readSystemTime &long:aValue &out'__win_systemtime:aSystemTime.
                                
        ^ aSystemTime Year - 1601.
    ]

    readMonths &long:aValue
    [
        #var(type:__win_systemtime)aSystemTime.

        $self readSystemTime &long:aValue &out'__win_systemtime:aSystemTime.
                                
        ^ aSystemTime Month - 1.
    ]

    readDays &long:aValue
    [
        #var(type:__win_systemtime)aSystemTime.

        $self readSystemTime &long:aValue &out'__win_systemtime:aSystemTime.
                                
        ^ aSystemTime Day - 1.
    ]
    
    readHours &long:aValue
    [
        #var(type:__win_systemtime)aSystemTime.

        $self readSystemTime &long:aValue &out'__win_systemtime:aSystemTime.
                                
        ^ aSystemTime Hour.
    ]

    readMinutes &long:aValue
    [
        #var(type:__win_systemtime)aSystemTime.

        $self readSystemTime &long:aValue &out'__win_systemtime:aSystemTime.
                                
        ^ aSystemTime Minute.
    ]

    readSeconds &long:aValue
    [
        #var(type:__win_systemtime)aSystemTime.

        $self readSystemTime &long:aValue &out'__win_systemtime:aSystemTime.
                                
        ^ aSystemTime Second.
    ]

    readMilliseconds &long:aValue = LongNumber new &long:(aValue / 10000l).

//    new &Years:aYear &Months:aMonth &Days:aDay &Hours:anHour &Minutes:aMinute &Seconds:aSecond
//    [
//        // HOTFIX: temporally, due to current implementation
//        #var(type:int)aYearWord := 1601 + aYear int.
//        #var(type:int)aMonthWord := 1 + aMonth int.
//        #var(type:int)aDayWord := 1 + aDay int.
//
//        #var aSystemStruct := Win32SystemTime new
//            &short:aYearWord
//            &short:aMonthWord
//            &short:aDayWord
//            &short:(anHour int)
//            &short:(aMinute int)
//            &short:(aSecond int)
//            &short:0.
//                                                
////        #var aSystemStruct := Win32SystemTime new
////            &type'short:(1601 + aYear type'int)
////            &type'short:(1 + aMonth type'int)
////            &type'short:(1 + aDay type'int)
////            &type'short:(anHour type'int)
////            &type'short:(aMinute type'int)
////            &type'short:(aSecond type'int)
////            &type'short:0.
//
//        ^ aSystemStruct win32'FileTime.
//    ]
}.
