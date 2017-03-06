#import system.

#subject __time    = system'calendar'SystemTime.
#subject __time_st = system'calendar'TimeStruct.

const int DATE_SHORTDATE  = 00000001h.
const int DATE_LONGDATE   = 00000002h.

// --- SystemTime ---

class $systemTimeFactory =
{
    stacksafe readUtcNow &__vtime:retVal
    [
        __time aTime := 0l.        
        system'external'KERNEL32 GetSystemTimeAsFileTime &__time:aTime.
        
        retVal := aTime.
    ]
    
    stacksafe readNow &__vtime:retVal
    [
        __time aTime := 0l.
        __time aLocalTime := 0l.
        
        system'external'KERNEL32 GetSystemTimeAsFileTime &__time:aTime.

        system'external'KERNEL32 FileTimeToLocalFileTime &__time:aTime &__time:aLocalTime.
        
        retVal := aLocalTime.
    ]
}.

sealed const embeddable struct SystemTime :: BaseValue
{
    __int64 theValue.

    sealed stacksafe conversion long:value
    [
        theValue := value.
    ]
            
    stacksafe constructor new &__time:value
        = value.

    stacksafe bool equal &__time:anOperand
        = theValue == anOperand.

    stacksafe bool less &__time:anOperand
        = theValue < anOperand.

    stacksafe outlprop(long) readValue &vlong:retVal
    [
        retVal := theValue.
    ]

    stacksafe __time subtract &__time:anOperand
    [
        __time result := theValue - anOperand.
        
        ^ SystemTime new &__time:result.
    ]

    stacksafe __time add &__time:anOperand
    [
        __time result := theValue + anOperand.
        
        ^ SystemTime new &__time:result.
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

    sealed stacksafe conversion __time:aTime
    [
        $self write &__time:aTime.
    ]

    constructor new
    [
        system'core_routines'__timest empty &__time_st:$self.
    ]

    stacksafe constructor new &__time:aTime
    [
        $self write &__time:aTime.
    ]

    stacksafe outnprop(year) readYear &vint:aRetVal
    [
        int val := theYear.
        
        aRetVal := val.
    ]

    stacksafe outnprop(month) readMonth &vint:aRetVal
    [
        int val := theMonth.
        
        aRetVal := val.
    ]

    stacksafe outnprop(dayOfWeek) readDayOfWeek &vint:aRetVal
    [
        int val := theDayOfWeek.
        
        aRetVal := val.
    ]

    stacksafe outnprop(day) readDay &vint:aRetVal
    [
        int val := theDay.
        
        aRetVal := val.
    ]

    stacksafe outnprop(hour) readHour &vint:aRetVal
    [
        int val := theHour.
        
        aRetVal := val.
    ]

    stacksafe outnprop(minute) readMinute &vint:aRetVal
    [
        int val := theMinute.
        
        aRetVal := val.
    ]

    stacksafe outnprop(second) readSecond &vint:aRetVal
    [
        int val := theSecond.
        
        aRetVal := val.
    ]

    stacksafe outnprop(millisecond) readMillisecond &vint:aRetVal
    [
        int val := theMillisecond.
        
        aRetVal := val.
    ]

    stacksafe innprop(year) writeYear &int:aValue
    [
        short n := aValue.

        theYear := n.
    ]

    stacksafe innprop(month) writeMonth &int:aValue 
    [ 
        short n := aValue.
            
        theMonth := n.
    ]

    stacksafe innprop(day) writeDay &int:aValue 
    [ 
        short n := aValue.
            
        theDay := n.
    ]

    stacksafe innprop(hour) writeHour &int:aValue 
    [ 
        short n := aValue.
            
        theHour := n.
    ]

    stacksafe innprop(minute) writeMinute &int:aValue 
    [ 
        short n := aValue.
            
        theMinute := n.
    ]

    stacksafe innprop(second) writeSecond &int:aValue 
    [ 
        short n := aValue.
            
        theSecond := n.
    ]

    stacksafe innprop(millisecond) writeMillisecond &int:aValue 
    [ 
        short n := aValue.
            
        theMillisecond := n.
    ]

//    stacksafe read &__vtime:aRetVal
//    [
//        system'external'KERNEL32 SystemTimeToFileTime &__time_st:$self &__time:aRetVal.
//    ]    
    
    stacksafe write &__time:aFileTime
    [
        system'external'KERNEL32 FileTimeToSystemTime &__time:aFileTime &__time_st:$self.
    ]
      
    stacksafe toWide &date_attributes:Flags
    [
        short(127) aBuffer := 0.

        int aLength := system'external'KERNEL32 GetDateFormatW
            &int:0
            &int:(Flags int)
            &__time_st:$self
            &int:0
            &shortarray:aBuffer
            &int:255
            &int:0.
            
        (aLength == 0)
            ? [ FormatException new:"Invalid date" raise. ].

        ^ WideLiteralValue $new &int:0 &int:(aLength - 1) &shortarray:aBuffer.                
    ]
    
    stacksafe toWide &time_attributes:Flags
    [
        short(127) aBuffer := 0.

        int aLength := system'external'KERNEL32 GetTimeFormatW
            &int:0
            &int:(Flags int)
            &__time_st:$self
            &int:0
            &shortarray:aBuffer
            &int:255
            &int:0.
            
        (aLength == 0)
            ? [ FormatException new:"Invalid time" raise. ].

        ^ WideLiteralValue $new &int:0 &int:(aLength - 1) &shortarray:aBuffer.                
    ]

    stacksafe toShortDate
        <= toWide &date_attributes:DATE_SHORTDATE.

    stacksafe toShortTime
        <= toWide &time_attributes:0.
}

#subject __vtime   = varof(__time).

