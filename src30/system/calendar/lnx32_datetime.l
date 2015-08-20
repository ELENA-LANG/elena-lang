#import system.

#subject(class:system'calendar'SystemTime)    __time.
#subject(class:system'calendar'SystemTimeVar) __vtime.
#subject(class:system'calendar'TimeStruct)    __time_st.

// --- SystemTime ---

#symbol $factory =
{
    #method(type:__time) utcNow
    [
        #var(type:__time)time.
                      
        system'external'libc gettimeofday 
            &__time:time
            &int:0.

        #var(type:__time_st)tm.
        tm writeUTC &__time:time.

        tm read &__vtime:time.
        
        ^ SystemTime new &__time:time.
    ]
    
    #method(type:__time) now
    [
        #var(type:__time)time.
                      
        system'external'libc gettimeofday 
            &__time:time
            &int:0.
        
        ^ SystemTime new &__time:time.
    ]
}.

#class(sealed,struct:embeddable,const) SystemTime :: BaseValue
{
    #field(type:int)tv_sec.
    #field(type:int)tv_usec.

    #constructor(stacksafe,embeddable) $new &int:sec &int:usec
        => system'internal'long_copystruct.

    #constructor(stacksafe,embeddable) $new &long:nanosec
    [
        #var(type:long)secs := nanosec / 1000000000l.
        
        #var(type:long)rem := nanosec - secs * 1000000000l.
        rem := rem / 1000l.
                        
        #var(type:int)tmp.
        intConvertor convert &long:secs &vint:tmp.
        tv_sec := tmp.
        
        intConvertor convert &long:rem &vint:tmp.
        tv_usec := tmp.        
    ]

    #constructor new &__time:aValue
        => system'internal'long_copy.

    #constructor now
        <= new &__time:($factory now).

    #constructor utcNow
        <= new &__time:($factory utcNow).
                        
    #method(stacksafe,type:bool) equal &__time:anOperand
        => system'internal'long_equal.
        
    #method(stacksafe,type:bool) less &__time:anOperand
        => system'internal'long_less.
    
    #method(stacksafe) read &vlong:aRetVal
    [
        #var(type:long)l.
        longConvertor convert &int:tv_sec &vlong:l.
        
        l := l * 1000000000l.
        
        #var(type:long)l2.
        longConvertor convert &int:tv_usec &vlong:l2.
        l2 := l2 * 1000l.
        
        l := l + l2.
        
        aRetVal << l.
    ]
        
    #method(stacksafe) read &vint:aRetVal
        => system'internal'int_save.
        
    #method(stacksafe) readMSecs &vint:aRetVal
        => system'internal'long_savehi.
        
    #method(stacksafe,type:__time) subtract &__time:anOperand
        => system'internal'long_sub.
        
    #method(stacksafe,type:__time) add &__time:anOperand
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
        aVariable write &__time:theValue.
    ]
}

// --- TimeStruct ---

#class(struct:embeddable,sealed) TimeStruct
{
    #field(type:int) tm_sec.         /* seconds */
    #field(type:int) tm_min.         /* minutes */
    #field(type:int) tm_hour.        /* hours */
    #field(type:int) tm_mday.        /* day of the month */
    #field(type:int) tm_mon.         /* month */
    #field(type:int) tm_year.        /* year */
    #field(type:int) tm_wday.        /* day of the week */
    #field(type:int) tm_yday.        /* day in the year */
    #field(type:int) tm_isdst.       /* daylight saving time */

    #field(type:int) tmp1.
    #field(type:int) tmp2.
    
    #field(type:int) tm_usec.
    
    #constructor new
        => system'internal'dlong_empty.
    
    #constructor new &__time:aTime
    [
        $self write &__time:aTime.
    ]

    #method __time_st = $self.

    #method(stacksafe) readYear &vint:aRetVal
    [
        #var(type:int)aYear := tm_year.
        aYear := aYear + 1900.

        aRetVal << aYear.
    ]

    #method(stacksafe) readMonth &vint:aRetVal
    [
        aRetVal << (tm_mon + 1).
    ]

    #method(stacksafe) readDayOfWeek &vint:aRetVal
    [
        aRetVal << tm_wday.
    ]
    
    #method(stacksafe) readDay &vint:aRetVal
    [
        aRetVal << tm_mday.
    ]

    #method(stacksafe) readHour &vint:aRetVal
    [
        aRetVal << tm_hour.
    ]

    #method(stacksafe) readMinute &vint:aRetVal
    [
        aRetVal << tm_min.
    ]

    #method(stacksafe) readSecond &vint:aRetVal
    [
        aRetVal << tm_sec.
    ]

    #method(stacksafe) readMillisecond &vint:aRetVal
    [
        #var(type:int)n := tm_usec.
        n := n / 1000. 

        aRetVal << n.
    ]

    #method(stacksafe) writeYear &int:aValue
    [ 
        tm_year := aValue - 1900.
    ]
    
    #method(stacksafe) writeMonth &int:aValue 
    [ 
        tm_mon := aValue - 1.
    ]

    #method(stacksafe) writeDay &int:aValue 
    [ 
        tm_mday := aValue.
    ]

    #method(stacksafe) writeHour &int:aValue 
    [ 
        tm_hour := aValue.
    ]

    #method(stacksafe) writeMinute &int:aValue 
    [ 
        tm_min := aValue.
    ]

    #method(stacksafe) writeSecond &int:aValue 
    [ 
        tm_sec := aValue.
    ]

    #method(stacksafe) writeMillisecond &int:aValue 
    [ 
        #var(type:int)n := aValue.
        n := n * 1000. 

        tm_usec := n.
    ]
    
    #method(stacksafe) read &__vtime:aRetVal
    [
       #var(type:int)tm_sec := system'external'libc mktime &__time_st:$self.
       #var(type:__time)tm := SystemTime $new &int:tm_sec &int:tm_usec.

       aRetVal write &__time:tm. 
    ]    
    
    #method(stacksafe) write &__time:aTime
    [
       #var(type:dirty_ptr)ptr := system'external'libc localtime &__time:aTime.

       system'external'libc memcpy &__time_st:$self &int:ptr &int:36.

       #var(type:int)tmp.     
       aTime readMSecs &vint:tmp.
       tm_usec := tmp.
    ]
    
    #method(stacksafe) writeUTC &__time:aTime                                     
    [
       #var(type:int)ptr := system'external'libc gmtime &__time:aTime.

       system'external'libc memcpy &__time_st:$self &int:ptr &int:36.

       #var(type:int)tmp.     
       aTime readMSecs &vint:tmp.
       tm_usec := tmp.
    ]
     
    #method(stacksafe) toShortDate
    [
        #var(type:int)aYear.
        #var(type:int)aMonth.
        $self readYear &vint:aYear.
        $self readMonth &vint:aMonth. 

        ^ tm_mday literal + "." + aMonth literal + "." + aYear literal.
    ]

    #method(stacksafe) toShortTime
    [
        ^ tm_hour literal + ":" + tm_min literal + ":" + tm_sec literal.
    ]
}
