#define system.

#symbol(const) DATE_SHORTDATE  = 00000001h.
#symbol(const) DATE_LONGDATE   = 00000002h.

// --- SystemTime ---

#type(class:system'calendar'Win32SystemTime)systemtime.
#type(type:int) int_time_attributes.
#type(type:int) int_date_attributes.

#class(type:systemtime,struct:embeddable,sealed) Win32SystemTime
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
    
    #method year = IntNumber new &short:theYear.

    #method month = IntNumber new &short:theMonth.

    #method dayOfWeek = IntNumber new &short:theDayOfWeek.

    #method day = IntNumber new &short:theDay.

    #method hour = IntNumber new &short:theHour.

    #method minute = IntNumber new &short:theMinute.

    #method second = IntNumber new &short:theSecond.

    #method millisecond = IntNumber new &short:theMillisecond.

    #method years
    [
        #var(type:short)aValue := theYear - 1601.
        
        ^ IntNumber new &short:aValue.
    ]

    #method months
    [
        #var(type:short)aValue := theMonth - 1.
        
        ^ IntNumber new &short:aValue.
    ]

    #method days
    [
        #var(type:short)aValue := theDay - 1.
        
        ^ IntNumber new &short:aValue.
    ]

    #method hours = IntNumber new &short:theHour.

    #method minutes = IntNumber new &short:theMinute.

    #method seconds = IntNumber new &short:theSecond.

    #method milliseconds
    [
        #var(type:long)aValue.
        system'external'KERNEL32 SystemTimeToFileTime &systemtime:$self &out'long:aValue.
        
        ^ LongNumber new &long:(aValue / 10000l).
    ]

    #method set &year:aValue 
    [ 
        theYear := aValue int short.
    ]

    #method set &month:aValue 
    [ 
        theMonth := aValue int short.
    ]

    #method set &day:aValue 
    [ 
        theDay := aValue int short.
    ]

    #method set &hour:aValue 
    [ 
        theHour := aValue int short.
    ]

    #method set &minute:aValue 
    [ 
        theMinute := aValue int short.
    ]

    #method set &second:aValue 
    [ 
        theSecond := aValue int short.
    ]

    #method set &millisecond:aValue 
    [ 
        theMillisecond := aValue int short.
    ]

    #method read &out'long:aRetVal
    [
        system'external'KERNEL32 SystemTimeToFileTime &systemtime:$self &out'long:aRetVal.
    ]    
    
    #method write &long:aFileTime
    [
        system'external'KERNEL32 FileTimeToSystemTime &long:aFileTime &systemtime:$self.
    ]
   
    #method get &literal &int_date_attributes:Flags
    [
        #var(type:shortarray,size:127)aBuffer.

        #var(type:int) aLength := system'external'KERNEL32 GetDateFormatEx
            &int:0
            &int:Flags
            &systemtime:$self
            &int:0
            &shortarray:aBuffer
            &int:255
            &int:0
            | raise:(FormatException new:"Invalid date").

        ^ LiteralValue new &length:(aLength - 1) &shortarray:aBuffer.                
    ]
    
    #method get &literal &int_time_attributes:Flags
    [
        #var(type:shortarray,size:127)aBuffer.

        #var(type:int) aLength := system'external'KERNEL32 GetTimeFormatEx
            &int:0
            &int:Flags
            &systemtime:$self
            &int:0
            &shortarray:aBuffer
            &int:255
            &int:0
            | raise:(FormatException new:"Invalid time").

        ^ LiteralValue new &length:(aLength - 1) &shortarray:aBuffer.                
    ]
}

// --- dateTimeControl ---

#symbol dateTimeControl =
{
    readCurrentUTCTime &out'long:aRetVal
    [
        system'external'KERNEL32 GetSystemTimeAsFileTime &out'long:aRetVal.
    ]

    convertUTC2Local &long:aSrcVal &out'long:aRetVal
    [
        system'external'KERNEL32 FileTimeToLocalFileTime &long:aSrcVal &out'long:aRetVal.
    ]
    
    toShortDateTime &long:aDateValue
    [
        #var(type:systemtime)aSystemTime.

        aSystemTime write &long:aDateValue.

        #var aDate := aSystemTime get &literal &int_date_attributes:DATE_SHORTDATE.
        #var aTime := aSystemTime get &literal &int_time_attributes:0.

        ^ aDate + " " + aTime.
    ]
    
    toShortDate &long:aDateValue
    [
        #var(type:systemtime)aSystemTime.

        aSystemTime write &long:aDateValue.

        #var aDate := aSystemTime get &literal &int_date_attributes:DATE_SHORTDATE.

        ^ aDate.
    ]
    
    toShortTime &long:aDateValue
    [
        #var(type:systemtime)aSystemTime.

        aSystemTime write &long:aDateValue.

        #var aTime := aSystemTime get &literal &int_time_attributes:0.

        ^ aTime.
    ]
}.

// --- dateTimeConvertor ---

//#symbol timeSpanControl =
//{
//    new &Years:aYear &Months:aMonth &Days:aDay &Hours:anHour &Minutes:aMinute &Seconds:aSecond
//    [
//        #var(type:__win_systemtime)aSystemTime.
//        
//        // HOTFIX: temporally, due to current implementation
//        #var(type:int)aYearWord := 1601 + aYear int.
//        #var(type:int)aMonthWord := 1 + aMonth int.
//        #var(type:int)aDayWord := 1 + aDay int.
//        
//        aSystemTime set &Year:aYearWord.
//        aSystemTime set &Month:aMonthWord.
//        aSystemTime set &Day:aDayWord.
//        aSystemTime set &Hour:anHour.
//        aSystemTime set &Minute:aMinute.
//        aSystemTime set &Second:aMinute.
//        aSystemTime set &Millisecond:0.
//
//        ^ aSystemTime long.
//    ]
//}.
