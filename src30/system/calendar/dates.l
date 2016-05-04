#import system.

// --- TimeSpan --

#class(sealed,struct:embeddable,dispatchable:timespan) TimeSpan :: BaseNumber
{
    #field(__time)theValue.

    #constructor new <= new &__time:(SystemTime $new &long:0l).

    #constructor min <= new &__time:(SystemTime $new &long:0l).

    #constructor(stacksafe) new &__time:aValue
    [
        theValue := aValue.
    ]
    
    #constructor new &hours:Hours &minutes:Minutes &seconds:Seconds
    [
        #var(long)aValue := 0l.        
        #var(long)n := Hours long.
        
        n *= 36000000000l.
        aValue := n.
        
        n := Minutes long.
        n *= 600000000l.                
        aValue += n.
        
        n := Seconds long.
        n *= 10000000l.
        aValue += n.
        
        theValue := SystemTime $new &long:aValue.
    ]

    #constructor new &days:Days &hours:Hours &minutes:Minutes &seconds:Seconds
    [
        #var(long)aValue := 0l.

        #var(long)n := Hours long.

        n *= 36000000000l.
        aValue := n.
        
        n := Minutes long.
        n *= 600000000l.                
        aValue += n.
        
        n := Seconds long.
        n *= 10000000l.
        aValue += n.

        n := Days long.
        n *= 864000000000l.
        aValue += n.

        theValue := SystemTime $new &long:aValue.
    ]

    #method(stacksafe) read &__vtime:retVal
    [
        retVal := theValue.
    ]

    #method __time = SystemTime new &__time:theValue.

    #method days
    [
        #var(long)aRetVal := theValue long.
        
        aRetVal /= 864000000000l.
        
        ^ intConvertor convert &long:aRetVal.
    ]

    #method hours
    [
        #var(long)aRetVal := theValue long.
        
        aRetVal /= 36000000000l.
        
        ^ intConvertor convert &long:aRetVal.
    ]
                        
    #method minutes
    [
        #var(long)aRetVal := theValue long.
        
        aRetVal /= 600000000l.
        
        ^ intConvertor convert &long:aRetVal.
    ]

    #method seconds
    [
        #var(long)aRetVal := theValue long.
        
        aRetVal /= 10000000l.
        
        ^ intConvertor convert &long:aRetVal.
    ]

    #method milliseconds
    [
        #var(long)aRetVal := theValue long.
        
        aRetVal /= 10000l.
        
        ^ LongNumber new &long:aRetVal.
    ]

    #method equal &timespan:aSpan
    [
        #var(__time)aValue.
        aSpan read &__vtime:aValue.

        ^ aValue equal &__time:theValue.
    ]

    #method less &timespan:aSpan
    [
        #var(__time)aValue.
        aSpan read &__vtime:aValue.

        ^ theValue less &__time:aValue.
    ]

    #method add &datetime:aDate
        = aDate add &timespan:$self.
    
    #method add &timespan:aSpan
    [
        #var(__time)aTime.
        aSpan read &__vtime:aTime.
                
        #var(long)aValue := aTime long.
        #var(long)aCurrentValue := theValue long.

        ^ TimeSpan new &__time:(SystemTime $new &long:(aValue + aCurrentValue)).
    ]

    #method subtract &timespan:aSpan
    [
        #var(__time)aTime.
        aSpan read &__vtime:aTime.

        #var(long)aValue := aTime long.
        #var(long)aCurrentValue := theValue long.
                                
        ^ TimeSpan new &__time:(SystemTime $new &long:(aCurrentValue - aValue)).
    ]

    #method literal
    [
        #var(__time_st)aStruct.
        aStruct writeSpan &__time:theValue.

        #var aValue := aStruct toShortTime literal.

        ^ ($self days literal + "." + aValue) literal.
    ]

    #method wide
    [
        #var(__time_st)aStruct.
        aStruct writeSpan &__time:theValue.        
        
        #var aValue := aStruct toShortTime literal.
        
         ^ ($self days literal + "." + aValue) wide.
    ]

    #method clone
        = TimeSpan new &__time:theValue.
}

// --- Date ---

#class(sealed,struct:embeddable,dispatchable:datetime) Date :: BaseNumber
{
    #field(__time)theValue.

    #constructor new <= new &__time:(SystemTime $new &long:0l).

    #constructor min <= new &__time:(SystemTime $new &long:0l).

    #constructor now
        <= new &__time:(SystemTime now).

    #constructor utcNow
        <= new &__time:(SystemTime utcNow).
            
    #constructor(stacksafe) new &__time:aValue
    [
        theValue := aValue.
    ]
  
    #constructor new &year:aYear &month:aMonth &day:aDay &hour:anHour &minute:aMinute &second:aSecond
    [
        #var(__time_st)aSystemTime.
        aSystemTime year        := aYear.
        aSystemTime month       := aMonth.
        aSystemTime day         := aDay.
        aSystemTime hour        := anHour.
        aSystemTime minute      := aMinute.
        aSystemTime second      := aSecond.
        aSystemTime millisecond := 0.

        #var(__time)aValue.
        aSystemTime read &__vtime:aValue.        
        theValue := aValue.
    ]
    
    #constructor new &year:aYear &month:aMonth &day:aDay
        <= new &year:aYear &month:aMonth &day:aDay &hour:0 &minute:0 &second:0.
  
    #method __time = SystemTime new &__time:theValue.

    #method(stacksafe) __time_st = TimeStruct new &__time:theValue.

    #method(stacksafe) read &__vtime : aRetVal
    [
        aRetVal := theValue.
    ]
   
    #method equal &datetime:aDate
    [
        #var(__time)aValue.
        aDate read &__vtime:aValue.
        
        ^ aValue equal &__time:theValue.
    ]

    #method less &datetime:aDate
    [
        #var(__time)aValue.
        aDate read &__vtime:aValue.
        
        ^ theValue less &__time:aValue.
    ]
  
    #method subtract &datetime:aDate
    [
        #var(__time)aValue.
        aDate read &__vtime:aValue.

        ^ TimeSpan new &__time:(theValue subtract &__time:aValue).
    ]
  
    #method subtract &timespan:aTimeSpan
    [
        #var(__time)aValue.
        aTimeSpan read &__vtime:aValue.
        
        ^ Date new &__time:(theValue subtract &__time:aValue).
    ]
  
    #method add &timespan:aTimeSpan
    [
        #var(__time)aValue.
        aTimeSpan read &__vtime:aValue.
        
        ^ Date new &__time:(theValue add &__time:aValue).
    ]

    #method year
    [
        #var(__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        
        ^ aSystemTime year.
    ]
    
    #method month
    [
        #var(__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        
        ^ aSystemTime month.
    ]

    #method day
    [
        #var(__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        
        ^ aSystemTime day.
    ]

    #method dayOfWeek
    [
        #var(__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        
        ^ aSystemTime dayOfWeek.
    ]

    #method hour
    [
        #var(__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        
        ^ aSystemTime hour.
    ]

    #method minute
    [
        #var(__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        
        ^ aSystemTime minute.
    ]

    #method second
    [
        #var(__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        
        ^ aSystemTime second.
    ]

    #method millisecond
    [
        #var(__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        
        ^ aSystemTime millisecond.
    ]

    #method add &years:aYears
    [
        #var(int)years.
        #var(__time_st)aSystemTime.
        aSystemTime write &__time:theValue.

        aSystemTime readYear &vint:years.
        years += aYears int.
        aSystemTime writeYear &int:years.

        #var(__time)aRetVal.
        aSystemTime read &__vtime:aRetVal.
        
        ^ Date new &__time:aRetVal.
    ]

    #method add &months:Months
    [
        #var(__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        
        #var(int) months := Months int.
        
        #var(int)years := months / 12.
        months := Months::intOp mod:12.

        #var(int) n.
        aSystemTime readMonth &vint:n.
        months += n.
        (months > 12)
            ? [ months := months - 12. years := years + 1. ].
                
        aSystemTime writeMonth &int:months.
        
        aSystemTime readYear &vint:n.
        aSystemTime writeYear &int:(n + years).
        
        #var(__time)aRetVal.
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
        #var(__time_st)aStruct.
        aStruct write &__time:theValue.

        #var aTimeValue := aStruct toShortTime literal.
        
        #var aDateValue := aStruct toShortDate literal.

        ^ (aDateValue + " " + aTimeValue) literal.
    ]

    #method wide
    [
        #var(__time_st)aStruct.
        aStruct write &__time:theValue.
        
        #var aTimeValue := aStruct toShortTime wide.
        #var aDateValue := aStruct toShortDate wide.

        ^ (aDateValue + " " + aTimeValue) wide.
    ]

    #method clone
        = Date new &__time:theValue.
}

//// --- now ---
//
//#symbol(type:datetime) now = Date now.
//
//#symbol(type:datetime) utcNow = Date utcNow.
