
// --- Array template ---

public sealed Array<T> : Indexable<T>
{
    embeddable T[] theArray;

//    constructor allocate(int length)
//        = new T[](length);
        
//    constructor(T[] array, int length)
//        <= allocate(length)
//    {
//        for(int i := 0 , i < length, i += 1)
//        {
//            theArray[i] := array[i]
//        }
//    }

    Enumerator<T> enumerator() = new BaseEnumerator<T>:
    {
        this theIndex := -1;
        
        this theLength := self.Length;
        
        T get() = self.at(this theIndex.Value);
        
        enumerable() = self;
        
        reset()
        {
            this theIndex := -1
        }
    };

//    cast T<Indexable> = Indexable::{ /*embeddable dispatch => self.*/T<Indexer> indexer => self. }.
//    
//    cast Enumerable<template_subj> = Enumerable<template_subj>::{ /*embeddable dispatch => self.*/Enumerator<template_subj> enumerator => self. }.
//
//    cast T<Enumerable> = Enumerable::{ /*embeddable dispatch => self.*/T<Enumerator> enumerator => self. }.

    Indexer<T> indexer() = new BaseIndexer<T>:
    {
        this theIndex := 0;
        
        this theLength := self.Length;
        
        T get()
            = self.at(theIndex);
            
        set(T o)
        {
            self.setAt(theIndex, o)
        }
    };

    vprop int Length
    {
        read(ref int retVal)
        {
            int len := 0;
            theArray $shr len;
            
            retVal := len
        }
    }

    T at(int n)
    {
        if (n < 0)
            { system'InvalidArgumentException.raise() };
        
        int len := 0;
        theArray $shr len;

        if (n < len)
        { 
            ^ theArray[n] 
        }
        else
        {
            system'InvalidArgumentException.raise()
        }
    }

    setAt(int n, T item)
    {
        if (n < 0)
            { system'InvalidArgumentException.raise() };
        
        int len := 0;
        theArray $shr len;

        if (n < len)
        {
            theArray[n] := item
        }
        else
        {
            system'InvalidArgumentException.raise()
        }
    }
               
//    setAt(object n, object object)
//        <= setAt(T<int>(n), T<template_subj>(object)).
//                  
//    /// Returns the item at the specified index
//    template_subj getAt : aParam
//        <= getAt(T<int>(aParam)).
}

// --- Array template ---

public sealed nonstructural VariadicArray<T> //:: Indexable<template_subj>
{
    embeddable T[] theArray;

    vprop int Length
    {
        read(ref int retVal)
            => system'core_routines'__args;
    }
    
    Array<T> cast()
    {
        int len := self.Length;
        
        T[] array := new T[](len);
        for(int i := 0, i < len, i += 1)
        {
            array[i] := theArray[i]
        };
        
        ^ array
    }
}

//// --- matrixof ---
//
//public sealed Matrix<template_subj>
//{
//    Array<template_subj> theArray.
//    int                  theRows.
//    int                  theColumns.
//    
//    constructor(params)
//    [
//        theRows := params rows.
//        theColumns := params columns.
//        
//        int len := theRows * theColumns.
//        
//        theArray := V<template_subj>(/*theRows * theColumns*/len).
//        
//        auto it := T<Indexer2D>(params) ?? nil.
//        ifnot(nil == it)
//            [
//                int i := 0.
//                int j := 0.
//                
//                while (i < theRows)
//                [
//                    j := 0.
//                    while (j < theColumns)
//                    [
//                        it writeIndex1(i).
//                        it writeIndex2(j).
//                        
//                        template_subj val := it get.
//                        
//                        self setAt(i, j, val).
//                        
//                        j := j + 1
//                    ].
//                    
//                    i := i + 1
//                ]
//            ]
//    ]
//    
//    /// Reads the matrix lengths to the output parameter
//    readLengths(ref<int> Rows, ref<int> Columns)
//    [
//        Rows value := theRows.
//        Columns value := theColumns.
//    ]
//
//    int rvprop length  :: readLengthTo.    
//    int rvprop columns :: readColumnsTo.
//    int rvprop rows    :: readRowsTo.
//
//    readLengthTo(ref<int> retVal)
//    [
//        retVal int := theRows.
//    ]
//
//    readColumnsTo(ref<int> retVal)
//    [
//        retVal value := theColumns.
//    ]
//
//    readRowsTo(ref<int> retVal)
//    [
//        retVal value := theRows.
//    ]
//
//    /// Writes an integer value at the specified position
//    setAt(int i, int j, template_subj v)
//    [
//        theArray setAt(i * theColumns + j, v).
//    ]
//
//    embeddable template_subj getAt(int i, int j)
//        = theArray getAt(i * theColumns + j).
//
//    getAt(i,j)
//        = self getAt(T<int>(i), T<int>(j)).
//
//    setAt(i, j, v)
//    [
//        self setAt(T<int>(i), T<int>(j), T<template_subj>(v)).    
//    ]
//
//    /// Returns a row
//    getAt(i) =
//    {
//        length = theColumns.
//        
//        getAt(j)
//            = self getAt(i,j).
//        
//        setAt(j, v)
//        [
//            self setAt(i,j,v).
//        ]
//    }.
////    
////    indexer2d = Indexer2D
////    {
////        
////    }.
//
//    enumerator 
//    [
//        ref<int> i := 0.
//        ref<int> j := -1.
//        int length1 := theRows.
//        int length2 := theColumns.
//        
//        ^ Enumerator::
//          {
//              get = self getAt(i value, j value).
//              
//              enumerable = self.
//              
//              reset [ i value := 0. j value := -1. ]
//              
//              bool next
//              [
//                  ifnot (i < length1)
//                  [
//                     ^ false.
//                  ].
//                  
//                  j := j + 1.
//                  
//                  ifnot (j < length2)
//                  [
//                      j := -1.
//                      i := i + 1.
//                      
//                      ^ false
//                  ].
//                  
//                  ^ true
//              ]
//          }.
//    ]
//}
//
//// === basic memory classes ===
//
//// --- ByteArray ---
//
///// A byte array
//public sealed ByteArray :: Indexable<byte>, Array<byte>
//{
//    /// Creates the byte array with the specified length
//    constructor new(int len)
//        = dynamic<byte>(len).
//
//    /// Creates the copy of a byte subarray
//    constructor new(int anIndex, int aLength, ByteArray anArray)
//        <= new(aLength);
//    [
//        anArray read(anIndex, aLength, self).
//    ]
//
//    /// Creates the byte array copy
//    constructor new(ByteArray anArray)
//        <= new(0, anArray length, anArray).
//
//    /// Reads the array length to the output parameter
//    readLengthTo(ref<int> aLength)
//        => system'core_routines'__bytearray.
//
//    /// Fills the sub array with a specified value
//    fill(int anIndex, int aLength, byte aValue)
//    [
//        int aTotalLength := self length.
//        
//        int i := anIndex.
//        int m := anIndex + aLength.
//        
//        if (i > aTotalLength)
//            [ system'InvalidArgumentException new; raise ].
//        
//        if (m > aTotalLength)
//            [ system'InvalidArgumentException new; raise ].
//            
//        while(i < m)
//        [
//            self write(i, aValue).
//            
//            i := i + 1
//        ]
//    ]
//
//    /// Copies a sub array to the provided one at the specified position
//    read(int anIndex, int aLength, ByteArray aDump)
//        => system'core_routines'__bytearray.
//
//    /// Copies the byte array at the specified position
//    write(int anIndex, int aLength, ByteArray aDump)
//        => system'core_routines'__bytearray.
//
//    /// Writes the byte value at specified position
//    write(int anIndex, byte anInt)
//        => system'core_routines'__bytearray.
//
//    /// Reads a byte value at the specified position and saves it into an output variable
//    read(int anIndex, ref<byte> anInt)
//        => system'core_routines'__bytearray.
//
//    embeddable byte getAt(int n)
//    [
//        byte retVal := 0.
//        
//        self read(n, &retVal).
//        
//        ^ retVal
//    ]
//
//    /// Writes a short integer value at the specified position
//    write(int anIndex, short anInt)
//        => system'core_routines'__bytearray.
//
//    /// Reads a short integer value at the specified position and saves it into an output variable
//    read(int anIndex, ref<short> aChar)
//        => system'core_routines'__bytearray.
//
//    /// Writes a character value at the specified position
//    write(int anIndex, char anInt)
//        => system'core_routines'__bytearray.
//
//    /// Reads a character value at the specified position and saves it into an output variable
//    read(int anIndex, ref<char> aChar)
//        => system'core_routines'__bytearray.
//
//    /// Writes an integer value at the specified position
//    write(int anIndex, int anInt)
//        => system'core_routines'__bytearray.
//
//    /// Reads an integer value at the specified position and saves it into an output variable
//    read(int anIndex, ref<int> anInt)
//        => system'core_routines'__bytearray.
//
//    /// Writes a long integer value at the specified position
//    write(int anIndex, long aLong)
//        => system'core_routines'__bytearray.
//
//    /// Reads a long integer value at the specified position and saves it into an output variable
//    read(int anIndex, ref<long> aLong)
//        => system'core_routines'__bytearray.
//
//    /// Writes a real value at the specified position
//    write(int anIndex, real aLong)
//        => system'core_routines'__bytearray.
//
//    /// Reads a real value at the specified position and saves it into an output variable
//    read(int anIndex, ref<real> aLong)
//        => system'core_routines'__bytearray.
//
//    move(int anIndex, int aLength, int anOffset)
//        => system'core_routines'__bytearray.
//    
//    /// Concatinates the arrays    
//    T<ByteArray> add(int aLength2, ByteArray anArray)
//    [
//        int aLength1 := self length.
//        
//        T<ByteArray> aRetVal := ByteArray new(aLength1 + aLength2).
//
//        byte tmp := 0.
//        int anIndex := 0.
//        while(anIndex < aLength1)
//        [
//            self read(anIndex, &tmp).
//            aRetVal write(anIndex, tmp).
//            
//            anIndex := anIndex + 1
//        ].
//
//        anIndex := 0.
//        while(anIndex < aLength2)
//        [
//            anArray read(anIndex, &tmp).
//            aRetVal write(aLength1 + anIndex, tmp).
//            
//            anIndex := anIndex + 1
//        ].
//            
//        ^ aRetVal
//    ]    
//
//    /// Creates the object copy
//    clone = ByteArray new(self).
//}
//
//// --- ShortArray ---
//
///// A short integer array
//public sealed ShortArray :: Indexable<short>, Array<short>
//{
//    /// Creates the short array with the specified length
//    constructor new(int aLength)
//        = dynamic<short>(aLength).
//    
//    /// Creates the copy of a short subarray
//    constructor new(int anIndex, int aLength, ShortArray anArray)
//        <= new(aLength);
//    [
//        anArray read(anIndex, aLength, self).
//    ]
//
//    /// Creates the short array copy
//    constructor new(ShortArray anArray)
//        <= new(0, anArray length, anArray).
//
//    /// Reads the array length to the output parameter
//    readLengthTo(ref<int> aLength)
//        => system'core_routines'__shortarray.
//
//    /// Fills the sub array with a specified value
//    fill(int anIndex, int aLength, short aValue)
//    [
//        int aTotalLength := self length.
//        
//        int i := anIndex.
//        int m := anIndex + aLength.
//        
//        if (i > aTotalLength)
//            [ system'InvalidArgumentException new; raise ].
//        
//        if (m > aTotalLength)
//            [ system'InvalidArgumentException new; raise ].
//            
//        while(i < m)
//        [
//            self write(i, aValue).
//            
//            i := i + 1
//        ]
//    ]
//    
//    /// Copies a sub array to the provided one at the specified position
//    read(int anIndex, int aLength, ShortArray anArray)
//        => system'core_routines'__shortarray.
//
//    /// Copies the short array at the specified position   
//    write(int anIndex, int aLength, ShortArray anArray)
//        => system'core_routines'__shortarray.
//
//    /// Writes a short integer value at the specified position
//    write(int anIndex, short aValue)
//        => system'core_routines'__shortarray.
//
//    embeddable short getAt(int n)
//    [
//        short retVal := 0.
//        
//        self read(n, &retVal).
//        
//        ^ retVal
//    ]
//
//    /// Reads a short integer value at the specified position and saves it into an output variable
//    read(int anIndex, ref<short> aValue)
//        => system'core_routines'__shortarray.
//
//    /// Creates the object copy
//    clone = ShortArray new(self).
//}
//
//// --- IntArray ---
//
///// An array of integers
//public sealed IntArray :: Indexable<int>, Array<int>
//{
//    /// Creates the array with the specified length
//    private constructor newPrivate(int length)
//        = dynamic<int>(length).
//        
//    /// Creates the array with the specified length
//    constructor new(int length)
//        <= newPrivate(length);
//    [
//        int i := 0.
//        while (i < length)
//        [
//            self[i] := 0.
//            
//            i := i + 1
//        ]
//    ]
//
////    /// Creates the array with the specified length
////    constructor new : aLength
////        <= new int:aLength.
//
//    /// Creates the copy of a subarray
//    constructor new(int anIndex, int aLength, IntArray anArray)
//        <= new(aLength);
//    [
//        anArray read(anIndex, aLength, self).
//    ]
//
//    /// Creates the short array copy
//    constructor new(IntArray anArray)
//        <= new(0, anArray length, anArray).
//    
//    constructor new(Indexer<int> it)
//        <= new(it length);
//    [
//        int len := self length.
//        int i := 0.
//        while (i < len)
//        [
//            it writeIndex(i).
//            
//            int val := it get.
//            
//            self write(i, val).
//            
//            i += 1
//        ]
//    ]
//    
//    /// Reads the array length to the output parameter
//    readLengthTo(ref<int> aLength)
//        => system'core_routines'__intarray.
//
//    /// Fills the sub array with a specified value
//    fill(int anIndex, int aLength, int aValue)
//    [
//        int aTotalLength := self length.
//        
//        int i := anIndex.
//        int m := anIndex + aLength.
//        
//        if (i > aTotalLength)
//            [ system'InvalidArgumentException new; raise ].
//        
//        if (m > aTotalLength)
//            [ system'InvalidArgumentException new; raise ].
//            
//        while(i < m)
//        [
//            self write(i, aValue).
//            
//            i := i + 1
//        ]
//    ]
//
//    /// Copies a sub array to the provided one at the specified position
//    read(int anIndex, int aLength, IntArray anArray)
//        => system'core_routines'__intarray.
//
//    /// Copies the short array at the specified position
//    write(int anIndex, int aLength, IntArray anArray)
//        => system'core_routines'__intarray.
//
//    /// Writes an integer value at the specified position
//    write(int anIndex, int aValue)
//        => system'core_routines'__intarray.
//
//    /// Reads an integer value at the specified position and saves it into an output variable
//    read(int anIndex, ref<int> anInt)
//        => system'core_routines'__intarray.
//
//    embeddable int getAt(int n)
//    [
//        int retVal := 0.
//        
//        self read(n, &retVal).
//        
//        ^ retVal
//    ]
//
//    /// Creates the object copy
//    clone = IntArray new(self).
//}
//
//// --- BitArray ---
//
//public sealed class BitArray
//{
//    T<IntArray> theArray.
//    int         theLength.
//    
//    /// Creates the array with the specified length
//    constructor new(int length)
//    [
//        theLength := length.
//        theArray := IntArray new(length / 32 + 1).
//    ]
//    
//    readLengthTo(ref<int> retVal)
//    [
//        retVal value := theLength.
//    ]
//    
//    int rvprop length :: readLengthTo.
//    
//    getAt:index
//        = self getAt(T<int>(index)).
//    
//    bool getAt(int index)
//    [
//        int arr_index := index / 32.
//        int bit_index := arr_index * 32.
//        bit_index := index - arr_index.
//        
//        int bit_mask := 1.
//        if (bit_index != 0)
//            [ bit_mask := bit_mask << bit_index ].
//        
//        int bits := 0.
//        theArray read(arr_index, &bits).
//        
//        ^ (bits && bit_mask) != 0
//    ]
////    
////    setAt:index:value
////    [
////        $self setAt int:index bool:value.
////    ]
//    
//    setAt(int index, bool value)
//    [
//        int arr_index := index / 32.
//        int bit_index := arr_index * 32.
//        bit_index := index - arr_index.
//        
//        int bit_mask := 1.
//        if (bit_index != 0)
//            [ bit_mask := bit_mask << bit_index ].
//        
//        int bits := 0.
//        theArray read(arr_index, &bits).
//        
//        value
//            ? [ bits := bits || bit_mask. ]
//            ! [  
//                bit_mask := bit_mask inverted.
//                
//                bits := bits && bit_mask
//            ].
//            
//        theArray write(arr_index, bits)
//    ]
//    
//    /// Returns the array enumerator
//    Enumerator<bool> enumerator = BitArrayEnumerator new(self).
//
//    /// Returns the array indexer
//    Indexer<bool> indexer = BitArrayIndexer new(self).
//}
//
//// --- BitArray32 ---
//
//public sealed class BitArray32
//{
//    int theValue.
//    
//    constructor new(int value)
//    [
//        theValue := value.
//    ]
//    
////    constructor new:value
////        <= new int:value.
//    
//    int length = 32.
//    
//    write(int value)
//    [
//        theValue := value.
//    ]
////    
////    write : value
////    [
////        value cast:%write to:$self.
////    ]
//    
////    getAt:index
////        = $self getAt int:index.
//    
//    bool getAt(int index)
//    [
//        int bit_mask := 1.
//        if (index != 0)
//            [ bit_mask := bit_mask << index ].
//        
//        ^ (theValue && bit_mask) != 0
//    ]
//    
////    setAt:index:value
////    [
////        $self setAt int:index bool:value.
////    ]
//    
//    setAt(int index, bool value)
//    [
////        int bit_mask := 1.
////        if (index != 0)
////            [ bit_mask := bit_mask << index ].
////        
////        if(value)
////            [ theValue := theValue || bit_mask ];
////            [  
////                bit_mask := bit_mask inverted.
////                
////                theValue := theValue && bit_mask
////            ].
//    ]
//    
//    /// Returns the array enumerator
//    T<Enumerator> enumerator = BitArray32Enumerator new(self).
//
//    /// Returns the array indexer
//    T<Indexer> indexer = BitArray32Indexer new(self).
//}
//
//// --- IntMatrix ---
//
//public sealed class IntMatrix
//{
//    T<IntArray> theArray.
//    int         theRows.
//    int         theColumns.
//    
//    constructor new(int Rows, int Columns)
//    [
//        theRows := Rows.
//        theColumns := Columns.
//        
//        theArray := IntArray new(Columns * Rows).        
//    ]
//        
//    constructor new(Rows,Columns)
//        <= new(T<int>(Rows), T<int>(Columns)).
//        
//    /// Reads the matrix lengths to the output parameter
//    readLengthsTo(ref<int> Rows, ref<int> Columns)
//    [
//        Rows value := theRows.
//        Columns value := theColumns
//    ]
//
//    readLengthTo(ref<int> retVal)
//    [
//        retVal value := theRows
//    ]
//    
//    int rvprop length :: readLengthTo.
//
//    readColumnsTo(ref<int> retVal)
//    [
//        retVal value := theColumns
//    ]
//    
//    int rvprop columns :: readColumnsTo.
//
//    readRowsTo(ref<int> retVal)
//    [
//        retVal value := theRows
//    ]
//    
//    int rvprop rows :: readRowsTo.
//
//    /// Writes an integer value at the specified position
//    write(int i, int j, int aValue)
//    [
//        theArray write(i * theColumns + j, aValue)
//    ]
//
//    /// Reads an integer value at the specified position and saves it into an output variable
//    read(int i, int j, ref<int> anInt)
//    [
//        theArray read(i * theColumns + j, anInt)
//    ]
//
//    embeddable int getAt(int i, int j)
//    [
//        int retVal := 0.
//        self read(i, j, &retVal).
//        ^ retVal
//    ]
//
//    getAt(object i, object j)
//    [
//        int aValue := 0.
//        self read(T<int>(i), T<int>(j), &aValue).
//        
//        ^ aValue
//    ]
//
//    setAt(object i, object j, object aValue)
//    [
//        self write(T<int>(i), T<int>(j), T<int>(aValue)).
//    ]
//
//    /// Returns a row
//    getAt(int i) = Indexable<int>::
//    {
//        length = theColumns.
//        
//        Indexer<int> indexer = BaseIndexer<int>::
//        {
//            @theIndex := 0.
//            
//            @theLength := self columns.
//            
//            int get
//                = self getAt(i,theIndex).
//                
//            set(int o)
//            [
//                self setAt(i,theIndex,o).
//            ]                        
//        }.    
//        
//        getAt(int j)
//            = self getAt(i,j).
//        
//        setAt(int j, int aValue)
//        [
//            self setAt(i,j,aValue).
//        ]
//    }.
//    
//    enumerator = Enumerator::
//    {
//        int i := -1.
//        
//        bool next
//        [
//            i += 1.
//            if (i >= theRows)
//            [
//                ^ false
//            ].
//            
//            ^ true
//        ]
//        
//        get = BaseEnumerator<int>::
//        {
//            @theIndex := -1.
//            @theLength := theColumns.
//        
//            reset [ theIndex value := -1 ]
//            
//            enumerable = self.
//        
//            int get
//                = self[i][theIndex value].
//        }.
//        
//        reset [ i := -1 ]
//        
//        enumerable = self.
//    }.
//}
//
//// --- RealArray ---
//
///// An array of real numbers
//public sealed RealArray :: Indexable<real>, Array<real>
//{
//    /// Creates the array with the specified length
//    constructor new(int aLength)
//        = dynamic<real>(aLength).
//
////    /// Creates the array with the specified length
////    constructor new : aLength
////        <= new int:aLength.
//
//    /// Creates the copy of a subarray
//    constructor new(int anIndex, int aLength, RealArray anArray)
//        <= new(aLength);
//    [
//        anArray read(anIndex, aLength, self)
//    ]
//
//    /// Creates the array copy
//    constructor new(RealArray anArray)
//        <= new(0, anArray length, anArray).
//
//    /// Fills the sub array with a specified value
//    fill(int anIndex, int aLength, real aValue)
//    [
//        int aTotalLength := self length.
//        
//        int i := anIndex.
//        int m := anIndex + aLength.
//        
//        if (i > aTotalLength)
//            [ system'InvalidArgumentException new; raise ].
//        
//        if (m > aTotalLength)
//            [ system'InvalidArgumentException new; raise ].
//          
//        while(i < m)
//        [
//            self setAt(i, aValue).
//            
//            i := i + 1.
//        ].
//    ]
//
//    /// Copies a sub array to the provided one at the specified position
//    read(int anIndex, int aLength, RealArray anArray)
//        => system'core_routines'__realarray.
//
//    /// Copies the short array at the specified position
//    write(int anIndex, int aLength, RealArray anArray)
//        => system'core_routines'__realarray.
//
//    /// Reads a real value at the specified position and saves it into an output variable
//    read(int anIndex, ref<real> aReal)
//        => system'core_routines'__realarray.
//
//    embeddable real getAt(int n)
//    [
//        real retVal := 0.
//        
//        self read(n, &retVal).
//        
//        ^ retVal.
//    ]
//
//    /// Creates the object copy
//    clone = RealArray new(self).
//}
//
//// --- RealMatrix ---
//
//public sealed class RealMatrix
//{
//    T<RealArray> theArray.
//    int          theRows.
//    int          theColumns.
//    
//    constructor new(int Rows, int Columns)
//    [
//        theArray := RealArray new(Columns * Rows).
//        
//        theRows := Rows.
//        theColumns := Columns.
//    ]
//        
//    constructor new(object Rows, object Columns)
//        <= new(T<int>(Columns), T<int>(Rows)).
//
//    /// Reads the matrix lengths to the output parameter
//    readLengthsTo(ref<int> Rows, ref<int> Columns)
//    [
//        Columns value := theColumns.
//        Rows value := theRows.
//    ]
//
//    readLengthTo(ref<int> retVal)
//    [
//        retVal value := theRows.
//    ]
//    
//    int rvprop length :: readLengthTo.
//
//    readColumnsTo(ref<int> retVal)
//    [
//        retVal value := theColumns.
//    ]
//    
//    int rvprop columns :: readColumnsTo.
//
//    readRowsTo(ref<int> retVal)
//    [
//        retVal value := theRows
//    ]
//    
//    int rvprop rows :: readRowsTo.
//
//    /// Writes an integer value at the specified position
//    write(int i, int j, real aValue)
//    [
//        int anIndex := i * theColumns + j.
//        
//        theArray setAt(anIndex, aValue)
//    ]
//
//    /// Reads an integer value at the specified position and saves it into an output variable
//    read(int i, int j, ref<real> aValue)
//    [
//        int anIndex := i * theColumns + j.
//        
//        theArray read(anIndex, aValue)
//    ]
//
//    embeddable real getAt(int i, int j)
//    [
//        real retVal := 0.0r.
//        self read(i, j, &retVal).
//        
//        ^ retVal
//    ]
//
//    getAt(object i, object j)
//    [
//        real aValue := 0.0r.
//        self read(T<int>(i), T<int>(j), &aValue).
//        
//        ^ RealNumber new(aValue)
//    ]
//
//    setAt(object i, object j, object aValue)
//    [
//        self write(i, j, aValue)
//    ]
//
//    /// Returns a row
//    getAt : i =
//    {
//        length = theColumns.
//        
//        getAt : j
//            = self getAt(i,j).
//        
//        setAt(object j, object aValue)
//        [
//            self setAt(i,j,aValue)
//        ]
//    }.
//}
//
//// --- CharArray ---
//
///// An array of unicode characters (UTF-32)
//public sealed CharArray :: Indexable<char>, Array<char>
//{
//    /// Creates the new array with a specified length
//    constructor new(int len)
//        = dynamic<char>(len).
//
//    /// Creates the new array with a specified length
//    constructor new : aLength
//        <= new(T<int>(aLength)).
//
//    /// Creates the copy of a subarray
//    constructor new(int anIndex, int aLength, CharArray anArray)
//        <= new(aLength);
//    [
//        anArray read(anIndex, aLength, self).
//    ]
//     
//    /// Creates the short array copy
//    constructor new(CharArray anArray)
//        <= new(0, anArray length, anArray).
//    
//    /// Reads the array length to the output parameter
//    readLengthTo(ref<int> aLength)
//        => system'core_routines'__intarray.
//
//    /// Fills the sub array with a specified value
//    fill(int anIndex, int aLength, char aValue)
//    [
//        int aTotalLength := self length.
//        
//        int i := anIndex.
//        int m := anIndex + aLength.
//        
//        if (i > aTotalLength)
//            [ system'InvalidArgumentException new; raise ].
//        
//        if (m > aTotalLength)
//            [ system'InvalidArgumentException new; raise ].
//            
//        while(i < m)
//        [
//            self write(i, aValue).
//            
//            i := i + 1
//        ]
//    ]
//
//    /// Copies a sub array to the provided one at the specified position
//    read(int anIndex, int aLength, CharArray anArray)
//        => system'core_routines'__chararray.
//
//    /// Writes a character value at the specified position
//    write(int anIndex, char aValue)
//        => system'core_routines'__chararray.
//
//    /// Reads a character value at the specified position and saves it into an output variable
//    read(int anIndex, ref<char> anInt)
//        => system'core_routines'__chararray.
//
//    embeddable char getAt(int n)
//    [
//        char retVal := $0.
//        
//        self read(n, &retVal).
//        
//        ^ retVal
//    ]
//
//    move(int anIndex, int aLength, int anOffset)
//        => system'core_routines'__chararray.
//
//    /// Creates the object copy
//    clone = CharArray new(self).
//}
        
// --- emptyString ---

/// An empty literal value
public const string emptyString = "";

// --- String ---

/// A UTF-8 literal value
public sealed const struct String : BaseValue
{
    __string byte[] theArray;

//    constructor new
//        <= min.
//
//    /// return the default value
//    constructor
//        = emptyLiteral.
//        
//    /// Creates the empty literal
//    constructor min
//        = emptyLiteral.
        
    /// Creates the literal string with specified length        
    constructor allocate(int size)
        = new byte[](size + 1);

//    /// Creates the literal string with specified length and copies the parameter value
//    constructor new(int aSize, literal aLiteral)
//        <= new(aSize);
//    [
//        system'core_routines'__literal copy(aLiteral, self).
//    ]

    /// Creates the literal string with specified length fills with specified character value        
    constructor fill(int size, char ch)
        => system'core_routines'__literal;

//    /// Creates the literal string copy              
//    constructor new(literal aLiteral)
//        <= new(aLiteral length, aLiteral).
        
    /// Creates the copy of a subarray        
    constructor new(int index, int length, char[] array)
        => system'core_routines'__literal;

    internal constructor fromByteArray(int index, int size, byte[] buffer)
        <= allocate(size)
    {
        intern system'core_routines'__literal.copy(self, index, size, buffer)
    }
    
    /// Converts to UTF-16 literal
    wide cast() = wideConvertor.convert(self);

//    cast Enumerable<char> = Enumerable<char>::{ Enumerator<char> enumerator => self. }.

    string Printable = self;

//    bool equal:n
//    [
//        var l := T<literal>(n) ?? nil.
//        if (nil == l)
//        [
//            ^ false.
//        ];
//        [
//            ^ self equal(l).
//        ]
//    ]
//
//    bool less:o
//        <= less(T<literal>(o)).
//        
//    bool equal(BaseVariable v)
//        <= equal(v value).
//        
//    bool less(BaseVariable v)
//        <= less(v value).
        
    /// Compares the values
    bool equal(string s)
        => system'core_routines'__literal;

    /// Compares the values
    bool less(string s)
        => system'core_routines'__literal;

//    /// Compares the values
//    bool equal(wide aLiteral)
//        = T<wide>(self) equal(aLiteral).
//
//    /// Compares the values
//    bool less(wide aLiteral)
//        = T<wide>(self) less(aLiteral).
//
//    /// Compares the values
//    bool equal(char aChar)
//        <= equal(T<literal>(aChar)).
//
//    /// Compares the values
//    bool less(char aChar)
//        <= less(T<literal>(aChar)).

    /// Returns the character at the specified index
    char at(int n)
    {
        char ch := $0;
        self.read(n, ref ch);
        
        ^ new CharValue(ch)
    }

//    literal add : anObject 
//        <= add(T<literal>(anObject)).
//
//    /// Concatinates the values
//    literal add(BaseVariable v)
//        <= add(v value).
        
    /// Concatinates the values
    string add(string s)
        => system'core_routines'__literal;

    /// Concatinates the values
    string add(char ch)
    {
        int len := self.Length;
        int ch_len := ch.Length;
        
        int new_len := len + ch_len;
        auto retVal := String.allocate(new_len);
        
        intern system'core_routines'__literal.copy(self, retVal);
        intern system'core_routines'__literal.insert(retVal, len, ch);
        intern system'core_routines'__literal.insert(retVal, new_len, $0);
        
        ^ retVal
    }

    // Reads the character at the specified index and saves into an output parameter
    read(int index, ref char ch)
        => system'core_routines'__literal;

//    // Reads the byte at the specified index and saves into an output parameter
//    read(int anIndex, ref<byte> anInt)
//        => system'core_routines'__literal.

    /// Saves the index of the first occurence of the specified substring into an output parameter.
    ///
    /// If the substring is not found. returns -1
    indexOf(int index, string literal, ref int retVal)
        => system'core_routines'__literal;

    /// Saves the index of the first occurence of the specified symbol into an output parameter.
    ///
    /// If the substring is not found. returns -1
    indexOf(int index, char ch, ref int retVal)
        => system'core_routines'__literal;

    /// Returns the index of the first occurence of the specified symbol.
    /// 
    /// If the substring is not found. returns -1
    indexOf(int index, char ch)
    {
        int retVal := 0;
        self.indexOf(index, ch, ref retVal);
        
        ^ new IntNumber(retVal)
    }

    /// Returns the index of the first occurence of the specified substring.
    ///
    /// If the substring is not found. returns -1
    indexOf(int index, string s)
    {
        int retVal := 0;
        self.indexOf(index, s, ref retVal);
        
        ^ new IntNumber(retVal)
    }

    /// Inserts the substring into and returns the new one
    string insert(int index, string s)
        => system'core_routines'__literal;

    /// Inserts a character into and returns the new one
    string insert(int index, char ch)
        = self.insert(index, cast string(ch));

//    /// Inserts an object into and returns the new one
//    insert : anObject at:anIndex
//        = self insert(anIndex, anObject).
//
//    /// Deletes the substring from the object and returns the new one
//    delete:aLength at:anIndex
//        = self delete(T<int>(anIndex), T<int>(aLength)).

    /// Deletes the substring from the object and returns the new one
    string delete(int index, int length)
        => system'core_routines'__literal;

//    /// Returns the substring
//    Substring : aLength at:anIndex
//        = self Substring(T<int>(anIndex), T<int>(aLength)).

    /// Returns the substring
    string Substring(int index, int length)
        => system'core_routines'__literal;

    vprop int Length
    {
        /// Reads the literal length into an output parameter
        read(ref int retVal)
            => system'core_routines'__literal;
    }


    /// Saves the literal into a UTF-32 character array
    save(int index, char[] array, ref int retVal)
        => system'core_routines'__literal;

    internal saveToByteArray(int index, byte[] array, int retVal)
        => system'core_routines'__literal;

    /// Returns the string enumerator
    Enumerator<char> enumerator() = new StringEnumerator(self);

//    /// Returns the string indexer
//    Indexer<char> indexer = LiteralIndexer new(self).
//    
//    /// Clones the value
//    clone = LiteralValue new(self).
}

// --- emptyWideString ---

/// An empty wide literal value
public const wide emptyWideString = ""w;

// --- WideString ---

/// A UTF-18 literal value
public sealed const struct WideString : BaseValue
{
    __string short[] theArray;

//    /// return the default value
//    constructor
//        = emptyWideLiteral.
//
//    /// Creates the empty literal
//    constructor min
//        = emptyWideLiteral.
        
    /// Creates the literal string with specified length
    constructor allocate(int size)
        = new short[](size + 1);

//    /// Creates the literal string with specified length and copies the parameter value
//    constructor new(int aSize, wide aLiteral)
//        <= new(aSize);
//    [
//        system'core_routines'__wide copy(aLiteral, self).
//    ]
//
//    /// Creates the literal string with specified length fills with spciefied character value
//    constructor new(int aSize, char aChar)
//        => system'core_routines'__wide.
//
//    /// Creates the literal string copy
//    constructor new(wide aLiteral)
//        <= new(aLiteral length, aLiteral).
//        
//    /// Creates the copy of a subarray
//    constructor new(int anIndex, int aSize, CharArray anArray)
//        => system'core_routines'__wide.

    internal constructor fromShortArray(int anIndex, int aSize, short[] anArray)
        <= allocate(aSize)
    {
        intern system'core_routines'__wide.copy(self, anIndex, aSize, anArray)
    }

    /// Converts to UTF-8 literal
    string cast() = stringConvertor.convert(self);

//    T<literal> literal = literalConvertor convert(self).
//    
//    bool equal:n
//    [
//        var l := T<wide>(n) ?? nil.
//        if (nil == l)
//        [
//            ^ false.
//        ];
//        [
//            ^ self equal(l).
//        ]
//    ]
//
//    bool less:n
//        <= less(T<wide>(n)).
//        
//    bool equal(BaseVariable v)
//        <= equal(v value).
//        
//    bool less(BaseVariable v)
//        <= less(v value).
//
//    /// Compares the values
//    bool equal(wide aLiteral)
//        => system'core_routines'__wide.
//
//    /// Compares the values
//    bool less(wide aLiteral)
//        => system'core_routines'__wide.
//
//    /// Compares the values
//    bool equal(char aChar)
//        <= equal(T<wide>(aChar)).
//
//    /// Compares the values
//    bool less(char aChar)
//        <= less(T<wide>(aChar)).
//
//    /// Returns the character at the specified index
//    getAt : aParam
//        <= getAt(T<int>(aParam)).

    /// Returns the character at the specified index
    char at(int n)
    {
        char ch := $0;
        self.read(n, ref ch);
        
        ^ new CharValue(ch)
    }

//    wide add : anObject 
//        <= add(T<literal>(anObject)).
//
//    /// Concatinates the values
//    wide add(BaseVariable v)
//        <= add(v value).
    
    /// Concatinates the values
    wide add(wide s)
        => system'core_routines'__wide;

//    /// Concatinates the values
//    wide add(char aChar)
//        = self add(wideConvertor convert(aChar)).
//
//    /// Concatinates the values
//    wide add(literal aLiteral)
//        = self add(T<wide>(aLiteral)).

    // Reads the character at the specified index and saves into an output parameter
    read(int n, ref char ch)
        => system'core_routines'__wide;

//    // Reads the short integer at the specified index and saves into an output parameter
//    read(int anIndex, ref<short>aValue)
//        => system'core_routines'__wide.
//
//    /// Saves the index of the first occurence of the specified substring into an output parameter.
//    ///
//    /// If the substring is not found. returns -1
//    indexOf(int anIndex, wide aLiteral, ref<int> aRetVal)
//        => system'core_routines'__wide.
//
//    /// Saves the index of the first occurence of the specified character into an output parameter.
//    ///
//    /// If the substring is not found. returns -1
//    indexOf(int anIndex, char aChar, ref<int> aRetVal)
//        => system'core_routines'__wide.
//
//    /// Returns the index of the first occurence of the specified character.
//    ///
//    /// If the substring is not found. returns -1
//    indexOf(char aChar) at(int anIndex)
//    [
//        int aRetVal := 0.
//        self indexOf(anIndex, aChar, &aRetVal).
//        
//        ^ IntNumber new(aRetVal).
//    ]
//
//    /// Returns the index of the first occurence of the specified substring.
//    ///
//    /// If the substring is not found. returns -1
//    indexOf(wide aLiteral) at(int anIndex)
//    [
//        int aRetVal := 0.
//        self indexOf(anIndex, aLiteral, &aRetVal).
//        
//        ^ IntNumber new(aRetVal).
//    ]
//
//    /// Inserts the substring into and returns the new one
//    wide insert(int anIndex, wide aLiteral)
//        => system'core_routines'__wide.
//
//    /// Inserts the substring into and returns the new one
//    wide insert(int anIndex, literal aLiteral)
//        = self insert(anIndex, T<wide>(aLiteral)).
//
//    /// Inserts a character into and returns the new one
//    wide insert(int anIndex, char aChar)
//        = self insert(anIndex, wideConvertor convert(aChar)).
//
//    /// Inserts an object into and returns the new one
//    insert : anObject at:anIndex
//        = self insert(anIndex, anObject).
//
//    /// Deletes the substring from the object and returns the new one
//    delete:aLength at:anIndex
//        = self delete(T<int>(anIndex), T<int>(aLength)).
            
    /// Deletes the substring from the object and returns the new one            
    wide delete(int index, int length)
        => system'core_routines'__wide;

    /// Returns the substring
    wide Substring(int n, int length)
        => system'core_routines'__wide;

//    wide Substring:aLength at:anIndex
//        <= Substring(T<int>(anIndex), T<int>(aLength)).

    /// Reads the literal length into an output parameter
    vprop int Length
    {
        /// Reads the literal length into an output parameter
        read(ref int retVal)
            => system'core_routines'__wide;
    }

//    /// Saves the literal into a UTF-32 character array
//    save(int anIndex, CharArray anArray, ref<int> aLength)
//        => system'core_routines'__wide.
//
//    internal saveToShortArray(int anIndex, ShortArray anArray, int aRetVal)
//        => system'core_routines'__wide.
//
//    /// Returns the object enumerator
//    Enumerator<char> enumerator = WideLiteralEnumerator new(self).
//    
//    /// Returns the object indexer
//    Indexer<char> indexer = WideLiteralIndexer new(self).
//    
//    /// Clones the value
//    clone = WideLiteralValue new(self).
}

// --- Variant ---

/// A basic type variant class
public sealed class Variant
{
    object theValue;
    
    constructor(object)
    {
        theValue := object
    }
    
//    /// Tries to convert the value to a byte number
//    cast T<byte>
//        = byteConvertor convert:theValue.
//
//    /// Tries to convert the value to a short integer number
//    cast T<short>
//        = shortConvertor convert:theValue.
//
//    /// Tries to convert the value to a character
//    cast T<char>
//        = charConvertor convert:theValue.
//
    /// Tries to convert the literal value to the integer number
    int cast()
        = intConvertor.convert(theValue);

//    /// Tries to convert the literal value to the long integer number
//    cast T<long>
//        = longConvertor convert:theValue.

    /// Tries to convert the literal value to the real number
    real cast()
        = realConvertor.convert(theValue);

    /// Returns a literal value
    string cast()
        = stringConvertor.convert(theValue);
    
//    /// Returns a wide literal value
//    cast T<wide>
//        = wideConvertor convert:theValue.
//
//    T<literal> literal => theValue.
//
//    /// Converts the parameter into a literal string
//    set value(int aValue)
//    [
//        theValue := aValue.
//    ]    
//
//    /// Converts the parameter into a literal string
//    set value(short aValue)
//    [
//        theValue := aValue.
//    ]    
//
//    /// Converts the parameter into a literal string
//    set value(byte aValue)
//    [
//        theValue := aValue.
//    ]    
//
//    /// Converts the parameter into a literal string
//    set value(long aValue)
//    [
//        theValue := aValue.
//    ]    
//
//    /// Converts the parameter into a literal string
//    set value(real aValue)
//    [
//        theValue := aValue.
//    ]    

    private castValueTo(target)
        = {
            eval(o)
                = o.Printable;
            
//            eval(BaseVariable v)
//                <= eval(v value).
//            
            eval(int val)
                = cast int(self);
                
            eval(byte val)
                = cast byte(self);
                
            eval(short val)
                = cast short(self);
                
            eval(long val)
                = cast long(self);
                
            eval(real val)
                = cast real(self);
                
            eval(string val)
                = cast string(self);
                
            eval(wide val)
                = cast wide(self);
        }.eval(target);

    saveTo(target)
    {
        target.Value := self.castValueTo(target.Value);
        
        ^ target
    }

////    add : aTarget
////        = aTarget add($self $castValueTo:aTarget).
////
////    subtract : aTarget
////        = $self $castValueTo:aTarget; subtract:aTarget.
////
////    multiply : aTarget
////        = $self $castValueTo:aTarget; multiply:aTarget.
////
////    divide : aTarget
////        = $self $castValueTo:aTarget; divide:aTarget.
////
////    bool equal : aTarget
////        = $self $castValueTo:aTarget; equal:aTarget.
}

// --- Array ---

/// An array
public sealed Array : Indexable//, Array<object>
{
    object[] theArray;
    
//    /// Creates an empty array
//    constructor min
//        = dynamic<object>(0).
//    
//    /// Creates the array with the specified length
//    constructor new : aLength
//        <= new(T<int>(aLength)).
    
    /// Creates the array with the specified length
    constructor allocate(int len)
        = new object[](len);

    /// Creates a copy of the provided array
    constructor(Array array)
        <= (0, array.Length, array);
    
    /// Creates a copy of the provided sub array
    constructor(int index, int length, Array array)
        <= allocate(length)
    {
        intern system'core_routines'__array.copy(self, index, length, array)
    }
        
//    constructor single(object singleItem)
//        <= new(1);
//    [
//        self setAt(0, singleItem).
//    ]
//
//    /// Concatinates the arrays    
//    add(Array anArray)
//    [
//        int aLength1 := self length.
//        int aLength2 := anArray length.
//        
//        auto aRetVal := Array new(aLength1 + aLength2).
//
//        int anIndex := 0.
//        while(anIndex < aLength1)
//        [
//            aRetVal setAt(anIndex, self getAt(anIndex)).
//            
//            anIndex := anIndex + 1.
//        ].
//
//        anIndex := 0.
//        while(anIndex < aLength2)
//        [
//            aRetVal setAt(aLength1 + anIndex, anArray getAt(anIndex)).
//            
//            anIndex := anIndex + 1.
//        ].
//            
//        ^ aRetVal
//    ]
//
    vprop int Length
    {
        /// Reads the array length into an output parameter
        read(ref int retVal)
            => system'core_routines'__array;
    }

    /// Sets the specified array member
    setAt(int n, object o)
        => system'core_routines'__array;

    /// Returns the specified array member
    object at(int n)
        => system'core_routines'__array;
 
    /// Returns the array enumerator
    Enumerator enumerator() = new ArrayEnumerator(self);

    /// Returns the array indexer
    Indexer indexer() = new ArrayIndexer(self);

    /// Clones the array
    Array clone()
        = new Array(self);
}

// === Convertors ===

// --- stringConvertor ---

/// A literal convertor
public singleton stringConvertor
{
    string convert(BaseVariable v)
        <= convert(v.Value);
    
    string convert(BaseVariable v, int radix)
        <= convert(v.Value, radix);
    
    /// Converts the parameter to the literal value
    convert(char ch, byte[] buffer, ref int length)
        => system'core_routines'__literalconvertor;
    
    string convert(char ch)
    {
        byte buffer[4];
        int  length := 0;
        
        self.convert(ch, buffer, ref length);
        
        ^ String.fromByteArray(0, length, buffer)
    }
    
    convert(int n, byte[] buffer, ref int length)
        => system'core_routines'__literalconvertor;
    
    convert(uint n, byte[] buffer, ref int length)
        => system'core_routines'__literalconvertor;
    
    /// Converts the parameter to the literal value
    string convert(int n)
    {
        byte buffer[12];
        int  length := 0;
        
        self.convert(n, buffer, ref length);
        
        ^ String.fromByteArray(0, length, buffer)
    }
    
    /// Converts the parameter to the literal value
    string convert(uint n)
    {
        byte buffer[12];
        int  length := 0;
        
        self.convert(n, buffer, ref length);
        
        ^ String.fromByteArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value
    convert(short n, byte[] buffer, ref int length)
        => system'core_routines'__literalconvertor;
    
    /// Converts the parameter to the literal value
    string convert(short n)
    {
        byte buffer[12];
        int  length := 0;
        
        self.convert(n, buffer, ref length);
        
        ^ String.fromByteArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value
    convert(byte n, byte[] buffer, ref int length)
        => system'core_routines'__literalconvertor;
    
    /// Converts the parameter to the literal value
    string convert(byte n)
    {
        byte buffer[4];
        int  length := 0;
        
        self.convert(n, buffer, ref length);
        
        ^ String.fromByteArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value
    convert(long n, byte[] buffer, ref int length)
        => system'core_routines'__literalconvertor;

    /// Converts the parameter to the literal value
    string convert(long n)
    {
        byte buffer[64];
        int  length := 0;
        
        self.convert(n, buffer, ref length);
        
        ^ String.fromByteArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value with the provided radix
    convert(int n, int radix, byte[] buffer, ref int length)
        => system'core_routines'__literalconvertor;

    /// Converts the parameter to the literal value with the provided radix
    convert(uint n, int radix, byte[] buffer, ref int length)
        => system'core_routines'__literalconvertor;

    /// Converts the parameter to the literal value with the provided radix
    string convert(int n, int radix)
    {
        byte buffer[48];
        int  length := 0;
        
        self.convert(n, radix, buffer, ref length);
        
        ^ String.fromByteArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value with the provided radix
    string convert(uint n, int radix)
    {
        byte buffer[48];
        int  length := 0;
        
        self.convert(n, radix, buffer, ref length);
        
        ^ String.fromByteArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value with the provided radix
    convert(short n, int radix, byte[] buffer, ref int length)
        => system'core_routines'__literalconvertor;

    /// Converts the parameter to the literal value with the provided radix
    string convert(short n, int radix)
    {
        byte buffer[32];
        int  length := 0;
        
        self.convert(n, radix, buffer, ref length);
        
        ^ String.fromByteArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value with the provided radix
    string convert(byte n, int radix, byte[] buffer, ref int length)
        => system'core_routines'__literalconvertor;

    /// Converts the parameter to the literal value with the provided radix
    string convert(byte n, int radix)
    {
        byte buffer[8];
        int  length := 0;
        
        self.convert(n, radix, buffer, ref length);
        
        ^ String.fromByteArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value with the provided radix
    convert(long n, int radix, byte[] buffer, ref int length)
        => system'core_routines'__literalconvertor;

    /// Converts the parameter to the literal value
    string convert(long n, int radix)
    {
        byte buffer[64];
        int  length := 0;
        
        self.convert(n, radix, buffer, ref length);
        
        ^ String.fromByteArray(0, length, buffer)
    }

    /// Converts the parameter to the literal value
    convert(real n, byte[] buffer, ref int length)
        => system'core_routines'__literalconvertor;

    /// Converts the parameter to the literal value
    string convert(real n)
    {
        byte buffer[32];
        int  length := 0;
        
        self.convert(n, buffer, ref length);
        
        ^ String.fromByteArray(0, length, buffer)
    }

    /// Converts UTF-16 literal to UTF-8 one
    convert(wide w, byte[] buffer, ref int length)
        => system'core_routines'__literalconvertor;

    string convert(wide w)
    {
        int  i := 0;
        int  m := 0;
        int  page := 0;
        int  l := w.Length;
        byte buffer[256];
        if (l < 128)
        {
            self.convert(w, buffer, ref m);
            
            ^ String.fromByteArray(0, m, buffer)
        };

        string retVal := emptyString;
        while(l > 0)
        {
            page := l;
            if (page > 127)
            {
               page := 127
            };
            
            self.convert(w.Substring(i, page), buffer, ref m);
            
            retVal := retVal.add(String.fromByteArray(0, m, buffer));
            
            i := i + page;
            l := l - page
        };
        
        ^ retVal
    }

    /// Returns a parameter                
    string convert(string s)
        = s;
        
    /// Returns the literal presentation of the parameter
    string convert(o)
        = o.Printable;
}

// --- wideConvertor ---

/// A wide literal convertor
public singleton wideConvertor
{
//    wide convert(BaseVariable v)
//        <= convert(v value).
//    
//    /// Converts the parameter to the literal value
//    convert(char aValue, ShortArray buffer, ref<int> length)
//        => system'core_routines'__wideconvertor.
//    
//    wide convert(char aValue)
//    [
//        T<short,2> buffer.
//        int        length := 0.
//        
//        self convert(aValue, T<ShortArray>(buffer), &length).
//        
//        ^ WideLiteralValue fromShortArray(0, length, T<ShortArray>(buffer))
//    ]    
//    
//    /// Converts the parameter to the literal value
//    convert(int aNumber, ShortArray buffer, ref<int> length)
//        => system'core_routines'__wideconvertor.
//
//    /// Converts the parameter to the literal value
//    convert(uint aNumber, ShortArray buffer, ref<int> length)
//        => system'core_routines'__wideconvertor.
//
//    /// Converts the parameter to the literal value
//    wide convert(int aNumber)
//    [
//        T<short,12> buffer.
//        int         length := 0.
//        
//        self convert(aNumber, T<ShortArray>(buffer), &length).
//        
//        ^ WideLiteralValue fromShortArray(0, length, T<ShortArray>(buffer))
//    ]
//
//    /// Converts the parameter to the literal value
//    wide convert(uint aNumber)
//    [
//        T<short,12> buffer.
//        int         length := 0.
//        
//        self convert(aNumber, buffer, T<ShortArray>(buffer)).
//        
//        ^ WideLiteralValue fromShortArray(0, length, T<ShortArray>(buffer))
//    ]
//
//    /// Converts the parameter to the literal value
//    convert(short aNumber, ShortArray buffer, ref<int> length)
//        => system'core_routines'__wideconvertor.
//
//    /// Converts the parameter to the literal value
//    wide convert(short aNumber)
//    [
//        T<short,12> buffer.
//        int         length := 0.
//        
//        self convert(aNumber, T<ShortArray>(buffer), &length).
//        
//        ^ WideLiteralValue fromShortArray(0, length, T<ShortArray>(buffer))
//    ]
//
//    /// Converts the parameter to the literal value
//    convert(byte aNumber, ShortArray buffer, ref<int> length)
//        => system'core_routines'__wideconvertor.
//
//    /// Converts the parameter to the literal value
//    wide convert(byte aNumber)
//    [
//        T<short,12> buffer.
//        int         length := 0.
//        
//        self convert(aNumber, T<ShortArray>(buffer), &length).
//        
//        ^ WideLiteralValue fromShortArray(0, length, T<ShortArray>(buffer))
//    ]
//
//    /// Converts the parameter to the literal value
//    convert(long aNumber, ShortArray buffer, ref<int> length)
//        => system'core_routines'__wideconvertor.
//
//    /// Converts the parameter to the literal value
//    wide convert(long aNumber)
//    [
//        T<short,64> buffer.
//        int         length := 0.
//        
//        self convert(aNumber, T<ShortArray>(buffer), &length).
//        
//        ^ WideLiteralValue fromShortArray(0, length, T<ShortArray>(buffer)).
//    ]
//
//    /// Converts the parameter to the literal value with the provided radix
//    convert(int aNumber, int aRadix, ShortArray buffer, ref<int> length)
//        => system'core_routines'__wideconvertor.
//
//    /// Converts the parameter to the literal value with the provided radix
//    convert(uint aNumber, int aRadix, ShortArray buffer, ref<int> length)
//        => system'core_routines'__wideconvertor.
//
//    /// Converts the parameter to the literal value with the provided radix
//    wide convert(int aNumber, int aRadix)
//    [
//        T<short,32> buffer.
//        int         length := 0.
//        
//        self convert(aNumber, aRadix, T<ShortArray>(buffer), &length).
//        
//        ^ WideLiteralValue fromShortArray(0, length, T<ShortArray>(buffer))
//    ]
//
//    /// Converts the parameter to the literal value with the provided radix
//    wide convert(uint aNumber, int aRadix)
//    [
//        T<short,32> buffer.
//        int         length := 0.
//        
//        self convert(aNumber, aRadix, T<ShortArray>(buffer), &length).
//        
//        ^ WideLiteralValue fromShortArray(0, length, T<ShortArray>(buffer))
//    ]
//
//    /// Converts the parameter to the literal value with the provided radix
//    convert(short aNumber, int aRadix, ShortArray buffer, ref<int> length)
//        => system'core_routines'__wideconvertor.
//
//    /// Converts the parameter to the literal value with the provided radix
//    wide convert(short aNumber, int aRadix)
//    [
//        T<short,16> buffer.
//        int         length := 0.
//        
//        self convert(aNumber, aRadix, T<ShortArray>(buffer), &length).
//        
//        ^ WideLiteralValue fromShortArray(0, length, T<ShortArray>(buffer))
//    ]
//
//    /// Converts the parameter to the literal value with the provided radix
//    convert(byte aNumber, int aRadix, ShortArray buffer, ref<int> length)
//        => system'core_routines'__wideconvertor.
//
//    /// Converts the parameter to the literal value with the provided radix
//    wide convert(byte aNumber, int aRadix)
//    [
//        T<short,12> buffer.
//        int         length := 0.
//        
//        self convert(aNumber, aRadix, T<ShortArray>(buffer), &length).
//        
//        ^ WideLiteralValue fromShortArray(0, length, T<ShortArray>(buffer))
//    ]
//
//    /// Converts the parameter to the literal value with the provided radix
//    convert(long aNumber, int aRadix, ShortArray buffer, ref<int> length)
//        => system'core_routines'__wideconvertor.
//
//    /// Converts the parameter to the literal value with the provided radix
//    wide convert(long aNumber, int aRadix)
//    [
//        T<short,64> buffer.
//        int         length := 0.
//        
//        self convert(aNumber, aRadix, T<ShortArray>(buffer), &length).
//        
//        ^ WideLiteralValue fromShortArray(0, length, T<ShortArray>(buffer))
//    ]
//
//    /// Converts the parameter to the literal value
//    convert(real aNumber, ShortArray buffer, ref<int> length)
//        => system'core_routines'__wideconvertor.
//
//    /// Converts the parameter to the literal value
//    wide convert(real aNumber)
//    [
//        T<short,32> buffer.
//        int         length := 0.
//        
//        self convert(aNumber, T<ShortArray>(buffer), &length).
//        
//        ^ WideLiteralValue fromShortArray(0, length, T<ShortArray>(buffer))
//    ]

    /// Converts UTF-8 literal to UTF-16e
    convert(string sl, short[] buffer, ref int length)
        => system'core_routines'__wideconvertor;

    /// Converts UTF-8 literal to UTF-16e
    wide convert(string s)
    {
        int        i := 0;
        int        m := 0;
        int        page := 0;
        int        l := s.Length;
        short buffer[256];            
        if (l < 256)
        {
            self.convert(s, buffer, ref m);

            ^ WideString.fromShortArray(0, m, buffer)
        };

        wide retVal := emptyWideString;
        while(l > 0)
        {
            page := l;
            if (page > 256)
            {
                page := 256
            };
            
            self.convert(s.Substring(i,page), buffer, ref m);
            
            retVal := retVal.add(WideString.fromShortArray(0, m, buffer));
            
            i := i + page;
            l := l - page
        };
        
        ^ retVal
    }
        
    /// Returns a parameter
    wide convert(wide aLiteral)
        = aLiteral;

//    /// Returns the literal presentation of the parameter
//    wide convert(o)
//        = cast wide(o literal);
}

/// An Unicode character convertor
public singleton charConvertor
{
//    /// Converts a byte value into a character and saves the result into an output parameter
//    convert(byte aValue, ref<char> aRetVal)
//        => system'core_routines'__charconvertor.
//
//    /// Converts a short integer into a character and saves the result into an output parameter
//    convert(short aValue, ref<char> aRetVal)
//        => system'core_routines'__charconvertor.
//
//    /// Converts an integer into a character and saves the result into an output parameter
//    convert(int aValue, ref<char> aRetVal)
//        => system'core_routines'__charconvertor.
//        
//    char convert(BaseVariable v)
//        <= convert(v value).
        
    /// Returns a parameter        
    char convert(char ch)
        = new CharValue(ch);
  
//    internal convert(int anIndex, literal aLiteral, ref<char> char)
//        => system'core_routines'__charconvertor.
//        
//    internal convert(int anIndex, ByteArray aLiteral, ref<char> char)
//        => system'core_routines'__charconvertor.
//  
//    internal convert(int anIndex, wide aLiteral, ref<char> char)
//        => system'core_routines'__charconvertor.

    internal convert(int index, short[] buffer, ref char char)
        => system'core_routines'__charconvertor;

    internal save(char ch, int index, short[] charArray, ref int length)
        => system'core_routines'__charconvertor;
    
    internal save(char ch, int index, byte[] charArray, ref int length)
        => system'core_routines'__charconvertor;

//    /// Converts a literal string into a character
//    char convert(literal aValue)
//    [
//        char aRetVal := $0.
//        self convert(0, aValue, &aRetVal).
//        
//        int l1 := 0.
//        int l2 := aValue length.
//        aRetVal readLengthTo(&l1).
//        
//        (l1 != l2)
//            ? [ system'InvalidArgumentException new; raise ].
//        
//        ^ CharValue new(aRetVal)
//    ]        
//        
//    /// Converts a wide literal string into a character                
//    char convert(wide aValue)
//    [
//        char aRetVal := $0.
//        self convert(0, aValue, &aRetVal).
//
//        int l1 := 0.
//        int l2 := aValue length.
//        aRetVal readWideLengthTo(&l1).
//                
//        (l1 != l2)
//            ? [ system'InvalidArgumentException new; raise ].
//        
//        ^ CharValue new(aRetVal)
//    ]        
//        
//    /// Converts a byte value into a character
//    char convert(byte aValue)
//    [
//        char aRetVal := $0.
//        self convert(aValue, &aRetVal).
//        
//        ^ CharValue new(aRetVal)
//    ]        
//        
//    /// Converts a short integer into a character
//    char convert(short aValue)
//    [
//        char aRetVal := $0.
//        self convert(aValue, &aRetVal).
//        
//        ^ CharValue new(aRetVal)
//    ]        
//        
//    /// Converts an integer into a character
//    char convert(int aValue)
//    [
//        char aRetVal := $0.
//        self convert(aValue, &aRetVal).
//        
//        ^ CharValue new(aRetVal)
//    ]
        
    char convert(o)
        = cast char(o);
}

// === Enumerators / Indexers

// --- StringEnumerator ---

/// A string enumerator
class StringEnumerator : Enumerator<char>
{
    string   theLiteral;
    ref<int> theIndex;
    ref<int> theDiff;
    char     theChar;
    
//    /// Creates an enumerator
//    constructor new : anObject
//        <= new(T<literal>(anObject)).
    
    /// Creates an enumerator
    constructor(string s)
    {
        theLiteral := s;
        theIndex := -1;
        theDiff := 1;
        theChar := $0
    }

    /// Resets the enumerator
    reset()
    {
        theIndex.Value := -1;
        theDiff.Value := 1
    }

    /// Goes to the next enumeration member and returns true if it is not the last member
    bool next()
    {
        int aLength := theLiteral.Length;
        
        theIndex.append(cast int(theDiff));
        
        if (theIndex < aLength)
            {
                theChar := theLiteral.at(cast int(theIndex));
                
                theDiff := theChar.Length;
                
                ^ true
            };

        theDiff.Value := 0;
        theChar := $0;

        ^ false
    }
    
    Enumerator cast() = new Enumerator:
    {
        embeddable dispatch() => self;
    };
    
    enumerable() = theLiteral;
    
    /// Returns the current character
    char get() = theChar;
}

//// --- LiteralIndexer ---
//
///// A literal value indexer
//class LiteralIndexer :: Indexer<char>
//{
//    literal  theLiteral.
//    ref<int> theIndex.
//    
//    /// Creates an indexer
//    constructor new : anObject
//        <= new(T<literal>(anObject)).
//    
//    /// Creates an indexer
//    constructor new(literal aLiteral)
//    [
//        theLiteral := aLiteral.
//        theIndex := 0.
//    ]
//    
//    /// Returns false if the index out of range
//    bool available
//    [
//        int aLength := theLiteral length.
//        int val := theIndex.
//        
//        ^ val < aLength.
//    ]
//
//    /// Returns the current character
//    char get
//    [
//        char ch := $0.
//        theLiteral read(T<int>(theIndex), &ch).
//        
//        ^ ch.
//    ]
//    
//    /// Sets the index by specified value
//    set (char ch)
//    [
//        system'NotSupportedException new; raise.
//    ]
//    
//    /// Reads the current index and saves it into an output parameter
//    readIndexTo(ref<int> retVal)
//    [
//        theIndex readValueTo(retVal).
//    ]
//    
//    /// Sets the index by specified value
//    writeIndex(int val)
//    [
//        theIndex value := val.
//    ]
//    
//    appendIndex(int val)
//    [
//        theIndex append(val).
//    ]
//    
//    /// Reads the length to the output parameter    
//    readLengthTo(ref<int> retVal)
//        => theLiteral.
//        
//    cast T<Indexer> = Indexer::{ embeddable dispatch => self. }.
//}
//
//// --- WideLiteralEnumerator ---
//
///// A literal enumerator
//class WideLiteralEnumerator :: Enumerator<char>
//{
//    wide      theLiteral.
//    ref<int>  theIndex.
//    ref<int>  theDiff.
//    ref<char> theChar.
//    
//    /// Creates an enumerator
//    constructor new : anObject
//        <= new(T<wide>(anObject)).
//    
//    /// Creates an enumerator
//    constructor new(wide aLiteral)
//    [
//        theLiteral := aLiteral.
//        theIndex := -1.
//        theDiff := 1.
//        theChar := $0.
//    ]
//
//    /// Resets the enumerator
//    reset
//    [
//        theIndex value := -1.
//        theDiff value := 1.
//    ]
//
//    /// Goes to the next enumeration member and returns true if it is not the last member
//    bool next
//    [
//        int aLength := theLiteral length.
//
//        int diff := theDiff value.
//        theIndex append(diff).
//        
//        if (theIndex < aLength)
//            [
//                theChar := theLiteral getAt(T<int>(theIndex)).
//                
//                theChar readLengthTo(theDiff).
//                theDiff append:1.
//                theDiff value := theDiff value / 2.
//                
//                ^ true
//            ].
//
//        theDiff value := 0.
//        theChar := $0.
//
//        ^ false
//    ]
//    
//    cast T<Enumerator> = Enumerator::
//    {
//        embeddable dispatch => self.
//    }.
//    
//    enumerable = theLiteral.
//    
//    /// Returns the current character
//    char get = theChar.
//}
//
//// --- WideLiteralInexer ---
//
///// A wide literal enumerator
//class WideLiteralIndexer :: Indexer<wide>
//{
//    wide     theLiteral.
//    ref<int> theIndex.
//    
//    /// Creates an indexer
//    constructor new : anObject
//        <= new(T<wide>(anObject)).
//    
//    /// Creates an indexer
//    constructor new(wide s)
//    [
//        theLiteral := s.
//        theIndex := 0.
//    ]
//
//    /// Returns false if the index out of range    
//    bool available
//    [
//        int aLength := theLiteral length.
//        
//        ^ theIndex < aLength
//    ]
//
//    /// Returns the current character
//    wide get
//        = theLiteral getAt(T<int>(theIndex)).
//    
//    set(wide ch)
//    [
//        system'NotSupportedException new; raise
//    ]
//    
//    /// Reads the current index and saves it into an output parameter
//    readIndexTo(ref<int> retVal)
//    [
//        theIndex readValueTo(retVal).
//    ]
//    
//    /// Sets the index by specified value
//    writeIndex(int val)
//    [
//        theIndex value := val.
//    ]
//    
//    appendIndex(int val)
//    [
//        theIndex append(val).
//    ]
//    
//    /// Reads the length to the output parameter    
//    readLengthTo(ref<int> retVal)
//        => theLiteral.
//        
//    cast T<Indexer> = Indexer::{ embeddable dispatch => self. }.        
//}

// --- ArrayIndexer ---

/// An array indexer
public class ArrayIndexer : Indexer
{
    Array    _array;
    ref<int> _index;

//    /// Returns the parameter indexer
//    constructor new : anArray
//        <= new(T<Array>(anArray)).
    
    /// Returns the parameter indexer
    constructor(Array arr)
    {
        _array := arr;
        _index := 0
    }
    
    /// Returns false if the index out of range
    get bool Available()
    {
        int len := _array.Length;

        ^ _index < len
    }

    /// Returns the current member
    get()
        = _array.at(_index.Value);
    
    /// Sets the current member
    set(object)
    {
        int n := _index.Value;
        
        _array.setAt(n, object)
    }
    
    /// Reads the current index and saves it into an output parameter
    readIndexTo(ref int retVal)
    {
        _index.readValueTo(ref retVal)
    }
    
    /// Sets the index by specified value
    writeIndex(int n)
    {
       _index.Value := n
    }
    
    /// Sets the index by specified value
    appendIndex(int offs)
    {
        _index.append(offs)
    }

//    /// Reads the length to the output parameter    
//    readLengthTo(ref<int> anOutput)
//        => theArray.
}

// --- ArrayEnumerator ---

/// An array enumerator
class ArrayEnumerator : Enumerator
{
    Array    theArray;
    ref<int> theIndex;
    
    /// Creates an array enumerator
    constructor(Array array)
    {
        theArray := array;
        theIndex := -1
    }

    /// Resets the enumerator
    reset()
    {
        theIndex.Value := -1
    }

    /// Goes to the next enumeration member and returns true if it is not the last member
    bool next()
    {
        int length := theArray.Length;
        
        theIndex.append(1);

        ^ cast int(theIndex) < length
    }
    
    /// Returns the current object
    get()
        = theArray.at(cast int(theIndex));
        
    enumerable() = theArray;
}

//// --- BitArrayIndexer ---
//
///// An array indexer
//class BitArrayIndexer :: Indexer<bool>
//{
//    T<BitArray> theArray.
//    ref<int>    theIndex.
//
//    /// Returns the parameter indexer
//    constructor new : anArray
//        <= new(T<BitArray>(anArray)).
//    
//    /// Returns the parameter indexer
//    constructor new(BitArray anArray)
//    [
//        theArray := anArray.
//        theIndex := 0.
//    ]
//    
//    /// Returns false if the index out of range
//    bool available
//    [
//        int aLength := theArray length.
//        
//        ^ T<int>(theIndex) < aLength.
//    ]
//
//    /// Returns the current member
//    bool get
//        = theArray getAt(T<int>(theIndex)).
//    
//    /// Sets the current member
//    set(bool val)
//    [
//        theArray setAt(T<int>(theIndex), val).
//    ]
//    
//    /// Reads the current index and saves it into an output parameter
//    readIndexTo(ref<int> retVal)
//    [
//        theIndex readValueTo(retVal)
//    ]
//    
//    /// Sets the index by specified value
//    writeIndex(int val)
//    [
//        theIndex value := val.
//    ]
//    
//    appendIndex(int val)
//    [
//        theIndex append(val).
//    ]
//    
//    /// Reads the length to the output parameter    
//    readLengthTo(ref<int> retVal)
//        => theArray.
//        
//    cast T<Indexer> = Indexer::{ embeddable dispatch => self. }.        
//}
//
//// --- BitArrayEnumerator ---
//
///// An array enumerator
//class BitArrayEnumerator :: Enumerator<bool>
//{
//    T<BitArray> theArray.
//    ref<int>    theIndex.
//    
//    /// Creates an array enumerator
//    constructor new : anArray
//        <= new(T<BitArray>(anArray)).
//    
//    /// Creates an array enumerator
//    constructor new(BitArray a)
//    [
//        theArray := a.
//        theIndex := -1.
//    ]
//
//    /// Resets the enumerator
//    reset
//    [
//        theIndex value := -1.
//    ]
//
//    /// Goes to the next enumeration member and returns true if it is not the last member
//    bool next
//    [
//        int aLength := theArray length.
//        
//        theIndex append(1).
//
//        ^ T<int>(theIndex) < aLength.
//    ]
//    
//    /// Returns the current object
//    bool get
//        = theArray getAt(T<int>(theIndex)).
//        
//    enumerable = theArray.
//    
//    cast T<Enumerator> = Enumerator::{ embeddable dispatch => self. }.
//}
//
//// --- BitArray32Indexer ---
//
///// An array indexer
//class BitArray32Indexer :: Indexer<bool>
//{
//    T<BitArray32> theArray.
//    ref<int>      theIndex.
//
//    /// Returns the parameter indexer
//    constructor new : anArray
//        <= new(T<BitArray32>(anArray)).
//    
//    /// Returns the parameter indexer
//    constructor new(BitArray32 bs)
//    [
//        theArray := bs.
//        theIndex := 0.
//    ]
//    
//    /// Returns false if the index out of range
//    bool available
//    [
//        ^ T<int>(theIndex) < 32
//    ]
//
//    /// Returns the current member
//    bool get
//        = theArray getAt(T<int>(theIndex)).
//    
//    /// Sets the current member
//    set(bool val)
//    [
//        theArray setAt(T<int>(theIndex), val)
//    ]
//    
//    /// Reads the current index and saves it into an output parameter
//    readIndexTo(ref<int> retVal)
//    [
//        theIndex readValueTo(retVal).
//    ]
//    
//    /// Sets the index by specified value
//    writeIndex(int val)
//    [
//        theIndex value := val.
//    ]
//    
//    appendIndex(int val)
//    [
//        theIndex append(val).
//    ]
//    
//    /// Reads the length to the output parameter    
//    readLengthTo(ref<int> retVal)
//        => theArray.
//        
//    cast T<Indexer> = Indexer::{ embeddable dispatch => self. }.        
//}
//
//// --- BitArray32Enumerator ---
//
///// An array enumerator
//class BitArray32Enumerator :: Enumerator<bool>
//{
//    T<BitArray32> theArray.
//    ref<int>      theIndex.
//    
//    /// Creates an array enumerator
//    constructor new : anArray
//        <= new(T<BitArray32>(anArray)).
//    
//    /// Creates an array enumerator
//    constructor new(BitArray32 bs)
//    [
//        theArray := bs.
//        theIndex := -1.
//    ]
//
//    /// Resets the enumerator
//    reset
//    [
//        theIndex value := -1.
//    ]
//
//    /// Goes to the next enumeration member and returns true if it is not the last member
//    bool next
//    [
//        theIndex append(1).
//
//        ^ T<int>(theIndex) < 32.
//    ]
//    
//    /// Returns the current object
//    bool get
//        = theArray getAt(T<int>(theIndex)).
//        
//    enumerable = theArray.        
//    
//    cast T<Enumerator> = Enumerator::{ embeddable dispatch => self. }.
//}