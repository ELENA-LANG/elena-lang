#define system.

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
    #constructor new &__time:aValue
        => system'internal'long_copy.

    #constructor $new &long:aValue
        => system'internal'long_copy.

    #constructor now
        <= new &__time:($factory now).

    #constructor utcNow
        <= new &__time:($factory utcNow).
                        
    #method(type:bool) equal &__time:anOperand
        => system'internal'long_equal.
        
    #method(type:bool) less &__time:anOperand
        => system'internal'long_less.
    
    #method read &vlong:aRetVal
        => system'internal'long_save.
        
    #method(type:__time) subtract &__time:anOperand
        => system'internal'long_sub.
        
    #method(type:__time) add &__time:anOperand
        => system'internal'long_add.
    
    #method __time = $self.
}

#class(sealed,struct:embeddable) SystemTimeVar
{
    #field(type:__time)theValue.
    
    #method __time = SystemTime new &__time:theValue.
    
    #method __vtime = $self.
    
    #method write &__time:aValue
    [
        theValue := aValue.
    ]
    
    #method read &__vtime:aVariable
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
    
    #method __time_st = $self.
    
    #method year = IntNumber new &short:theYear.

    #method month = IntNumber new &short:theMonth.

    #method dayOfWeek = IntNumber new &short:theDayOfWeek.

    #method day = IntNumber new &short:theDay.

    #method hour = IntNumber new &short:theHour.

    #method minute = IntNumber new &short:theMinute.

    #method second = IntNumber new &short:theSecond.

    #method millisecond = IntNumber new &short:theMillisecond.

    #method readYear &vshort:aRetVal
    [
        aRetVal << theYear.
    ]

    #method readMonth &vshort:aRetVal
    [
        aRetVal << theMonth.
    ]

    #method readDayOfWeek &vshort:aRetVal
    [
        aRetVal << theDayOfWeek.
    ]

    #method readDay &vshort:aRetVal
    [
        aRetVal << theDay.
    ]

    #method readHour &vshort:aRetVal
    [
        aRetVal << theHour.
    ]

    #method readMinute &vshort:aRetVal
    [
        aRetVal << theMinute.
    ]

    #method readSecond &vshort:aRetVal
    [
        aRetVal << theSecond.
    ]

    #method readMillisecond &vshort:aRetVal
    [
        aRetVal << theMillisecond.
    ]

    #method set &year:aValue 
    [ 
        theYear := aValue short.
    ]

    #method set &month:aValue 
    [ 
        theMonth := aValue short.
    ]

    #method set &day:aValue 
    [ 
        theDay := aValue short.
    ]

    #method set &hour:aValue 
    [ 
        theHour := aValue short.
    ]

    #method set &minute:aValue 
    [ 
        theMinute := aValue short.
    ]

    #method set &second:aValue 
    [ 
        theSecond := aValue short.
    ]

    #method set &millisecond:aValue 
    [ 
        theMillisecond := aValue short.
    ]

    #method read &__vtime:aRetVal
    [
        system'external'KERNEL32 SystemTimeToFileTime &__time_st:$self &__vtime:aRetVal.
    ]    
    
    #method write &__time:aFileTime
    [
        system'external'KERNEL32 FileTimeToSystemTime &__time:aFileTime &__time_st:$self.
    ]
      
    #method toWide &int_date_attributes:Flags
    [
        #var(type:shortarray,size:127)aBuffer.

        #var(type:int) aLength := system'external'KERNEL32 GetDateFormatEx
            &int:0
            &int:Flags
            &__time_st:$self
            &int:0
            &shortarray:aBuffer
            &int:255
            &int:0
            | raise:(FormatException new:"Invalid date").

        ^ WideLiteralValue $new &length:(aLength - 1) &index:0 &shortarray:aBuffer.                
    ]
    
    #method toWide &int_time_attributes:Flags
    [
        #var(type:shortarray,size:127)aBuffer.

        #var(type:int) aLength := system'external'KERNEL32 GetTimeFormatEx
            &int:0
            &int:Flags
            &__time_st:$self
            &int:0
            &shortarray:aBuffer
            &int:255
            &int:0
            | raise:(FormatException new:"Invalid time").

        ^ WideLiteralValue $new &length:(aLength - 1) &index:0 &shortarray:aBuffer.                
    ]

    #method toShortDate
        <= %toWide &int_date_attributes:DATE_SHORTDATE.

    #method toShortTime
        <= %toWide &int_time_attributes:0.
}
