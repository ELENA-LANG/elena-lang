#import system.

#subject __time    = system'calendar'SystemTime.
#subject __time_st = system'calendar'TimeStruct.

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

#class(sealed,struct:embeddable,const) SystemTime :: BaseValue
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

    #method(bool) equal &__time:anOperand
        = theValue == anOperand.

    #method(bool) less &__time:anOperand
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

#class(struct:embeddable,sealed) TimeStruct
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

//    #method(stacksafe) month = IntNumber new &short:theMonth.
//
//    #method(stacksafe) dayOfWeek = IntNumber new &short:theDayOfWeek.
//
//    #method(stacksafe) day = IntNumber new &short:theDay.
//
//    #method(stacksafe) hour = IntNumber new &short:theHour.
//
//    #method(stacksafe) minute = IntNumber new &short:theMinute.
//
//    #method(stacksafe) second = IntNumber new &short:theSecond.
//
//    #method(stacksafe) millisecond = IntNumber new &short:theMillisecond.
//
//    #method set &year:aValue
//        <= writeYear &int:(aValue int).
//
//    #method set &month:aValue 
//        <= writeMonth &int:(aValue int).
//
//    #method(stacksafe) set &day:aValue 
//        <= writeDay &int:(aValue int).
//
//    #method(stacksafe) set &hour:aValue 
//        <= writeHour &int:(aValue int).
//
//    #method(stacksafe) set &minute:aValue 
//        <= writeMinute &int:(aValue int).
//
//    #method(stacksafe) set &second:aValue 
//        <= writeSecond &int:(aValue int).
//
//    #method(stacksafe) set &millisecond:aValue 
//        <= writeMillisecond &int:(aValue int).

    #method(stacksafe,outnprop:year) readYear &vint:aRetVal
    [
        #var(int)val := theYear.
        
        aRetVal := val.
    ]

//    #method(stacksafe) readMonth &vint:aRetVal
//    [
//        aRetVal write &short:theMonth.
//    ]
//
//    #method(stacksafe) readDayOfWeek &vint:aRetVal
//    [
//        aRetVal write &short:theDayOfWeek.
//    ]
//
//    #method(stacksafe) readDay &vint:aRetVal
//    [
//        aRetVal write &short:theDay.
//    ]
//
//    #method(stacksafe) readHour &vint:aRetVal
//    [
//        aRetVal write &short:theHour.
//    ]
//
//    #method(stacksafe) readMinute &vint:aRetVal
//    [
//        aRetVal write &short:theMinute.
//    ]
//
//    #method(stacksafe) readSecond &vint:aRetVal
//    [
//        aRetVal write &short:theSecond.
//    ]
//
//    #method(stacksafe) readMillisecond &vint:aRetVal
//    [
//        aRetVal write &short:theMillisecond.
//    ]
//
//    #method(stacksafe) writeYear &int:aValue
//    [ 
//        #var(type:short)n.
//        aValue readShort &vshort:n.
//            
//        theYear := n.
//    ]
//
//    #method(stacksafe) writeMonth &int:aValue 
//    [ 
//        #var(type:short)n.
//        aValue readShort &vshort:n.
//            
//        theMonth := n.
//    ]
//
//    #method(stacksafe) writeDay &int:aValue 
//    [ 
//        #var(type:short)n.
//        aValue readShort &vshort:n.
//            
//        theDay := n.
//    ]
//
//    #method(stacksafe) writeHour &int:aValue 
//    [ 
//        #var(type:short)n.
//        aValue readShort &vshort:n.
//            
//        theHour := n.
//    ]
//
//    #method(stacksafe) writeMinute &int:aValue 
//    [ 
//        #var(type:short)n.
//        aValue readShort &vshort:n.
//            
//        theMinute := n.
//    ]
//
//    #method(stacksafe) writeSecond &int:aValue 
//    [ 
//        #var(type:short)n.
//        aValue readShort &vshort:n.
//            
//        theSecond := n.
//    ]
//
//    #method(stacksafe) writeMillisecond &int:aValue 
//    [ 
//        #var(type:short)n.
//        aValue readShort &vshort:n.
//            
//        theMillisecond := n.
//    ]
//
//    #method(stacksafe) read &__time:aRetVal
//    [
//        system'external'KERNEL32 SystemTimeToFileTime &__time_st:$self &__time:aRetVal.
//    ]    
//    
//    #method(stacksafe) writeSpan &__time:aTime
//    [
//        $self write &__time:aTime.
//    ] 

    #method(stacksafe) write &__time:aFileTime
    [
        system'external'KERNEL32 FileTimeToSystemTime &__time:aFileTime &__time_st:$self.
    ]
      
//    #method(stacksafe) toWide &date_attributes:Flags
//    [
//        #var(type:shortarray,size:127)aBuffer.
//
//        #var(type:int) aLength := system'external'KERNEL32 GetDateFormatW
//            &int:0
//            &int:(Flags int)
//            &__time_st:$self
//            &int:0
//            &shortarray:aBuffer
//            &int:255
//            &int:0.
//            
//        (aLength == 0)
//            ? [ #throw FormatException new:"Invalid date". ].
//
//        ^ WideLiteralValue $new &int:0 &int:(aLength - 1) &shortarray:aBuffer.                
//    ]
//    
//    #method(stacksafe) toWide &time_attributes:Flags
//    [
//        #var(type:shortarray,size:127)aBuffer.
//
//        #var(type:int) aLength := system'external'KERNEL32 GetTimeFormatW
//            &int:0
//            &int:(Flags int)
//            &__time_st:$self
//            &int:0
//            &shortarray:aBuffer
//            &int:255
//            &int:0.
//            
//        (aLength == 0)
//            ? [ #throw FormatException new:"Invalid time". ].
//
//        ^ WideLiteralValue $new &int:0 &int:(aLength - 1) &shortarray:aBuffer.                
//    ]
//
//    #method(stacksafe) toShortDate
//        <= %toWide &date_attributes:DATE_SHORTDATE.
//
//    #method(stacksafe) toShortTime
//        <= %toWide &time_attributes:0.
}
