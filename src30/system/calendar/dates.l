#define system.
#define system'math.

#subject(class:system'calendar'Date) datetime.
#subject(class:system'calendar'TimeSpan) timespan.

// --- TimeSpan --

#class(sealed,struct:embeddable) TimeSpan :: BaseNumber
{
    #field(type:__time)theValue.

    #constructor min <= %new &__time:(SystemTime $new &long:0l).

    #constructor new &__time:aValue
    [
        theValue := aValue.
    ]
    
    #constructor new &hours:Hours &minutes:Minutes &seconds:Seconds
    [
        #var(type:long)aValue := 0l.
        
        aValue := Hours long * 36000000000l.
        aValue := aValue + Minutes long multiply &long:600000000l.
        aValue := aValue + Seconds long multiply &long:10000000l.
        
        theValue := SystemTime $new &long:aValue.
    ]
            
    #constructor new &days:Days &hours:Hours &minutes:Minutes &seconds:Seconds
    [
        #var(type:long)aValue := 0l.
        
        aValue := Hours long * 36000000000l.
        aValue := aValue + Minutes long multiply &long:600000000l.
        aValue := aValue + Seconds long multiply &long:10000000l.
        aValue := aValue + Days long multiply &long:864000000000l.
        
        theValue := SystemTime $new &long:aValue.
    ]
    
    #method read &__vtime : aRetVal
    [
        aRetVal << theValue.
    ]

    #method __time = SystemTime new &__time:theValue.

    #method timespan = $self.
            
    #method days
    [
        #var(type:long)aRetVal.
        theValue read &vlong:aRetVal.
        
        aRetVal := aRetVal / 864000000000l.
        
        ^ intConvertor convert &long:aRetVal.
    ]
                        
    #method hours
    [
        #var(type:long)aRetVal.
        theValue read &vlong:aRetVal.
        
        aRetVal := aRetVal / 36000000000l.
        
        ^ intConvertor convert &long:aRetVal.
    ]
                        
    #method minutes
    [
        #var(type:long)aRetVal.
        theValue read &vlong:aRetVal.
        
        aRetVal := aRetVal / 600000000l.
        
        ^ intConvertor convert &long:aRetVal.
    ]
                        
    #method seconds
    [
        #var(type:long)aRetVal.
        theValue read &vlong:aRetVal.
        
        aRetVal := aRetVal / 10000000l.
        
        ^ intConvertor convert &long:aRetVal.
    ]

    #method milliseconds
    [
        #var(type:long)aRetVal.
        theValue read &vlong:aRetVal.
        
        aRetVal := aRetVal / 10000l.
        
        ^ LongNumber new &long:aRetVal.
    ]
   
    #method equal &timespan:aSpan
    [
        #var(type:__time)aValue.
        aSpan read &__vtime:aValue.
        
        ^ aValue == theValue.
    ]

    #method less &timespan:aSpan
    [
        #var(type:__time)aValue.
        aSpan read &__vtime:aValue.
        
        ^ aValue < theValue.
    ]

    #method add &datetime:aDate
        = aDate add &timespan:$self.
    
    #method add &timespan:aSpan
    [
        #var(type:__time)aValue.
        aSpan read &__vtime:aValue.
                
        aValue := aValue + theValue.
        
        ^ TimeSpan new &__time:aValue.
    ]

    #method subtract &timespan:aSpan
    [
        #var(type:__time)aValue.
        aSpan read &__vtime:aValue.
                
        aValue := theValue subtract &__time:aValue.
        
        ^ TimeSpan new &__time:aValue.
    ]

    #method literal
    [
        #var(type:__time_st)aStruct.
        aStruct write &__time:theValue.
        
        #var aValue := aStruct toShortTime literal.

        ^ ($self days literal + "." + aValue) literal.
    ]

    #method wide
    [
        #var(type:__time_st)aStruct.
        aStruct write &__time:theValue.
        
        #var aValue := aStruct toShortTime literal.
        
         ^ ($self days literal + "." + aValue) wide.
    ]

    #method clone
        = TimeSpan new &__time:theValue.

    #method cast:aVerb &to:aTarget = aTarget::aVerb eval &timespan:$self.
    
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &timespan:$self.
}

// --- Date ---

#class(sealed,struct:embeddable) Date :: BaseNumber
{
    #field(type:__time)theValue.

    #constructor min <= %new &__time:(SystemTime $new &long:0l).

    #constructor now
        <= %new &__time:(SystemTime now).

    #constructor utcNow
        <= %new &__time:(SystemTime utcNow).
            
    #constructor new &__time:aValue
    [
        theValue := aValue.
    ]
  
    #constructor new &year:aYear &month:aMonth &day:aDay &hour:anHour &minute:aMinute &second:aSecond
    [
        #var(type:__time_st)aSystemTime.
        aSystemTime set &year:(aYear int).
        aSystemTime set &month:(aMonth int).
        aSystemTime set &day:(aDay int).
        aSystemTime set &hour:(anHour int).
        aSystemTime set &minute:(aMinute int).
        aSystemTime set &second:(aSecond int).
        aSystemTime set &millisecond:0.
        
        #var(type:__time)aValue.
        aSystemTime read &__vtime:aValue.        
        theValue := aValue.
    ]
    
    #constructor new &year:aYear &month:aMonth &day:aDay
        <= %new &year:aYear &month:aMonth &day:aDay &hour:0 &minute:0 &second:0.
  
    #method __time = SystemTime new &__time:theValue.
  
    #method __time_st = TimeStruct new write &__time:theValue.

    #method read &__vtime : aRetVal
    [
        aRetVal << theValue.
    ]
   
    #method equal &datetime:aDate
    [
        #var(type:__time)aValue.
        aDate read &__vtime:aValue.
        
        ^ aValue == theValue.
    ]

    #method less &datetime:aDate
    [
        #var(type:__time)aValue.
        aDate read &__vtime:aValue.
        
        ^ aValue < theValue.
    ]
  
    #method subtract &datetime:aDate
    [
        #var(type:__time)aValue.
        aDate read &__vtime:aValue.
        
        ^ TimeSpan new &__time:(theValue subtract &__time:aValue).
    ]
  
    #method subtract &timespan:aTimeSpan
    [
        #var(type:__time)aValue.
        aTimeSpan read &__vtime:aValue.
        
        ^ Date new &__time:(theValue subtract &__time:aValue).
    ]
  
    #method add &timespan:aTimeSpan
    [
        #var(type:__time)aValue.
        aTimeSpan read &__vtime:aValue.
        
        ^ Date new &__time:(theValue add &__time:aValue).
    ]

    #method year
    [
        #var(type:__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        
        ^ aSystemTime year.
    ]
    
    #method month
    [
        #var(type:__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        
        ^ aSystemTime month.
    ]

    #method day
    [
        #var(type:__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        
        ^ aSystemTime day.
    ]

    #method dayOfWeek
    [
        #var(type:__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        
        ^ aSystemTime dayOfWeek.
    ]

    #method hour
    [
        #var(type:__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        
        ^ aSystemTime hour.
    ]

    #method minute
    [
        #var(type:__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        
        ^ aSystemTime minute.
    ]

    #method second
    [
        #var(type:__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        
        ^ aSystemTime second.
    ]

    #method millisecond
    [
        #var(type:__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        
        ^ aSystemTime millisecond.
    ]

    #method add &years:aYears
    [
        #var(type:__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        
        #var m := aSystemTime month.
        
        aSystemTime set &year:(aSystemTime year + aYears).
        
        #var m2 := aSystemTime month.
        
        #var(type:__time)aRetVal.
        aSystemTime read &__vtime:aRetVal.
        
        ^ Date new &__time:aRetVal.
    ]

    #method add &months:Months
    [
        #var(type:int) aMonths:= Months::intOp mod:12.
        #var(type:int) Years := Months / 12.
        
        ($self month + aMonths > 12)
            ? [ Years := Years + 1. aMonths := 12 - ($self month + aMonths). ].

        #var(type:__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        
        aSystemTime set &year:(aSystemTime year add &int:Years).
        aSystemTime set &month:(aSystemTime month add &int:aMonths).
        
        #var(type:__time)aRetVal.
        aSystemTime read &__vtime:aRetVal.
        
        ^ Date new &__time:aRetVal.
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
    [
        #var(type:__time_st)aStruct.
        aStruct write &__time:theValue.
        
        #var aTimeValue := aStruct toShortTime literal.
        #var aDateValue := aStruct toShortDate literal.

        ^ (aDateValue + " " + aTimeValue) literal.
    ]

    #method wide
    [
        #var(type:__time_st)aStruct.
        aStruct write &__time:theValue.
        
        #var aTimeValue := aStruct toShortTime wide.
        #var aDateValue := aStruct toShortDate wide.

        ^ (aDateValue + " " + aTimeValue) wide.
    ]
        
    #method cast:aVerb &to:aTarget = aTarget::aVerb eval &datetime:$self.
    
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &datetime:$self.
    
    #method datetime = $self.

    #method clone
        = Date new &__time:theValue.
    
}

// --- now ---

#symbol(type:datetime) now = Date now.

#symbol(type:datetime) utcNow = Date utcNow.
