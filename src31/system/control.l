// === Basic prototypes ===

// --- Enumerator ---

/// An enumerator prototype
limited class Enumerator :: dispatchable(enumerator)
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

// --- Indexer ---

/// An indexer prototype
limited class Indexer :: dispatchable(indexer)
{
    /// Returns false if the index out of range
    bool available = $nil.

    /// Returns the current member
    get = $nil.
    
    /// Sets the current member
    set : anObject []
    
    /// Reads the current index into an output parameter
    stacksafe readIndexTo vint:retVal []

    int index :: outnprop(readIndexTo).
    
    /// Sets the index by specified value
    stacksafe writeIndex int:anIndex [ ] 
    
    /// Sets the index by specified value
    set index:anIndex 
        <= writeIndex int:anIndex.
    
    /// Increaments the index by specified value
    indexer append index:anOffset
    [
        int anIndex := 0.
        $self readIndexTo vint:anIndex.
        
        anIndex := anIndex + anOffset int.
        
        $self writeIndex int:anIndex.
        
        ^ $self
    ]

    /// Decreaments the index by specified value
    reduce index:anOffset
    [
        int anIndex := 0.
        $self readIndexTo vint:anIndex.
        
        anIndex := anIndex - anOffset int.
        
        $self writeIndex int:anIndex.
    ]

    /// Reads the length to the output parameter    
    stacksafe readLengthTo vint:retVal 
    [
        retVal int := 0
    ]
    
    int length :: outnprop(readLengthTo).
}

// --- MatrixIndexer ---

/// An indexer prototype
limited class Indexer2D :: dispatchable(indexer2d)
{
    /// Returns false if the index out of range
    bool available = $nil.

    /// Returns the current member
    get = $nil.
    
    /// Sets the current member
    set : anObject []
    
    /// Reads the current i-index into an output parameter
    stacksafe readIndex1To vint:anIndex []
    
    /// Reads the current j-index into an output parameter
    stacksafe readIndex2To vint:anIndex []
    
    int index1 :: outnprop(readIndex1To).
    
    int index2 :: outnprop(readIndex2To).
    
    /// Sets the i-index by specified value
    stacksafe writeIndex1 int:anIndex [ ] 
    
    /// Sets the i-index by specified value
    stacksafe writeIndex2 int:anIndex [ ] 
    
    /// Sets the index by specified value
    set index1:anIndex 
        <= writeIndex1 int:anIndex.
    
    /// Sets the index by specified value
    set index2:anIndex 
        <= writeIndex2 int:anIndex.
    
    /// Increaments the index by specified value
    indexer2d append index1:anOffset
    [
        int anIndex := 0.
        $self readIndex1 vint:anIndex.
        
        anIndex := anIndex + anOffset int.
        
        $self writeIndex1To int:anIndex.
        
        ^ $self.
    ]
    
    /// Increaments the index by specified value
    indexer2d append index2:anOffset
    [
        int anIndex := 0.
        $self readIndex2To vint:anIndex.
        
        anIndex := anIndex + anOffset int.
        
        $self writeIndex2 int:anIndex.
        
        ^ $self.
    ]

    /// Decreaments the index by specified value
    reduce index1:anOffset
    [
        int anIndex := 0.
        $self readIndex1To vint:anIndex.
        
        anIndex := anIndex - anOffset int.
        
        $self writeIndex1 int:anIndex.
    ]

    /// Decreaments the index by specified value
    reduce index2:anOffset
    [
        int anIndex := 0.
        $self readIndex2To vint:anIndex.
        
        anIndex := anIndex - anOffset int.
        
        $self writeIndex2 int:anIndex.
    ]

    /// Reads the length to the output parameter    
    stacksafe readSizeTo vint:n vint:m
    [
    ]

    /// Reads the length to the output parameter    
    stacksafe readColumnsTo vint:m
    [
    ]

    int columns :: outnprop(readColumnsTo).

    /// Reads the length to the output parameter    
    stacksafe readRownsTo vint:n
    [
    ]
    
    int rows :: outnprop(readRownsTo).
}

// --- base functions ---

/// A base action
limited class BaseFunction :: dispatchable(func)
{
    /// Executes the action
    action eval []
    
    /// Executes the action while the condition is true
    sealed run while:aBool
    [
        while (aBool)
        [
            $self eval
        ]
    ]

    /// Executes the action while the condition is false
    ///
    /// Executes at least once
    sealed run until:aBool
    [
        bool flag := false.
        
        until (flag)
        [
            $self eval.
            
            flag := aBool bool
        ].
    ]
}

/// A base function
limited class BaseFunction1 :: dispatchable(func1)
{
    /// Executes the function
    action eval : aParam []

    /// Executes the function for every enumeration item
    run enumerator:anEnumerator
    [
        while(anEnumerator next)
        [
            $self eval:(anEnumerator get)
        ].        
    ]
    
    /// Executes the function for every enumeration item until it returns true
    sealed retrieve enumerator:anEnumerator
    [
        object anItem := $nil.
        while (anEnumerator next)
        [
            anItem := anEnumerator get.

            if($self eval:anItem)
                [ ^ anItem ]
        ].
        
        ^ $nil.
    ]
}

/// A base two variable function
limited class BaseFunction2 :: dispatchable(func2)
{
    /// Dispatches the verb to the target with func2 subject
    /// Executes the function
    action eval : aParam1 : aParam2 []
    
    /// Executes the function for parameters and every enumeration item
    sealed run : aParam enumerator:anEnumerator
    [
        while(anEnumerator next)
        [
            $self eval:aParam:(anEnumerator get)
        ]
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true
    sealed retrieve : aParam enumerator:anEnumerator
    [
        object anItem := $nil.
        while (anEnumerator next)
        [
            anItem := anEnumerator get.
            
            // !! temporal
            if ($self eval:aParam:anItem)
                [ ^ anItem ]
        ].        
        
        ^ $nil
    ]
}

/// A base three variable function
limited class BaseFunction3 :: dispatchable(func3)
{
    /// Executes the function
    action eval : aParam1 : aParam2 : aParam3 []
    
    /// Executes the function for parameters and every enumeration item
    sealed run : aParam1 : aParam2 enumerator:anEnumerator
    [
        while (anEnumerator next)
        [
            $self eval:aParam1:aParam2:(anEnumerator get)
        ]
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    sealed retrieve : aParam1 : aParam2 enumerator:anEnumerator
    [
        object anItem := $nil.
        while (anEnumerator next)
        [
            anItem := anEnumerator get.
            
            if ($self eval:aParam1:aParam2:anItem)
                [ ^ anItem ]
        ].        
        
        ^ $nil
    ]
}

/// A base four variable function
limited class BaseFunction4 :: dispatchable(func4)
{
    /// Executes the function
    action eval : aParam1 : aParam2 : aParam3 : aParam4 []
    
    /// Executes the function for parameters and every enumeration item
    sealed run : aParam1 : aParam2 : aParam3 enumerator:anEnumerator
    [
        while (anEnumerator next)
        [
            $self eval:aParam1:aParam2:aParam3:(anEnumerator get)
        ]
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    sealed retrieve : aParam1 : aParam2 : aParam3 enumerator:anEnumerator
    [
        object anItem := $nil.
        while (anEnumerator next)
        [
            anItem := anEnumerator get.
            
            // !! temporal
            if ($self eval:aParam1:aParam2:aParam3:anItem)
                [ ^ anItem ]
        ].        
        
        ^ $nil
    ]
}

/// A base five variable function
limited class BaseFunction5 :: dispatchable(func5)
{
    /// Executes the function
    action eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 []
    
    /// Executes the function for parameters and every enumeration item
    sealed run : aParam1 : aParam2 : aParam3 : aParam4 enumerator:anEnumerator
    [
        while (anEnumerator next)
        [
            $self eval:aParam1:aParam2:aParam3:aParam4:(anEnumerator get)
        ]
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    sealed retrieve : aParam1 : aParam2 : aParam3 : aParam4 enumerator:anEnumerator
    [
        object anItem := $nil.
        while (anEnumerator next)
        [
            anItem := anEnumerator get.
            
            if ($self eval:aParam1:aParam2:aParam3:aParam4:anItem)
                [ ^ anItem ]
        ].        
        
        ^ $nil
    ]
}

/// A base six variable function
limited class BaseFunction6 :: dispatchable(func6)
{
    /// Executes the function
    action eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 []
    
    /// Executes the function for parameters and every enumeration item
    sealed run : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 enumerator:anEnumerator
    [
        while (anEnumerator next)
        [
            $self eval:aParam1:aParam2:aParam3:aParam4:aParam5:(anEnumerator get)
        ]
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    sealed retrieve : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 enumerator:anEnumerator
    [
        object anItem := $nil.
        while (anEnumerator next)
        [
            anItem := anEnumerator get.
            
            if ($self eval:aParam1:aParam2:aParam3:aParam4:aParam5:anItem)
                [ ^ anItem ]
        ].        
        
        ^ $nil
    ]
}

/// A base seven variable function
limited class BaseFunction7 :: dispatchable(func7)
{
    /// Executes the function
    action eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 []
    
    /// Executes the function for parameters and every enumeration item
    sealed run : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 enumerator:anEnumerator
    [
        while (anEnumerator next)
        [
            $self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:(anEnumerator get)
        ]
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    sealed retrieve : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 enumerator:anEnumerator
    [
        object anItem := $nil.
        while (anEnumerator next)
        [
            anItem := anEnumerator get.
            
            if ($self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:anItem)
                [ ^ anItem ]
        ].        
        
        ^ $nil
    ]
}

/// A base eight variable function
limited class BaseFunction8 :: dispatchable(func8)
{
    /// Executes the function    
    action eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 []
    
    /// Executes the function for parameters and every enumeration item
    sealed run : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 enumerator:anEnumerator
    [
        while (anEnumerator next)
        [
            $self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:(anEnumerator get)
        ]
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    sealed retrieve : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 enumerator:anEnumerator
    [
        object anItem := $nil.
        while (anEnumerator next)
        [
            anItem := anEnumerator get.
            
            if ($self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:anItem)
                [ ^ anItem ]
        ].        
        
        ^ $nil
    ]
}

/// A base nine variable function
limited class BaseFunction9 :: dispatchable(func9)
{
    /// Executes the function
    action eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 []
    
    /// Executes the function for parameters and every enumeration item
    sealed run : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 enumerator:anEnumerator
    [
        while (anEnumerator next)
        [
            $self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:aParam8:(anEnumerator get)
        ]
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    sealed retrieve : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 enumerator:anEnumerator
    [
        object anItem := $nil.
        while (anEnumerator next)
        [
            anItem := anEnumerator get.
            
            if ($self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:aParam8:anItem)
                [ ^ anItem ]
        ].        
        
        ^ $nil
    ]
}

/// A base ten variable function
limited class BaseFunction10 :: dispatchable(func10)
{
    /// Executes the function    
    action eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 []
    
    /// Executes the function for parameters and every enumeration item
    sealed run : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 enumerator:anEnumerator
    [
        while (anEnumerator next)
        [
            $self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:aParam8:aParam9:(anEnumerator get)
        ]
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    sealed retrieve : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 enumerator:anEnumerator
    [
        var anItem := $nil.
        while (anEnumerator next)
        [
            anItem := anEnumerator get.
            
            if ($self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:aParam8:aParam9:anItem)
                [ ^ anItem ]
        ].        
        
        ^ $nil
    ]
}

/// A base eleven variable functions
limited class BaseFunction11 :: dispatchable(func11)
{
    /// Executes the function
    action eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 : aParam11 []
    
    /// Executes the function for parameters and every enumeration item
    sealed run : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 enumerator:anEnumerator
    [
        while (anEnumerator next)
        [
            $self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:aParam8:aParam9:aParam10:(anEnumerator get)
        ]
    ]
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    sealed retrieve : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 enumerator:anEnumerator
    [
        var anItem := $nil.
        while (anEnumerator next)
        [
            anItem := anEnumerator get.
            
            if ($self eval:aParam1:aParam2:aParam3:aParam4:aParam5:aParam6:aParam7:aParam8:aParam9:aParam10:anItem)
                [ ^ anItem ]
        ].        
        
        ^ $nil
    ]
}

// --- base lazy expression ---

limited class BaseLazyExpression
{
    eval []
    
    dispatch => system'core_routines'__lazyexpression.
}

// --- IndexFunction ---

/// A base index function
limited class IndexFunction1 :: dispatchable(nfunc)
{
    /// Executes the function
    stacksafe action eval int:n []

    /// Executes the function for every value in the specified integer range (starting value, ending value, -step)
    sealed stacksafe rewind int:aStart int:n int:dx
    [
        int i := aStart.
        
        while (i >= n)
        [
            $self eval int:i.
            
            i := i - dx
        ].
        
        ^ $nil
    ]          
    
    /// Executes the function for every value in the specified range (starting value, ending value, step)
    sealed stacksafe run int:aStart int:n int:dx
    [
        int i := aStart.
        
        while (i <= n)
        [
            $self eval int:i.
            
            i := i + dx
        ].
        
        ^ $nil
    ]

    /// Executes the function for every value in the specified range (starting value, ending value, step) until it returns true    
    sealed stacksafe retrieve int:aStart int:n int:dx
    [
        int i := aStart.
        
        while(i <= n)
        [
            if ($self eval int:i)
                [ ^ IntNumber new int:i ].
            
            i := i + dx
        ].
        
        ^ $nil
    ]
}

/// A base two variable index function
limited class IndexFunction2 :: dispatchable(nnfunc)
{
    /// Executes the function
    stacksafe action eval int:i int:j []

    /// Executes the function for every value in the specified integer area (starting x-value, starting y-value, ending x-value, ending y-value, -step)
    sealed rewind int:i int:j int:n int:m int:dx
    [
        int x := i.
        int y := j.
        
        while(x >= n)
        [
            y := j.
            
            while (y >= m)
            [
                $self eval int:x int:y.
                
                y := y - dx
            ].
                        
            x := x - dx
        ].
        
        ^ $nil
    ]          
    
    /// Executes the function for every value in the specified range (starting x-value, starting y-value, ending x-value, ending y-value, step)
    sealed run int:i int:j int:n int:m int:dx
    [
        int x := i.
        int y := j.
        
        while(x <= n)
        [
            y := j.
            
            while (y <= m)
            [
                $self eval int:x int:y.
                
                y := y + dx
            ].
                        
            x := x + dx
        ].
        
        ^ $nil
    ]
}

/// A base three variable index function
limited class IndexFunction3 :: dispatchable(n3func)
{
    /// Executes the function
    stacksafe action eval int:i int:j int:k []

    /// Executes the function for every value in the specified integer area (starting x-value, starting y-value, starting z-value, ending x-value, ending y-value, ending z-value, -step)
    sealed rewind int:i int:j int:k int:n int:m int:p int:dx
    [
        int x := i.
        int y := j.
        int z := k.
        
        while(x >= n)
        [
            y := j.
            
            while(y >= m)
            [
                z := k.
                
                while(z >= p)
                [
                    $self eval int:x int:y int:z.
                    
                    z := z - dx
                ].                
                
                y := y - dx
            ].
                        
            x := x - dx
        ].
        
        ^ $nil
    ]          
    
    /// Executes the function for every value in the specified range (starting x-value, starting y-value, starting z-value, ending x-value, ending y-value, ending z-value, step)
    sealed run int:i int:j int:k int:n int:m int:p int:dx
    [
        int x := i.
        int y := j.
        int z := k.
        
        while(x <= n)
        [
            y := j.
            
            while(y <= m)
            [
                z := k.
                
                while(z <= p)
                [
                    $self eval int:x int:y int:z.
                    
                    z := z + dx
                ].                
                
                y := y + dx
            ].
                        
            x := x + dx
        ].
        
        ^ $nil
    ]
}

//// --- Boolean ---
//
///// A boolean variable. 
//#class(varof:bool) Boolean :: BaseValue
//{
//    /// Creates a boolean variable with an initial value (false)
//    #constructor new
//    [        
//        $value := false.
//    ]
//
//    /// Creates a boolean variable with specified value
//    #constructor new : aValue
//    [        
//        $value := true equal &bool:(aValue bool).
//    ]
//    
//    /// Creates a boolean variable with specified value
//    #constructor new &bool:aValue
//    [        
//        $value := aValue.
//    ]
//
//    /// Returns the boolean value
//    #method bool = $value.
//
//    /// Executes logical OR operation
//    #method append : aValue
//    [
//        $value := $value or:aValue.
//    ]
//
//    /// Executes logical AND operation            
//    #method multiplyBy : aValue
//    [
//        $value := $value and:aValue.
//    ]
//            
//    /// Inverts the boolean value
//    #method invert
//    [
//        $value := $value not bool.
//    ]            
//
//    /// Clones the object
//    #method clone = Boolean new:$value.
//}

// --- BaseIndexer ---

class BaseIndexer :: Indexer
{
    int theIndex.
    int theLength.
    
    /// Returns false if the index out of range
    available = (theIndex <= theLength).
    
    /// Reads the current i-index into an output parameter
    readIndexTo vint:retVal
    [
        retVal int := theIndex
    ]    
    
    /// Sets the i-index by specified value
    writeIndex int:anIndex 
    [ 
        theIndex := anIndex
    ] 
}

// --- BaseMatrixIndexer ---

class BaseMatrixIndexer :: Indexer2D
{
    int theIndex1.
    int theIndex2.
    int theLength1.
    int theLength2.
    
    /// Returns false if the index out of range
    available = (theIndex1 <= theLength1) && (theIndex2 <= theLength2).
    
    /// Reads the current i-index into an output parameter
    readIndex1To vint:retVal
    [
        retVal int := theIndex1.
    ]    
    
    /// Reads the current j-index into an output parameter
    readIndex2To vint:retVal
    [
        retVal int := theIndex2.
    ]
    
    /// Sets the i-index by specified value
    writeIndex1 int:anIndex 
    [ 
        theIndex1 := anIndex.
    ] 
    
    /// Sets the i-index by specified value
    writeIndex2 int:anIndex 
    [
        theIndex2 := anIndex.
    ] 

    /// Reads the length to the output parameter    
    stacksafe readSizeTo vint:n vint:m
    [
        n int := theLength1.
        m int := theLength2.
    ]

    /// Reads the length to the output parameter    
    readColumnsTo vint:m
    [
        m int := theLength2.
    ]

    /// Reads the length to the output parameter    
    readRownsTo vint:n
    [
        n int := theLength1.
    ]    
}