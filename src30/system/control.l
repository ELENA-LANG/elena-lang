// === Basic prototypes ===

// --- Enumerator ---

/// An enumerator prototype
limited dispatchable(enumerator) class Enumerator
{
    /// Returns the current object
    get = $nil.
    
    /// Goes to the next enumeration member and returns true if it is not the last member
    bool next = $nil.
    
    /// Resets the enumerator
    reset []
    
    /// returns the enumerator owner
    enumerable = $nil.
}

// --- Iterator ---

/// An iterator prototype
limited dispatchable(iterator) class Iterator
{
    /// Returns false if the index out of range
    bool available = $nil.

    /// Returns the current member
    get = $nil.
    
    /// Sets the current member
    set : anObject []
    
//    /// Reads the current index into an output parameter
//    #method(stacksafe,outnprop:index) readIndex &vint:anIndex []
    
    /// Sets the index by specified value
    ///
    /// Default implementation for compatibility calls obsolete write&index method
    stacksafe writeIndex &int:anIndex 
        [ $self write &index:(IntNumber new &int:anIndex). ] 
    
    /// Sets the index by specified value
    set &index:anIndex 
        <= writeIndex &int:(anIndex int).
    
    /// obsolete
    write &index:anIndex []
    
    /// Increaments the index by specified value
    iterator append &index:anOffset
    [
        int anIndex := 0.
        $self readIndex &vint:anIndex.
        
        anIndex := anIndex + anOffset int.
        
        $self writeIndex &int:anIndex.
        
        ^ $self.
    ]

    /// Decreaments the index by specified value
    reduce &index:anOffset
    [
        int anIndex := 0.
        $self readIndex &vint:anIndex.
        
        anIndex := anIndex - anOffset int.
        
        $self writeIndex &int:anIndex.
    ]

//    /// Reads the length to the output parameter    
//    #method(stacksafe,outnprop:length) readLength &vint:anOutput
//    [
//        anOutput := 0. 
//    ]
}

//// --- base functions ---
//
///// A base action
//#class(limited,dispatchable:func)BaseFunction
//{
//    /// Executes the action
//    #method(action) eval []
//    
//    /// Executes the action while the condition is true
//    #method(sealed,w2) run &if:aBool
//    [
//        #loop aBool ?
//        [
//            $self eval.
//        ].
//    ]
//
//    /// Executes the action while the condition is false
//    ///
//    /// Executes at least once
//    #method(sealed) run &until:aBool
//    [
//        #var(bool)flag := false.
//        
//        #loop flag !
//        [
//            $self eval.
//            
//            flag := aBool bool.
//        ].
//    ]
//}
//
///// A base function
//#class(limited,dispatchable:func1)BaseFunction1
//{
//    /// Executes the function
//    #method(action) eval : aParam []
//
//    /// Executes the function for every enumeration item
//    #method run &enumerator:anEnumerator
//    [
//        #loop (anEnumerator next) ?
//        [
//            $self eval:(anEnumerator get).
//        ].        
//    ]
//    
//    /// Executes the function for every enumeration item until it returns true
//    #method(sealed,w2) retrieve &enumerator:anEnumerator
//    [
//        #var anItem := $nil.
//        #loop (anEnumerator next) ?
//        [
//            anItem := anEnumerator get.
//            
//            // !! temporal
//            ($self eval:anItem bool) ?
//                [ ^ anItem. ].
//        ].        
//        
//        ^ $nil.        
//    ]
//}
//
///// A base two variable function
//#class(limited,dispatchable:func2)BaseFunction2
//{
//    /// Dispatches the verb to the target with func2 subject
//    /// Executes the function
//    #method(action) eval : aParam1 : aParam2 []
//    
//    /// Executes the function for parameters and every enumeration item
//    #method(sealed) run : aParam &enumerator:anEnumerator
//    [
//        #loop (anEnumerator next) ?
//        [
//            $self eval:aParam:(anEnumerator get).
//        ].        
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true
//    #method(sealed,w2) retrieve : aParam &enumerator:anEnumerator
//    [
//        #var anItem := $nil.
//        #loop (anEnumerator next) ?
//        [
//            anItem := anEnumerator get.
//            
//            // !! temporal
//            ($self eval:aParam:anItem bool) ?
//                [ ^ anItem. ].
//        ].        
//        
//        ^ $nil.        
//    ]
//}
//
///// A base three variable function
//#class(limited,dispatchable:func3)BaseFunction3
//{
//    /// Executes the function
//    #method(action) eval : aParam1 : aParam2 : aParam3 []
//    
//    /// Executes the function for parameters and every enumeration item
//    #method(sealed) run : aParam1 : aParam2 &enumerator:anEnumerator
//    [
//        #loop (anEnumerator next) ?
//        [
//            $self eval:aParam1:aParam2:(anEnumerator get).
//        ].        
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    #method(sealed,w2) retrieve : aParam1 : aParam2 &enumerator:anEnumerator
//    [
//        #var anItem := $nil.
//        #loop (anEnumerator next) ?
//        [
//            anItem := anEnumerator get.
//            
//            // !! temporal
//            ($self eval:aParam1:aParam2:anItem bool) ?
//                [ ^ anItem. ].
//        ].        
//        
//        ^ $nil.        
//    ]
//}
//
///// A base four variable function
//#class(limited,dispatchable:func4)BaseFunction4
//{
//    /// Executes the function
//    #method(action) eval : aParam1 : aParam2 : aParam3 : aParam4 []
//    
//    /// Executes the function for parameters and every enumeration item
//    #method(sealed) run : aParam1 : aParam2 : aParam3 &enumerator:anEnumerator
//    [
//        #loop (anEnumerator next) ?
//        [
//            $self eval:aParam1:aParam2:aParam3:(anEnumerator get).
//        ].        
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    #method(sealed,w2) retrieve : aParam1 : aParam2 : aParam3 &enumerator:anEnumerator
//    [
//        #var anItem := $nil.
//        #loop (anEnumerator next) ?
//        [
//            anItem := anEnumerator get.
//            
//            // !! temporal
//            ($self eval:aParam1:aParam2:aParam3:anItem bool) ?
//                [ ^ anItem. ].
//        ].        
//        
//        ^ $nil.
//    ]
//}
//
///// A base five variable function
//#class(limited,dispatchable:func5)BaseFunction5
//{
//    /// Executes the function
//    #method(action) eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 []
//    
//    /// Executes the function for parameters and every enumeration item
//    #method(sealed) run : aParam1 : aParam2 : aParam3 : aParam4 &enumerator:anEnumerator
//    [
//        #loop (anEnumerator next) ?
//        [
//            $self eval:aParam1:aParam2:aParam3:aParam4:(anEnumerator get).
//        ].        
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    #method(sealed,w2) retrieve : aParam1 : aParam2 : aParam3 : aParam4 &enumerator:anEnumerator
//    [
//        #var anItem := $nil.
//        #loop (anEnumerator next) ?
//        [
//            anItem := anEnumerator get.
//            
//            // !! temporal
//            ($self eval:aParam1:aParam2:aParam3:aParam4:anItem bool) ?
//                [ ^ anItem. ].
//        ].        
//        
//        ^ $nil.
//    ]
//}
//
///// A base six variable function
//#class(limited,dispatchable:func6)BaseFunction6
//{
//    /// Executes the function
//    #method(action) eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 []
//    
//    /// Executes the function for parameters and every enumeration item
//    #method(sealed) run : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 &enumerator:anEnumerator
//    [
//        #loop (anEnumerator next) ?
//        [
//            $self eval:aParam1:aParam2:aParam3:aParam4:aParam5:(anEnumerator get).
//        ].        
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    #method(sealed,w2) retrieve : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 &enumerator:anEnumerator
//    [
//        #var anItem := $nil.
//        #loop (anEnumerator next) ?
//        [
//            anItem := anEnumerator get.
//            
//            // !! temporal
//            ($self eval:aParam1:aParam2:aParam3:aParam4:aParam5:anItem bool) ?
//                [ ^ anItem. ].
//        ].        
//        
//        ^ $nil.        
//    ]
//}
//
///// A base seven variable function
//#class(limited,dispatchable:func7)BaseFunction7
//{
//    /// Executes the function
//    #method(action) eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 []
//    
//    /// Executes the function for parameters and every enumeration item
//    #method(sealed) run : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 &enumerator:anEnumerator
//    [
//        #loop (anEnumerator next) ?
//        [
//            $self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:(anEnumerator get).
//        ].        
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    #method(sealed,w2) retrieve : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 &enumerator:anEnumerator
//    [
//        #var anItem := $nil.
//        #loop (anEnumerator next) ?
//        [
//            anItem := anEnumerator get.
//            
//            // !! temporal
//            ($self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:anItem bool) ?
//                [ ^ anItem. ].
//        ].        
//        
//        ^ $nil.        
//    ]
//}
//
///// A base eight variable function
//#class(limited,dispatchable:func8)BaseFunction8
//{
//    /// Executes the function    
//    #method(action) eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 []
//    
//    /// Executes the function for parameters and every enumeration item
//    #method(sealed) run : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 &enumerator:anEnumerator
//    [
//        #loop (anEnumerator next) ?
//        [
//            $self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:(anEnumerator get).
//        ].        
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    #method(sealed,w2) retrieve : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 &enumerator:anEnumerator
//    [
//        #var anItem := $nil.
//        #loop (anEnumerator next) ?
//        [
//            anItem := anEnumerator get.
//            
//            // temporal
//            ($self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:anItem bool) ?
//                [ ^ anItem. ].
//        ].        
//        
//        ^ $nil.        
//    ]
//}
//
///// A base nine variable function
//#class(limited,dispatchable:func9)BaseFunction9
//{
//    /// Executes the function
//    #method(action) eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 []
//    
//    /// Executes the function for parameters and every enumeration item
//    #method(sealed) run : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 &enumerator:anEnumerator
//    [
//        #loop (anEnumerator next) ?
//        [
//            $self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:aParam8:(anEnumerator get).
//        ].        
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    #method(sealed,w2) retrieve : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 &enumerator:anEnumerator
//    [
//        #var anItem := $nil.
//        #loop (anEnumerator next) ?
//        [
//            anItem := anEnumerator get.
//            
//            // !! temporal
//            ($self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:aParam8:anItem bool) ?
//                [ ^ anItem. ].
//        ].        
//        
//        ^ $nil.        
//    ]
//}
//
///// A base ten variable function
//#class(limited,dispatchable:func10)BaseFunction10
//{
//    /// Executes the function    
//    #method(action) eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 []
//    
//    /// Executes the function for parameters and every enumeration item
//    #method(sealed) run : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 &enumerator:anEnumerator
//    [
//        #loop (anEnumerator next) ?
//        [
//            $self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:aParam8:aParam9:(anEnumerator get).
//        ].        
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    #method(sealed,w2) retrieve : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 &enumerator:anEnumerator
//    [
//        #var anItem := $nil.
//        #loop (anEnumerator next) ?
//        [
//            anItem := anEnumerator get.
//            
//            // !! temporal
//            ($self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:aParam8:aParam9:anItem bool) ?
//                [ ^ anItem. ].
//        ].        
//        
//        ^ $nil.        
//    ]
//}
//
///// A base eleven variable functions
//#class(limited,dispatchable:func11)BaseFunction11
//{
//    /// Executes the function
//    #method(action) eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 : aParam11 []
//    
//    /// Executes the function for parameters and every enumeration item
//    #method(sealed) run : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 &enumerator:anEnumerator
//    [
//        #loop (anEnumerator next) ?
//        [
//            $self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:aParam8:aParam9:aParam10:(anEnumerator get).
//        ].        
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    #method(sealed,w2) retrieve : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 &enumerator:anEnumerator
//    [
//        #var anItem := $nil.
//        #loop (anEnumerator next) ?
//        [
//            anItem := anEnumerator get.
//            
//            // !! temporal
//            ($self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:aParam8:aParam9:aParam10:anItem bool) ?
//                [ ^ anItem. ].
//        ].        
//        
//        ^ $nil.        
//    ]
//}
//
//// --- base lazy expression ---
//
//#class(limited)BaseLazyExpression
//{
//    #method eval []
//    
//    #method => system'core_routines'__lazyexpression.
//}
//
//// --- IndexFunction ---
//
///// A base index function
//#class(limited,dispatchable:nfunc)IndexFunction1
//{
//    /// Executes the function
//    ///
//    /// Default implementation calls eval&index for compatibility with old code
//    #method(stacksafe,action) eval &int:n 
//        = $self eval &index:(IntNumber new &int:n).
//
//    /// Obsolete
//    #method(stacksafe,action) eval &index:n []
//
//    /// Executes the function for every value in the specified integer range (starting value, ending value, -step)
//    #method(stacksafe,sealed) rewind &int:aStart &int:n &int:dx
//    [
//        #var(int)i := aStart.
//        
//        #loop (i >= n) ?
//        [
//            $self eval &int:i.
//            
//            i := i - dx.
//        ].
//        
//        ^ $nil.        
//    ]          
//    
//    /// Executes the function for every value in the specified range (starting value, ending value, step)
//    #method(stacksafe,sealed) run &int:aStart &int:n &int:dx
//    [
//        #var(int)i := aStart.
//        
//        #loop (i <= n) ?
//        [
//            $self eval &int:i.
//            
//            i := i + dx.
//        ].
//        
//        ^ $nil.        
//    ]
//
//    /// Executes the function for every value in the specified range (starting value, ending value, step) until it returns true    
//    #method(stacksafe,sealed,w2) retrieve &int:aStart &int:n &int:dx
//    [
//        #var(int)i := aStart.
//        
//        #loop(i <= n) ?
//        [
//            ($self eval &int:i)
//                ? [ ^ IntNumber new &int:i. ].
//            
//            i := i + dx.
//        ].
//        
//        ^ $nil.        
//    ]
//}
//
///// A base two variable index function
//#class(limited,dispatchable:nnfunc)IndexFunction2
//{
//    /// Executes the function
//    ///
//    /// Default implementation calls eval&index&index for compatibility with old code
//    #method(stacksafe,action) eval &int:i &int:j
//        = $self eval &index:(IntNumber new &int:i) &index:(IntNumber new &int:j).
//
//    /// Obsolete
//    #method(action) eval &index:i &index:j []
//
//    /// Executes the function for every value in the specified integer area (starting x-value, starting y-value, ending x-value, ending y-value, -step)
//    #method(sealed) rewind &int:i &int:j &int:n &int:m &int:dx
//    [
//        #var(int)x := i.
//        #var(int)y := j.
//        
//        #loop(x >= n) ?
//        [
//            y := j.
//            
//            #loop (y >= m) ?
//            [
//                $self eval &int:x &int:y.
//                
//                y := y - dx.
//            ].
//                        
//            x := x - dx.
//        ].
//        
//        ^ $nil.        
//    ]          
//    
//    /// Executes the function for every value in the specified range (starting x-value, starting y-value, ending x-value, ending y-value, step)
//    #method(sealed) run &int:i &int:j &int:n &int:m &int:dx
//    [
//        #var(int)x := i.
//        #var(int)y := j.
//        
//        #loop(x <= n) ?
//        [
//            y := j.
//            
//            #loop (y <= m) ?
//            [
//                $self eval &int:x &int:y.
//                
//                y := y + dx.
//            ].
//                        
//            x := x + dx.
//        ].
//        
//        ^ $nil.        
//    ]
//}
//
///// A base three variable index function
//#class(limited,dispatchable:n3func)IndexFunction3
//{
//    /// Executes the function
//    #method(stacksafe,action) eval &int:i &int:j &int:k []
//
//    /// Executes the function for every value in the specified integer area (starting x-value, starting y-value, starting z-value, ending x-value, ending y-value, ending z-value, -step)
//    #method(sealed) rewind &int:i &int:j &int:k &int:n &int:m &int:p &int:dx
//    [
//        #var(int)x := i.
//        #var(int)y := j.
//        #var(int)z := k.
//        
//        #loop(x >= n) ?
//        [
//            y := j.
//            
//            #loop (y >= m) ?
//            [
//                z := k.
//                
//                #loop (z >= p) ?
//                [
//                    $self eval &int:x &int:y &int:z.
//                    
//                    z := z - dx.
//                ].                
//                
//                y := y - dx.
//            ].
//                        
//            x := x - dx.
//        ].
//        
//        ^ $nil.        
//    ]          
//    
//    /// Executes the function for every value in the specified range (starting x-value, starting y-value, starting z-value, ending x-value, ending y-value, ending z-value, step)
//    #method(sealed) run &int:i &int:j &int:k &int:n &int:m &int:p &int:dx
//    [
//        #var(int)x := i.
//        #var(int)y := j.
//        #var(int)z := k.
//        
//        #loop(x <= n) ?
//        [
//            y := j.
//            
//            #loop (y <= m) ?
//            [
//                z := k.
//                
//                #loop (z <= p) ?
//                [
//                    $self eval &int:x &int:y &int:z.
//                    
//                    z := z + dx.
//                ].                
//                
//                y := y + dx.
//            ].
//                        
//            x := x + dx.
//        ].
//        
//        ^ $nil.        
//    ]
//}
//
////// --- Boolean ---
////
/////// A boolean variable. 
////#class(varof:bool) Boolean :: BaseValue
////{
////    /// Creates a boolean variable with an initial value (false)
////    #constructor new
////    [        
////        $value := false.
////    ]
////
////    /// Creates a boolean variable with specified value
////    #constructor new : aValue
////    [        
////        $value := true equal &bool:(aValue bool).
////    ]
////    
////    /// Creates a boolean variable with specified value
////    #constructor new &bool:aValue
////    [        
////        $value := aValue.
////    ]
////
////    /// Returns the boolean value
////    #method bool = $value.
////
////    /// Executes logical OR operation
////    #method append : aValue
////    [
////        $value := $value or:aValue.
////    ]
////
////    /// Executes logical AND operation            
////    #method multiplyBy : aValue
////    [
////        $value := $value and:aValue.
////    ]
////            
////    /// Inverts the boolean value
////    #method invert
////    [
////        $value := $value not bool.
////    ]            
////
////    /// Clones the object
////    #method clone = Boolean new:$value.
////}
