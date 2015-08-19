#import system.

#subject(class:system'calendar'SystemTime)    __time.
#subject(class:system'calendar'SystemTimeVar) __vtime.
#subject(class:system'calendar'TimeStruct)    __time_st.

#symbol(const,type:int) DATE_SHORTDATE  = 00000001h.
#symbol(const,type:int) DATE_LONGDATE   = 00000002h.

// --- SystemTime ---

#symbol $factory =
{
    #method(type:__time) utcNow
    [
        #var(type:__time)aTime.
        
        system'external'KERNEL32 GetSystemTimeAsFileTime &__vtime:aTime.
        
        ^ SystemTime new &__time:aTime.
    ]
    
    #method(type:__time) now
    [
        #var(type:__time)aTime.
        #var(type:__time)aLocalTime.
        
        system'external'KERNEL32 GetSystemTimeAsFileTime &__vtime:aTime.
        
        system'external'KERNEL32 FileTimeToLocalFileTime &__time:aTime &__vtime:aLocalTime.
        
        ^ SystemTime new &__time:aLocalTime.
    ]
}.

#class(sealed,integer:8) SystemTime :: BaseValue
{
    #constructor(stacksafe) new &__time:aValue
        => system'internal'long_copy.

    #constructor(stacksafe) $new &long:aValue
        => system'internal'long_copy.

    #constructor now
        <= new &__time:($factory now).

    #constructor utcNow
        <= new &__time:($factory utcNow).
                        
    #method(type:bool) equal &__time:anOperand
        => system'internal'long_equal.
        
    #method(type:bool) less &__time:anOperand
        => system'internal'long_less.
    
    #method(stacksafe) read &vlong:aRetVal
        => system'internal'long_save.
        
    #method(type:__time,stacksafe) subtract &__time:anOperand
        => system'internal'long_sub.
        
    #method(type:__time,stacksafe) add &__time:anOperand
        => system'internal'long_add.
    
    #method __time = $self.
}

#class(sealed,struct:embeddable) SystemTimeVar
{
    #field(type:__time)theValue.
    
    #method __time = SystemTime new &__time:theValue.
    
    #method __vtime = $self.
    
    #method(stacksafe) write &__time:aValue
    [
        theValue := aValue.
    ]
    
    #method(stacksafe) read &__vtime:aVariable
    [
        aVariable << theValue.
    ]
}

// --- TimeStruct ---

#class(struct:embeddable,sealed) TimeStruct
{
    #field(type:short)theYear.
    #field(type:short)theMonth.
    #field(type:short)theDayOfWeek.
    #field(type:short)theDay.
    #field(type:short)theHour.
    #field(type:short)theMinute.
    #field(type:short)theSecond.
    #field(type:short)theMillisecond.
    
    #constructor new
        => system'internal'dlong_empty.

    #constructor new &__time:aTime
    [
        $self write &__time:aTime.
    ]
        
    #method __time_st = $self.
    
    #method(stacksafe) year = IntNumber new &short:theYear.

    #method(stacksafe) month = IntNumber new &short:theMonth.

    #method(stacksafe) dayOfWeek = IntNumber new &short:theDayOfWeek.

    #method(stacksafe) day = IntNumber new &short:theDay.

    #method(stacksafe) hour = IntNumber new &short:theHour.

    #method(stacksafe) minute = IntNumber new &short:theMinute.

    #method(stacksafe) second = IntNumber new &short:theSecond.

    #method(stacksafe) millisecond = IntNumber new &short:theMillisecond.

    #method set &year:aValue
        <= writeYear &int:(aValue int).

    #method set &month:aValue 
        <= writeMonth &int:(aValue int).

    #method(stacksafe) set &day:aValue 
        <= writeDay &int:(aValue int).

    #method(stacksafe) set &hour:aValue 
        <= writeHour &int:(aValue int).

    #method(stacksafe) set &minute:aValue 
        <= writeMinute &int:(aValue int).

    #method(stacksafe) set &second:aValue 
        <= writeSecond &int:(aValue int).

    #method(stacksafe) set &millisecond:aValue 
        <= writeMillisecond &int:(aValue int).

    #method(stacksafe) readYear &vint:aRetVal
    [
        aRetVal write &short:theYear.
    ]

    #method(stacksafe) readMonth &vint:aRetVal
    [
        aRetVal write &short:theMonth.
    ]

    #method(stacksafe) readDayOfWeek &vint:aRetVal
    [
        aRetVal write &short:theDayOfWeek.
    ]

    #method(stacksafe) readDay &vint:aRetVal
    [
        aRetVal write &short:theDay.
    ]

    #method(stacksafe) readHour &vint:aRetVal
    [
        aRetVal write &short:theHour.
    ]

    #method(stacksafe) readMinute &vint:aRetVal
    [
        aRetVal write &short:theMinute.
    ]

    #method(stacksafe) readSecond &vint:aRetVal
    [
        aRetVal write &short:theSecond.
    ]

    #method(stacksafe) readMillisecond &vint:aRetVal
    [
        aRetVal write &short:theMillisecond.
    ]

    #method(stacksafe) writeYear &int:aValue
    [ 
        #var(type:short)n.
        aValue readShort &vshort:n.
            
        theYear := n.
    ]

    #method(stacksafe) writeMonth &int:aValue 
    [ 
        #var(type:short)n.
        aValue readShort &vshort:n.
            
        theMonth := n.
    ]

    #method(stacksafe) writeDay &int:aValue 
    [ 
        #var(type:short)n.
        aValue readShort &vshort:n.
            
        theDay := n.
    ]

    #method(stacksafe) writeHour &int:aValue 
    [ 
        #var(type:short)n.
        aValue readShort &vshort:n.
            
        theHour := n.
    ]

    #method(stacksafe) writeMinute &int:aValue 
    [ 
        #var(type:short)n.
        aValue readShort &vshort:n.
            
        theMinute := n.
    ]

    #method(stacksafe) writeSecond &int:aValue 
    [ 
        #var(type:short)n.
        aValue readShort &vshort:n.
            
        theSecond := n.
    ]

    #method(stacksafe) writeMillisecond &int:aValue 
    [ 
        #var(type:short)n.
        aValue readShort &vshort:n.
            
        theMillisecond := n.
    ]

    #method(stacksafe) read &__vtime:aRetVal
    [
        system'external'KERNEL32 SystemTimeToFileTime &__time_st:$self &__vtime:aRetVal.
    ]    
    
    #method(stacksafe) write &__time:aFileTime
    [
        system'external'KERNEL32 FileTimeToSystemTime &__time:aFileTime &__time_st:$self.
    ]
      
    #method(stacksafe) toWide &date_attributes:Flags
    [
        #var(type:shortarray,size:127)aBuffer.

        #var(type:int) aLength := system'external'KERNEL32 GetDateFormatEx
            &int:0
            &int:(Flags int)
            &__time_st:$self
            &int:0
            &shortarray:aBuffer
            &int:255
            &int:0
            | raise:(FormatException new:"Invalid date").

        ^ WideLiteralValue $new &int:0 &int:(aLength - 1) &shortarray:aBuffer.                
    ]
    
    #method(stacksafe) toWide &time_attributes:Flags
    [
        #var(type:shortarray,size:127)aBuffer.

        #var(type:int) aLength := system'external'KERNEL32 GetTimeFormatEx
            &int:0
            &int:(Flags int)
            &__time_st:$self
            &int:0
            &shortarray:aBuffer
            &int:255
            &int:0
            | raise:(FormatException new:"Invalid time").

        ^ WideLiteralValue $new &int:0 &int:(aLength - 1) &shortarray:aBuffer.                
    ]

    #method(stacksafe) toShortDate
        <= %toWide &date_attributes:DATE_SHORTDATE.

    #method(stacksafe) toShortTime
        <= %toWide &time_attributes:0.
}
