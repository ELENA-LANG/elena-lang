const int DATE_SHORTDATE  = 00000001h.
const int DATE_LONGDATE   = 00000002h.

// --- SystemTime ---

singleton systemTimeFactory
{
    readUtcNow(ref<SystemTime> retVal)
    [
        T<SystemTime> aTime := 0l.        
        system'external'KERNEL32 GetSystemTimeAsFileTime(aTime).
        
        retVal value := aTime.
    ]
    
    readNow(ref<SystemTime> retVal)
    [
        T<SystemTime> aTime := 0l.
        T<SystemTime> aLocalTime := 0l.
        
        system'external'KERNEL32 GetSystemTimeAsFileTime(aTime).

        system'external'KERNEL32 FileTimeToLocalFileTime(aTime, aLocalTime).
        
        retVal value := aLocalTime.
    ]
}

public sealed const struct SystemTime :: BaseValue
{
    T<__int,8> theValue.

    constructor(long value)
    [
        theValue := value.
    ]
            
    constructor new(SystemTime value)
        = value.

    bool equal(SystemTime anOperand)
        = theValue == anOperand.

    bool less(SystemTime anOperand)
        = theValue < anOperand.

    cast T<long> = theValue.

    long rvprop long :: readValueTo.

//    //long :: outlprop(readValueTo).
//    stacksafe embeddable long
//    [
//        long val := 0.        
//        $self readValueTo vlong:val.
//        
//        ^ val.
//    ]    

    readValueTo(ref<long> retVal)
    [
        retVal value := theValue.
    ]

    T<SystemTime> subtract(SystemTime anOperand)
    [
        T<SystemTime> result := theValue - anOperand.
        
        ^ SystemTime new(result).
    ]

    T<SystemTime> add(SystemTime anOperand)
    [
        T<SystemTime> result := theValue + anOperand.
        
        ^ SystemTime new(result).
    ]
}

// --- TimeStruct ---

public sealed struct TimeStruct
{
    short theYear.
    short theMonth.
    short theDayOfWeek.
    short theDay.
    short theHour.
    short theMinute.
    short theSecond.
    short theMillisecond.

    constructor(SystemTime aTime)
    [
        self write(aTime).
    ]

    embeddable constructor new
    [
        system'core_routines'__timest empty(self).
    ]

    constructor new(SystemTime aTime)
    [
        self write(aTime).
    ]

    int vprop  year        :: readYearTo,writeYear.
    int vprop  month       :: readMonthTo,writeMonth.
    int rvprop dayOfWeek   :: readDayOfWeekTo.
    int vprop  day         :: readDayTo,writeDay.
    int vprop  hour        :: readHourTo,writeHour.
    int vprop  minute      :: readMinuteTo,writeMinute.
    int vprop  second      :: readSecondTo,writeSecond.
    int vprop  millisecond :: readMillisecondTo,writeMillisecond.

    readYearTo(ref<int> aRetVal)
    [
        int val := theYear.
        
        aRetVal value := val.
    ]

    readMonthTo(ref<int> aRetVal)
    [
        int val := theMonth.
        
        aRetVal value := val.
    ]

    readDayOfWeekTo(ref<int> aRetVal)
    [
        int val := theDayOfWeek.
        
        aRetVal value := val.
    ]

    readDayTo(ref<int> aRetVal)
    [
        int val := theDay.
        
        aRetVal value := val.
    ]

    readHourTo(ref<int> aRetVal)
    [
        int val := theHour.
        
        aRetVal value := val.
    ]

    readMinuteTo(ref<int> aRetVal)
    [
        int val := theMinute.
        
        aRetVal value := val.
    ]

    readSecondTo(ref<int> aRetVal)
    [
        int val := theSecond.
        
        aRetVal value := val.
    ]

    readMillisecondTo(ref<int> aRetVal)
    [
        int val := theMillisecond.
        
        aRetVal value := val.
    ]

    writeYear(int aValue)
    [
        short n := aValue.

        theYear := n.
    ]

    writeMonth(int aValue)
    [ 
        short n := aValue.
            
        theMonth := n.
    ]

    writeDay(int aValue)
    [ 
        short n := aValue.
            
        theDay := n.
    ]

    writeHour(int aValue)
    [ 
        short n := aValue.
            
        theHour := n.
    ]

    writeMinute(int aValue)
    [ 
        short n := aValue.
            
        theMinute := n.
    ]

    writeSecond(int aValue)
    [ 
        short n := aValue.
            
        theSecond := n.
    ]

    writeMillisecond(int aValue)
    [ 
        short n := aValue.
            
        theMillisecond := n.
    ]

    read(ref<SystemTime> aRetVal)
    [
        T<SystemTime> value := 0l.        
        system'external'KERNEL32 SystemTimeToFileTime(self, value).
        
        aRetVal value := value.
    ]    
    
    write(SystemTime aFileTime)
    [
        system'external'KERNEL32 FileTimeToSystemTime(aFileTime, self).
    ]
      
    private wide dateToWide(int Flags)
    [
        T<short,127> aBuffer.

        int aLength := system'external'KERNEL32 GetDateFormatW(
            0, Flags,self,0,T<ShortArray>(aBuffer),255,0).
            
        if (aLength == 0)
            [ FormatException new:"Invalid date"; raise ].

        ^ WideLiteralValue fromShortArray(0, aLength - 1, T<ShortArray>(aBuffer)).
    ]
    
    private wide timeToWide(int Flags)
    [
        T<short,127> aBuffer.

        int aLength := system'external'KERNEL32 GetTimeFormatW(
            0,Flags,self,0,T<ShortArray>(aBuffer),255,0).
            
        if (aLength == 0)
            [ FormatException new:"Invalid time"; raise ].

        ^ WideLiteralValue fromShortArray(0, aLength - 1, T<ShortArray>(aBuffer)).
    ]

    wide toShortDate
        = self dateToWide(DATE_SHORTDATE).

    wide toShortTime
        = self timeToWide(0).
}
