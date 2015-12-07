// ==== ver 3.4.1 ===

#import system.

#subject(class:system'calendar'SystemTime)    __time.
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

        tm read &__time:time.
        
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

#class(sealed,struct:embeddable) SystemTime :: BaseValue
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
                        
    #method(stacksafe) $read &vint:aSeconds &vint:aMicroSeconds 
    [
       aSeconds << tv_sec.
       aMicroSeconds << tv_usec.
    ]

    #method(stacksafe)readMSec &vint:aMicroSeconds 
    [
       aMicroSeconds << tv_usec.
    ]

    #method(stacksafe,type:bool) equal &__time:anOperand
        => system'internal'long_equal.
        
    #method(stacksafe,type:bool) less &__time:anOperand
    [
       #var(type:int)sec.
       #var(type:int)usec.
       anOperand $read &vint:sec &vint:usec.
  
       (sec == tv_sec)?
           [ ^ tv_usec < usec. ].

       ^ tv_sec < sec.
    ]
    
    #method(stacksafe) write &__time:aValue
        => system'internal'long_copy.
    
    #method(stacksafe) read &vlong:aRetVal
    [
        #var(type:long)l.
        longConvertor convert &int:tv_sec &vlong:l.
        
        l := l * 10000000l.
        
        #var(type:long)l2.
        longConvertor convert &int:tv_usec &vlong:l2.
        l2 := l2 * 10l.
        
        l := l + l2.
        
        aRetVal << l.
    ]
        
    #method(stacksafe,type:__time) subtract &__time:anOperand
    [
       #var(type:int)sec.
       #var(type:int)usec.
       anOperand $read &vint:sec &vint:usec.

       #var(type:int)ms := tv_usec.
       (usec > tv_usec)
          ? [  sec := sec + 1. ms := ms + 1000000. ].

       ms := ms - usec.

       ^ SystemTime $new &int:(tv_sec - sec) &int:ms.
    ]
        
    #method(stacksafe,type:__time) add &__time:anOperand
    [
       #var(type:int)sec.
       #var(type:int)usec.
       anOperand $read &vint:sec &vint:usec.

       #var(type:int)ms := tv_usec + usec.

       (ms > 1000000)
          ? [  sec := sec + 1. ms := ms - 1000000. ].

       ^ SystemTime $new &int:(tv_sec + sec) &int:ms.
    ]
    
    #method __time = $self.
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
        #var(type:int)aMonth := tm_mon.
        aMonth := aMonth + 1.

        aRetVal << aMonth.
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

    #method hour
    [
        #var(type:int)n.
        $self readHour &vint:n.

        ^ n.
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
    
    #method(stacksafe) read &__time:aRetVal
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
       aTime readMSec &vint:tmp.
       tm_usec := tmp.

       //system'console writeLine:(tm_hour). 
    ]
    
    #method(stacksafe) writeUTC &__time:aTime                                     
    [
       #var(type:int)ptr := system'external'libc gmtime &__time:aTime.

       system'external'libc memcpy &__time_st:$self &int:ptr &int:36.

       #var(type:int)tmp.     
       aTime readMSec &vint:tmp.
       tm_usec := tmp.
    ]
     
    #method(stacksafe) toShortDate
    [
        #var(type:int)aYear.
        #var(type:int)aMonth.
        $self readYear &vint:aYear.
        $self readMonth &vint:aMonth. 

        ^ tm_mday literal padLeft &char:#48 &int:2 
              + "." + aMonth literal padLeft &char:#48 &int:2
              + "." + aYear literal padLeft &char:#48 &int:2.
    ]

    #method(stacksafe) toShortTime
    [
        ^ tm_hour literal padLeft &char:#48 &int:2
             + ":" + tm_min literal padLeft &char:#48 &int:2
             + ":" + tm_sec literal padLeft &char:#48 &int:2.
    ]
}
