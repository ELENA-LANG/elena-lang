#define system.

#type(class:system'calendar'Date) datetime.
#type(class:system'calendar'TimeSpan) timespan.

// --- TimeSpan --

#class(sealed,struct:embeddable,type:timespan) TimeSpan : BaseNumber
{
    #field(type:long)theValue.

    #constructor new &long:aValue
    [
        theValue := aValue.
    ]
            
    #method read &out'long : aRetVal
    [
        aRetVal := theValue.
    ]

    #method long = theValue.
    
    #method years
    [
        #var(type:systemtime)aSystemTime.
        aSystemTime write &long:theValue.
        
        ^ aSystemTime years.
    ]

    #method months
    [
        #var(type:systemtime)aSystemTime.
        aSystemTime write &long:theValue.
        
        ^ aSystemTime months.
    ]
                        
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
    
    #method value = $self.
}

// --- Date ---

#class(sealed,struct:embeddable,type:datetime) Date : BaseNumber
{
    #field(type:long)theValue.
    
//    #constructor new &Now
//    [
//        dateTimeControl read &CurrentLocalTime &out'long:theValue.
//    ]
//    
//    #constructor new &UTCNow
//    [
//        dateTimeControl read &CurrentUTCTime &out'long:theValue.
//    ]
  
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
        
        aSystemTime read &out'long:theValue.
    ]
    
    #constructor new &year:aYear &month:aMonth &day:aDay
        <= %new &year:aYear &month:aMonth &day:aDay &hour:0 &minute:0 &second:0.
  
    #method long = theValue.

    #method read &out'long : aRetVal
    [
        aRetVal := theValue.
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

//    #method add &year:aYear
//        = $self add &long::(timeSpanControl new &Years:aYear &Months:0 &Days:0 &Hours:0 &Minutes:0 &Seconds:0).
//
//    #method add &month:aMonth
//        = $self add &long::(timeSpanControl new &Years:0 &Months:aMonth &Days:0 &Hours:0 &Minutes:0 &Seconds:0).
//
//    #method add &day:aDay
//        = $self add &long::(timeSpanControl new &Years:0 &Months:0 &Days:aDay &Hours:0 &Minutes:0 &Seconds:0).
//
//    #method add &hour:anHour
//        = $self add &long::(timeSpanControl new &Years:0 &Months:0 &Days:0 &Hours:anHour &Minutes:0 &Seconds:0).
//
//    #method add &minute:aMinute
//        = $self add &long::(timeSpanControl new &Years:0 &Months:0 &Days:0 &Hours:0 &Minutes:aMinute &Seconds:0).
//
//    #method add &second:aSecond
//        = $self add &long::(timeSpanControl new &Years:0 &Months:0 &Days:0 &Hours:0 &Minutes:0 &Seconds:aSecond).

    #method literal
        = dateTimeControl toShortDateTime &long:theValue literal.
        
//    #method cast : anObject = anObject datetime.
    
    #method value = $self.
    
    #method datetime = $self.
}

// --- now ---

#symbol now = control do:
[
    #var(type:long)aValue.
    dateTimeControl readCurrentUTCTime &out'long:aValue.
    
    dateTimeControl convertUTC2Local &long:aValue &out'long:aValue.
    
    ^ Date new &long:aValue.
].

#symbol utcNow = control do:
[
    #var(type:long)aValue.
    dateTimeControl readCurrentUTCTime &out'long:aValue.
    
    ^ Date new &long:aValue.
].
