type __time    :: system'calendar'SystemTime.
type __time_st :: system'calendar'TimeStruct.

const int DATE_SHORTDATE  = 00000001h.
const int DATE_LONGDATE   = 00000002h.

// --- SystemTime ---

class $systemTimeFactory =
{
    stacksafe readUtcNow __vtime:retVal
    [
        __time aTime := 0l.        
        system'external'KERNEL32 GetSystemTimeAsFileTime __time:aTime.
        
        retVal __time := aTime.
    ]
    
    stacksafe readNow __vtime:retVal
    [
        __time aTime := 0l.
        __time aLocalTime := 0l.
        
        system'external'KERNEL32 GetSystemTimeAsFileTime __time:aTime.

        system'external'KERNEL32 FileTimeToLocalFileTime __time:aTime __time:aLocalTime.
        
        retVal __time := aLocalTime.
    ]
}.

sealed const embeddable struct SystemTime :: BaseValue
{
    __int64 theValue.

    stacksafe implicit long:value
    [
        theValue := value.
    ]
            
    stacksafe constructor new __time:value
        = value.

    stacksafe bool equal __time:anOperand
        = theValue == anOperand.

    stacksafe bool less __time:anOperand
        = theValue < anOperand.

    //long :: outlprop(readValueTo).
    stacksafe embeddable long
    [
        long val := 0.        
        $self readValueTo vlong:val.
        
        ^ val.
    ]    

    stacksafe readValueTo vlong:retVal
    [
        retVal long := theValue.
    ]

    stacksafe __time subtract __time:anOperand
    [
        __time result := theValue - anOperand.
        
        ^ SystemTime new __time:result.
    ]

    stacksafe __time add __time:anOperand
    [
        __time result := theValue + anOperand.
        
        ^ SystemTime new __time:result.
    ]
}

// --- TimeStruct ---

sealed embeddable struct TimeStruct
{
    short theYear.
    short theMonth.
    short theDayOfWeek.
    short theDay.
    short theHour.
    short theMinute.
    short theSecond.
    short theMillisecond.

    stacksafe implicit __time:aTime
    [
        $self write __time:aTime.
    ]

    constructor new
    [
        system'core_routines'__timest empty __time_st:$self.
    ]

    stacksafe constructor new __time:aTime
    [
        $self write __time:aTime.
    ]

    int year        :: nprop(readYearTo,writeYear).    
    int month       :: nprop(readMonthTo,writeMonth).    
    int dayOfWeek   :: nprop(readDayOfWeekTo,writeDayOfWeek).
    int day         :: nprop(readDayTo,writeDay).    
    int hour        :: nprop(readHourTo,writeHour).    
    int minute      :: nprop(readMinuteTo,writeMinute).
    int second      :: nprop(readSecondTo,writeSecond).
    int millisecond :: nprop(readMillisecondTo,writeMillisecond).

    stacksafe readYearTo vint:aRetVal
    [
        int val := theYear.
        
        aRetVal int := val.
    ]

    stacksafe readMonthTo vint:aRetVal
    [
        int val := theMonth.
        
        aRetVal int := val.
    ]

    stacksafe readDayOfWeekTo vint:aRetVal
    [
        int val := theDayOfWeek.
        
        aRetVal int := val.
    ]

    stacksafe readDayTo vint:aRetVal
    [
        int val := theDay.
        
        aRetVal int := val.
    ]

    stacksafe readHourTo vint:aRetVal
    [
        int val := theHour.
        
        aRetVal int := val.
    ]

    stacksafe readMinuteTo vint:aRetVal
    [
        int val := theMinute.
        
        aRetVal int := val.
    ]

    stacksafe readSecondTo vint:aRetVal
    [
        int val := theSecond.
        
        aRetVal int := val.
    ]

    stacksafe readMillisecondTo vint:aRetVal
    [
        int val := theMillisecond.
        
        aRetVal int := val.
    ]

    stacksafe writeYear int:aValue
    [
        short n := aValue.

        theYear := n.
    ]

    stacksafe writeMonth int:aValue 
    [ 
        short n := aValue.
            
        theMonth := n.
    ]

    stacksafe writeDay int:aValue 
    [ 
        short n := aValue.
            
        theDay := n.
    ]

    stacksafe writeHour int:aValue 
    [ 
        short n := aValue.
            
        theHour := n.
    ]

    stacksafe writeMinute int:aValue 
    [ 
        short n := aValue.
            
        theMinute := n.
    ]

    stacksafe writeSecond int:aValue 
    [ 
        short n := aValue.
            
        theSecond := n.
    ]

    stacksafe writeMillisecond int:aValue 
    [ 
        short n := aValue.
            
        theMillisecond := n.
    ]

    stacksafe read __vtime:aRetVal
    [
        system'external'KERNEL32 SystemTimeToFileTime __time_st:$self __time:aRetVal.
    ]    
    
    stacksafe write __time:aFileTime
    [
        system'external'KERNEL32 FileTimeToSystemTime __time:aFileTime __time_st:$self.
    ]
      
    stacksafe toWide date_attributes:Flags
    [
        short aBuffer(127).

        int aLength := system'external'KERNEL32 GetDateFormatW
            int:0
            int:(Flags int)
            __time_st:$self
            int:0
            shortarray:aBuffer
            int:255
            int:0.
            
        if (aLength == 0)
            [ FormatException new:"Invalid date"; raise ].

        ^ WideLiteralValue $new int:0 int:(aLength - 1) shortarray:aBuffer.                
    ]
    
    stacksafe toWide time_attributes:Flags
    [
        short aBuffer(127).

        int aLength := system'external'KERNEL32 GetTimeFormatW
            int:0
            int:(Flags int)
            __time_st:$self
            int:0
            shortarray:aBuffer
            int:255
            int:0.
            
        if (aLength == 0)
            [ FormatException new:"Invalid time"; raise ].

        ^ WideLiteralValue $new int:0 int:(aLength - 1) shortarray:aBuffer.                
    ]

    stacksafe toShortDate
        <= toWide date_attributes:DATE_SHORTDATE.

    stacksafe toShortTime
        <= toWide time_attributes:0.
}

type __vtime   :: varof(__time).


