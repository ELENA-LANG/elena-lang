#import system.

#subject __time    = system'calendar'SystemTime.
#subject __time_st = system'calendar'TimeStruct.
#subject __vtime   = varof:__time.

#symbol(const,int) DATE_SHORTDATE  = 00000001h.
#symbol(const,int) DATE_LONGDATE   = 00000002h.

// --- SystemTime ---

#symbol $factory =
{
    #method(__time) utcNow
    [
        #var(__time)aTime.        
        system'external'KERNEL32 GetSystemTimeAsFileTime &__time:aTime.
        
        ^ SystemTime new &__time:aTime.
    ]
    
    #method(__time) now
    [
        #var(__time)aTime.
        #var(__time)aLocalTime.
        
        system'external'KERNEL32 GetSystemTimeAsFileTime &__time:aTime.

        system'external'KERNEL32 FileTimeToLocalFileTime &__time:aTime &__time:aLocalTime.

        ^ SystemTime new &__time:aLocalTime.
    ]
}.

#class(sealed,reference,const) SystemTime :: BaseValue
{
    #field(long)theValue.
    
    #constructor(stacksafe,suppress:w3) new &__time:value
        = value.

    #constructor(stacksafe) $new &long:aValue
    [
        theValue := aValue.
    ]

    #constructor now
        = $factory now.

    #constructor utcNow
        = $factory utcNow.

    #method(bool,stacksafe) equal &__time:anOperand
        = theValue == anOperand.

    #method(bool,stacksafe) less &__time:anOperand
        = theValue < anOperand.

    #method(stacksafe,outlprop:long) readValue &vlong:retVal
    [
        retVal := theValue.
    ]

    #method(__time,stacksafe) subtract &__time:anOperand
    [
        #var(long)val := anOperand long.
        
        ^ SystemTime $new &long:(theValue - val).
    ]

    #method(__time,stacksafe) add &__time:anOperand
    [
        #var(long)val := anOperand long.
        
        ^ SystemTime $new &long:(theValue + val).
    ]
}

// --- TimeStruct ---

#class(embeddable,sealed) TimeStruct
{
    #field(short)theYear.
    #field(short)theMonth.
    #field(short)theDayOfWeek.
    #field(short)theDay.
    #field(short)theHour.
    #field(short)theMinute.
    #field(short)theSecond.
    #field(short)theMillisecond.
    
    #constructor new
    [
        system'core_routines'__timest empty &__time_st:$self.
    ]

    #constructor(stacksafe) new &__time:aTime
    [
        $self write &__time:aTime.
    ]

    #method(stacksafe,outnprop:year) readYear &vint:aRetVal
    [
        #var(int)val := theYear.
        
        aRetVal := val.
    ]

    #method(stacksafe,outnprop:month) readMonth &vint:aRetVal
    [
        #var(int)val := theMonth.
        
        aRetVal := val.
    ]

    #method(stacksafe,outnprop:dayOfWeek) readDayOfWeek &vint:aRetVal
    [
        #var(int)val := theDayOfWeek.
        
        aRetVal := val.
    ]

    #method(stacksafe,outnprop:day) readDay &vint:aRetVal
    [
        #var(int)val := theDay.
        
        aRetVal := val.
    ]

    #method(stacksafe,outnprop:hour) readHour &vint:aRetVal
    [
        #var(int)val := theHour.
        
        aRetVal := val.
    ]

    #method(stacksafe,outnprop:minute) readMinute &vint:aRetVal
    [
        #var(int)val := theMinute.
        
        aRetVal := val.
    ]

    #method(stacksafe,outnprop:second) readSecond &vint:aRetVal
    [
        #var(int)val := theSecond.
        
        aRetVal := val.
    ]

    #method(stacksafe,outnprop:millisecond) readMillisecond &vint:aRetVal
    [
        #var(int)val := theMillisecond.
        
        aRetVal := val.
    ]

    #method(stacksafe,innprop:year) writeYear &int:aValue
    [
        #var(short)n := aValue.

        theYear := n.
    ]

    #method(stacksafe,innprop:month) writeMonth &int:aValue 
    [ 
        #var(short)n := aValue.
            
        theMonth := n.
    ]

    #method(stacksafe,innprop:day) writeDay &int:aValue 
    [ 
        #var(short)n := aValue.
            
        theDay := n.
    ]

    #method(stacksafe,innprop:hour) writeHour &int:aValue 
    [ 
        #var(short)n := aValue.
            
        theHour := n.
    ]

    #method(stacksafe,innprop:minute) writeMinute &int:aValue 
    [ 
        #var(short)n := aValue.
            
        theMinute := n.
    ]

    #method(stacksafe,innprop:second) writeSecond &int:aValue 
    [ 
        #var(short)n := aValue.
            
        theSecond := n.
    ]

    #method(stacksafe,innprop:millisecond) writeMillisecond &int:aValue 
    [ 
        #var(short)n := aValue.
            
        theMillisecond := n.
    ]

    #method(stacksafe) read &__vtime:aRetVal
    [
        system'external'KERNEL32 SystemTimeToFileTime &__time_st:$self &__time:aRetVal.
    ]    
    
    #method(stacksafe) writeSpan &__time:aTime
    [
        $self write &__time:aTime.
    ] 

    #method(stacksafe) write &__time:aFileTime
    [
        system'external'KERNEL32 FileTimeToSystemTime &__time:aFileTime &__time_st:$self.
    ]
      
    #method(stacksafe) toWide &date_attributes:Flags
    [
        #var(short:127)aBuffer.

        #var(int) aLength := system'external'KERNEL32 GetDateFormatW
            &int:0
            &int:(Flags int)
            &__time_st:$self
            &int:0
            &shortarray:aBuffer
            &int:255
            &int:0.
            
        (aLength == 0)
            ? [ #throw FormatException new:"Invalid date". ].

        ^ WideLiteralValue $new &int:0 &int:(aLength - 1) &shortarray:aBuffer.                
    ]
    
    #method(stacksafe) toWide &time_attributes:Flags
    [
        #var(short:127)aBuffer.

        #var(int) aLength := system'external'KERNEL32 GetTimeFormatW
            &int:0
            &int:(Flags int)
            &__time_st:$self
            &int:0
            &shortarray:aBuffer
            &int:255
            &int:0.
            
        (aLength == 0)
            ? [ #throw FormatException new:"Invalid time". ].

        ^ WideLiteralValue $new &int:0 &int:(aLength - 1) &shortarray:aBuffer.                
    ]

    #method(stacksafe) toShortDate
        <= toWide &date_attributes:DATE_SHORTDATE.

    #method(stacksafe) toShortTime
        <= toWide &time_attributes:0.
}
