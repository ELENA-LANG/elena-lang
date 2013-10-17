#define system.

// --- RandomGenerator ---

#class RandomGenerator
{
    #field(type:long)theData.
    
    #constructor new
    [
        system'external'CoreAPI rnd_init &type'long:$self.
    ]
    
    #method eval : aMaxValue
        = IntNumber new &type'int:(system'external'CoreAPI rnd_next &type'long:$self &type'int::aMaxValue).
}

// --- randomGenerator ---

#static randomGenerator = RandomGenerator new.

// -- ERandomizer --

#class(role) ERandomizer
{
    #method eval : aValue
    [
        #var i := aValue.
        #var j := randomGenerator:aValue.
        
        arrayControl exchange:i &with:j &in:self.
    ]
}

// --- randomize ---

#symbol randomControl =
{
    new : aMaxValue
        = randomGenerator eval:aMaxValue.
    
    randomize:aCount &array:anArray
    [
        DownCounter new:Wrap(ERandomizer, anArray) for:aCount.
    ]
}.