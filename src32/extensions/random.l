import system'routines.

// --- RandomGenerator ---

sealed struct RandomGenerator
{
    long theSeed.
    
    constructor new
    [
        system'external core_rnd_init vlong:theSeed.
    ]

    stacksafe eval int:aMaxValue vint:aRetVal
    [
        system'external core_rnd_next long:theSeed int:aMaxValue vint:aRetVal.
    ]

    stacksafe eval vint:aRetVal
    [
        system'external core_rnd_nextint long:theSeed vint:aRetVal.
    ]

    eval : minValue : maxValue
        = minValue + $self eval:(maxValue - minValue).

    eval : aMaxValue
    [
        int aNextValue := 0.
        $self eval int:(aMaxValue int) vint:aNextValue.
        
        ^ IntNumber new int:aNextValue.
    ]
    
    nextInt
    [
        int aNextValue := 0.
        $self eval vint:aNextValue.
        
        ^ IntNumber new int:aNextValue.
    ]

    nextInt : aValue
        = $self eval:aValue.
        
    nextReal
    [
        int next := 0.
        $self eval int:1000000 vint:next.
        
        real retVal := 0.0r.
        realConvertor convert int:next vreal:retVal.
        
        retVal := retVal / 1000000.0r.
        
        ^ RealNumber new real:retVal.
    ]
}

// --- randomGenerator ---

static randomGenerator = RandomGenerator new.

// -- ERandomizer --

extension randomOp
{
    randomize : aCount
    [
        var list := self clone.
        
        int n := aCount int.
        int i := 0.
        
        var max := self length.
        
        while (i < n)
        [
            var j := randomGenerator eval:max.
            
            list~indexableOp exchange int:i int:(j int).
            
            i := i + 1
        ].
        
        ^ list
    ]
}
