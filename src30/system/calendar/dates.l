// ==== ver 3.4.1 ===

#import system.

// --- TimeSpan --

#class(sealed,struct:embeddable) TimeSpan :: BaseNumber
{
    #field(type:__time)theValue.

    #constructor new <= %new &__time:(SystemTime $new &long:0l).

    #constructor min <= %new &__time:(SystemTime $new &long:0l).

    #constructor(stacksafe) new &__time:aValue
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
    
    #method(stacksafe) read &__time : aRetVal
    [
        aRetVal write &__time:theValue.
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
        aSpan read &__time:aValue.
        
        ^ aValue == theValue.
    ]

    #method less &timespan:aSpan
    [
        #var(type:__time)aValue.
        aSpan read &__time:aValue.
        
        ^ aValue < theValue.
    ]

    #method add &datetime:aDate
        = aDate add &timespan:$self.
    
    #method add &timespan:aSpan
    [
        #var(type:__time)aTime.
        aSpan read &__time:aTime.
                
        #var(type:long)aValue.
        aTime read &vlong:aValue.

        #var(type:long)aCurrentValue.
        theValue read &vlong:aCurrentValue.

        ^ TimeSpan new &__time:(SystemTime $new &long:(aValue + aCurrentValue)).
    ]

    #method subtract &timespan:aSpan
    [
        #var(type:__time)aValue.
        aSpan read &__time:aValue.
                
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

    #constructor new <= %new &__time:(SystemTime $new &long:0l).

    #constructor min <= %new &__time:(SystemTime $new &long:0l).

    #constructor now
        <= %new &__time:(SystemTime now).

    #constructor utcNow
        <= %new &__time:(SystemTime utcNow).
            
    #constructor(stacksafe) new &__time:aValue
    [
        theValue := aValue.
    ]
  
    #constructor new &year:aYear &month:aMonth &day:aDay &hour:anHour &minute:aMinute &second:aSecond
    [
        #var(type:__time_st)aSystemTime.
        aSystemTime writeYear &int:(aYear int).
        aSystemTime writeMonth &int:(aMonth int).
        aSystemTime writeDay &int:(aDay int).
        aSystemTime writeHour &int:(anHour int).
        aSystemTime writeMinute &int:(aMinute int).
        aSystemTime writeSecond &int:(aSecond int).
        aSystemTime writeMillisecond &int:0.
        
        #var(type:__time)aValue.
        aSystemTime read &__time:aValue.        
        theValue := aValue.
    ]
    
    #constructor new &year:aYear &month:aMonth &day:aDay
        <= %new &year:aYear &month:aMonth &day:aDay &hour:0 &minute:0 &second:0.
  
    #method __time = SystemTime new &__time:theValue.
  
    #method(stacksafe) __time_st = TimeStruct new &__time:theValue.

    #method(stacksafe) read &__time : aRetVal
    [
        aRetVal write &__time:theValue.
    ]
   
    #method equal &datetime:aDate
    [
        #var(type:__time)aValue.
        aDate read &__time:aValue.
        
        ^ aValue == theValue.
    ]

    #method less &datetime:aDate
    [
        #var(type:__time)aValue.
        aDate read &__time:aValue.
        
        ^ aValue < theValue.
    ]
  
    #method subtract &datetime:aDate
    [
        #var(type:__time)aValue.
        aDate read &__time:aValue.
        
        ^ TimeSpan new &__time:(theValue subtract &__time:aValue).
    ]
  
    #method subtract &timespan:aTimeSpan
    [
        #var(type:__time)aValue.
        aTimeSpan read &__time:aValue.
        
        ^ Date new &__time:(theValue subtract &__time:aValue).
    ]
  
    #method add &timespan:aTimeSpan
    [
        #var(type:__time)aValue.
        aTimeSpan read &__time:aValue.
        
        ^ Date new &__time:(theValue add &__time:aValue).
    ]

    #method year
    [
        #var(type:int)aYear.
        #var(type:__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        aSystemTime readYear &vint:aYear.

        ^ IntNumber new &int:aYear.
    ]
    
    #method month
    [
        #var(type:int)aMonth.
        #var(type:__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        aSystemTime readMonth &vint:aMonth.

        ^ IntNumber new &int:aMonth.
    ]

    #method day
    [
        #var(type:int)aDay.
        #var(type:__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        aSystemTime readDay &vint:aDay.

        ^ IntNumber new &int:aDay.
    ]

    #method dayOfWeek
    [
        #var(type:int)aDayOfWeek.
        #var(type:__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        aSystemTime readDayOfWeek &vint:aDayOfWeek.

        ^ IntNumber new &int:aDayOfWeek.
    ]

    #method hour
    [
        #var(type:int)aHour.
        #var(type:__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        aSystemTime readHour &vint:aHour.

        ^ IntNumber new &int:aHour.
    ]

    #method minute
    [
        #var(type:int)aMinute.
        #var(type:__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        aSystemTime readMinute &vint:aMinute.

        ^ IntNumber new &int:aMinute.
    ]

    #method second
    [
        #var(type:int)aSecond.
        #var(type:__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        aSystemTime readSecond &vint:aSecond.

        ^ IntNumber new &int:aSecond.
    ]

    #method millisecond
    [
        #var(type:int)aMSecond.
        #var(type:__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        aSystemTime readMillisecond &vint:aMSecond.

        ^ IntNumber new &int:aMSecond.
    ]

    #method add &years:aYears
    [
        #var(type:int)years.
        #var(type:__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        
        aSystemTime readYear &vint:years.
        years := years + aYears int.
        aSystemTime writeYear &int:years.
        
        #var(type:__time)aRetVal.
        aSystemTime read &__time:aRetVal.
        
        ^ Date new &__time:aRetVal.
    ]

    #method add &months:Months
    [
        #var(type:__time_st)aSystemTime.
        aSystemTime write &__time:theValue.
        
        #var(type:int) months := Months int.
        
        #var(type:int)years := months / 12.
        months := Months::intOp mod:12.

        #var(type:int) n.
        aSystemTime readMonth &vint:n.
        months := months + n.
        (months > 12)
            ? [ months := months - 12. years := years + 1. ].
                
        aSystemTime writeMonth &int:months.
        
        aSystemTime readYear &vint:n.
        aSystemTime writeYear &int:(n + years).
        
        #var(type:__time)aRetVal.
        aSystemTime read &__time:aRetVal.
        
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
