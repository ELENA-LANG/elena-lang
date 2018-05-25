const int DATE_SHORTDATE  = 00000001h.
const int DATE_LONGDATE   = 00000002h.

// --- SystemTime ---

singleton systemTimeFactory
{
    stacksafe readUtcNow(ref<SystemTime> retVal)
    [
        T<SystemTime> aTime := 0l.        
        system'external'KERNEL32 GetSystemTimeAsFileTime(aTime).
        
        retVal value := aTime.
    ]
    
    stacksafe readNow(ref<SystemTime> retVal)
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

    cast eval(long value)
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

    cast eval(SystemTime aTime)
    [
        self write(aTime).
    ]

    embeddable constructor new
    [
        system'core_routines'__timest empty(self).
    ]

    stacksafe constructor new(SystemTime aTime)
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

    stacksafe readYearTo(ref<int> aRetVal)
    [
        int val := theYear.
        
        aRetVal value := val.
    ]

    stacksafe readMonthTo(ref<int> aRetVal)
    [
        int val := theMonth.
        
        aRetVal value := val.
    ]

    stacksafe readDayOfWeekTo(ref<int> aRetVal)
    [
        int val := theDayOfWeek.
        
        aRetVal value := val.
    ]

    stacksafe readDayTo(ref<int> aRetVal)
    [
        int val := theDay.
        
        aRetVal value := val.
    ]

    stacksafe readHourTo(ref<int> aRetVal)
    [
        int val := theHour.
        
        aRetVal value := val.
    ]

    stacksafe readMinuteTo(ref<int> aRetVal)
    [
        int val := theMinute.
        
        aRetVal value := val.
    ]

    stacksafe readSecondTo(ref<int> aRetVal)
    [
        int val := theSecond.
        
        aRetVal value := val.
    ]

    stacksafe readMillisecondTo(ref<int> aRetVal)
    [
        int val := theMillisecond.
        
        aRetVal value := val.
    ]

    stacksafe writeYear(int aValue)
    [
        short n := aValue.

        theYear := n.
    ]

    stacksafe writeMonth(int aValue)
    [ 
        short n := aValue.
            
        theMonth := n.
    ]

    stacksafe writeDay(int aValue)
    [ 
        short n := aValue.
            
        theDay := n.
    ]

    stacksafe writeHour(int aValue)
    [ 
        short n := aValue.
            
        theHour := n.
    ]

    stacksafe writeMinute(int aValue)
    [ 
        short n := aValue.
            
        theMinute := n.
    ]

    stacksafe writeSecond(int aValue)
    [ 
        short n := aValue.
            
        theSecond := n.
    ]

    stacksafe writeMillisecond(int aValue)
    [ 
        short n := aValue.
            
        theMillisecond := n.
    ]

    stacksafe read(ref<SystemTime> aRetVal)
    [
        T<SystemTime> value := 0l.        
        system'external'KERNEL32 SystemTimeToFileTime(self, value).
        
        aRetVal value := value.
    ]    
    
    stacksafe write(SystemTime aFileTime)
    [
        system'external'KERNEL32 FileTimeToSystemTime(aFileTime, self).
    ]
      
    private stacksafe wide dateToWide(int Flags)
    [
        T<short,127> aBuffer.

        int aLength := system'external'KERNEL32 GetDateFormatW(
            0, Flags,self,0,T<ShortArray>(aBuffer),255,0).
            
        if (aLength == 0)
            [ FormatException new:"Invalid date"; raise ].

        ^ WideLiteralValue fromShortArray(0, aLength - 1, T<ShortArray>(aBuffer)).
    ]
    
    private stacksafe wide timeToWide(int Flags)
    [
        T<short,127> aBuffer.

        int aLength := system'external'KERNEL32 GetTimeFormatW(
            0,Flags,self,0,T<ShortArray>(aBuffer),255,0).
            
        if (aLength == 0)
            [ FormatException new:"Invalid time"; raise ].

        ^ WideLiteralValue fromShortArray(0, aLength - 1, T<ShortArray>(aBuffer)).
    ]

    stacksafe wide toShortDate
        = self dateToWide(DATE_SHORTDATE).

    stacksafe wide toShortTime
        = self timeToWide(0).
}
