//// ==== ver 3.4.1 ===
//
//#import system.
//#import system'routines.
//
//// --- RandomGenerator ---
//
//#class(struct,sealed) RandomGenerator
//{
//    #field(type:long)theSeed.
//    
//    #constructor new
//    [
//        system'external core_rnd_init &vlong:theSeed.
//    ]
//
//    #method(stacksafe) eval &int:aMaxValue &vint:aRetVal
//    [
//        system'external core_rnd_next &long:theSeed &int:aMaxValue &vint:aRetVal.
//    ]
//
//    #method(stacksafe) eval &vint:aRetVal
//    [
//        system'external core_rnd_nextint &long:theSeed &vint:aRetVal.
//    ]
//
//    #method eval : minValue : maxValue
//        = minValue + $self eval:(maxValue - minValue).
//
//    #method eval : aMaxValue
//    [
//        #var(type:int)aNextValue.
//        $self eval &int:(aMaxValue int) &vint:aNextValue.
//        
//        ^ IntNumber new &int:aNextValue.
//    ]
//    
//    #method nextInt
//    [
//        #var(type:int)aNextValue.
//        $self eval &vint:aNextValue.
//        
//        ^ IntNumber new &int:aNextValue.
//    ]
//
//    #method nextInt : aValue
//        = $self eval:aValue.
//        
//    #method nextReal
//    [
//        #var(type:int)next.
//        $self eval &int:1000000 &vint:next.
//        
//        #var(type:real)retVal.
//        realConvertor convert &int:next &vreal:retVal.
//        
//        retVal := retVal / 1000000.0r.
//        
//        ^ RealNumber new &real:retVal.
//    ]
//}
//
//// --- randomGenerator ---
//
//#static randomGenerator = RandomGenerator new.
//
//// -- ERandomizer --
//
//#class(extension) randomOp
//{
//    #method randomize : aCount
//    [
//        #var(type:int)n := aCount int.
//        #var(type:int)i := 0.
//        
//        #var max := self length.
//        
//        #loop (i < n) ?
//        [
//            #var j := randomGenerator eval:max.
//            
//            self::indexableOp exchange &int:i &int:(j int).
//            
//            i := i + 1.
//        ].
//        
//        ^ self.
//    ]
//}
