// --- TimeSpan --

public sealed struct Timespan : BaseNumber
{
    SystemTime theValue;

    static SystemTime Default
        = new Timespan();

    constructor()
    {
        theValue := 0l
    }

//    #constructor min <= new &__time:(SystemTime $new &long:0l).

    constructor(SystemTime val)
    {
        theValue := val
    }

//    constructor new(SystemTime aValue)
//    [
//        theValue := aValue.
//    ]
//    
//    constructor new(int Hours, int Minutes, int Seconds)
//    [
//        long aValue := 0l.        
//        long n := T<long>(Hours).
//        
//        n := n * 36000000000l.
//        aValue := n.
//        
//        n := T<long>(Minutes).
//        n := n * 600000000l.                
//        aValue := aValue + n.
//        
//        n := T<long>(Seconds).
//        n := n * 10000000l.
//        aValue := aValue + n.
//        
//        theValue := aValue.
//    ]
//
//    constructor new(int Days, int Hours, int Minutes, int Seconds)
//    [
//        long aValue := 0l.
//
//        long n := T<long>(Hours).
//
//        n := n * 36000000000l.
//        aValue := n.
//        
//        n := T<long>(Minutes).
//        n := n * 600000000l.                
//        aValue := aValue + n.
//        
//        n := T<long>(Seconds).
//        n := n * 10000000l.
//        aValue := aValue + n.
//
//        n := T<long>(Days).
//        n := n * 864000000000l.
//        aValue := aValue + n.
//
//        theValue := aValue.
//    ]
//
//    cast T<SystemTime> = theValue.
//
//    readValueTo(ref<SystemTime> retVal)
//    [
//        retVal value := theValue.
//    ]
//
//    int days
//    [
//        long aRetVal := theValue long.
//        
//        aRetVal := aRetVal / 864000000000l.
//        
//        ^ intConvertor convert(aRetVal).
//    ]
//
//    int hours
//    [
//        long aRetVal := theValue long.
//        
//        aRetVal := aRetVal / 36000000000l.
//        
//        ^ intConvertor convert(aRetVal).
//    ]
//                        
//    int minutes
//    [
//        long aRetVal := theValue long.
//        
//        aRetVal := aRetVal / 600000000l.
//        
//        ^ intConvertor convert(aRetVal).
//    ]
//
//    int seconds
//    [
//        long aRetVal := theValue long.
//        
//        aRetVal := aRetVal / 10000000l.
//        
//        ^ intConvertor convert(aRetVal).
//    ]

    get long Milliseconds()
    {
        long retVal := theValue.Value;
        
        retVal := retVal / 10000l;
        
        ^ new LongNumber(retVal)
    }

//    bool equal(Timespan aSpan)
//    [
//        T<SystemTime> aValue := 0l.
//        aSpan readValueTo(&aValue).
//
//        ^ aValue equal(theValue).
//    ]
//
//    bool less(Timespan aSpan)
//    [
//        T<SystemTime> aValue := 0l.
//        aSpan readValueTo(&aValue).
//
//        ^ theValue less(aValue).
//    ]
//
//    add(Date aDate)
//        = aDate add(self).
//    
//    add(Timespan aSpan)
//    [
//        T<SystemTime> aValue := 0l.
//        aSpan readValueTo(&aValue).
//
//        T<SystemTime> retVal := theValue + aValue.
//
//        ^ Timespan new(retVal).
//    ]
//
//    subtract(Timespan aSpan)
//    [
//        T<SystemTime> aValue := 0l.
//        aSpan readValueTo(&aValue).
//
//        T<SystemTime> retVal := theValue - aValue.
//        ^ Timespan new(retVal).
//    ]
//
//    T<literal> literal
//    [
//        T<TimeStruct> aStruct := theValue.
//
//        literal aValue := aStruct toShortTime.
//
//        ^ (self days; literal + "." + aValue) literal.
//    ]
//
//    T<Timespan> clone
//        = Timespan new(theValue).
}

// --- Date ---

public sealed struct Date : BaseNumber
{
    SystemTime theValue;

    constructor()
    {
        theValue := 0l
    }
    
    static Date Default = new Date();

    static Date MinValue = new Date();

    get constructor Now()
    {
       SystemTime v;
       systemTimeFactory.readNow(ref v); 
        
       //systemTimeFactory.readNow(ref theValue)
       theValue := v
    }

//    constructor utcNow
//    [
//        systemTimeFactory readUtcNow(&theValue).
//    ]
//            
//    constructor(SystemTime aValue)
//    [
//        theValue := aValue.
//    ]
//    
//    constructor new(SystemTime aValue)
//    [
//        theValue := aValue.
//    ]
//  
//    constructor new(int aYear, int aMonth, int aDay, int anHour, int aMinute, int aSecond)
//    [
//        T<TimeStruct> aSystemTime := TimeStruct new.
//        aSystemTime year        := aYear.
//        aSystemTime month       := aMonth.
//        aSystemTime day         := aDay.
//        aSystemTime hour        := anHour.
//        aSystemTime minute      := aMinute.
//        aSystemTime second      := aSecond.
//        aSystemTime millisecond := 0.
//
//        T<SystemTime> aValue := 0l.
//        aSystemTime read(&aValue).
//        theValue := aValue.
//    ]
//    
//    constructor new(int aYear, int aMonth, int aDay)
//        <= new(aYear, aMonth, aDay, 0, 0, 0).
//  
//    cast T<SystemTime> = SystemTime new(theValue).
//
//    cast T<TimeStruct> = TimeStruct new(theValue).

    vprop SystemTime SystemTime
    {
        read(ref SystemTime retVal)
        {
            retVal := theValue
        }
    }
//   
//    bool equal(Date aDate)
//    [
//        T<SystemTime> aValue := 0l.
//        aDate readValueTo(&aValue).
//        
//        ^ aValue equal(theValue)
//    ]
//
//    bool less(Date aDate)
//    [
//        T<SystemTime> aValue := 0l.
//        aDate readValueTo(&aValue).
//        
//        ^ theValue less(aValue)
//    ]
  
    subtract(Date dt)
    {
        SystemTime val := dt.SystemTime;

        ^ new Timespan(theValue.subtract(val))
     }
  
//    subtract(Timespan aTimeSpan)
//    [
//        T<SystemTime> aValue := 0l.
//        aTimeSpan readValueTo(&aValue).
//        
//        ^ Date new(theValue subtract(aValue)).
//    ]
//  
//    add(Timespan aTimeSpan)
//    [
//        T<SystemTime> aValue := 0l.
//        aTimeSpan readValueTo(&aValue).
//        
//        ^ Date new(theValue add(aValue)).
//    ]
//
//    int year
//    [
//        T<TimeStruct> aSystemTime := theValue.
//        
//        ^ aSystemTime year.
//    ]
//    
//    int month
//    [
//        T<TimeStruct> aSystemTime := theValue.
//        
//        ^ aSystemTime month.
//    ]
//
//    int day
//    [
//        T<TimeStruct> aSystemTime := theValue.
//        
//        ^ aSystemTime day.
//    ]
//
//    int dayOfWeek
//    [
//        T<TimeStruct> aSystemTime := theValue.
//        
//        ^ aSystemTime dayOfWeek.
//    ]
//
//    int hour
//    [
//        T<TimeStruct> aSystemTime := theValue.
//        
//        ^ aSystemTime hour.
//    ]
//
//    int minute
//    [
//        T<TimeStruct> aSystemTime := theValue.
//        
//        ^ aSystemTime minute.
//    ]
//
//    int second
//    [
//        T<TimeStruct> aSystemTime := theValue.
//        
//        ^ aSystemTime second.
//    ]
//
//    int millisecond
//    [
//        T<TimeStruct> aSystemTime := theValue.
//        
//        ^ aSystemTime millisecond.
//    ]
//
//    addYears(int val)
//    [
//        int years := 0.
//        
//        T<TimeStruct> aSystemTime := theValue.
//
//        aSystemTime readYearTo(&years).
//        years := years + val.
//        aSystemTime writeYear(years).
//
//        T<SystemTime> aRetVal := 0l.
//        aSystemTime read(&aRetVal).
//        
//        ^ Date new(aRetVal).
//    ]
//
//    addMonths(int val)
//    [
//        T<TimeStruct> aSystemTime := theValue.
//        
//        int months := val.
//        
//        int years := months / 12.        
//        months := months -  months / 12 * 12.
//
//        int n := 0.
//        aSystemTime readMonthTo(&n).
//        months := months + n.
//        if (months > 12)
//            [ months := months - 12. years := years + 1 ].
//                
//        aSystemTime writeMonth(months).
//        
//        aSystemTime readYearTo(&n).
//        aSystemTime writeYear(n + years).
//        
//        T<SystemTime> aRetVal := 0l.
//        aSystemTime read(&aRetVal).
//        
//        ^ Date new(aRetVal).
//    ]
//
//    addDays(int val)
//        = self add(Timespan new(val, 0, 0, 0)).
//
//    addHours(int val)
//        = self add(Timespan new(0, val, 0, 0)).
//
//    addMinutes(int val)
//        = self add(Timespan new(0, 0, val, 0)).
//
//    addSeconds(int val)
//        = self add(Timespan new(0, 0, 0, val)).

    get string Printable()
    {
        TimeStruct struct := theValue;

        string timeValue := struct.toShortTime();
        
        string dateValue := struct.toShortDate();

        ^ dateValue + " " + timeValue
    }

//    #method clone
//        = Date new &__time:theValue.
}

// --- now ---

public Date now = Date.Now;

//public T<Date> utcNow = Date utcNow.
