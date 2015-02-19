#define system.
#define system'math.

#subject(class:system'calendar'Date) datetime.
#subject(class:system'calendar'TimeSpan) timespan.

// --- TimeSpan --

#class(sealed,struct:embeddable) TimeSpan :: BaseNumber
{
    #field(type:long)theValue.

    #constructor min <= %new &long:0l.

    #constructor new &long:aValue
    [
        theValue := aValue.
    ]
            
    #constructor new &hours:Hours &minutes:Minutes &seconds:Seconds
    [
        #var(type:long)aValue := 0l.
        
        aValue := Hours long * 36000000000l.
        aValue := aValue + Minutes long multiply &long:600000000l.
        aValue := aValue + Seconds long multiply &long:10000000l.
        
        theValue := aValue.
    ]
            
    #constructor new &days:Days &hours:Hours &minutes:Minutes &seconds:Seconds
    [
        #var(type:long)aValue := 0l.
        
        aValue := Hours long * 36000000000l.
        aValue := aValue + Minutes long multiply &long:600000000l.
        aValue := aValue + Seconds long multiply &long:10000000l.
        aValue := aValue + Days long multiply &long:864000000000l.
        
        theValue := aValue.
    ]
            
    #method read &out'long : aRetVal
    [
        aRetVal := theValue.
    ]

    #method long = theValue.

    #method timespan = $self.
            
    #method days
    [
        #var(type:long)aRetVal := theValue.
        
        aRetVal := aRetVal / 864000000000l.
        
        ^ intConvertor convert &long:aRetVal.
    ]
                        
    #method hours
    [
        #var(type:long)aRetVal := theValue.
        
        aRetVal := aRetVal / 36000000000l.
        
        ^ intConvertor convert &long:aRetVal.
    ]
                        
    #method minutes
    [
        #var(type:long)aRetVal := theValue.
        
        aRetVal := aRetVal / 600000000l.
        
        ^ intConvertor convert &long:aRetVal.
    ]
                        
    #method seconds
    [
        #var(type:long)aRetVal := theValue.
        
        aRetVal := aRetVal / 10000000l.
        
        ^ intConvertor convert &long:aRetVal.
    ]

    #method milliseconds
    [
        #var(type:long)aRetVal := theValue.
        
        aRetVal := aRetVal / 10000l.
        
        ^ LongNumber new &long:aRetVal.
    ]
   
    #method equal &timespan:aSpan
    [
        #var(type:long)aValue := aSpan long.
        
        ^ aValue == theValue.
    ]

    #method less &timespan:aSpan
    [
        #var(type:long)aValue := aSpan long.
        
        ^ aValue < theValue.
    ]

    #method add &datetime:aDate
        = aDate add &timespan:$self.
    
    #method add &timespan:aSpan
    [
        #var(type:long)aValue := aSpan long.
        
        ^ TimeSpan new &long:(aValue + theValue).
    ]
    
    #method add &long:aValue
    [
        ^ TimeSpan new &long:(aValue + theValue).
    ]

    #method subtract &timespan:aSpan
    [
        #var(type:long)aValue := aSpan long.
        
        ^ TimeSpan new &long:(theValue - aValue).
    ]
    
    #method subtract &long:aValue
    [
        ^ TimeSpan new &long:(theValue - aValue).
    ]

    #method literal
    [
        #var aValue := dateTimeControl toShortTime &long:theValue.
        
         ^ ($self days literal + "." + aValue) literal.
    ]

    #method clone
        = TimeSpan new &long:theValue.

    #method cast:aVerb &to:aTarget = aTarget::aVerb eval &timespan:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &timespan:$self.
}

// --- Date ---

#class(sealed,struct:embeddable) Date :: BaseNumber
{
    #field(type:long)theValue.

    #constructor min <= %new &long:0l.

    #constructor now
    [
        #var(type:long)aValue := dateTimeControl read &CurrentUTCTime.
        
        aValue := dateTimeControl convertUTC2Local &long:aValue.
        
        theValue := aValue.
    ]
            
    #constructor new &long:aValue
    [
        theValue := aValue.
    ]
  
    #constructor new &year:aYear &month:aMonth &day:aDay &hour:anHour &minute:aMinute &second:aSecond
    [
        #var(type:systemtime)aSystemTime.
        aSystemTime set &year:(aYear int).
        aSystemTime set &month:(aMonth int).
        aSystemTime set &day:(aDay int).
        aSystemTime set &hour:(anHour int).
        aSystemTime set &minute:(aMinute int).
        aSystemTime set &second:(aSecond int).
        aSystemTime set &millisecond:0.
        
        theValue := aSystemTime long.
    ]
    
    #constructor new &year:aYear &month:aMonth &day:aDay
        <= %new &year:aYear &month:aMonth &day:aDay &hour:0 &minute:0 &second:0.
  
    #method long = theValue.

    #method read &out'long : aRetVal
    [
        aRetVal := theValue.
    ]
   
    #method equal &datetime:aDate
    [
        #var(type:long)aValue := aDate long.
        
        ^ aValue == theValue.
    ]

    #method less &datetime:aDate
    [
        #var(type:long)aValue := aDate long.
        
        ^ aValue < theValue.
    ]

    #method add &long:aValue
        = Date new &long:(theValue + aValue).

    #method subtract &long:aValue
        = Date new &long:(theValue - aValue).
  
    #method subtract &datetime:aDate
    [
        #var(type:long)aLongValue := aDate long.
        
        ^ TimeSpan new &long:(theValue - aLongValue).
    ]
  
    #method subtract &timespan:aTimeSpan
    [
        #var(type:long)aLongValue := aTimeSpan long.
        
        ^ Date new &long:(theValue + aLongValue).
    ]
  
    #method add &timespan:aTimeSpan
    [
        #var(type:long)aLongValue := aTimeSpan long.
        
        ^ Date new &long:(theValue + aLongValue).
    ]

    #method year
    [
        #var(type:systemtime)aSystemTime.
        aSystemTime write &long:theValue.
        
        ^ aSystemTime year.
    ]
    
    #method month
    [
        #var(type:systemtime)aSystemTime.
        aSystemTime write &long:theValue.
        
        ^ aSystemTime month.
    ]

    #method day
    [
        #var(type:systemtime)aSystemTime.
        aSystemTime write &long:theValue.
        
        ^ aSystemTime day.
    ]

    #method dayOfWeek
    [
        #var(type:systemtime)aSystemTime.
        aSystemTime write &long:theValue.
        
        ^ aSystemTime dayOfWeek.
    ]

    #method hour
    [
        #var(type:systemtime)aSystemTime.
        aSystemTime write &long:theValue.
        
        ^ aSystemTime hour.
    ]

    #method minute
    [
        #var(type:systemtime)aSystemTime.
        aSystemTime write &long:theValue.
        
        ^ aSystemTime minute.
    ]

    #method second
    [
        #var(type:systemtime)aSystemTime.
        aSystemTime write &long:theValue.
        
        ^ aSystemTime second.
    ]

    #method millisecond
    [
        #var(type:systemtime)aSystemTime.
        aSystemTime write &long:theValue.
        
        ^ aSystemTime millisecond.
    ]

    #method add &years:aYears
    [
        #var(type:systemtime)aSystemTime.
        aSystemTime write &long:theValue.
        
        #var m := aSystemTime month.
        
        aSystemTime set &year:(aSystemTime year + aYears).
        
        #var m2 := aSystemTime month.
        
        #var(type:long)aRetVal := aSystemTime long.
        
        ^ Date new &long:aRetVal.
    ]

    #method add &months:Months
    [
        #var aMonths:= Months::intOp mod:12.
        #var Years := Months / 12.
        
        ($self month + aMonths > 12)
            ? [ Years := Years + 1. aMonths := 12 - ($self month + aMonths). ].

        #var(type:systemtime)aSystemTime.
        aSystemTime write &long:theValue.
        
        aSystemTime set &year:(aSystemTime year add &int:Years).
        aSystemTime set &month:(aSystemTime month add &int:aMonths).
        
        #var(type:long)aRetVal := aSystemTime long.
        
        ^ Date new &long:aRetVal.
    ]

    #method add &days:aDays
        = $self add &timespan:(TimeSpan new &days:aDays &hours:0 &minutes:0 &seconds:0).

    #method add &hours:anHours
        = $self add &timespan:(TimeSpan new &days:0 &hours:anHours &minutes:0 &seconds:0).

    #method add &minutes:aMinutes
        = $self add &timespan:(TimeSpan new &days:0 &hours:0 &minutes:aMinutes &seconds:0).

    #method add &seconds:Seconds
        = $self add &timespan:(TimeSpan new &days:0 &hours:0 &minutes:0 &seconds:Seconds).

    #method literal
        = dateTimeControl toShortDateTime &long:theValue literal.
        
    #method cast:aVerb &to:aTarget = aTarget::aVerb eval &datetime:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &datetime:$self.
    
    #method datetime = $self.

    #method clone
        = Date new &long:theValue.
    
}

// --- now ---

#symbol now = Date now.

#symbol utcNow =
[
    #var(type:long)aValue := dateTimeControl read &CurrentUTCTime.
    
    ^ Date new &long:aValue.
] eval.
