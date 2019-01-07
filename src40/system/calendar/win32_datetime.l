const int DATE_SHORTDATE  = 00000001h;
const int DATE_LONGDATE   = 00000002h;

// --- SystemTime ---

public sealed const struct SystemTime : BaseValue
{
    embeddable __int theValue[8];

    static SystemTime Default
        = 0l;

    constructor(long value)
    {
        theValue := value
    }
         
    constructor(SystemTime value)
        = value;

    bool equal(SystemTime operand)
        = theValue == operand;

    bool less(SystemTime operand)
        = theValue < operand;

    long cast() = theValue;

    vprop long Value
    {
        read(ref long retVal)
        {
            retVal := theValue
        }        
    }

    SystemTime subtract(SystemTime operand)
    {
        SystemTime result := theValue - operand;
        
        ^ new SystemTime(result)
    }

//    T<SystemTime> add(SystemTime anOperand)
//    [
//        T<SystemTime> result := theValue + anOperand.
//        
//        ^ SystemTime new(result).
//    ]
}

// --- TimeStruct ---

public sealed struct TimeStruct
{
    short theYear;
    short theMonth;
    short theDayOfWeek;
    short theDay;
    short theHour;
    short theMinute;
    short theSecond;
    short theMillisecond;

    constructor(SystemTime time)
    {
        self.init(time)
    }

//    embeddable constructor new
//    [
//        system'core_routines'__timest empty(self).
//    ]
//
//    constructor new(SystemTime aTime)
//    [
//        self write(aTime).
//    ]
//
//    int vprop  year        :: readYearTo,writeYear.
//    int vprop  month       :: readMonthTo,writeMonth.
//    int rvprop dayOfWeek   :: readDayOfWeekTo.
//    int vprop  day         :: readDayTo,writeDay.
//    int vprop  hour        :: readHourTo,writeHour.
//    int vprop  minute      :: readMinuteTo,writeMinute.
//    int vprop  second      :: readSecondTo,writeSecond.
//    int vprop  millisecond :: readMillisecondTo,writeMillisecond.
//
//    readYearTo(ref<int> aRetVal)
//    [
//        int val := theYear.
//        
//        aRetVal value := val.
//    ]
//
//    readMonthTo(ref<int> aRetVal)
//    [
//        int val := theMonth.
//        
//        aRetVal value := val.
//    ]
//
//    readDayOfWeekTo(ref<int> aRetVal)
//    [
//        int val := theDayOfWeek.
//        
//        aRetVal value := val.
//    ]
//
//    readDayTo(ref<int> aRetVal)
//    [
//        int val := theDay.
//        
//        aRetVal value := val.
//    ]
//
//    readHourTo(ref<int> aRetVal)
//    [
//        int val := theHour.
//        
//        aRetVal value := val.
//    ]
//
//    readMinuteTo(ref<int> aRetVal)
//    [
//        int val := theMinute.
//        
//        aRetVal value := val.
//    ]
//
//    readSecondTo(ref<int> aRetVal)
//    [
//        int val := theSecond.
//        
//        aRetVal value := val.
//    ]
//
//    readMillisecondTo(ref<int> aRetVal)
//    [
//        int val := theMillisecond.
//        
//        aRetVal value := val.
//    ]
//
//    writeYear(int aValue)
//    [
//        short n := aValue.
//
//        theYear := n.
//    ]
//
//    writeMonth(int aValue)
//    [ 
//        short n := aValue.
//            
//        theMonth := n.
//    ]
//
//    writeDay(int aValue)
//    [ 
//        short n := aValue.
//            
//        theDay := n.
//    ]
//
//    writeHour(int aValue)
//    [ 
//        short n := aValue.
//            
//        theHour := n.
//    ]
//
//    writeMinute(int aValue)
//    [ 
//        short n := aValue.
//            
//        theMinute := n.
//    ]
//
//    writeSecond(int aValue)
//    [ 
//        short n := aValue.
//            
//        theSecond := n.
//    ]
//
//    writeMillisecond(int aValue)
//    [ 
//        short n := aValue.
//            
//        theMillisecond := n.
//    ]
//
//    read(ref<SystemTime> aRetVal)
//    [
//        T<SystemTime> value := 0l.        
//        system'external'KERNEL32 SystemTimeToFileTime(self, value).
//        
//        aRetVal value := value.
//    ]    
    
    private init(SystemTime fileTime)
    {
        extern KERNEL32.FileTimeToSystemTime(fileTime, self)
    }
      
    private wide dateToWide(int flags)
    {
        short buffer[127];

        int length := extern KERNEL32.GetDateFormatW(
            0, flags,self,0,buffer,255,0);
            
        if (length == 0)
            { FormatException.new:"Invalid date".raise() };

        ^ WideString.fromShortArray(0, length - 1, buffer)
    }
    
    private wide timeToWide(int flags)
    {
        short buffer[127];

        int length := extern KERNEL32.GetTimeFormatW(
            0,flags,self,0,buffer,255,0);
            
        if (length == 0)
            { FormatException.new:"Invalid time".raise() };

        ^ WideString.fromShortArray(0, length - 1, buffer)
    }

    wide toShortDate()
        = self.dateToWide(DATE_SHORTDATE);

    wide toShortTime()
        = self.timeToWide(0);
}

public singleton systemTimeFactory
{
//    readUtcNow(ref<SystemTime> retVal)
//    [
//        T<SystemTime> aTime := 0l.        
//        system'external'KERNEL32 GetSystemTimeAsFileTime(aTime).
//        
//        retVal value := aTime.
//    ]
    
    readNow(ref SystemTime retVal)
    {
        SystemTime time := 0l;
        SystemTime localTime := 0l;
        
        extern KERNEL32.GetSystemTimeAsFileTime(time);

        extern KERNEL32.FileTimeToLocalFileTime(time, localTime);
        
        retVal := localTime
    }
}