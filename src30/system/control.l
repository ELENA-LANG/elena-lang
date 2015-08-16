// --- true ---

#class(limited) BaseBoolValue
{
    #method then : aTrueAction = $nil.

    #method then : aTrueAction &else:aFalseAction = $nil.

    #method else : aTrueAction = $nil.
    
    #method iif:aTrueValue:aFalseValue = $nil.

    #method(type:bool) or : anExpr = $nil.

    #method(type:bool) and : anExpr = $nil.

    #method(type:bool) xor : aBool = $nil.

    #method equal : anExpr = $nil.

    #method(type:bool) equal &bool:aBool = $nil.

    #method(type:bool) not = $nil.

    #method literal = $nil.

    #method wide = $nil.

    #method bool = $self.

    #method clone = $nil.
    
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &bool:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &bool:$self.
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
    #method(type:bool) or : anExpr = $self.

    /// Executes logical AND operation
    #method(type:bool) and : anExpr = anExpr bool.

    /// Executes logical XOR operation
    #method(type:bool) xor : aBool
    [
        aBool ? [ ^ false. ].
        
        ^ $self.
    ]

    /// Determines whether the specified object is equal to the current object boolean value.
    #method equal : anExpr = anExpr bool.

    /// Determines whether the specified object is equal to the current object boolean value.
    #method(type:bool) equal &bool:aBool = aBool.

    /// Always returns false.
    #method(type:bool) not = false.

    /// Returns literal representation.
    #method literal = "true".

    /// Returns literal representation.
    #method wide = "true" wide. // !! temporal

    /// Returns itself.
    #method bool = $self.

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
    #method(type:bool) or : anExpr = anExpr bool.

    /// Executes logical AND operation
    ///
    /// Always return false
    #method(type:bool) and : anExpr = $self.

    /// Executes logical XOR operation
    #method(type:bool) xor : anExpr 
    [
        anExpr ? [ ^ true. ].
        
        ^ $self.
    ]

    /// Determines whether the specified object is equal to the current object boolean value
    #method equal : anExpr
    [
        anExpr ? [ ^ $self. ].
        
        ^ true.
    ]

    /// Determines whether the specified object is equal to the current object boolean value
    #method(type:bool) equal &bool:anExpr
    [
        anExpr ? [ ^ $self. ].
        
        ^ true.
    ]

    /// Always returns true
    #method not = true.

    /// Returns literal representation
    #method literal = "false".
    
    /// Returns literal representation
    #method wide = "false" wide. // !! temporal

    /// Returns itself
    #method bool = $self.

    /// Returns itself
    #method clone = $self.
}.

// === Basic prototypes ===

// --- Enumerator ---

/// An enumerator prototype
#class(limited) Enumerator
{
    /// Returns itself
    #method enumerator = $self.
    
    /// Returns the current object
    #method get = $nil.
    
    /// Goes to the next enumeration member and returns true if it is not the last member
    #method(type:bool) next = $nil.
    
    /// Resets the enumerator
    #method reset []
    
    /// Dispatches the verb to the target object with enumerator subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &enumerator:$self.
    
    /// Dispatches the verb to the target object with enumerator subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &enumerator:$self.
}

// --- Iterator ---

/// An iterator prototype
#class(limited) Iterator
{
    /// Returns itself
    #method iterator = $self.
    
    /// Returns false if the index out of range
    #method(type:bool) available = $nil.

    /// Returns the current member
    #method get = $nil.
    
    /// Sets the current member
    #method set : anObject []
    
    /// Reads the current index into an output parameter
    #method(stacksafe) readIndex &vint:anIndex []
    
    /// Sets the index by specified value
    ///
    /// Default implementation for compatibility calls obsolete write&index method
    #method(stacksafe) writeIndex &int:anIndex 
        [ $self write &index:(IntNumber new &int:anIndex). ] 
    
    /// Returns the index value
    #method index
    [
        #var(type:int)anIndex := 0.
        $self readIndex &vint:anIndex.
                        
        ^ IntNumber new &int:anIndex.
    ]                
    
    /// Sets the index by specified value
    #method set &index:anIndex 
        <= writeIndex &int:(anIndex int).
    
    /// obsolete
    #method write &index:anIndex []
    
    /// Increaments the index by specified value
    #method append &index:anOffset
    [
        #var(type:int)anIndex := 0.
        $self readIndex &vint:anIndex.
        
        anIndex := anIndex + anOffset int.
        
        $self writeIndex &int:anIndex.
    ]

    /// Decreaments the index by specified value
    #method reduce &index:anOffset
    [
        #var(type:int)anIndex := 0.
        $self readIndex &vint:anIndex.
        
        anIndex := anIndex - anOffset int.
        
        $self writeIndex &int:anIndex.
    ]

    /// Reads the length to the output parameter    
    #method(stacksafe) readLength &vint:anOutput
    [
        anOutput << 0. 
    ]
    
    /// Returns the end of indexer position
    #method length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]
    
    /// Dispatches the verb to the target object with iterator subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &iterator:$self.
    
    /// Dispatches the verb to the target object with iterator subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &iterator:$self.
}

// --- base functions ---

/// A base action
#class(limited)BaseFunction
{
    /// Executes the action
    #method eval []
    
    /// Executes the action while the condition is true
    #method run &if:aBool
    [
        #loop aBool ?
        [
            $self eval.
        ].
    ]

    /// Executes the action while the condition is false
    ///
    /// Executes at least once
    #method run &until:aBool
    [
        #var(type:bool)flag := false.
        
        #loop flag !
        [
            $self eval.
            
            flag := aBool bool.
        ].
    ]
    
    /// Returns itself
    #method func = $self.
    
    /// Dispatches the verb to the target with func subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &func:$self.
    
    /// Dispatches the verb to the target with func subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &func:$self.
}

/// A base function
#class(limited)BaseFunction1
{
    /// Returns itself
    #method func1 = $self.
    
    /// Dispatches the verb to the target with func1 subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &func1:$self.
    
    /// Dispatches the verb to the target with func1 subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &func1:$self.
    
    /// Executes the function
    #method eval : aParam []

    /// Executes the function for every enumeration item
    #method run &enumerator:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            $self eval:(anEnumerator get).
        ].        
    ]
    
    /// Executes the function for every enumeration item until it returns true
    #method retrieve &enumerator:anEnumerator
    [
        #var anItem := nil.
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
#class(limited)BaseFunction2
{
    /// Returns itself
    #method func2 = $self.
    
    /// Dispatches the verb to the target with func2 subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &func2:$self.
    
    /// Dispatches the verb to the target with func2 subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &func2:$self.
    
    /// Executes the function
    #method eval : aParam1 : aParam2 []
    
    /// Executes the function for parameters and every enumeration item
    #method run : aParam &enumerator:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            $self eval:aParam:(anEnumerator get).
        ].        
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true
    #method retrieve : aParam &enumerator:anEnumerator
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
#class(limited)BaseFunction3
{
    /// Returns itself
    #method func3 = $self.
    
    /// Dispatches the verb to the target object with func3 subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &func3:$self.
    
    /// Dispatches the verb to the target object with func3 subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &func3:$self.
    
    /// Executes the function
    #method eval : aParam1 : aParam2 : aParam3 []
    
    /// Executes the function for parameters and every enumeration item
    #method run : aParam1 : aParam2 &enumerator:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            $self eval:aParam1:aParam2:(anEnumerator get).
        ].        
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    #method retrieve : aParam1 : aParam2 &enumerator:anEnumerator
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
#class(limited)BaseFunction4
{
    /// Returns itself
    #method func4 = $self.
    
    /// Dispatches the verb to the target object with func4 subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &func4:$self.
    
    /// Dispatches the verb to the target object with func4 subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &func4:$self.
    
    /// Executes the function
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 []
    
    /// Executes the function for parameters and every enumeration item
    #method run : aParam1 : aParam2 : aParam3 &enumerator:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            $self eval:aParam1:aParam2:aParam3:(anEnumerator get).
        ].        
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    #method retrieve : aParam1 : aParam2 : aParam3 &enumerator:anEnumerator
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
#class(limited)BaseFunction5
{
    /// Returns itself
    #method func5 = $self.
    
    /// Dispatches the verb to the target object with func5 subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &func5:$self.
    
    /// Dispatches the verb to the target object with func5 subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &func5:$self.
    
    /// Executes the function
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 []
    
    /// Executes the function for parameters and every enumeration item
    #method run : aParam1 : aParam2 : aParam3 : aParam4 &enumerator:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            $self eval:aParam1:aParam2:aParam3:aParam4:(anEnumerator get).
        ].        
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    #method retrieve : aParam1 : aParam2 : aParam3 : aParam4 &enumerator:anEnumerator
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
#class(limited)BaseFunction6
{
    /// Returns itself
    #method func6 = $self.
    
    /// Dispatches the verb to the target object with func6 subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &func6:$self.
    
    /// Dispatches the verb to the target object with func6 subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &func6:$self.
    
    /// Executes the function
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 []
    
    /// Executes the function for parameters and every enumeration item
    #method run : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 &enumerator:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            $self eval:aParam1:aParam2:aParam3:aParam4:aParam5:(anEnumerator get).
        ].        
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    #method retrieve : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 &enumerator:anEnumerator
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
#class(limited)BaseFunction7
{
    /// Returns itself
    #method func7 = $self.
    
    /// Dispatches the verb to the target object with func7 subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &func7:$self.
    
    /// Dispatches the verb to the target object with func7 subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &func7:$self.
    
    /// Executes the function
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 []
    
    /// Executes the function for parameters and every enumeration item
    #method run : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 &enumerator:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            $self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:(anEnumerator get).
        ].        
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    #method retrieve : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 &enumerator:anEnumerator
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
#class(limited)BaseFunction8
{
    /// Returns itself
    #method func8 = $self.
    
    /// Dispatches the verb to the target object with func8 subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &func8:$self.
    
    /// Dispatches the verb to the target object with func8 subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &func8:$self.

    /// Executes the function    
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 []
    
    /// Executes the function for parameters and every enumeration item
    #method run : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 &enumerator:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            $self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:(anEnumerator get).
        ].        
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    #method retrieve : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 &enumerator:anEnumerator
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
#class(limited)BaseFunction9
{
    /// Returns itself
    #method func9 = $self.
    
    /// Dispatches the verb to the target object with func9 subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &func9:$self.
    
    /// Dispatches the verb to the target object with func9 subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &func9:$self.
    
    /// Executes the function
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 []
    
    /// Executes the function for parameters and every enumeration item
    #method run : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 &enumerator:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            $self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:aParam8:(anEnumerator get).
        ].        
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    #method retrieve : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 &enumerator:anEnumerator
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
#class(limited)BaseFunction10
{
    /// Returns itself
    #method func10 = $self.
    
    /// Dispatches the verb to the target object with func10 subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &func10:$self.
    
    /// Dispatches the verb to the target object with func10 subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &func10:$self.

    /// Executes the function    
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 []
    
    /// Executes the function for parameters and every enumeration item
    #method run : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 &enumerator:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            $self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:aParam8:aParam9:(anEnumerator get).
        ].        
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    #method retrieve : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 &enumerator:anEnumerator
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
#class(limited)BaseFunction11
{
    /// Returns itself
    #method func11 = $self.
    
    /// Dispatches the verb to the target object with func11 subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &func11:$self.
    
    /// Dispatches the verb to the target object with func11 subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &func11:$self.
    
    /// Executes the function
    #method eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 : aParam11 []
    
    /// Executes the function for parameters and every enumeration item
    #method run : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 &enumerator:anEnumerator
    [
        #loop (anEnumerator next) ?
        [
            $self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:aParam8:aParam9:aParam10:(anEnumerator get).
        ].        
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    #method retrieve : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 &enumerator:anEnumerator
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
    
    #method => system'internal'expression_dispatch.
}

// --- IndexFunction ---

/// A base index function
#class(limited)IndexFunction1
{
    /// Returns itslef
    #method nfunc = $self.

    /// Dispatches the verb to the target object with nfunc subject    
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &nfunc:$self.
    
    /// Dispatches the verb to the target object with nfunc subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &nfunc:$self.
    
    /// Executes the function
    ///
    /// Default implementation calls eval&index for compatibility with old code
    #method(stacksafe) eval &int:n 
        = $self eval &index:(IntNumber new &int:n).

    /// Obsolete
    #method(stacksafe) eval &index:n []

    /// Executes the function for every value in the specified integer range (starting value, ending value, -step)
    #method(stacksafe) rewind &int:aStart &int:n &int:dx
    [
        #var(type:int)i := aStart.
        
        #loop (i >= n) ?
        [
            $self eval &int:i.
            
            i := i - dx.
        ].
        
        ^ $nil.        
    ]          
    
    /// Executes the function for every value in the specified range (starting value, ending value, step)
    #method(stacksafe) run &int:aStart &int:n &int:dx
    [
        #var(type:int)i := aStart.
        
        #loop (i <= n) ?
        [
            $self eval &int:i.
            
            i := i + dx.
        ].
        
        ^ $nil.        
    ]

    /// Executes the function for every value in the specified range (starting value, ending value, step) until it returns true    
    #method(stacksafe) retrieve &int:aStart &int:n &int:dx
    [
        #var(type:int)i := aStart.
        
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
#class(limited)IndexFunction2
{
    /// Returns itslef
    #method nnfunc = $self.
    
    /// Dispatches the verb to the target object with nnfunc subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &nnfunc:$self.
    
    /// Dispatches the verb to the target object with nnfunc subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &nnfunc:$self.
    
    /// Executes the function
    ///
    /// Default implementation calls eval&index&index for compatibility with old code
    #method(stacksafe) eval &int:i &int:j
        = $self eval &index:(IntNumber new &int:i) &index:(IntNumber new &int:j).

    /// Obsolete
    #method eval &index:i &index:j []

    /// Executes the function for every value in the specified integer area (starting x-value, starting y-value, ending x-value, ending y-value, -step)
    #method rewind &int:i &int:j &int:n &int:m &int:dx
    [
        #var(type:int)x := i.
        #var(type:int)y := j.
        
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
    #method run &int:i &int:j &int:n &int:m &int:dx
    [
        #var(type:int)x := i.
        #var(type:int)y := j.
        
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
#class(limited)IndexFunction3
{
    /// Returns itself
    #method n3func = $self.
    
    /// Dispatches the verb to the target object with n3func subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &n3func:$self.
    
    /// Dispatches the verb to the target object with n3func subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &n3func:$self.
    
    /// Executes the function
    #method(stacksafe) eval &int:i &int:j &int:k []

    /// Executes the function for every value in the specified integer area (starting x-value, starting y-value, starting z-value, ending x-value, ending y-value, ending z-value, -step)
    #method rewind &int:i &int:j &int:k &int:n &int:m &int:p &int:dx
    [
        #var(type:int)x := i.
        #var(type:int)y := j.
        #var(type:int)z := k.
        
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
    #method run &int:i &int:j &int:k &int:n &int:m &int:p &int:dx
    [
        #var(type:int)x := i.
        #var(type:int)y := j.
        #var(type:int)z := k.
        
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

// --- Boolean ---

/// A boolean variable. 
///
/// Extends a boolean value
#class Boolean :: BaseValue
{
    #field(type:bool) theValue.

    /// Creates a boolean variable with an initial value (false)
    #constructor new
    [        
        theValue := false.
    ]

    /// Creates a boolean variable with specified value
    #constructor new : aValue
    [        
        theValue := true equal &bool:(aValue bool).
    ]
    
    /// Creates a boolean variable with specified value
    #constructor new &bool:aValue
    [        
        theValue := aValue.
    ]

    /// Returns the boolean value
    #method value = theValue.

    /// Writes the value
    #method write : aValue
    [
        theValue := aValue bool.
    ]

    /// Writes the value
    #method write &bool:aValue
    [
        theValue := aValue.
    ]

    /// Returns the boolean value
    #method bool = theValue.

    /// Executes logical OR operation
    #method append : aValue
    [
        theValue := theValue or:aValue.
    ]

    /// Executes logical AND operation            
    #method multiplyBy : aValue
    [
        theValue := theValue and:aValue.
    ]
            
    /// Inverts the boolean value
    #method invert
    [
        theValue := theValue not bool.
    ]            

    /// Clones the object
    #method clone = Boolean new:theValue.
            
    /// Redirects the other messages to the boolean value            
    #method => theValue.
}
