#define system.
#define system'routines.
//#define system'dynamic.

// --- RandomGenerator ---

#class(struct,sealed) RandomGenerator
{
    #field(type:long)theSeed.
    
    #constructor new
        => system'internal'rnd_init.

    #method eval &int:aMaxValue &out'int:aRetVal
        => system'internal'rnd_next.

    #method eval : aMaxValue
    [
        #var(type:int)aNextValue := $self eval &int:(aMaxValue int).
        
        ^ IntNumber new &int:aNextValue.
    ]
}

// --- randomGenerator ---

#static randomGenerator = RandomGenerator new.

// -- ERandomizer --

#class(extension:indexable) randomOp
{
    #method randomize : aCount
    [
        #var(type:int)n := aCount int.
        #var(type:int)i := 0.
        
        #var max := self length.
        
        #loop (i < n) ?
        [
            #var j := randomGenerator eval:max.
            
            self::indexableOp exchange:i:j.
            
            i := i + 1.
        ].
        
        ^ self.
    ]
}
