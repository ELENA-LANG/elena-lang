// --- TimeSpan --

sealed embeddable struct TimeSpan :: BaseNumber, dispatchable(timespan)
{
    __time theValue.

    constructor new
    [
        theValue := 0l.
    ]

//    #constructor min <= new &__time:(SystemTime $new &long:0l).

    stacksafe implicit __time:aValue
    [
        theValue := aValue.
    ]

    stacksafe constructor new __time:aValue
    [
        theValue := aValue.
    ]
    
    constructor new hours:Hours minutes:Minutes seconds:Seconds
    [
        long aValue := 0l.        
        long n := Hours long.
        
        n := n * 36000000000l.
        aValue := n.
        
        n := Minutes long.
        n := n * 600000000l.                
        aValue := aValue + n.
        
        n := Seconds long.
        n := n * 10000000l.
        aValue := aValue + n.
        
        theValue := aValue.
    ]

    constructor new days:Days hours:Hours minutes:Minutes seconds:Seconds
    [
        long aValue := 0l.

        long n := Hours long.

        n := n * 36000000000l.
        aValue := n.
        
        n := Minutes long.
        n := n * 600000000l.                
        aValue := aValue + n.
        
        n := Seconds long.
        n := n * 10000000l.
        aValue := aValue + n.

        n := Days long.
        n := n * 864000000000l.
        aValue := aValue + n.

        theValue := aValue.
    ]

    stacksafe read __vtime:retVal
    [
        retVal __time := theValue.
    ]

    __time = SystemTime new __time:theValue.

    days
    [
        long aRetVal := theValue long.
        
        aRetVal := aRetVal / 864000000000l.
        
        ^ intConvertor convert long:aRetVal.
    ]

    hours
    [
        long aRetVal := theValue long.
        
        aRetVal := aRetVal / 36000000000l.
        
        ^ intConvertor convert long:aRetVal.
    ]
                        
    minutes
    [
        long aRetVal := theValue long.
        
        aRetVal := aRetVal / 600000000l.
        
        ^ intConvertor convert long:aRetVal.
    ]

    seconds
    [
        long aRetVal := theValue long.
        
        aRetVal := aRetVal / 10000000l.
        
        ^ intConvertor convert long:aRetVal.
    ]

    milliseconds
    [
        long aRetVal := theValue long.
        
        aRetVal := aRetVal / 10000l.
        
        ^ LongNumber new long:aRetVal.
    ]

    equal timespan:aSpan
    [
        __time aValue := 0l.
        aSpan read __vtime:aValue.

        ^ aValue equal __time:theValue.
    ]

    less timespan:aSpan
    [
        __time aValue := 0l.
        aSpan read __vtime:aValue.

        ^ theValue less __time:aValue.
    ]

    add datetime:aDate
        = aDate add timespan:$self.
    
    add timespan:aSpan
    [
        __time aValue := 0l.
        aSpan read __vtime:aValue.

        __time retVal := theValue + aValue.

        ^ TimeSpan new __time:retVal.
    ]

    subtract timespan:aSpan
    [
        __time aValue := 0l.
        aSpan read __vtime:aValue.

        __time retVal := theValue - aValue.
        ^ TimeSpan new __time:retVal.
    ]

    literal
    [
        __time_st aStruct := theValue.

        literal aValue := aStruct toShortTime.

        ^ ($self days; literal + "." + aValue) literal.
    ]

    clone
        = TimeSpan new __time:theValue.
}

// --- Date ---

sealed embeddable struct Date :: BaseNumber, dispatchable(datetime)
{
    __time theValue.

    constructor new
    [
        theValue := 0l.
    ]

//    #constructor min <= new &__time:(SystemTime $new &long:0l).

    constructor now
    [
        $systemTimeFactory readNow __vtime:theValue.
    ]

    constructor utcNow
    [
        $systemTimeFactory readUtcNow __vtime:theValue.
    ]
            
    stacksafe implicit __time:aValue
    [
        theValue := aValue.
    ]
    
    stacksafe constructor new __time:aValue
    [
        theValue := aValue.
    ]
  
    constructor new year:aYear month:aMonth day:aDay hour:anHour minute:aMinute second:aSecond
    [
        __time_st aSystemTime := 0.
        aSystemTime year        := aYear.
        aSystemTime month       := aMonth.
        aSystemTime day         := aDay.
        aSystemTime hour        := anHour.
        aSystemTime minute      := aMinute.
        aSystemTime second      := aSecond.
        aSystemTime millisecond := 0.

        __time aValue := 0l.
        aSystemTime read __vtime:aValue.
        theValue := aValue.
    ]
    
    constructor new year:aYear month:aMonth day:aDay
        <= new year:aYear month:aMonth day:aDay hour:0 minute:0 second:0.
  
    __time = SystemTime new __time:theValue.

    stacksafe __time_st = TimeStruct new __time:theValue.

    stacksafe read __vtime : aRetVal
    [
        aRetVal __time := theValue.
    ]
   
    equal datetime:aDate
    [
        __time aValue := 0l.
        aDate read __vtime:aValue.
        
        ^ aValue equal __time:theValue
    ]

    less datetime:aDate
    [
        __time aValue := 0l.
        aDate read __vtime:aValue.
        
        ^ theValue less __time:aValue
    ]
  
    subtract datetime:aDate
    [
        __time aValue := 0l.
        aDate read __vtime:aValue.

        ^ TimeSpan new __time:(theValue subtract __time:aValue)
    ]
  
    subtract timespan:aTimeSpan
    [
        __time aValue := 0l.
        aTimeSpan read __vtime:aValue.
        
        ^ Date new __time:(theValue subtract __time:aValue).
    ]
  
    add timespan:aTimeSpan
    [
        __time aValue := 0l.
        aTimeSpan read __vtime:aValue.
        
        ^ Date new __time:(theValue add __time:aValue).
    ]

    year
    [
        __time_st aSystemTime := theValue.
        
        ^ aSystemTime year.
    ]
    
    month
    [
        __time_st aSystemTime := theValue.
        
        ^ aSystemTime month.
    ]

    day
    [
        __time_st aSystemTime := theValue.
        
        ^ aSystemTime day.
    ]

    dayOfWeek
    [
        __time_st aSystemTime := theValue.
        
        ^ aSystemTime dayOfWeek.
    ]

    hour
    [
        __time_st aSystemTime := theValue.
        
        ^ aSystemTime hour.
    ]

    minute
    [
        __time_st aSystemTime := theValue.
        
        ^ aSystemTime minute.
    ]

    second
    [
        __time_st aSystemTime := theValue.
        
        ^ aSystemTime second.
    ]

    millisecond
    [
        __time_st aSystemTime := theValue.
        
        ^ aSystemTime millisecond.
    ]

    add years:aYears
    [
        int years := 0.
        __time_st aSystemTime := theValue.

        aSystemTime readYearTo vint:years.
        years := years + aYears int.
        aSystemTime writeYear int:years.

        __time aRetVal := 0l.
        aSystemTime read __vtime:aRetVal.
        
        ^ Date new __time:aRetVal.
    ]

    add months:Months
    [
        __time_st aSystemTime := theValue.
        
        int months := Months int.
        
        int years := months / 12.        
        months := months -  months / 12 * 12.

        int n := 0.
        aSystemTime readMonthTo vint:n.
        months := months + n.
        if (months > 12)
            [ months := months - 12. years := years + 1 ].
                
        aSystemTime writeMonth int:months.
        
        aSystemTime readYearTo vint:n.
        aSystemTime writeYear int:(n + years).
        
        __time aRetVal := 0l.
        aSystemTime read __vtime:aRetVal.
        
        ^ Date new __time:aRetVal
    ]

    add days:aDays
        = $self add timespan:(TimeSpan new days:aDays hours:0 minutes:0 seconds:0).

    add hours:anHours
        = $self add timespan:(TimeSpan new days:0 hours:anHours minutes:0 seconds:0).

    add minutes:aMinutes
        = $self add timespan:(TimeSpan new days:0 hours:0 minutes:aMinutes seconds:0).

    add seconds:Seconds
        = $self add timespan:(TimeSpan new days:0 hours:0 minutes:0 seconds:Seconds).

    literal
    [
        __time_st aStruct := theValue.

        literal aTimeValue := aStruct toShortTime.
        
        literal aDateValue := aStruct toShortDate.

        ^ (aDateValue + " " + aTimeValue) literal.
    ]

//    #method clone
//        = Date new &__time:theValue.
}

// --- now ---

datetime now = Date now.

datetime utcNow = Date utcNow.
