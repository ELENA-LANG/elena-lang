// --- true ---

#class(limited,dispatchable:bool) BaseBoolValue
{
    #method then : aTrueAction = $nil.

    #method then : aTrueAction &else:aFalseAction = $nil.

    #method else : aTrueAction = $nil.
    
    #method iif:aTrueValue:aFalseValue = $nil.

    #method(bool) or : anExpr = $nil.

    #method(bool) and : anExpr = $nil.

    #method(bool) xor : aBool = $nil.

    #method equal : anExpr = $nil.

    #method(bool) equal &bool:aBool = $nil.

    #method(bool) not = $nil.

    #method literal = $nil.

    #method wide = $nil.

    #method clone = $nil.
}

/// A true value symbol
#symbol true = BaseBoolValue
{
    /// Executes the action
    #method then : aTrueAction = aTrueAction eval.

    /// Executes then-action
    #method then : aTrueAction &else:aFalseAction = aTrueAction eval.

    /// Always returns nil.
    #method else : aTrueAction = $nil.

    #method iif:aTrueValue:aFalseValue = aTrueValue.
    
    /// Executes logical OR operation
    ///
    /// Always return true
    #method(bool) or : anExpr = $self.

    /// Executes logical AND operation
    #method(bool) and : anExpr = anExpr bool.

    /// Executes logical XOR operation
    #method(bool,suppress:w2) xor : aBool
    [
        aBool ? [ ^ false. ].
        
        ^ $self.
    ]

    /// Determines whether the specified object is equal to the current object boolean value.
    #method equal : anExpr = anExpr bool.

    /// Determines whether the specified object is equal to the current object boolean value.
    #method(bool) equal &bool:aBool = aBool.

    /// Always returns false.
    #method(bool) not = false.

    /// Returns literal representation.
    #method literal = "true".

    /// Returns literal representation.
    #method wide = "true"w.

    /// Returns itself.
    #method clone = $self.
}.

// --- false ---

/// A false value symbol
#symbol false = BaseBoolValue
{
    /// Always returns nil
    #method then : anAction = $nil.
    
    /// Executes else-action
    #method then : aTrueAction &else:aFalseAction = aFalseAction eval.

    /// Executes the action
    #method else : aFalseAction = aFalseAction eval.

    #method iif:aTrueValue:aFalseValue = aFalseValue.

    /// Executes logical OR operation
    #method(bool) or : anExpr = anExpr bool.

    /// Executes logical AND operation
    ///
    /// Always return false
    #method(bool) and : anExpr = $self.

    /// Executes logical XOR operation
    #method(bool,suppress:w2) xor : anExpr 
    [
        anExpr ? [ ^ true. ].
        
        ^ $self.
    ]

    /// Determines whether the specified object is equal to the current object boolean value
    #method(suppress:w2) equal : anExpr
    [
        anExpr ? [ ^ $self. ].
        
        ^ true.
    ]

    /// Determines whether the specified object is equal to the current object boolean value
    #method(bool) equal &bool:anExpr
    [
        anExpr ? [ ^ $self. ].
        
        ^ true.
    ]

    /// Always returns true
    #method not = true.

    /// Returns literal representation
    #method literal = "false".
    
    /// Returns literal representation
    #method wide = "false"w.

    /// Returns itself
    #method clone = $self.
}.

// === Basic prototypes ===

// --- Enumerator ---

/// An enumerator prototype
#class(limited,dispatchable:enumerator) Enumerator
{
    /// Returns the current object
    #method get = $nil.
    
    /// Goes to the next enumeration member and returns true if it is not the last member
    #method(bool) next = $nil.
    
    /// Resets the enumerator
    #method reset []
    
    /// returns the enumerator owner
    #method enumerable = $nil.
}

// --- Iterator ---

/// An iterator prototype
#class(limited,dispatchable:iterator) Iterator
{
    /// Returns false if the index out of range
    #method(bool) available = $nil.

    /// Returns the current member
    #method get = $nil.
    
    /// Sets the current member
    #method set : anObject []
    
    /// Reads the current index into an output parameter
    #method(stacksafe,outnprop:index) readIndex &vint:anIndex []
    
    /// Sets the index by specified value
    ///
    /// Default implementation for compatibility calls obsolete write&index method
    #method(stacksafe) writeIndex &int:anIndex 
        [ $self write &index:(IntNumber new &int:anIndex). ] 
    
    /// Sets the index by specified value
    #method set &index:anIndex 
        <= writeIndex &int:(anIndex int).
    
    /// obsolete
    #method write &index:anIndex []
    
    /// Increaments the index by specified value
    #method append &index:anOffset
    [
        #var(int)anIndex := 0.
        $self readIndex &vint:anIndex.
        
        anIndex := anIndex + anOffset int.
        
        $self writeIndex &int:anIndex.
    ]

    /// Decreaments the index by specified value
    #method reduce &index:anOffset
    [
        #var(int)anIndex := 0.
        $self readIndex &vint:anIndex.
        
        anIndex := anIndex - anOffset int.
        
        $self writeIndex &int:anIndex.
    ]

    /// Reads the length to the output parameter    
    #method(stacksafe,outnprop:length) readLength &vint:anOutput
    [
        anOutput := 0. 
    ]
}

// --- base functions ---

/// A base action
#class(limited,dispatchable:func)BaseFunction
{
    /// Executes the action
    #method(action) eval []
    
    /// Executes the action while the condition is true
    #method(sealed,suppress:w2) run &if:aBool
    [
        #loop aBool ?
        [
            $self eval.
        ].
    ]

    /// Executes the action while the condition is false
    ///
    /// Executes at least once
    #method(sealed) run &until:aBool
    [
        #var(bool)flag := false.
        
        #loop flag !
        [
            $self eval.
            
            flag := aBool bool.
        ].
    ]
}

/// A base function
#class(limited,dispatchable:func1)BaseFunction1
{
    /// Executes the function
    #method(action) eval : aParam []

    /// Executes the function for every enumeration item
    #method run &enumerator:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            $self eval:(anEnumerator get).
        ].        
    ]
    
    /// Executes the function for every enumeration item until it returns true
    #method(sealed,suppress:w2) retrieve &enumerator:anEnumerator
    [
        #var anItem := $nil.
        #loop (anEnumerator next) ?
        [
            anItem := anEnumerator get.
            
            ($self eval:anItem) ?
                [ ^ anItem. ].
        ].        
        
        ^ $nil.        
    ]
}

/// A base two variable function
#class(limited,dispatchable:func2)BaseFunction2
{
    /// Dispatches the verb to the target with func2 subject
    /// Executes the function
    #method(action) eval : aParam1 : aParam2 []
    
    /// Executes the function for parameters and every enumeration item
    #method(sealed) run : aParam &enumerator:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            $self eval:aParam:(anEnumerator get).
        ].        
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true
    #method(sealed,suppress:w2) retrieve : aParam &enumerator:anEnumerator
    [
        #var anItem := nil.
        #loop (anEnumerator next) ?
        [
            anItem := anEnumerator get.
            
            ($self eval:aParam:anItem) ?
                [ ^ anItem. ].
        ].        
        
        ^ $nil.        
    ]
}

/// A base three variable function
#class(limited,dispatchable:func3)BaseFunction3
{
    /// Executes the function
    #method(action) eval : aParam1 : aParam2 : aParam3 []
    
    /// Executes the function for parameters and every enumeration item
    #method(sealed) run : aParam1 : aParam2 &enumerator:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            $self eval:aParam1:aParam2:(anEnumerator get).
        ].        
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    #method(sealed,suppress:w2) retrieve : aParam1 : aParam2 &enumerator:anEnumerator
    [
        #var anItem := nil.
        #loop (anEnumerator next) ?
        [
            anItem := anEnumerator get.
            
            ($self eval:aParam1:aParam2:anItem) ?
                [ ^ anItem. ].
        ].        
        
        ^ $nil.        
    ]
}

/// A base four variable function
#class(limited,dispatchable:func4)BaseFunction4
{
    /// Executes the function
    #method(action) eval : aParam1 : aParam2 : aParam3 : aParam4 []
    
    /// Executes the function for parameters and every enumeration item
    #method(sealed) run : aParam1 : aParam2 : aParam3 &enumerator:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            $self eval:aParam1:aParam2:aParam3:(anEnumerator get).
        ].        
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    #method(sealed,suppress:w2) retrieve : aParam1 : aParam2 : aParam3 &enumerator:anEnumerator
    [
        #var anItem := $nil.
        #loop (anEnumerator next) ?
        [
            anItem := anEnumerator get.
            
            ($self eval:aParam1:aParam2:aParam3:anItem) ?
                [ ^ anItem. ].
        ].        
        
        ^ $nil.
    ]
}

/// A base five variable function
#class(limited,dispatchable:func5)BaseFunction5
{
    /// Executes the function
    #method(action) eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 []
    
    /// Executes the function for parameters and every enumeration item
    #method(sealed) run : aParam1 : aParam2 : aParam3 : aParam4 &enumerator:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            $self eval:aParam1:aParam2:aParam3:aParam4:(anEnumerator get).
        ].        
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    #method(sealed,suppress:w2) retrieve : aParam1 : aParam2 : aParam3 : aParam4 &enumerator:anEnumerator
    [
        #var anItem := $nil.
        #loop (anEnumerator next) ?
        [
            anItem := anEnumerator get.
            
            ($self eval:aParam1:aParam2:aParam3:aParam4:anItem) ?
                [ ^ anItem. ].
        ].        
        
        ^ $nil.
    ]
}

/// A base six variable function
#class(limited,dispatchable:func6)BaseFunction6
{
    /// Executes the function
    #method(action) eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 []
    
    /// Executes the function for parameters and every enumeration item
    #method(sealed) run : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 &enumerator:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            $self eval:aParam1:aParam2:aParam3:aParam4:aParam5:(anEnumerator get).
        ].        
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    #method(sealed,suppress:w2) retrieve : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 &enumerator:anEnumerator
    [
        #var anItem := $nil.
        #loop (anEnumerator next) ?
        [
            anItem := anEnumerator get.
            
            ($self eval:aParam1:aParam2:aParam3:aParam4:aParam5:anItem) ?
                [ ^ anItem. ].
        ].        
        
        ^ $nil.        
    ]
}

/// A base seven variable function
#class(limited,dispatchable:func7)BaseFunction7
{
    /// Executes the function
    #method(action) eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 []
    
    /// Executes the function for parameters and every enumeration item
    #method(sealed) run : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 &enumerator:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            $self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:(anEnumerator get).
        ].        
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    #method(sealed,suppress:w2) retrieve : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 &enumerator:anEnumerator
    [
        #var anItem := $nil.
        #loop (anEnumerator next) ?
        [
            anItem := anEnumerator get.
            
            ($self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:anItem) ?
                [ ^ anItem. ].
        ].        
        
        ^ $nil.        
    ]
}

/// A base eight variable function
#class(limited,dispatchable:func8)BaseFunction8
{
    /// Executes the function    
    #method(action) eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 []
    
    /// Executes the function for parameters and every enumeration item
    #method(sealed) run : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 &enumerator:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            $self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:(anEnumerator get).
        ].        
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    #method(sealed,suppress:w2) retrieve : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 &enumerator:anEnumerator
    [
        #var anItem := $nil.
        #loop (anEnumerator next) ?
        [
            anItem := anEnumerator get.
            
            ($self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:anItem) ?
                [ ^ anItem. ].
        ].        
        
        ^ $nil.        
    ]
}

/// A base nine variable function
#class(limited,dispatchable:func9)BaseFunction9
{
    /// Executes the function
    #method(action) eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 []
    
    /// Executes the function for parameters and every enumeration item
    #method(sealed) run : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 &enumerator:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            $self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:aParam8:(anEnumerator get).
        ].        
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    #method(sealed,suppress:w2) retrieve : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 &enumerator:anEnumerator
    [
        #var anItem := $nil.
        #loop (anEnumerator next) ?
        [
            anItem := anEnumerator get.
            
            ($self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:aParam8:anItem) ?
                [ ^ anItem. ].
        ].        
        
        ^ $nil.        
    ]
}

/// A base ten variable function
#class(limited,dispatchable:func10)BaseFunction10
{
    /// Executes the function    
    #method(action) eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 []
    
    /// Executes the function for parameters and every enumeration item
    #method(sealed) run : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 &enumerator:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            $self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:aParam8:aParam9:(anEnumerator get).
        ].        
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    #method(sealed,suppress:w2) retrieve : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 &enumerator:anEnumerator
    [
        #var anItem := $nil.
        #loop (anEnumerator next) ?
        [
            anItem := anEnumerator get.
            
            ($self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:aParam8:aParam9:anItem) ?
                [ ^ anItem. ].
        ].        
        
        ^ $nil.        
    ]
}

/// A base eleven variable functions
#class(limited,dispatchable:func11)BaseFunction11
{
    /// Executes the function
    #method(action) eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 : aParam11 []
    
    /// Executes the function for parameters and every enumeration item
    #method(sealed) run : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 &enumerator:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            $self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:aParam8:aParam9:aParam10:(anEnumerator get).
        ].        
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    #method(sealed,suppress:w2) retrieve : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 &enumerator:anEnumerator
    [
        #var anItem := $nil.
        #loop (anEnumerator next) ?
        [
            anItem := anEnumerator get.
            
            ($self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:aParam8:aParam9:aParam10:anItem) ?
                [ ^ anItem. ].
        ].        
        
        ^ $nil.        
    ]
}

// --- base lazy expression ---

#class(limited)BaseLazyExpression
{
    #method eval []
    
    #method => system'core_routines'__lazyexpression.
}

// --- IndexFunction ---

/// A base index function
#class(limited,dispatchable:nfunc)IndexFunction1
{
    /// Executes the function
    ///
    /// Default implementation calls eval&index for compatibility with old code
    #method(stacksafe,action) eval &int:n 
        = $self eval &index:(IntNumber new &int:n).

    /// Obsolete
    #method(stacksafe,action) eval &index:n []

    /// Executes the function for every value in the specified integer range (starting value, ending value, -step)
    #method(stacksafe,sealed) rewind &int:aStart &int:n &int:dx
    [
        #var(int)i := aStart.
        
        #loop (i >= n) ?
        [
            $self eval &int:i.
            
            i := i - dx.
        ].
        
        ^ $nil.        
    ]          
    
    /// Executes the function for every value in the specified range (starting value, ending value, step)
    #method(stacksafe,sealed) run &int:aStart &int:n &int:dx
    [
        #var(int)i := aStart.
        
        #loop (i <= n) ?
        [
            $self eval &int:i.
            
            i := i + dx.
        ].
        
        ^ $nil.        
    ]

    /// Executes the function for every value in the specified range (starting value, ending value, step) until it returns true    
    #method(stacksafe,sealed,suppress:w2) retrieve &int:aStart &int:n &int:dx
    [
        #var(int)i := aStart.
        
        #loop(i <= n) ?
        [
            ($self eval &int:i)
                ? [ ^ IntNumber new &int:i. ].
            
            i := i + dx.
        ].
        
        ^ $nil.        
    ]
}

/// A base two variable index function
#class(limited,dispatchable:nnfunc)IndexFunction2
{
    /// Executes the function
    ///
    /// Default implementation calls eval&index&index for compatibility with old code
    #method(stacksafe,action) eval &int:i &int:j
        = $self eval &index:(IntNumber new &int:i) &index:(IntNumber new &int:j).

    /// Obsolete
    #method(action) eval &index:i &index:j []

    /// Executes the function for every value in the specified integer area (starting x-value, starting y-value, ending x-value, ending y-value, -step)
    #method(sealed) rewind &int:i &int:j &int:n &int:m &int:dx
    [
        #var(int)x := i.
        #var(int)y := j.
        
        #loop(x >= n) ?
        [
            y := j.
            
            #loop (y >= m) ?
            [
                $self eval &int:x &int:y.
                
                y := y - dx.
            ].
                        
            x := x - dx.
        ].
        
        ^ $nil.        
    ]          
    
    /// Executes the function for every value in the specified range (starting x-value, starting y-value, ending x-value, ending y-value, step)
    #method(sealed) run &int:i &int:j &int:n &int:m &int:dx
    [
        #var(int)x := i.
        #var(int)y := j.
        
        #loop(x <= n) ?
        [
            y := j.
            
            #loop (y <= m) ?
            [
                $self eval &int:x &int:y.
                
                y := y + dx.
            ].
                        
            x := x + dx.
        ].
        
        ^ $nil.        
    ]
}

/// A base three variable index function
#class(limited,dispatchable:n3func)IndexFunction3
{
    /// Executes the function
    #method(stacksafe,action) eval &int:i &int:j &int:k []

    /// Executes the function for every value in the specified integer area (starting x-value, starting y-value, starting z-value, ending x-value, ending y-value, ending z-value, -step)
    #method(sealed) rewind &int:i &int:j &int:k &int:n &int:m &int:p &int:dx
    [
        #var(int)x := i.
        #var(int)y := j.
        #var(int)z := k.
        
        #loop(x >= n) ?
        [
            y := j.
            
            #loop (y >= m) ?
            [
                z := k.
                
                #loop (z >= p) ?
                [
                    $self eval &int:x &int:y &int:z.
                    
                    z := z - dx.
                ].                
                
                y := y - dx.
            ].
                        
            x := x - dx.
        ].
        
        ^ $nil.        
    ]          
    
    /// Executes the function for every value in the specified range (starting x-value, starting y-value, starting z-value, ending x-value, ending y-value, ending z-value, step)
    #method(sealed) run &int:i &int:j &int:k &int:n &int:m &int:p &int:dx
    [
        #var(int)x := i.
        #var(int)y := j.
        #var(int)z := k.
        
        #loop(x <= n) ?
        [
            y := j.
            
            #loop (y <= m) ?
            [
                z := k.
                
                #loop (z <= p) ?
                [
                    $self eval &int:x &int:y &int:z.
                    
                    z := z + dx.
                ].                
                
                y := y + dx.
            ].
                        
            x := x + dx.
        ].
        
        ^ $nil.        
    ]
}

//// --- Boolean ---
//
///// A boolean variable. 
/////
///// Extends a boolean value
//#class Boolean :: BaseValue
//{
//    #field(type:bool) theValue.
//
//    /// Creates a boolean variable with an initial value (false)
//    #constructor new
//    [        
//        theValue := false.
//    ]
//
//    /// Creates a boolean variable with specified value
//    #constructor new : aValue
//    [        
//        theValue := true equal &bool:(aValue bool).
//    ]
//    
//    /// Creates a boolean variable with specified value
//    #constructor new &bool:aValue
//    [        
//        theValue := aValue.
//    ]
//
//    /// Returns the boolean value
//    #method value = theValue.
//
//    /// Writes the value
//    #method write : aValue
//    [
//        theValue := aValue bool.
//    ]
//
//    /// Writes the value
//    #method write &bool:aValue
//    [
//        theValue := aValue.
//    ]
//
//    /// Returns the boolean value
//    #method bool = theValue.
//
//    /// Executes logical OR operation
//    #method append : aValue
//    [
//        theValue := theValue or:aValue.
//    ]
//
//    /// Executes logical AND operation            
//    #method multiplyBy : aValue
//    [
//        theValue := theValue and:aValue.
//    ]
//            
//    /// Inverts the boolean value
//    #method invert
//    [
//        theValue := theValue not bool.
//    ]            
//
//    /// Clones the object
//    #method clone = Boolean new:theValue.
//            
//    /// Redirects the other messages to the boolean value            
//    #method => theValue.
//}
