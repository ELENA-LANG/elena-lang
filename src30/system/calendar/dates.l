#define system.

// --- TimeSpan --

#class TimeSpan : BaseNumber
{
    #field(type:long)theData.
    
    #constructor new &long:aLong
    [
        theData << aLong.
    ]
    
    #method long = $self.
    
    #method Years = timeSpanControl readYears:$self.

    #method Months = timeSpanControl readMonths:$self.
                        
    #method Days = timeSpanControl readDays:$self.
                        
    #method Hours = timeSpanControl readHours:$self.
                        
    #method Minutes = timeSpanControl readMinutes:$self.
                        
    #method Seconds = timeSpanControl readSeconds:$self.

    #method Milliseconds = timeSpanControl readMilliseconds:$self.    
}

// --- Date ---

#class Date : BaseNumber
{
    #field(type:long)theData.
    
    #constructor new &long:aLong
    [
        theData << aLong.
    ]
  
    #constructor new &Year:aYear &Month:aMonth &Day:aDay
        => &new &type'long:(dateTimeControl new &Year:aYear &Month:aMonth &Day:aDay &Hour:0 &Minute:0 &Second:0).
  
    #constructor new &Year:aYear &Month:aMonth &Day:aDay &Hour:anHour &Minute:aMinute &Second:aSecond
        => &new &type'long:(dateTimeControl new &Year:aYear &Month:aMonth &Day:aDay &Hour:anHour &Minute:aMinute &Second:aSecond).
  
    #method long = $self.
    
    #method add &long:aValue
        = Date new &long:(theData + aValue).
    
    #method subtract &long:aValue
        = Date new &long:(theData - aValue).
  
    #method subtract &datetime:aValue
        = TimeSpan new &long:(theData - aValue long).

    #method Year = dateTimeControl readYear:$self.

    #method Month = dateTimeControl readMonth:$self.
                        
    #method Day = dateTimeControl readDay:$self.
                        
    #method DayOfWeek = dateTimeControl readDayOfWeek:$self.
                        
    #method Hour = dateTimeControl readHour:$self.
                        
    #method Minute = dateTimeControl readMinute:$self.
                        
    #method Second = dateTimeControl readSecond:$self.
                        
    #method Millisecond = dateTimeControl readMillisecond:$self.
                        
    #method add &Years:aYear
        = $self add &long:(timeSpanControl new &Years:aYear &Months:0 &Days:0 &Hours:0 &Minutes:0 &Seconds:0).

    #method add &Months:aMonth
        = $self add &long:(timeSpanControl new &Years:0 &Months:aMonth &Days:0 &Hours:0 &Minutes:0 &Seconds:0).

    #method add &Days:aDay
        = $self add &long:(timeSpanControl new &Years:0 &Months:0 &Days:aDay &Hours:0 &Minutes:0 &Seconds:0).

    #method add &Hours:anHour
        = $self add &long:(timeSpanControl new &Years:0 &Months:0 &Days:0 &Hours:anHour &Minutes:0 &Seconds:0).

    #method add &Minutes:aMinute
        = $self add &long:(timeSpanControl new &Years:0 &Months:0 &Days:0 &Hours:0 &Minutes:aMinute &Seconds:0).

    #method add &Seconds:aSecond
        = $self add &long:(timeSpanControl new &Years:0 &Months:0 &Days:0 &Hours:0 &Minutes:0 &Seconds:aSecond).

    #method literal
        = dateTimeControl toShortDateTime:$self.

    #method <= &datetime:$self.
}

// --- now ---

#symbol now = Date new &long:(dateTimeControl CurrentLocalTime).

#symbol utcNow = Date new &long:(dateTimeControl CurrentUTCTime).
