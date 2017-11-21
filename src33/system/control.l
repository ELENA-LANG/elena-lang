// === Basic prototypes ===

// --- Enumerator ---

/// An enumerator prototype
limited class Enumerator
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
limited class Indexer
{
    /// Returns false if the index out of range
    bool available = $nil.

    /// Returns the current member
    get = $nil.
    
    /// Sets the current member
    set : anObject []
    
    /// Reads the current index into an output parameter
    stacksafe readIndexTo vint:retVal []

    int vvprop index :: readIndexTo, writeIndex.

    /// Sets the index by specified value
    ///
    /// Should be overidden(or set&index)
    stacksafe writeIndex int:anIndex 
    [ 
        $self set index:anIndex.
    ] 

    /// Reads the length to the output parameter    
    stacksafe readLengthTo vint:retVal 
    [
        retVal int := 0
    ]
    
    int rvprop length :: readLengthTo.
}

// --- MatrixIndexer ---

/// An indexer prototype
limited class Indexer2D
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
    
    int rvprop index1 :: readIndex1To.
    
    int rvprop index2 :: readIndex2To.
    
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
        $self readIndex1To vint:anIndex.
        
        anIndex := anIndex + anOffset int.
        
        $self writeIndex1 int:anIndex.
        
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

    int rvprop columns :: readColumnsTo.

    /// Reads the length to the output parameter    
    stacksafe readRownsTo vint:n
    [
    ]
    
    int rvprop rows :: readRownsTo.
}

// --- base functions ---

/// A base action
limited class BaseFunction
{
    /// Executes the action
    closure []
    
    /// Executes the action
    eval = $self().
    
    /// Executes the action while the condition is true
    sealed run while:aBool
    [
        while (aBool)
        [
            $self()
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

///// A base function
//limited class BaseFunction1
//{
//    /// Executes the function
//    action eval : aParam []
//
//    /// Executes the function for every enumeration item
//    run enumerator:anEnumerator
//    [
//        while(anEnumerator next)
//        [
//            $self eval:(anEnumerator get)
//        ].        
//    ]
//    
//    /// Executes the function for every enumeration item until it returns true
//    sealed retrieve enumerator:anEnumerator
//    [
//        object anItem := $nil.
//        while (anEnumerator next)
//        [
//            anItem := anEnumerator get.
//
//            if($self eval:anItem)
//                [ ^ anItem ]
//        ].
//        
//        ^ $nil.
//    ]
//}
//
///// A base two variable function
//limited class BaseFunction2
//{
//    /// Dispatches the verb to the target with func2 subject
//    /// Executes the function
//    action eval : aParam1 : aParam2 []
//    
//    /// Executes the function for parameters and every enumeration item
//    sealed run object:aParam enumerator:anEnumerator
//    [
//        while(anEnumerator next)
//        [
//            $self eval(aParam,anEnumerator get)
//        ]
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true
//    sealed retrieve object:aParam enumerator:anEnumerator
//    [
//        object anItem := $nil.
//        while (anEnumerator next)
//        [
//            anItem := anEnumerator get.
//            
//            // !! temporal
//            if ($self eval(aParam,anItem))
//                [ ^ anItem ]
//        ].        
//        
//        ^ $nil
//    ]
//}
//
///// A base three variable function
//limited class BaseFunction3
//{
//    /// Executes the function
//    action eval : aParam1 : aParam2 : aParam3 []
//    
//    /// Executes the function for parameters and every enumeration item
//    sealed run object:aParam1 object:aParam2 enumerator:anEnumerator
//    [
//        while (anEnumerator next)
//        [
//            $self eval(aParam1,aParam2,anEnumerator get)
//        ]
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    sealed retrieve object:aParam1 object:aParam2 enumerator:anEnumerator
//    [
//        object anItem := $nil.
//        while (anEnumerator next)
//        [
//            anItem := anEnumerator get.
//            
//            if ($self eval(aParam1,aParam2,anItem))
//                [ ^ anItem ]
//        ].        
//        
//        ^ $nil
//    ]
//}
//
///// A base four variable function
//limited class BaseFunction4
//{
//    /// Executes the function
//    action eval : aParam1 : aParam2 : aParam3 : aParam4 []
//    
//    /// Executes the function for parameters and every enumeration item
//    sealed run object:aParam1 object:aParam2 object:aParam3 enumerator:anEnumerator
//    [
//        while (anEnumerator next)
//        [
//            $self eval(aParam1,aParam2,aParam3,anEnumerator get)
//        ]
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    sealed retrieve object:aParam1 object:aParam2 object:aParam3 enumerator:anEnumerator
//    [
//        object anItem := $nil.
//        while (anEnumerator next)
//        [
//            anItem := anEnumerator get.
//            
//            // !! temporal
//            if ($self eval(aParam1,aParam2,aParam3,anItem))
//                [ ^ anItem ]
//        ].        
//        
//        ^ $nil
//    ]
//}
//
///// A base five variable function
//limited class BaseFunction5
//{
//    /// Executes the function
//    action eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 []
//    
//    /// Executes the function for parameters and every enumeration item
//    sealed run object:aParam1 object:aParam2 object:aParam3 object:aParam4 enumerator:anEnumerator
//    [
//        while (anEnumerator next)
//        [
//            $self eval(aParam1,aParam2,aParam3,aParam4,anEnumerator get)
//        ]
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    sealed retrieve object:aParam1 object:aParam2 object:aParam3 object:aParam4 enumerator:anEnumerator
//    [
//        object anItem := $nil.
//        while (anEnumerator next)
//        [
//            anItem := anEnumerator get.
//            
//            if ($self eval(aParam1,aParam2,aParam3,aParam4,anItem))
//                [ ^ anItem ]
//        ].        
//        
//        ^ $nil
//    ]
//}
//
///// A base six variable function
//limited class BaseFunction6
//{
//    /// Executes the function
//    action eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 []
//    
//    /// Executes the function for parameters and every enumeration item
//    sealed run object:aParam1 object:aParam2 object:aParam3 object:aParam4 object:aParam5 enumerator:anEnumerator
//    [
//        while (anEnumerator next)
//        [
//            $self eval(aParam1,aParam2,aParam3,aParam4,aParam5,anEnumerator get)
//        ]
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    sealed retrieve object:aParam1 object:aParam2 object:aParam3 object:aParam4 object:aParam5 enumerator:anEnumerator
//    [
//        object anItem := $nil.
//        while (anEnumerator next)
//        [
//            anItem := anEnumerator get.
//            
//            if ($self eval(aParam1,aParam2,aParam3,aParam4,aParam5,anItem))
//                [ ^ anItem ]
//        ].        
//        
//        ^ $nil
//    ]
//}
//
///// A base seven variable function
//limited class BaseFunction7
//{
//    /// Executes the function
//    action eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 []
//    
//    /// Executes the function for parameters and every enumeration item
//    sealed run object:aParam1 object:aParam2 object:aParam3 object:aParam4 object:aParam5 object:aParam6 enumerator:anEnumerator
//    [
//        while (anEnumerator next)
//        [
//            $self eval(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,anEnumerator get)
//        ]
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    sealed retrieve object:aParam1 object:aParam2 object:aParam3 object:aParam4 object:aParam5 object:aParam6 enumerator:anEnumerator
//    [
//        object anItem := $nil.
//        while (anEnumerator next)
//        [
//            anItem := anEnumerator get.
//            
//            if ($self eval(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,anItem))
//                [ ^ anItem ]
//        ].        
//        
//        ^ $nil
//    ]
//}
//
///// A base eight variable function
//limited class BaseFunction8
//{
//    /// Executes the function    
//    action eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 []
//    
//    /// Executes the function for parameters and every enumeration item
//    sealed run object:aParam1 object:aParam2 object:aParam3 object:aParam4 object:aParam5 object:aParam6 object:aParam7 enumerator:anEnumerator
//    [
//        while (anEnumerator next)
//        [
//            $self eval(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,aParam7,anEnumerator get)
//        ]
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    sealed retrieve object:aParam1 object:aParam2 object:aParam3 object:aParam4 object:aParam5 object:aParam6 object:aParam7 enumerator:anEnumerator
//    [
//        object anItem := $nil.
//        while (anEnumerator next)
//        [
//            anItem := anEnumerator get.
//            
//            if ($self eval(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,aParam7,anItem))
//                [ ^ anItem ]
//        ].        
//        
//        ^ $nil
//    ]
//}
//
///// A base nine variable function
//limited class BaseFunction9
//{
//    /// Executes the function
//    action eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 []
//    
//    /// Executes the function for parameters and every enumeration item
//    sealed run object:aParam1 object:aParam2 object:aParam3 object:aParam4 object:aParam5 object:aParam6 object:aParam7 object:aParam8 enumerator:anEnumerator
//    [
//        while (anEnumerator next)
//        [
//            $self eval(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,aParam7,aParam8,anEnumerator get)
//        ]
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    sealed retrieve object:aParam1 object:aParam2 object:aParam3 object:aParam4 object:aParam5 object:aParam6 object:aParam7 object:aParam8 enumerator:anEnumerator
//    [
//        object anItem := $nil.
//        while (anEnumerator next)
//        [
//            anItem := anEnumerator get.
//            
//            if ($self eval(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,aParam7,aParam8,anItem))
//                [ ^ anItem ]
//        ].        
//        
//        ^ $nil
//    ]
//}
//
///// A base ten variable function
//limited class BaseFunction10
//{
//    /// Executes the function    
//    action eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 []
//    
//    /// Executes the function for parameters and every enumeration item
//    sealed run object:aParam1 object:aParam2 object:aParam3 object:aParam4 object:aParam5 object:aParam6 object:aParam7 object:aParam8 object:aParam9 
//                enumerator:anEnumerator
//    [
//        while (anEnumerator next)
//        [
//            $self eval(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,aParam7,aParam8,aParam9,anEnumerator get)
//        ]
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    sealed retrieve object:aParam1 object:aParam2 object:aParam3 object:aParam4 object:aParam5 object:aParam6 object:aParam7 object:aParam8 object:aParam9 enumerator:anEnumerator
//    [
//        var anItem := $nil.
//        while (anEnumerator next)
//        [
//            anItem := anEnumerator get.
//            
//            if ($self eval(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,aParam7,aParam8,aParam9,anItem))
//                [ ^ anItem ]
//        ].        
//        
//        ^ $nil
//    ]
//}
//
///// A base eleven variable functions
//limited class BaseFunction11
//{
//    /// Executes the function
//    action eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 : aParam11 []
//    
//    /// Executes the function for parameters and every enumeration item
//    sealed run object:aParam1 object:aParam2 object:aParam3 object:aParam4 object:aParam5 object:aParam6 object:aParam7 object:aParam8 object:aParam9 
//               object:aParam10 enumerator:anEnumerator
//    [
//        while (anEnumerator next)
//        [
//            $self eval(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,aParam7,aParam8,aParam9,aParam10,anEnumerator get)
//        ]
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    sealed retrieve object:aParam1 object:aParam2 object:aParam3 object:aParam4 object:aParam5 object:aParam6 object:aParam7 object:aParam8 object:aParam9 
//                    object:aParam10 enumerator:anEnumerator
//    [
//        var anItem := $nil.
//        while (anEnumerator next)
//        [
//            anItem := anEnumerator get.
//            
//            if ($self eval(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,aParam7,aParam8,aParam9,aParam10,anItem))
//                [ ^ anItem ]
//        ].        
//        
//        ^ $nil
//    ]
//}
//
//// --- base lazy expression ---
//
//limited class BaseLazyExpression
//{
//    eval []
//    
//    dispatch => system'core_routines'__lazyexpression.
//}
//
//// --- IndexFunction ---
//
///// A base index function
//limited class IndexFunction1
//{
//    /// Executes the function
//    stacksafe action eval int:n []
//
//    /// Executes the function for every value in the specified integer range (starting value, ending value, -step)
//    sealed stacksafe rewind int:aStart int:n int:dx
//    [
//        int i := aStart.
//        
//        while (i >= n)
//        [
//            $self eval int:i.
//            
//            i := i - dx
//        ].
//        
//        ^ $nil
//    ]          
//    
//    /// Executes the function for every value in the specified range (starting value, ending value, step)
//    sealed stacksafe run int:aStart int:n int:dx
//    [
//        int i := aStart.
//        
//        while (i <= n)
//        [
//            $self eval int:i.
//            
//            i := i + dx
//        ].
//        
//        ^ $nil
//    ]
//
//    /// Executes the function for every value in the specified range (starting value, ending value, step) until it returns true    
//    sealed stacksafe retrieve int:aStart int:n int:dx
//    [
//        int i := aStart.
//        
//        while(i <= n)
//        [
//            if ($self eval int:i)
//                [ ^ IntNumber new int:i ].
//            
//            i := i + dx
//        ].
//        
//        ^ $nil
//    ]
//}
//
///// A base two variable index function
//limited class IndexFunction2
//{
//    /// Executes the function
//    stacksafe action eval int:i int:j []
//
//    /// Executes the function for every value in the specified integer area (starting x-value, starting y-value, ending x-value, ending y-value, -step)
//    sealed rewind int:i int:j int:n int:m int:dx
//    [
//        int x := i.
//        int y := j.
//        
//        while(x >= n)
//        [
//            y := j.
//            
//            while (y >= m)
//            [
//                $self eval int:x int:y.
//                
//                y := y - dx
//            ].
//                        
//            x := x - dx
//        ].
//        
//        ^ $nil
//    ]          
//    
//    /// Executes the function for every value in the specified range (starting x-value, starting y-value, ending x-value, ending y-value, step)
//    sealed run int:i int:j int:n int:m int:dx
//    [
//        int x := i.
//        int y := j.
//        
//        while(x <= n)
//        [
//            y := j.
//            
//            while (y <= m)
//            [
//                $self eval int:x int:y.
//                
//                y := y + dx
//            ].
//                        
//            x := x + dx
//        ].
//        
//        ^ $nil
//    ]
//}
//
///// A base three variable index function
//limited class IndexFunction3
//{
//    /// Executes the function
//    stacksafe action eval int:i int:j int:k []
//
//    /// Executes the function for every value in the specified integer area (starting x-value, starting y-value, starting z-value, ending x-value, ending y-value, ending z-value, -step)
//    sealed rewind int:i int:j int:k int:n int:m int:p int:dx
//    [
//        int x := i.
//        int y := j.
//        int z := k.
//        
//        while(x >= n)
//        [
//            y := j.
//            
//            while(y >= m)
//            [
//                z := k.
//                
//                while(z >= p)
//                [
//                    $self eval int:x int:y int:z.
//                    
//                    z := z - dx
//                ].                
//                
//                y := y - dx
//            ].
//                        
//            x := x - dx
//        ].
//        
//        ^ $nil
//    ]          
//    
//    /// Executes the function for every value in the specified range (starting x-value, starting y-value, starting z-value, ending x-value, ending y-value, ending z-value, step)
//    sealed run int:i int:j int:k int:n int:m int:p int:dx
//    [
//        int x := i.
//        int y := j.
//        int z := k.
//        
//        while(x <= n)
//        [
//            y := j.
//            
//            while(y <= m)
//            [
//                z := k.
//                
//                while(z <= p)
//                [
//                    $self eval int:x int:y int:z.
//                    
//                    z := z + dx
//                ].                
//                
//                y := y + dx
//            ].
//                        
//            x := x + dx
//        ].
//        
//        ^ $nil
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
//
//// --- BaseIndexer ---
//
//class BaseIndexer :: Indexer
//{
//    int theIndex.
//    int theBaseIndexerLength.
//    
//    /// Returns false if the index out of range
//    bool available = (theIndex < theBaseIndexerLength).
//    
//    /// Reads the current i-index into an output parameter
//    readIndexTo vint:retVal
//    [
//        retVal int := theIndex
//    ]    
//    
//    /// Sets the i-index by specified value
//    writeIndex int:anIndex 
//    [ 
//        theIndex := anIndex
//    ] 
//}
//
//// --- BaseMatrixIndexer ---
//
//class BaseMatrixIndexer :: Indexer2D
//{
//    int theIndex1.
//    int theIndex2.
//    int theLength1.
//    int theLength2.
//    
//    /// Returns false if the index out of range
//    bool available = (theIndex1 <= theLength1) && (theIndex2 <= theLength2).
//    
//    /// Reads the current i-index into an output parameter
//    readIndex1To vint:retVal
//    [
//        retVal int := theIndex1.
//    ]    
//    
//    /// Reads the current j-index into an output parameter
//    readIndex2To vint:retVal
//    [
//        retVal int := theIndex2.
//    ]
//    
//    /// Sets the i-index by specified value
//    writeIndex1 int:anIndex 
//    [ 
//        theIndex1 := anIndex.
//    ] 
//    
//    /// Sets the i-index by specified value
//    writeIndex2 int:anIndex 
//    [
//        theIndex2 := anIndex.
//    ] 
//
//    /// Reads the length to the output parameter    
//    stacksafe readSizeTo vint:n vint:m
//    [
//        n int := theLength1.
//        m int := theLength2.
//    ]
//
//    /// Reads the length to the output parameter    
//    readColumnsTo vint:m
//    [
//        m int := theLength2.
//    ]
//
//    /// Reads the length to the output parameter    
//    readRownsTo vint:n
//    [
//        n int := theLength1.
//    ]    
//}