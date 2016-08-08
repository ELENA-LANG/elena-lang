#import system.

#subject __time    = system'calendar'SystemTime.
#subject __time_st = system'calendar'TimeStruct.
#subject __vtime   = varof:__time.

// --- SystemTime ---

#symbol $factory =
{
    #method(__time) utcNow
    [
        #var(__time)time.
                      
        system'external'libc gettimeofday 
            &__time:time
            &int:0.

        #var(__time_st)tm.
        tm writeUTC &__time:time.

        tm read &__time:time.
        
        ^ SystemTime new &__time:time.
    ]
    
    #method(__time) now
    [
        #var(__time)time.
                      
        system'external'libc gettimeofday 
            &__time:time
            &int:0.
        
        ^ SystemTime new &__time:time.
    ]
}.

#class(sealed,struct:reference,const) SystemTime :: BaseValue
{
    #field(int)tv_sec.
    #field(int)tv_usec.

    #constructor(stacksafe,embeddable) $new &int:sec &int:usec
    [
        tv_sec := sec.
        tv_usec := usec.
    ]

    #constructor(stacksafe) $new &long:nanosec
    [
        #var(long)secs := nanosec / 1000000000l.
        
        #var(long)rem := nanosec - secs * 1000000000l.
        rem := rem / 1000l.
                        
        #var(int)tmp := intConvertor convert &long:secs.
        tv_sec := tmp.
        
        tmp := intConvertor convert &long:rem.
        tv_usec := tmp.        
    ]

    #constructor new &__time:value
        = value.

    #constructor now
        <= new &__time:($factory now).

    #constructor utcNow
        <= new &__time:($factory utcNow).
                        
    #method(stacksafe) $read &vint:aSeconds &vint:aMicroSeconds 
    [
       aSeconds := tv_sec.
       aMicroSeconds := tv_usec.
    ]

    #method(stacksafe)readMSec &vint:aMicroSeconds 
    [
       aMicroSeconds := tv_usec.
    ]

    #method(stacksafe,bool) equal &__time:anOperand
    [
       #var(int)sec.
       #var(int)usec.
       anOperand $read &vint:sec &vint:usec.
  
       (sec == tv_sec)?
           [ ^ tv_usec == usec. ].

       ^ false.
    ]
        
    #method(stacksafe,bool) less &__time:anOperand
    [
       #var(int)sec.
       #var(int)usec.
       anOperand $read &vint:sec &vint:usec.
  
       (sec == tv_sec)?
           [ ^ tv_usec < usec. ].

       ^ tv_sec < sec.
    ]
    
    #method(stacksafe) write &__time:aValue
    [
       #var(int)sec.
       #var(int)usec.
       anOperand $read &vint:sec &vint:usec.
       
       tv_sec := sec.
       tv_usec := usec.       
    ]
    
    #method(stacksafe) read &vlong:aRetVal
    [
        #var(long)l := longConvertor convert &int:tv_sec.
        
        l := l * 10000000l.
        
        #var(long)l2 := longConvertor convert &int:tv_usec.
        l2 := l2 * 10l.
        
        l := l + l2.
        
        aRetVal := l.
    ]
        
    #method(stacksafe,__time) subtract &__time:anOperand
    [
       #var(int)sec.
       #var(int)usec.
       anOperand $read &vint:sec &vint:usec.

       #var(int)ms := tv_usec.
       (usec > tv_usec)
          ? [  sec := sec + 1. ms := ms + 1000000. ].

       ms := ms - usec.

       ^ SystemTime $new &int:(tv_sec - sec) &int:ms.
    ]
        
    #method(stacksafe,__time) add &__time:anOperand
    [
       #var(int)sec.
       #var(int)usec.
       anOperand $read &vint:sec &vint:usec.

       #var(int)ms := tv_usec + usec.

       (ms > 1000000)
          ? [  sec := sec + 1. ms := ms - 1000000. ].

       ^ SystemTime $new &int:(tv_sec + sec) &int:ms.
    ]
    
    #method __time = $self.
}

// --- TimeStruct ---

#class(struct:embeddable,sealed) TimeStruct
{
    #field(int) tm_sec.         /* seconds */
    #field(int) tm_min.         /* minutes */
    #field(int) tm_hour.        /* hours */
    #field(int) tm_mday.        /* day of the month */
    #field(int) tm_mon.         /* month */
    #field(int) tm_year.        /* year */
    #field(int) tm_wday.        /* day of the week */
    #field(int) tm_yday.        /* day in the year */
    #field(int) tm_isdst.       /* daylight saving time */

    #field(int) tmp1.
    #field(int) tmp2.
    
    #field(int) tm_usec.
    
    #constructor new 
    [
       tm_isdst := -1.  
    ]
    
    #constructor new &__time:aTime
    [
        $self write &__time:aTime.
    ]

    #method __time_st = $self.

    #method(stacksafe) readYear &vint:aRetVal
    [
        #var(int)aYear := tm_year.
        aYear += 1900.

        aRetVal := aYear.
    ]

    #method(stacksafe) readMonth &vint:aRetVal
    [
        #var(int)aMonth := tm_mon.
        aMonth += 1.

        aRetVal := aMonth.
    ]

    #method(stacksafe) readDayOfWeek &vint:aRetVal
    [
        aRetVal := tm_wday.
    ]
    
    #method(stacksafe) readDay &vint:aRetVal
    [
        aRetVal := tm_mday.
    ]

    #method(stacksafe) readHour &vint:aRetVal
    [
        aRetVal := tm_hour.
    ]

    #method hour
    [
        #var(int)n.
        $self readHour &vint:n.

        ^ n.
    ]

    #method(stacksafe) readMinute &vint:aRetVal
    [
        aRetVal := tm_min.
    ]

    #method(stacksafe) readSecond &vint:aRetVal
    [
        aRetVal := tm_sec.
    ]

    #method(stacksafe) readMillisecond &vint:aRetVal
    [
        #var(int)n := tm_usec.
        n /= 1000. 

        aRetVal := n.
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
        #var(int)n := aValue.
        n *= 1000. 

        tm_usec := n.
    ]
    
    #method(stacksafe) read &__time:aRetVal
    [
       // HOTFIX
       tm_isdst := -1.

       #var(int)tm_sec := system'external'libc mktime &__time_st:$self.
       #var(__time)tm := SystemTime $new &int:tm_sec &int:tm_usec.

       aRetVal write &__time:tm. 
    ]    
    
    #method(stacksafe) writeSpan &__time:aTime
    [
        $self writeUTC &__time:aTime.
    ] 

    #method(stacksafe) write &__time:aTime
    [
       #var(dirty_ptr)ptr := system'external'libc localtime &__time:aTime.

       system'external'libc memcpy &__time_st:$self &int:ptr &int:44.

       #var(int)tmp.     
       aTime readMSec &vint:tmp.
       tm_usec := tmp.
    ]
    
    #method(stacksafe) writeUTC &__time:aTime                                     
    [
       #var(int)ptr := system'external'libc gmtime &__time:aTime.

       system'external'libc memcpy &__time_st:$self &int:ptr &int:44.

       #var(int)tmp.     
       aTime readMSec &vint:tmp.
       tm_usec := tmp.
    ]
     
    #method(stacksafe) toShortDate
    [
        #var(int)aYear.
        #var(int)aMonth.
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
