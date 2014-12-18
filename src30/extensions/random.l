//#define system.
//#define system'routines.
//#define system'dynamic.
//
//// --- RandomGenerator ---
//
//#class RandomGenerator
//{
//    #field(type:long)theData.
//    
//    #constructor new
//        <rnd_init>.
//
//    #method eval &int:aMaxValue &out'int:aRetVal
//        <rnd_next>.
//
//    #method eval : aMaxValue
//    [
//        #var(type:int)aNextValue.
//        $self eval &int::aMaxValue &out'int:aNextValue.
//        
//        ^ IntNumber new &int:aNextValue.
//    ]
//}
//
//// --- randomGenerator ---
//
//#static randomGenerator = RandomGenerator new.
//
//// -- ERandomizer --
//
//#class(role) ERandomizer
//{
//    #method eval : aValue
//    [
//        #var i := aValue - 1.
//        #var j := randomGenerator:aValue.
//        
//        arrayControl exchange:i &with:j &in:self.
//    ]
//}
//
//// --- randomize ---
//
//#symbol randomControl =
//{
//    new : aMaxValue
//        = randomGenerator eval:aMaxValue.
//    
//    randomize:aCount &array:anArray
//    [
//        DownCounter new:Wrap(anArray, ERandomizer) for:aCount.
//        
//        ^ anArray.
//    ]
//}.