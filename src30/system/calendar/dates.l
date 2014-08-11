#define system.

#type datetime.

// --- TimeSpan --

#class TimeSpan : BaseNumber
{
    #field(type:long)theData.
    
//    #method long = $self.
//    
//    #method Years = timeSpanControl readYears:$self.
//
//    #method Months = timeSpanControl readMonths:$self.
//                        
//    #method Days = timeSpanControl readDays:$self.
//                        
//    #method Hours = timeSpanControl readHours:$self.
//                        
//    #method Minutes = timeSpanControl readMinutes:$self.
//                        
//    #method Seconds = timeSpanControl readSeconds:$self.
//
//    #method Milliseconds = timeSpanControl readMilliseconds:$self.    
}

// --- Date ---

#class(type:datetime) Date : BaseNumber
{
    #field(type:long)theValue.
    
    #constructor new &Now
    [
        dateTimeControl read &CurrentLocalTime &out'long:theValue.
    ]
    
    #constructor new &UTCNow
    [
        dateTimeControl read &CurrentUTCTime &out'long:theValue.
    ]
  
//    #constructor new &Year:aYear &Month:aMonth &Day:aDay
//        => &new &long:(dateTimeControl new &Year:aYear &Month:aMonth &Day:aDay &Hour:0 &Minute:0 &Second:0).
//  
//    #constructor new &Year:aYear &Month:aMonth &Day:aDay &Hour:anHour &Minute:aMinute &Second:aSecond
//        => &new &long:(dateTimeControl new &Year:aYear &Month:aMonth &Day:aDay &Hour:anHour &Minute:aMinute &Second:aSecond).

//    #method add &long:aValue
//        = Date new &long:(theData + aValue).
//    
//    #method subtract &long:aValue
//        = Date new &long:(theData - aValue).
//  
//    #method subtract &datetime:aValue
//        = TimeSpan new &long:(theData - aValue long).

    #method Year = dateTimeControl readYear &long:theValue.

    #method Month = dateTimeControl readMonth &long:theValue.
                        
    #method Day = dateTimeControl readDay &long:theValue.
                        
    #method DayOfWeek = dateTimeControl readDayOfWeek &long:theValue.
                        
    #method Hour = dateTimeControl readHour &long:theValue.
                        
    #method Minute = dateTimeControl readMinute &long:theValue.
                        
    #method Second = dateTimeControl readSecond &long:theValue.
                        
    #method Millisecond = dateTimeControl readMillisecond &long:theValue.
                        
//    #method add &Years:aYear
//        = $self add &long:(timeSpanControl new &Years:aYear &Months:0 &Days:0 &Hours:0 &Minutes:0 &Seconds:0).
//
//    #method add &Months:aMonth
//        = $self add &long:(timeSpanControl new &Years:0 &Months:aMonth &Days:0 &Hours:0 &Minutes:0 &Seconds:0).
//
//    #method add &Days:aDay
//        = $self add &long:(timeSpanControl new &Years:0 &Months:0 &Days:aDay &Hours:0 &Minutes:0 &Seconds:0).
//
//    #method add &Hours:anHour
//        = $self add &long:(timeSpanControl new &Years:0 &Months:0 &Days:0 &Hours:anHour &Minutes:0 &Seconds:0).
//
//    #method add &Minutes:aMinute
//        = $self add &long:(timeSpanControl new &Years:0 &Months:0 &Days:0 &Hours:0 &Minutes:aMinute &Seconds:0).
//
//    #method add &Seconds:aSecond
//        = $self add &long:(timeSpanControl new &Years:0 &Months:0 &Days:0 &Hours:0 &Minutes:0 &Seconds:aSecond).

    #method literal
        = dateTimeControl toShortDateTime &long:theValue literal.
}

// --- now ---

#symbol now = Date new &Now.

#symbol utcNow = Date new &UTCNow.
