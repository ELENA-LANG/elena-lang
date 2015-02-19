#define system.

#symbol(const,type:int) DATE_SHORTDATE  = 00000001h.
#symbol(const,type:int) DATE_LONGDATE   = 00000002h.

// --- SystemTime ---

#subject(class:system'calendar'Win32SystemTime)systemtime.

#subject(class:system'IntNumber) int_time_attributes.
#subject(class:system'IntNumber) int_date_attributes.

#class(struct:embeddable,sealed) Win32SystemTime
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
        => system'internal'dlong_empty.
    
    #method year = IntNumber new &short:theYear.

    #method month = IntNumber new &short:theMonth.

    #method dayOfWeek = IntNumber new &short:theDayOfWeek.

    #method day = IntNumber new &short:theDay.

    #method hour = IntNumber new &short:theHour.

    #method minute = IntNumber new &short:theMinute.

    #method second = IntNumber new &short:theSecond.

    #method millisecond = IntNumber new &short:theMillisecond.

    #method set &year:aValue 
    [ 
        theYear := aValue short.
    ]

    #method set &month:aValue 
    [ 
        theMonth := aValue short.
    ]

    #method set &day:aValue 
    [ 
        theDay := aValue short.
    ]

    #method set &hour:aValue 
    [ 
        theHour := aValue short.
    ]

    #method set &minute:aValue 
    [ 
        theMinute := aValue short.
    ]

    #method set &second:aValue 
    [ 
        theSecond := aValue short.
    ]

    #method set &millisecond:aValue 
    [ 
        theMillisecond := aValue short.
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
        #var(type:chararray,size:127)aBuffer.

        #var(type:int) aLength := system'external'KERNEL32 GetDateFormatEx
            &int:0
            &int:Flags
            &systemtime:$self
            &int:0
            &chararray:aBuffer
            &int:255
            &int:0
            | raise:(FormatException new:"Invalid date").

        ^ LiteralValue new &length:(aLength - 1) &chararray:aBuffer.                
    ]
    
    #method get &literal &int_time_attributes:Flags
    [
        #var(type:chararray,size:127)aBuffer.

        #var(type:int) aLength := system'external'KERNEL32 GetTimeFormatEx
            &int:0
            &int:Flags
            &systemtime:$self
            &int:0
            &chararray:aBuffer
            &int:255
            &int:0
            | raise:(FormatException new:"Invalid time").

        ^ LiteralValue new &length:(aLength - 1) &chararray:aBuffer.                
    ]
}

// --- dateTimeControl ---

#symbol dateTimeControl =
{
    read &CurrentUTCTime &out'long:aRetVal
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
