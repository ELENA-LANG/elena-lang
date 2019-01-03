// === Basic prototypes ===

// --- Enumerator ---

/// An enumerator prototype
public interface Enumerator
{
    /// Returns the current object
    abstract get() {}
    
    /// Goes to the next enumeration member and returns true if it is not the last member
    abstract bool next() {}
    
    /// Resets the enumerator
    abstract reset() {}
    
    /// returns the enumerator owner
    abstract enumerable() {}
    
    Enumerator enumerator() = self;
}

// --- enumerator<> ---

public interface Enumerator<T>
{
    abstract bool next() {}
    
    abstract T get() {}
    
    /// Resets the enumerator
    abstract reset() {}
    
    /// returns the enumerator owner
    abstract enumerable() {}

    abstract Enumerator cast() {}
    
    Enumerator<T> enumerator() = self;
}

public abstract Enumerable
{
    abstract Enumerator enumerator() {}
}

public abstract Enumerable<T>
{
    abstract Enumerator<T> enumerator() {}
}

// --- Indexer ---

/// An indexer prototype
public interface Indexer
{
    /// Returns false if the index out of range
    abstract get bool Available() {}

    /// Returns the current member
    abstract get() {}
    
    /// Sets the current member
    abstract set(object) {}
    
    vprop int Index
    {
        read(ref int retVal)
        {
            self.readIndexTo(ref retVal)
        }
        
        write(int retVal)
        {
            self.writeIndex(retVal)
        }
    }

    /// Reads the current index into an output parameter
    abstract readIndexTo(ref int retVal) {}

    /// Sets the index by specified value
    abstract writeIndex(int anIndex) {}

    abstract appendIndex(int anIndex) {}

//    /// Reads the length to the output parameter    
//    abstract readLengthTo(ref int retVal) {}
    
//    int rvprop length :: readLengthTo.
}

public interface Indexer<T>
{
    /// Returns false if the index out of range
    abstract get bool Available() {}

    /// Returns the current member
    abstract T get() {}
    
    /// Sets the current member
    abstract set(T object) {}

    vprop int Index
    {
        read(ref int retVal)
        {
            self.readIndexTo(ref retVal)
        }
        
        write(int retVal)
        {
            self.writeIndex(retVal)
        }
    }

    /// Reads the current index into an output parameter
    abstract readIndexTo(ref int retVal) {}

    /// Sets the index by specified value
    abstract writeIndex(int anIndex) {}

    abstract appendIndex(int anIndex) {}
                                
//    /// Reads the length to the output parameter    
//    abstract readLengthTo(ref<int> retVal) []
//    
//    int rvprop length :: readLengthTo.
    
    abstract Indexer cast() {}
}

public abstract Indexable
{
   abstract Indexer indexer() {}
}

public abstract Indexable<T>
{
   abstract Indexer<T> indexer() {}
}

//// --- MatrixIndexer ---
//
///// An indexer prototype
//public interface Indexer2D
//{
//    /// Returns false if the index out of range
//    abstract bool available []
//
//    /// Returns the current member
//    abstract get []
//    
//    /// Sets the current member
//    abstract set : anObject []
//    
//    /// Reads the current i-index into an output parameter
//    abstract readIndex1To(ref<int> retVal) []
//    
//    /// Reads the current j-index into an output parameter
//    abstract readIndex2To(ref<int> retVal) []
//    
//    int rvprop index1 :: readIndex1To.
//    
//    int rvprop index2 :: readIndex2To.
//    
//    /// Sets the i-index by specified value
//    abstract writeIndex1(int i) [ ] 
//    
//    /// Sets the i-index by specified value
//    abstract writeIndex2(int j) [ ] 
//    
//    /// Sets the index by specified value
//    set index1(int i)
//        <= writeIndex1(i).
//    
//    /// Sets the index by specified value
//    set index2(int j)
//        <= writeIndex2(j).
//    
//    /// Increaments the index by specified value
//    appendIndex1(int offs)
//    [
//        int anIndex := 0.
//        self readIndex1To(&anIndex).
//        
//        anIndex := anIndex + offs.
//        
//        self writeIndex1(anIndex).
//    ]
//    
//    /// Increaments the index by specified value
//    appendIndex2(int offs)
//    [
//        int anIndex := 0.
//        self readIndex2To(&anIndex).
//        
//        anIndex := anIndex + offs.
//        
//        self writeIndex2(anIndex).
//    ]
//
//    /// Reads the length to the output parameter    
//    abstract readSizeTo(ref<int> n, ref<int> m)
//    [
//    ]
//
//    /// Reads the length to the output parameter    
//    abstract readColumnsTo(ref<int> m)
//    [
//    ]
//
//    int rvprop columns :: readColumnsTo.
//
//    /// Reads the length to the output parameter    
//    abstract readRowsTo(ref<int> n)
//    [
//    ]
//    
//    int rvprop rows :: readRowsTo.
//}

// --- base functions ---

/// A base action
public interface Func
{
    /// Executes the action
    abstract closure() {}
    
////    /// Executes the action
////    eval = $self().
////    
////    /// Executes the action while the condition is true
////    sealed run while:aBool
////    [
////        while (aBool)
////        [
////            $self()
////        ]
////    ]
////
////    /// Executes the action while the condition is false
////    ///
////    /// Executes at least once
////    sealed run until:aBool
////    [
////        bool flag := false.
////        
////        until (flag)
////        [
////            $self eval.
////            
////            flag := aBool bool
////        ].
////    ]
}

/// A base function
public interface Func1
{
    /// Executes the action
    abstract closure(param) {}
    
    /// Executes the function for every enumeration item
    run(Enumerator en)
    {
        while(en.next())
        {
            self(en.get())
        }
    }
    
    /// Executes the function for every enumeration item until it returns true
    retrieve(Enumerator en)
    {
        object item := nil;
        while (en.next())
        {
            item := en.get();

            if(self(item))
                 { ^ item }
        };
        
        ^ nil
    }
}

public interface Func<T, TResult>
{
    /// Executes the action
    abstract TResult closure(T param) {}
    
    run(Enumerator<T> e)
    {
        while(e.next())
        {
            self(e.get())
        }
    }
}

/// A base two variable function
public interface Func2
{
    /// Executes the action
    abstract closure(param1, param2) {}
    
//    /// Executes the function for parameters and every enumeration item
//    sealed run(object aParam, Enumerator anEnumerator)
//    [
//        while(anEnumerator next)
//        [
//            self(aParam,anEnumerator get).
//        ]
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true
//    sealed retrieve(object aParam, Enumerator anEnumerator)
//    [
//        object anItem := nil.
//        while (anEnumerator next)
//        [
//            anItem := anEnumerator get.
//            
//            // !! temporal
//            if (self(aParam,anItem))
//                [ ^ anItem ]
//        ].        
//        
//        ^ nil
//    ]
}

public interface Func<T1,T2,TResult>
{
    /// Executes the action
    abstract TResult closure(T1 param1, T2 param2) {}
    
//    run(enumerator<type> e)
//    [
//    ]
}

///// A base three variable function
//public interface Func3
//{
//    /// Executes the function
//    abstract closure(object aParam1, object aParam2, object aParam3) []
//    
////    /// Executes the function
////    eval : aParam1 : aParam2 : aParam3 = $self(aParam1,aParam2,aParam3).
//    
//    /// Executes the function for parameters and every enumeration item
//    sealed run(object aParam1, object aParam2, Enumerator anEnumerator)
//    [
//        while (anEnumerator next)
//        [
//            self(aParam1,aParam2,anEnumerator get)
//        ]
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    sealed retrieve(object aParam1, object aParam2, Enumerator anEnumerator)
//    [
//        object anItem := nil.
//        while (anEnumerator next)
//        [
//            anItem := anEnumerator get.
//            
//            if (self(aParam1,aParam2,anItem))
//                [ ^ anItem ]
//        ].        
//        
//        ^ nil
//    ]
//}
//
//public interface Func<type1,type2,type3, ret_type>
//{
//    /// Executes the action
//    abstract ret_type closure(type1 param1, type2 param2, type3 param3) []
//    
////    run(enumerator<type> e)
////    [
////    ]
//}
//
///// A base three variable function
//public interface Func4
//{
//    /// Executes the function
//    abstract closure(object aParam1, object aParam2, object aParam3, object aParam4) []
////    
////    /// Executes the function
////    eval : aParam1 : aParam2 : aParam3 : aParam4 = $self(aParam1,aParam2,aParam3,aParam4).
//    
//    /// Executes the function for parameters and every enumeration item
//    sealed run(object aParam1, object aParam2, object aParam3, Enumerator anEnumerator)
//    [
//        while (anEnumerator next)
//        [
//            self(aParam1,aParam2,aParam3,anEnumerator get)
//        ]
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    sealed retrieve(object aParam1, object aParam2, object aParam3, Enumerator anEnumerator)
//    [
//        object anItem := nil.
//        while (anEnumerator next)
//        [
//            anItem := anEnumerator get.
//            
//            // !! temporal
//            if (self(aParam1,aParam2,aParam3,anItem))
//                [ ^ anItem ]
//        ].        
//        
//        ^ nil
//    ]
//}
//
//public interface Func<type1,type2,type3,type4, ret_type>
//{
//    /// Executes the action
//    abstract ret_type closure(type1 param1, type2 param2, type3 param3, type4 param4) []
//    
////    run(enumerator<type> e)
////    [
////    ]
//}
//
///// A base five variable function
//public interface Func5
//{
//    /// Executes the function
//    abstract closure(object aParam1, object aParam2, object aParam3, object aParam4, object aParam5) []
//
////    /// Executes the function
////    eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 = $self(aParam1,aParam2,aParam3,aParam4,aParam5).
//    
//    /// Executes the function for parameters and every enumeration item
//    sealed run(object aParam1, object aParam2, object aParam3, object aParam4, Enumerator anEnumerator)
//    [
//        while (anEnumerator next)
//        [
//            self(aParam1,aParam2,aParam3,aParam4,anEnumerator get)
//        ]
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    sealed retrieve(object aParam1, object aParam2, object aParam3, object aParam4, Enumerator anEnumerator)
//    [
//        object anItem := nil.
//        while (anEnumerator next)
//        [
//            anItem := anEnumerator get.
//            
//            if (self(aParam1,aParam2,aParam3,aParam4,anItem))
//                [ ^ anItem ]
//        ].        
//        
//        ^ nil
//    ]
//}
//
//public interface Func<type1,type2,type3,type4,type5, ret_type>
//{
//    /// Executes the action
//    abstract ret_type closure(type1 param1, type2 param2, type3 param3, type4 param4, type5 param5) []
//    
////    run(enumerator<type> e)
////    [
////    ]
//}
//
///// A base six variable function
//public interface Func6
//{
//    /// Executes the function
//    abstract closure(object aParam1, object aParam2, object aParam3, object aParam4, object aParam5, object aParam6) []
//
////    /// Executes the function
////    eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 = $self(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6).
//    
//    /// Executes the function for parameters and every enumeration item
//    sealed run(object aParam1, object aParam2, object aParam3, object aParam4, object aParam5, Enumerator anEnumerator)
//    [
//        while (anEnumerator next)
//        [
//            self(aParam1,aParam2,aParam3,aParam4,aParam5,anEnumerator get)
//        ]
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    sealed retrieve(object aParam1, object aParam2, object aParam3, object aParam4, object aParam5, Enumerator anEnumerator)
//    [
//        object anItem := nil.
//        while (anEnumerator next)
//        [
//            anItem := anEnumerator get.
//            
//            if (self(aParam1,aParam2,aParam3,aParam4,aParam5,anItem))
//                [ ^ anItem ]
//        ].        
//        
//        ^ nil
//    ]
//}
//
//public interface Func<type1,type2,type3,type4,type5,type6, ret_type>
//{
//    /// Executes the action
//    abstract ret_type closure(type1 param1, type2 param2, type3 param3, type4 param4, type5 param5, type6 param6) []
//    
////    run(enumerator<type> e)
////    [
////    ]
//}
//
///// A base seven variable function
//public interface Func7
//{
//    /// Executes the function
//    abstract closure(object aParam1, object aParam2, object aParam3, object aParam4, object aParam5, object aParam6, object aParam7) []
////    
////    eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 
////        = $self(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,aParam7).
//    
//    /// Executes the function for parameters and every enumeration item
//    sealed run(object aParam1, object aParam2, object aParam3, object aParam4, object aParam5, object aParam6, Enumerator anEnumerator)
//    [
//        while (anEnumerator next)
//        [
//            self(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,anEnumerator get)
//        ]
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    sealed retrieve(object aParam1, object aParam2, object aParam3, object aParam4, object aParam5, object aParam6, Enumerator anEnumerator)
//    [
//        object anItem := nil.
//        while (anEnumerator next)
//        [
//            anItem := anEnumerator get.
//            
//            if (self(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,anItem))
//                [ ^ anItem ]
//        ].        
//        
//        ^ nil
//    ]
//}
//
//public interface Func<type1,type2,type3,type4,type5,type6,type7, ret_type>
//{
//    /// Executes the action
//    abstract ret_type closure(type1 param1, type2 param2, type3 param3, type4 param4, type5 param5, type6 param6, type7 param7) []
//    
////    run(enumerator<type> e)
////    [
////    ]
//}
//
///// A base eight variable function
//public interface Func8
//{
//    /// Executes the function
//    abstract closure(object aParam1, object aParam2, object aParam3, object aParam4, object aParam5, object aParam6, 
//                        object aParam7, object aParam8) []
////    
////    /// Executes the function    
////    eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8
////        = $self(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,aParam7,aParam8).
//    
//    /// Executes the function for parameters and every enumeration item
//    sealed run(object aParam1, object aParam2, object aParam3, object aParam4, object aParam5, object aParam6, object aParam7, Enumerator anEnumerator)
//    [
//        while (anEnumerator next)
//        [
//            self(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,aParam7,anEnumerator get)
//        ]
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    sealed retrieve(object aParam1, object aParam2, object aParam3, object aParam4, object aParam5, object aParam6, object aParam7, Enumerator anEnumerator)
//    [
//        object anItem := nil.
//        while (anEnumerator next)
//        [
//            anItem := anEnumerator get.
//            
//            if (self(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,aParam7,anItem))
//                [ ^ anItem ]
//        ].        
//        
//        ^ nil
//    ]
//}
//
//public interface Func<type1,type2,type3,type4,type5,type6,type7,type8, ret_type>
//{
//    /// Executes the action
//    abstract ret_type closure(type1 param1, type2 param2, type3 param3, type4 param4, type5 param5, type6 param6, type7 param7, type8 param8) []
//    
////    run(enumerator<type> e)
////    [
////    ]
//}
//
///// A base nine variable function
//public interface Func9
//{
//    /// Executes the function
//    abstract closure(object aParam1, object aParam2, object aParam3, object aParam4, object aParam5, object aParam6, 
//                        object aParam7, object aParam8, object aParam9) []
//
////    /// Executes the function
////    eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9
////        = $self(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,aParam7,aParam8,aParam9).
//    
//    /// Executes the function for parameters and every enumeration item
//    sealed run(object aParam1, object aParam2, object aParam3, object aParam4, object aParam5, object aParam6, object aParam7, 
//                object aParam8, Enumerator anEnumerator)
//    [
//        while (anEnumerator next)
//        [
//            self(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,aParam7,aParam8,anEnumerator get)
//        ]
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    sealed retrieve(object aParam1, object aParam2, object aParam3, object aParam4, object aParam5, object aParam6, 
//                        object aParam7, object aParam8, Enumerator anEnumerator)
//    [
//        object anItem := nil.
//        while (anEnumerator next)
//        [
//            anItem := anEnumerator get.
//            
//            if (self(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,aParam7,aParam8,anItem))
//                [ ^ anItem ]
//        ].        
//        
//        ^ nil
//    ]
//}
//
//public interface Func<type1,type2,type3,type4,type5,type6,type7,type8,type9, ret_type>
//{
//    /// Executes the action
//    abstract ret_type closure(type1 param1, type2 param2, type3 param3, type4 param4, type5 param5, type6 param6, type7 param7, type8 param8, type9 param9) []
//    
////    run(enumerator<type> e)
////    [
////    ]
//}
//
///// A base ten variable function
//public interface Func10
//{
//    /// Executes the function
//    abstract closure(object aParam1, object aParam2, object aParam3, object aParam4, object aParam5, object aParam6, 
//                        object aParam7, object aParam8, object aParam9, object aParam10) []
//
////    /// Executes the function    
////    eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10
////        = $self(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,aParam7,aParam8,aParam9,aParam10).
//    
//    /// Executes the function for parameters and every enumeration item
//    sealed run(object aParam1, object aParam2, object aParam3, object aParam4, object aParam5, object aParam6, object aParam7, object aParam8, object aParam9,
//                Enumerator anEnumerator)
//    [
//        while (anEnumerator next)
//        [
//            self(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,aParam7,aParam8,aParam9,anEnumerator get)
//        ]
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    sealed retrieve(object aParam1, object aParam2, object aParam3, object aParam4, object aParam5, object aParam6, object aParam7, 
//                        object aParam8, object aParam9, Enumerator anEnumerator)
//    [
//        var anItem := nil.
//        while (anEnumerator next)
//        [
//            anItem := anEnumerator get.
//            
//            if (self(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,aParam7,aParam8,aParam9,anItem))
//                [ ^ anItem ]
//        ].        
//        
//        ^ nil
//    ]
//}
//
//public interface Func<type1,type2,type3,type4,type5,type6,type7,type8,type9,type10, ret_type>
//{
//    /// Executes the action
//    abstract ret_type closure(type1 param1, type2 param2, type3 param3, type4 param4, type5 param5, type6 param6, type7 param7, type8 param8,
//                            type9 param9, type10 param10) []
//    
////    run(enumerator<type> e)
////    [
////    ]
//}
//
///// A base eleven variable functions
//public interface Func11
//{
//    /// Executes the function
//    abstract closure(object aParam1, object aParam2, object aParam3, object aParam4, object aParam5, object aParam6, 
//                        object aParam7, object aParam8, object aParam9, object aParam10, object aParam11) []
//
////    
////    /// Executes the function
////    eval : aParam1 : aParam2 : aParam3 : aParam4 : aParam5 : aParam6 : aParam7 : aParam8 : aParam9 : aParam10 : aParam11
////        = $self(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,aParam7,aParam8,aParam9,aParam10,aParam11).
//    
//    /// Executes the function for parameters and every enumeration item
//    sealed run(object aParam1, object aParam2, object aParam3, object aParam4, object aParam5, object aParam6, object aParam7, object aParam8, object aParam9,
//               object aParam10, Enumerator anEnumerator)
//    [
//        while (anEnumerator next)
//        [
//            self(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,aParam7,aParam8,aParam9,aParam10,anEnumerator get)
//        ]
//    ]
//    
//    /// Executes the function for parameters and every enumeration item until it returns true    
//    sealed retrieve(object aParam1, object aParam2, object aParam3, object aParam4, object aParam5, object aParam6, object aParam7, 
//                        object aParam8, object aParam9, object aParam10, Enumerator anEnumerator)
//    [
//        var anItem := nil.
//        while (anEnumerator next)
//        [
//            anItem := anEnumerator get.
//            
//            if (self(aParam1,aParam2,aParam3,aParam4,aParam5,aParam6,aParam7,aParam8,aParam9,aParam10,anItem))
//                [ ^ anItem ]
//        ].        
//        
//        ^ nil
//    ]
//}
//
//public interface Func<type1,type2,type3,type4,type5,type6,type7,type8,type9,type10,type11, ret_type>
//{
//    /// Executes the action
//    abstract ret_type closure(type1 param1, type2 param2, type3 param3, type4 param4, type5 param5, type6 param6, type7 param7, type8 param8,
//                            type9 param9, type10 param10, type11 param11) []
//    
////    run(enumerator<type> e)
////    [
////    ]
//}
//
//// --- base lazy expression ---
//
//public abstract closed BaseLazyExpression
//{
//    abstract eval []
//    
//    dispatch => system'core_routines'__lazyexpression.
//}
//
//// --- IndexFunction ---
//
//public extension<Func<int,object>> indexFunctionOp
//{
//    /// Executes the function for every value in the specified integer range (starting value, ending value, -step)
//    rewind(int aStart, int n, int dx)
//    [
//        int i := aStart.
//        
//        while (i >= n)
//        [
//            self(i).
//            
//            i := i - dx
//        ].
//        
//        ^ nil
//    ]          
//    
//    /// Executes the function for every value in the specified range (starting value, ending value, step)
//    run(int aStart, int n, int dx)
//    [
//        int i := aStart.
//        
//        while (i <= n)
//        [
//            self(i).
//            
//            i := i + dx
//        ].
//        
//        ^ nil
//    ]
//}
//
/////// A base index function
////limited closure class IndexFunction1
////{
////    /// Executes the function
////    stacksafe closure int:n []
////
////    stacksafe eval int:n = $self(n).
////
////
////    /// Executes the function for every value in the specified range (starting value, ending value, step) until it returns true    
////    sealed stacksafe retrieve int:aStart int:n int:dx
////    [
////        int i := aStart.
////        
////        while(i <= n)
////        [
////            if ($self(i))
////                [ ^ IntNumber new int:i ].
////            
////            i := i + dx
////        ].
////        
////        ^ $nil
////    ]
////}
////
/////// A base two variable index function
////limited closure class IndexFunction2
////{
////    /// Executes the function
////    stacksafe closure(int i, int j)[]
////
////    stacksafe eval int:n int:m = $self(n,m).
////
////    /// Executes the function for every value in the specified integer area (starting x-value, starting y-value, ending x-value, ending y-value, -step)
////    sealed rewind int:i int:j int:n int:m int:dx
////    [
////        int x := i.
////        int y := j.
////        
////        while(x >= n)
////        [
////            y := j.
////            
////            while (y >= m)
////            [
////                $self eval int:x int:y.
////                
////                y := y - dx
////            ].
////                        
////            x := x - dx
////        ].
////        
////        ^ $nil
////    ]          
////    
////    /// Executes the function for every value in the specified range (starting x-value, starting y-value, ending x-value, ending y-value, step)
////    sealed run int:i int:j int:n int:m int:dx
////    [
////        int x := i.
////        int y := j.
////        
////        while(x <= n)
////        [
////            y := j.
////            
////            while (y <= m)
////            [
////                $self eval int:x int:y.
////                
////                y := y + dx
////            ].
////                        
////            x := x + dx
////        ].
////        
////        ^ $nil
////    ]
////}
////
/////// A base three variable index function
////limited closure class IndexFunction3
////{
////    /// Executes the function
////    stacksafe closure(int i, int j, int k) []
////
////    stacksafe eval int:n int:m int:k = $self(n,m,k).
////
////    /// Executes the function for every value in the specified integer area (starting x-value, starting y-value, starting z-value, ending x-value, ending y-value, ending z-value, -step)
////    sealed rewind int:i int:j int:k int:n int:m int:p int:dx
////    [
////        int x := i.
////        int y := j.
////        int z := k.
////        
////        while(x >= n)
////        [
////            y := j.
////            
////            while(y >= m)
////            [
////                z := k.
////                
////                while(z >= p)
////                [
////                    $self eval int:x int:y int:z.
////                    
////                    z := z - dx
////                ].                
////                
////                y := y - dx
////            ].
////                        
////            x := x - dx
////        ].
////        
////        ^ $nil
////    ]          
////    
////    /// Executes the function for every value in the specified range (starting x-value, starting y-value, starting z-value, ending x-value, ending y-value, ending z-value, step)
////    sealed run int:i int:j int:k int:n int:m int:p int:dx
////    [
////        int x := i.
////        int y := j.
////        int z := k.
////        
////        while(x <= n)
////        [
////            y := j.
////            
////            while(y <= m)
////            [
////                z := k.
////                
////                while(z <= p)
////                [
////                    $self eval int:x int:y int:z.
////                    
////                    z := z + dx
////                ].                
////                
////                y := y + dx
////            ].
////                        
////            x := x + dx
////        ].
////        
////        ^ $nil
////    ]
////}
////
//////// --- Boolean ---
//////
///////// A boolean variable. 
//////#class(varof:bool) Boolean :: BaseValue
//////{
//////    /// Creates a boolean variable with an initial value (false)
//////    #constructor new
//////    [        
//////        $value := false.
//////    ]
//////
//////    /// Creates a boolean variable with specified value
//////    #constructor new : aValue
//////    [        
//////        $value := true equal &bool:(aValue bool).
//////    ]
//////    
//////    /// Creates a boolean variable with specified value
//////    #constructor new &bool:aValue
//////    [        
//////        $value := aValue.
//////    ]
//////
//////    /// Returns the boolean value
//////    #method bool = $value.
//////
//////    /// Executes logical OR operation
//////    #method append : aValue
//////    [
//////        $value := $value or:aValue.
//////    ]
//////
//////    /// Executes logical AND operation            
//////    #method multiplyBy : aValue
//////    [
//////        $value := $value and:aValue.
//////    ]
//////            
//////    /// Inverts the boolean value
//////    #method invert
//////    [
//////        $value := $value not bool.
//////    ]            
//////
//////    /// Clones the object
//////    #method clone = Boolean new:$value.
//////}

// --- BaseEnumerator<type> ---

public abstract class BaseEnumerator<T> : Enumerator<T>
{
    ref<int> theIndex;
    int      theLength;
    
    bool next()
    {
        theIndex.append(1);
        
        ^ (cast int(theIndex)) < theLength
    }
    
    Enumerator cast() = new Enumerator:{ embeddable dispatch() => self; };
}

//// --- BaseIndexer ---
//
//public abstract class BaseIndexer :: Indexer
//{
//    int theIndex.
//    int theLength.
//    
//    /// Returns false if the index out of range
//    bool available = (theIndex < theLength).
//    
//    /// Reads the current i-index into an output parameter
//    readIndexTo(ref<int> retVal)
//    [
//        retVal value := theIndex
//    ]    
//    
//    /// Sets the i-index by specified value
//    writeIndex(int anIndex)
//    [ 
//        theIndex := anIndex
//    ] 
//    
//    appendIndex(int anIndex)
//    [ 
//        theIndex := theIndex + anIndex.
//    ] 
//    
//    readLengthTo(ref<int> retVal)
//    [
//        retVal value := theLength.
//    ]    
//}

public abstract BaseIndexer<T> : Indexer<T>
{
    int theIndex;
    int theLength;
    
    /// Returns false if the index out of range
    bool Available = (theIndex < theLength);
    
    /// Reads the current i-index into an output parameter
    readIndexTo(ref int retVal)
    {
        retVal := theIndex
    }    
    
    /// Sets the i-index by specified value
    writeIndex(int anIndex)
    {
        theIndex := anIndex
    } 
    
    appendIndex(int anIndex)
    { 
        theIndex := theIndex + anIndex
    }     
    
//    readLengthTo(ref<int> retVal)
//    [
//        retVal value := theLength.
//    ]
    
    Indexer cast() = new Indexer:{ embeddable dispatch() => self; };
}

//// --- BaseMatrixIndexer ---
//
//public abstract BaseMatrixIndexer :: Indexer2D
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
//    readIndex1To(ref<int> retVal)
//    [
//        retVal value := theIndex1.
//    ]    
//    
//    /// Reads the current j-index into an output parameter
//    readIndex2To(ref<int> retVal)
//    [
//        retVal value := theIndex2.
//    ]
//    
//    /// Sets the i-index by specified value
//    writeIndex1(int anIndex)
//    [ 
//        theIndex1 := anIndex.
//    ] 
//    
//    /// Sets the i-index by specified value
//    writeIndex2(int anIndex)
//    [
//        theIndex2 := anIndex.
//    ] 
//
//    /// Reads the length to the output parameter    
//    readSizeTo(ref<int> n, ref<int> m)
//    [
//        n value := theLength1.
//        m value := theLength2.
//    ]
//
//    /// Reads the length to the output parameter    
//    readColumnsTo(ref<int> retVal)
//    [
//        retVal value := theLength2.
//    ]
//
//    /// Reads the length to the output parameter    
//    readRowsTo(ref<int> retVal)
//    [
//        retVal value := theLength1.
//    ]    
//}

// --- Range --

class IntRangeEnumerator : Enumerator<int>
{
    int theIndex;
    int theStart;
    int theEnd;

    constructor(IntNumber start, IntNumber end)
    {
        theStart := start;
        theEnd := end;
        
        self.reset();
    }

    int get() = theIndex;
            
    /// Returns false if the index out of range
    bool next()
    {
        theIndex += 1;
        
        ^ theIndex < theEnd
    }
    
    /// Sets the i-index by specified value
    reset()
    {
        theIndex := theStart - 1;
    } 
    
    Enumerator cast() = new Enumerator:{ embeddable dispatch() => self; };
    
    enumerable() = new Range(theStart, theEnd - theStart);
}

public sealed class Range : Enumerable<int>
{
    int theStart;
    int theEnd;
    
    constructor(IntNumber start, IntNumber count)
    {
        theStart := start;
        theEnd := start + count
    }
    
//    constructor from(IntNumber start) till(IntNumber end)
//    [
//        theStart := start.
//        theEnd := end.
//    ]
//    
//    constructor from(IntNumber start) to(IntNumber end)
//    [
//        theStart := start.
//        theEnd := end + 1.
//    ]
//    
    vprop int Length
    {
        read(ref int retVal)
        {
            int l := theEnd - theStart;
            
            retVal := l
        }
    }
    
    Enumerator<int> enumerator() = new IntRangeEnumerator(theStart, theEnd);
    
    Enumerable cast() = new Enumerable:{ Enumerator enumerator() => self; };
}
