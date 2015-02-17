#define system.

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
        #var(type:systemtime)aSystemTime.
        aSystemTime write &long:0l.
        
        aSystemTime set &days:0 set &hours:Hours set &minutes:Minutes set &seconds:Seconds.
        
        theValue := aSystemTime long.
    ]
            
    #constructor new &days:Days &hours:Hours &minutes:Minutes &seconds:Seconds
    [
        #var(type:systemtime)aSystemTime.
        aSystemTime write &long:0l.
        
        aSystemTime set &days:Days set &hours:Hours set &minutes:Minutes set &seconds:Seconds.
        
        theValue := aSystemTime long.
    ]
            
    #method read &out'long : aRetVal
    [
        aRetVal := theValue.
    ]

    #method long = theValue.

    #method timespan = $self.
            
    #method days
    [
        #var(type:systemtime)aSystemTime.
        aSystemTime write &long:theValue.
        
        ^ aSystemTime days.
    ]
                        
    #method hours
    [
        #var(type:systemtime)aSystemTime.
        aSystemTime write &long:theValue.
        
        ^ aSystemTime hours.
    ]
                        
    #method minutes
    [
        #var(type:systemtime)aSystemTime.
        aSystemTime write &long:theValue.
        
        ^ aSystemTime minutes.
    ]
                        
    #method seconds
    [
        #var(type:systemtime)aSystemTime.
        aSystemTime write &long:theValue.
        
        ^ aSystemTime seconds.
    ]

    #method milliseconds
    [
        #var(type:systemtime)aSystemTime.
        aSystemTime write &long:theValue.
        
        ^ aSystemTime milliseconds.
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
        #var aValue := dateTimeControl toShortDateTime &long:theValue.
        
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

    #method add &year:aYears
    [
        #var(type:systemtime)aSystemTime.
        aSystemTime set &years:aYears.
        aSystemTime set &months:0.
        aSystemTime set &days:0.
        aSystemTime set &hours:0.
        aSystemTime set &minutes:0.
        aSystemTime set &seconds:0.
        
        #var(type:long)aValue := aSystemTime long.
        
        ^ $self add &long:aValue.
    ]

    #method add &month:aMonths
    [
        #var(type:systemtime)aSystemTime.
        aSystemTime set &years:0.
        aSystemTime set &months:aMonths.
        aSystemTime set &days:0.
        aSystemTime set &hours:0.
        aSystemTime set &minutes:0.
        aSystemTime set &seconds:0.
        
        #var(type:long)aValue := aSystemTime long.
        
        ^ $self add &long:aValue.
    ]

    #method add &day:aDays
    [
        #var(type:systemtime)aSystemTime.
        aSystemTime set &years:0.
        aSystemTime set &months:0.
        aSystemTime set &days:aDays.
        aSystemTime set &hours:0.
        aSystemTime set &minutes:0.
        aSystemTime set &seconds:0.
        
        #var(type:long)aValue := aSystemTime long.
        
        ^ $self add &long:aValue.
    ]

    #method add &hour:anHours
    [
        #var(type:systemtime)aSystemTime.
        aSystemTime set &years:0.
        aSystemTime set &months:0.
        aSystemTime set &days:0.
        aSystemTime set &hours:anHours.
        aSystemTime set &minutes:0.
        aSystemTime set &seconds:0.
        
        #var(type:long)aValue := aSystemTime long.
        
        ^ $self add &long:aValue.
    ]

    #method add &minute:aMinutes
    [
        #var(type:systemtime)aSystemTime.
        aSystemTime set &years:0.
        aSystemTime set &months:0.
        aSystemTime set &days:0.
        aSystemTime set &hours:0.
        aSystemTime set &minutes:aMinutes.
        aSystemTime set &seconds:0.
        
        #var(type:long)aValue := aSystemTime long.
        
        ^ $self add &long:aValue.
    ]

    #method add &second:aSeconds
    [
        #var(type:systemtime)aSystemTime.
        aSystemTime set &years:0.
        aSystemTime set &months:0.
        aSystemTime set &days:0.
        aSystemTime set &hours:0.
        aSystemTime set &minutes:0.
        aSystemTime set &seconds:aSeconds.
        
        #var(type:long)aValue := aSystemTime long.
        
        ^ $self add &long:aValue.
    ]

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
