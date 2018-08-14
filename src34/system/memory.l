
// --- Array template ---

public sealed Array<template_subj> :: Indexable<template_subj>
{
    dynamic<template_subj> theArray.

//    cast T<int>:n
//        = V<template_subj>(n).

    Enumerator<template_subj> enumerator = BaseEnumerator<template_subj>::
    {
        @theIndex := -1.
        
        @theLength := self length.
        
        template_subj get = self getAt(@theIndex).
        
        enumerable = self.
        
        reset
        [
            @theIndex := -1
        ]
    }.

    cast T<Indexable> = Indexable::{ /*embeddable dispatch => self.*/T<Indexer> indexer => self. }.
    
    cast Enumerable<template_subj> = Enumerable<template_subj>::{ /*embeddable dispatch => self.*/Enumerator<template_subj> enumerator => self. }.

    cast T<Enumerable> = Enumerable::{ /*embeddable dispatch => self.*/T<Enumerator> enumerator => self. }.

    Indexer<template_subj> indexer = BaseIndexer<template_subj>::
    {
        @theIndex := 0.
        
        @theLength := self length.
        
        template_subj get
            = self getAt(theIndex).
            
        set(template_subj o)
        [
            self setAt(theIndex, o).
        ]
    }.    

    readLengthTo(ref<int> retVal)
    [
        int len := 0.
        theArray >> len.
        
        retVal value := len
    ]
    
    int rvprop length :: readLengthTo.

    template_subj getAt(int n)
    [
        if (n < 0)
            [ system'InvalidArgumentException new; raise ].
        
        int len := 0.
        theArray >> len.

        if (n < len)
            [ ^ theArray[n] ];
            [ system'InvalidArgumentException new; raise ]
    ]

    setAt(int n, template_subj item)
    [
        if (n < 0)
            [ system'InvalidArgumentException new; raise ].
        
        int len := 0.
        theArray >> len.

        if (n < len)
            [ theArray[n] := item ];
            [ system'InvalidArgumentException new; raise ]
    ]
               
    setAt(object n, object object)
        <= setAt(T<int>(n), T<template_subj>(object)).
                  
    /// Returns the item at the specified index
    template_subj getAt : aParam
        <= getAt(T<int>(aParam)).
}

//// --- matrixof ---
//
//sealed template matrix :: template_subj
//{
//    array<template_subj> theArray.
//    int                  theRows.
//    int                  theColumns.
//    
//    implicit object:params
//    [
//        theRows := params rows.
//        theColumns := params columns.
//        
//        int len := theRows * theColumns.
//        
//        theArray := template_subj<>(/*theRows * theColumns*/len).
//        
//        var it := params indexer2d \ back:$nil.
//        ifnot($nil == it)
//            [
//                int i := 0.
//                int j := 0.
//                
//                while (i < theRows)
//                [
//                    j := 0.
//                    while (j < theColumns)
//                    [
//                        it writeIndex1 int:i.
//                        it writeIndex2 int:j.
//                        
//                        var val := it get.
//                        
//                        $self setAt int:i int:j template_subj:val.
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
//    stacksafe readLengths vint:Rows vint:Columns
//    [
//        Rows int := theRows.
//        Columns int := theColumns.
//    ]
//
//    int rvprop length  :: readLength.    
//    int rvprop columns :: readColumns.
//    int rvprop rows    :: readRows.
//
//    stacksafe readLength vint:retVal
//    [
//        retVal int := theRows.
//    ]
//
//    stacksafe readColumns vint:retVal
//    [
//        retVal int := theColumns.
//    ]
//
//    stacksafe readRows vint:retVal
//    [
//        retVal int := theRows.
//    ]
//
//    /// Writes an integer value at the specified position
//    stacksafe setAt int:i int:j template_subj:aValue
//    [
//        theArray setAt int(i * theColumns + j) template_subj:aValue.
//    ]
//
//    embeddable template_subj getAt int:i int:j
//        = theArray getAt int(i * theColumns + j).
//
//    getAt : i : j
//        = $self getAt int:i int:j.
//
//    setAt : i : j : aValue
//    [
//        $self setAt int:i int:j template_subj:aValue.    
//    ]
//
//    /// Returns a row
//    getAt : i =
//    {
//        length = theColumns int.
//        
//        getAt : j
//            = $self getAt(i,j).
//        
//        setAt : j : aValue
//        [
//            $self setAt(i,j,aValue).
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
//        vint i := 0.
//        vint j := -1.
//        int length1 := theRows.
//        int length2 := theColumns.
//        
//        ^ Enumerator::
//          {
//              get = $self getAt int:i int:j.
//              
//              enumerable = $self.
//              
//              reset [ i := 0. j := -1. ]
//              
//              next
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

// === basic memory classes ===

// --- ByteArray ---

/// A byte array
public sealed ByteArray :: Indexable<byte>, Array<byte>
{
    /// Creates the byte array with the specified length
    constructor new(int len)
        = dynamic<byte>(len).

    /// Creates the copy of a byte subarray
    constructor new(int anIndex, int aLength, ByteArray anArray)
        <= new(aLength);
    [
        anArray read(anIndex, aLength, self).
    ]

    /// Creates the byte array copy
    constructor new(ByteArray anArray)
        <= new(0, anArray length, anArray).

    /// Reads the array length to the output parameter
    readLengthTo(ref<int> aLength)
        => system'core_routines'__bytearray.

    /// Fills the sub array with a specified value
    fill(int anIndex, int aLength, byte aValue)
    [
        int aTotalLength := self length.
        
        int i := anIndex.
        int m := anIndex + aLength.
        
        if (i > aTotalLength)
            [ system'InvalidArgumentException new; raise ].
        
        if (m > aTotalLength)
            [ system'InvalidArgumentException new; raise ].
            
        while(i < m)
        [
            self write(i, aValue).
            
            i := i + 1
        ]
    ]

    /// Copies a sub array to the provided one at the specified position
    read(int anIndex, int aLength, ByteArray aDump)
        => system'core_routines'__bytearray.

    /// Copies the byte array at the specified position
    write(int anIndex, int aLength, ByteArray aDump)
        => system'core_routines'__bytearray.

    /// Writes the byte value at specified position
    write(int anIndex, byte anInt)
        => system'core_routines'__bytearray.

    /// Reads a byte value at the specified position and saves it into an output variable
    read(int anIndex, ref<byte> anInt)
        => system'core_routines'__bytearray.

    embeddable byte getAt(int n)
    [
        byte retVal := 0.
        
        self read(n, &retVal).
        
        ^ retVal
    ]

    /// Writes a short integer value at the specified position
    write(int anIndex, short anInt)
        => system'core_routines'__bytearray.

    /// Reads a short integer value at the specified position and saves it into an output variable
    read(int anIndex, ref<short> aChar)
        => system'core_routines'__bytearray.

    /// Writes a character value at the specified position
    write(int anIndex, char anInt)
        => system'core_routines'__bytearray.

    /// Reads a character value at the specified position and saves it into an output variable
    read(int anIndex, ref<char> aChar)
        => system'core_routines'__bytearray.

    /// Writes an integer value at the specified position
    write(int anIndex, int anInt)
        => system'core_routines'__bytearray.

    /// Reads an integer value at the specified position and saves it into an output variable
    read(int anIndex, ref<int> anInt)
        => system'core_routines'__bytearray.

    /// Writes a long integer value at the specified position
    write(int anIndex, long aLong)
        => system'core_routines'__bytearray.

    /// Reads a long integer value at the specified position and saves it into an output variable
    read(int anIndex, ref<long> aLong)
        => system'core_routines'__bytearray.

    /// Writes a real value at the specified position
    write(int anIndex, real aLong)
        => system'core_routines'__bytearray.

    /// Reads a real value at the specified position and saves it into an output variable
    read(int anIndex, ref<real> aLong)
        => system'core_routines'__bytearray.

    move(int anIndex, int aLength, int anOffset)
        => system'core_routines'__bytearray.
    
    /// Concatinates the arrays    
    T<ByteArray> add(int aLength2, ByteArray anArray)
    [
        int aLength1 := self length.
        
        T<ByteArray> aRetVal := ByteArray new(aLength1 + aLength2).

        byte tmp := 0.
        int anIndex := 0.
        while(anIndex < aLength1)
        [
            self read(anIndex, &tmp).
            aRetVal write(anIndex, tmp).
            
            anIndex := anIndex + 1
        ].

        anIndex := 0.
        while(anIndex < aLength2)
        [
            anArray read(anIndex, &tmp).
            aRetVal write(aLength1 + anIndex, tmp).
            
            anIndex := anIndex + 1
        ].
            
        ^ aRetVal
    ]    

    /// Creates the object copy
    clone = ByteArray new(self).
}

// --- ShortArray ---

/// A short integer array
public sealed ShortArray :: Indexable<short>, Array<short>
{
    /// Creates the short array with the specified length
    constructor new(int aLength)
        = dynamic<short>(aLength).
    
    /// Creates the copy of a short subarray
    constructor new(int anIndex, int aLength, ShortArray anArray)
        <= new(aLength);
    [
        anArray read(anIndex, aLength, self).
    ]

    /// Creates the short array copy
    constructor new(ShortArray anArray)
        <= new(0, anArray length, anArray).

    /// Reads the array length to the output parameter
    readLengthTo(ref<int> aLength)
        => system'core_routines'__shortarray.

    /// Fills the sub array with a specified value
    fill(int anIndex, int aLength, short aValue)
    [
        int aTotalLength := self length.
        
        int i := anIndex.
        int m := anIndex + aLength.
        
        if (i > aTotalLength)
            [ system'InvalidArgumentException new; raise ].
        
        if (m > aTotalLength)
            [ system'InvalidArgumentException new; raise ].
            
        while(i < m)
        [
            self write(i, aValue).
            
            i := i + 1
        ]
    ]
    
    /// Copies a sub array to the provided one at the specified position
    read(int anIndex, int aLength, ShortArray anArray)
        => system'core_routines'__shortarray.

    /// Copies the short array at the specified position   
    write(int anIndex, int aLength, ShortArray anArray)
        => system'core_routines'__shortarray.

    /// Writes a short integer value at the specified position
    write(int anIndex, short aValue)
        => system'core_routines'__shortarray.

    embeddable short getAt(int n)
    [
        short retVal := 0.
        
        self read(n, &retVal).
        
        ^ retVal
    ]

    /// Reads a short integer value at the specified position and saves it into an output variable
    read(int anIndex, ref<short> aValue)
        => system'core_routines'__shortarray.

    /// Creates the object copy
    clone = ShortArray new(self).
}

// --- IntArray ---

/// An array of integers
public sealed IntArray :: Indexable<int>, Array<int>
{
    /// Creates the array with the specified length
    /*private*/ constructor new(int aLength)
        = dynamic<int>(aLength).
        
    /// Creates the array with the specified length
//    stacksafe constructor new int : aLength
//        <= $new(aLength);
//    [
//        int i := 0.
//        while (i < aLength)
//        [
//            $self[i] := 0.
//            
//            i := i + 1
//        ]
//    ]

//    /// Creates the array with the specified length
//    constructor new : aLength
//        <= new int:aLength.

    /// Creates the copy of a subarray
    stacksafe constructor new(int anIndex, int aLength, IntArray anArray)
        <= new(aLength);
    [
        anArray read(anIndex, aLength, self).
    ]

    /// Creates the short array copy
    stacksafe constructor new(IntArray anArray)
        <= new(0, anArray length, anArray).
    
    constructor new(Indexer<int> it)
        <= new(it length);
    [
        int len := self length.
        int i := 0.
        while (i < len)
        [
            it writeIndex(i).
            
            int val := it get.
            
            self write(i, val).
            
            i += 1
        ]
    ]
    
    /// Reads the array length to the output parameter
    readLengthTo(ref<int> aLength)
        => system'core_routines'__intarray.

    /// Fills the sub array with a specified value
    fill(int anIndex, int aLength, int aValue)
    [
        int aTotalLength := self length.
        
        int i := anIndex.
        int m := anIndex + aLength.
        
        if (i > aTotalLength)
            [ system'InvalidArgumentException new; raise ].
        
        if (m > aTotalLength)
            [ system'InvalidArgumentException new; raise ].
            
        while(i < m)
        [
            self write(i, aValue).
            
            i := i + 1
        ]
    ]

    /// Copies a sub array to the provided one at the specified position
    stacksafe read(int anIndex, int aLength, IntArray anArray)
        => system'core_routines'__intarray.

    /// Copies the short array at the specified position
    stacksafe write(int anIndex, int aLength, IntArray anArray)
        => system'core_routines'__intarray.

    /// Writes an integer value at the specified position
    stacksafe write(int anIndex, int aValue)
        => system'core_routines'__intarray.

    /// Reads an integer value at the specified position and saves it into an output variable
    stacksafe read(int anIndex, ref<int> anInt)
        => system'core_routines'__intarray.

    stacksafe embeddable int getAt(int n)
    [
        int retVal := 0.
        
        self read(n, &retVal).
        
        ^ retVal
    ]

    /// Creates the object copy
    clone = IntArray new(self).
}

// --- BitArray ---

public sealed class BitArray
{
    T<IntArray> theArray.
    int         theLength.
    
    /// Creates the array with the specified length
    constructor new(int length)
    [
        theLength := length.
        theArray := IntArray new(length / 32 + 1).
    ]
    
    readLengthTo(ref<int> retVal)
    [
        retVal value := theLength.
    ]
    
    int rvprop length :: readLengthTo.
    
    getAt:index
        = self getAt(T<int>(index)).
    
    bool getAt(int index)
    [
        int arr_index := index / 32.
        int bit_index := arr_index * 32.
        bit_index := index - arr_index.
        
        int bit_mask := 1.
        if (bit_index != 0)
            [ bit_mask := bit_mask << bit_index ].
        
        int bits := 0.
        theArray read(arr_index, &bits).
        
        ^ (bits && bit_mask) != 0
    ]
//    
//    setAt:index:value
//    [
//        $self setAt int:index bool:value.
//    ]
    
    setAt(int index, bool value)
    [
        int arr_index := index / 32.
        int bit_index := arr_index * 32.
        bit_index := index - arr_index.
        
        int bit_mask := 1.
        if (bit_index != 0)
            [ bit_mask := bit_mask << bit_index ].
        
        int bits := 0.
        theArray read(arr_index, &bits).
        
        value
            ? [ bits := bits || bit_mask. ]
            ! [  
                bit_mask := bit_mask inverted.
                
                bits := bits && bit_mask
            ].
            
        theArray write(arr_index, bits)
    ]
    
//    /// Returns the array enumerator
//    enumerator = BitArrayEnumerator new bitarray:$self.
//
//    /// Returns the array indexer
//    indexer = BitArrayIndexer new bitarray:$self.    
}

// --- BitArray32 ---

public sealed class BitArray32
{
    int theValue.
    
    constructor new(int value)
    [
        theValue := value.
    ]
    
//    constructor new:value
//        <= new int:value.
    
    int length = 32.
    
    write(int value)
    [
        theValue := value.
    ]
//    
//    write : value
//    [
//        value cast:%write to:$self.
//    ]
    
//    getAt:index
//        = $self getAt int:index.
    
    bool getAt(int index)
    [
        int bit_mask := 1.
        if (index != 0)
            [ bit_mask := bit_mask << index ].
        
        ^ (theValue && bit_mask) != 0
    ]
    
//    setAt:index:value
//    [
//        $self setAt int:index bool:value.
//    ]
    
    setAt(int index, bool value)
    [
//        int bit_mask := 1.
//        if (index != 0)
//            [ bit_mask := bit_mask << index ].
//        
//        if(value)
//            [ theValue := theValue || bit_mask ];
//            [  
//                bit_mask := bit_mask inverted.
//                
//                theValue := theValue && bit_mask
//            ].
    ]
    
//    /// Returns the array enumerator
//    enumerator = BitArray32Enumerator new bitset:$self.
//
//    /// Returns the array indexer
//    indexer = BitArray32Indexer new bitset:$self.
}

// --- IntMatrix ---

public sealed class IntMatrix
{
    T<IntArray> theArray.
    int         theRows.
    int         theColumns.
    
    constructor new(int Rows, int Columns)
    [
        theRows := Rows.
        theColumns := Columns.
        
        theArray := IntArray new(Columns * Rows).        
    ]
        
//    constructor new : Rows : Columns
//        <= new int:Rows int:Columns.
        
    /// Reads the matrix lengths to the output parameter
    stacksafe readLengthsTo(ref<int> Rows, ref<int> Columns)
    [
        Rows value := theRows.
        Columns value := theColumns
    ]

    stacksafe readLengthTo(ref<int> retVal)
    [
        retVal value := theRows
    ]
    
    int rvprop length :: readLengthTo.

    stacksafe readColumnsTo(ref<int> retVal)
    [
        retVal value := theColumns
    ]
    
    int rvprop columns :: readColumnsTo.

    stacksafe readRowsTo(ref<int> retVal)
    [
        retVal value := theRows
    ]
    
    int rvprop rows :: readRowsTo.

    /// Writes an integer value at the specified position
    stacksafe write(int i, int j, int aValue)
    [
        theArray write(i * theColumns + j, aValue)
    ]

    /// Reads an integer value at the specified position and saves it into an output variable
    stacksafe read(int i, int j, ref<int> anInt)
    [
        theArray read(i * theColumns + j, anInt)
    ]

    embeddable int getAt(int i, int j)
    [
        int retVal := 0.
        self read(i, j, &retVal).
        ^ retVal
    ]

    getAt(object i, object j)
    [
        int aValue := 0.
        self read(T<int>(i), T<int>(j), &aValue).
        
        ^ aValue
    ]

    setAt(object i, object j, object aValue)
    [
        self write(T<int>(i), T<int>(j), T<int>(aValue)).
    ]

    /// Returns a row
    getAt(int i) = Indexable<int>::
    {
        length = theColumns.
        
        Indexer<int> indexer = BaseIndexer<int>::
        {
            @theIndex := 0.
            
            @theLength := self columns.
            
            int get
                = self getAt(i,theIndex).
                
            set(int o)
            [
                self setAt(i,theIndex,o).
            ]                        
        }.    
        
        getAt(int j)
            = self getAt(i,j).
        
        setAt(int j, int aValue)
        [
            self setAt(i,j,aValue).
        ]
    }.
    
    enumerator = Enumerator::
    {
        int i := -1.
        
        bool next
        [
            i += 1.
            if (i >= theRows)
            [
                ^ false
            ].
            
            ^ true
        ]
        
        get = BaseEnumerator<int>::
        {
            @theIndex := -1.
            @theLength := theColumns.
        
            reset [ theIndex value := -1 ]
            
            enumerable = self.
        
            int get
                = self[i][theIndex value].
        }.
        
        reset [ i := -1 ]
        
        enumerable = self.
    }.
}

// --- RealArray ---

/// An array of real numbers
public sealed RealArray :: Indexable<real>, Array<real>
{
    /// Creates the array with the specified length
    constructor new(int aLength)
        = dynamic<real>(aLength).

//    /// Creates the array with the specified length
//    constructor new : aLength
//        <= new int:aLength.

    /// Creates the copy of a subarray
    constructor new(int anIndex, int aLength, RealArray anArray)
        <= new(aLength);
    [
        anArray read(anIndex, aLength, self)
    ]

    /// Creates the array copy
    constructor new(RealArray anArray)
        <= new(0, anArray length, anArray).

    /// Fills the sub array with a specified value
    fill(int anIndex, int aLength, real aValue)
    [
        int aTotalLength := self length.
        
        int i := anIndex.
        int m := anIndex + aLength.
        
        if (i > aTotalLength)
            [ system'InvalidArgumentException new; raise ].
        
        if (m > aTotalLength)
            [ system'InvalidArgumentException new; raise ].
          
        while(i < m)
        [
            self setAt(i, aValue).
            
            i := i + 1.
        ].
    ]

    /// Copies a sub array to the provided one at the specified position
    read(int anIndex, int aLength, RealArray anArray)
        => system'core_routines'__realarray.

    /// Copies the short array at the specified position
    write(int anIndex, int aLength, RealArray anArray)
        => system'core_routines'__realarray.

    /// Reads a real value at the specified position and saves it into an output variable
    read(int anIndex, ref<real> aReal)
        => system'core_routines'__realarray.

    embeddable real getAt(int n)
    [
        real retVal := 0.
        
        self read(n, &retVal).
        
        ^ retVal.
    ]

    /// Creates the object copy
    clone = RealArray new(self).
}

// --- RealMatrix ---

public sealed class RealMatrix
{
    T<RealArray> theArray.
    int          theRows.
    int          theColumns.
    
    stacksafe constructor new(int Rows, int Columns)
    [
        theArray := RealArray new(Columns * Rows).
        
        theRows := Rows.
        theColumns := Columns.
    ]
        
    constructor new(object Rows, object Columns)
        <= new(T<int>(Columns), T<int>(Rows)).

    /// Reads the matrix lengths to the output parameter
    stacksafe readLengthsTo(ref<int> Rows, ref<int> Columns)
    [
        Columns value := theColumns.
        Rows value := theRows.
    ]

    stacksafe readLengthTo(ref<int> retVal)
    [
        retVal value := theRows.
    ]
    
    int rvprop length :: readLengthTo.

    stacksafe readColumnsTo(ref<int> retVal)
    [
        retVal value := theColumns.
    ]
    
    int rvprop columns :: readColumnsTo.

    stacksafe readRowsTo(ref<int> retVal)
    [
        retVal value := theRows
    ]
    
    int rvprop rows :: readRowsTo.

    /// Writes an integer value at the specified position
    stacksafe write(int i, int j, real aValue)
    [
        int anIndex := i * theColumns + j.
        
        theArray setAt(anIndex, aValue)
    ]

    /// Reads an integer value at the specified position and saves it into an output variable
    stacksafe read(int i, int j, ref<real> aValue)
    [
        int anIndex := i * theColumns + j.
        
        theArray read(anIndex, aValue)
    ]

    embeddable real getAt(int i, int j)
    [
        real retVal := 0.0r.
        self read(i, j, &retVal).
        
        ^ retVal
    ]

    getAt(object i, object j)
    [
        real aValue := 0.0r.
        self read(T<int>(i), T<int>(j), &aValue).
        
        ^ RealNumber new(aValue)
    ]

    setAt(object i, object j, object aValue)
    [
        self write(i, j, aValue)
    ]

    /// Returns a row
    getAt : i =
    {
        length = theColumns.
        
        getAt : j
            = self getAt(i,j).
        
        setAt(object j, object aValue)
        [
            self setAt(i,j,aValue)
        ]
    }.
}

// --- CharArray ---

/// An array of unicode characters (UTF-32)
public sealed CharArray :: Indexable<char>, Array<char>
{
    /// Creates the new array with a specified length
    constructor new(int len)
        = dynamic<char>(len).

    /// Creates the new array with a specified length
    constructor new : aLength
        <= new(T<int>(aLength)).

    /// Creates the copy of a subarray
    constructor new(int anIndex, int aLength, CharArray anArray)
        <= new(aLength);
    [
        anArray read(anIndex, aLength, self).
    ]
     
    /// Creates the short array copy
    constructor new(CharArray anArray)
        <= new(0, anArray length, anArray).
    
    /// Reads the array length to the output parameter
    readLengthTo(ref<int> aLength)
        => system'core_routines'__intarray.

    /// Fills the sub array with a specified value
    fill(int anIndex, int aLength, char aValue)
    [
        int aTotalLength := self length.
        
        int i := anIndex.
        int m := anIndex + aLength.
        
        if (i > aTotalLength)
            [ system'InvalidArgumentException new; raise ].
        
        if (m > aTotalLength)
            [ system'InvalidArgumentException new; raise ].
            
        while(i < m)
        [
            self write(i, aValue).
            
            i := i + 1
        ]
    ]

    /// Copies a sub array to the provided one at the specified position
    read(int anIndex, int aLength, CharArray anArray)
        => system'core_routines'__chararray.

    /// Copies the array at the specified position
    write(int anIndex, int aLength, CharArray anArray)
        => system'core_routines'__chararray.

    /// Writes a character value at the specified position
    write(int anIndex, char aValue)
        => system'core_routines'__chararray.

    /// Reads a character value at the specified position and saves it into an output variable
    read(int anIndex, ref<char> anInt)
        => system'core_routines'__chararray.

    embeddable char getAt(int n)
    [
        char retVal := $0.
        
        self read(n, &retVal).
        
        ^ retVal
    ]

    move(int anIndex, int aLength, int anOffset)
        => system'core_routines'__chararray.

    /// Creates the object copy
    clone = CharArray new(self).
}
        
// --- emptyliteralvalue ---

/// An empty literal value
const literal public emptyLiteral = "".

// --- LiteralValue ---

/// A UTF-8 literal value
public sealed __string LiteralValue :: BaseValue
{
    dynamic<byte> theArray.

    constructor new
        <= min.

    /// return the default value
    constructor
        = emptyLiteral.
        
    /// Creates the empty literal
    constructor min
        = emptyLiteral.
        
    /// Creates the literal string with specified length        
    stacksafe constructor new(int size)
        = dynamic<byte>(size + 1).

    /// Creates the literal string with specified length and copies the parameter value
    stacksafe constructor new(int aSize, literal aLiteral)
        <= new(aSize);
    [
        system'core_routines'__literal copy(aLiteral, self).
    ]

    /// Creates the literal string with specified length fills with spciefied character value        
    stacksafe constructor new(int aSize, char aChar)
        => system'core_routines'__literal.

    /// Creates the literal string copy              
    constructor new(literal aLiteral)
        <= new(aLiteral length, aLiteral).
        
    /// Creates the copy of a subarray        
    stacksafe constructor new(int anIndex, int aSize, CharArray anArray)
        => system'core_routines'__literal.

    internal stacksafe constructor fromByteArray(int anIndex, int aSize, ByteArray anArray)
        <= new(aSize);
    [
        system'core_routines'__literal copy(self, anIndex, aSize, anArray).
    ]
    
    /// Converts to UTF-16 literal
    cast T<wide> = wideConvertor convert(self).

    cast Enumerable<char> = Enumerable<char>::{ Enumerator<char> enumerator => self. }.

    T<literal> literal = self.

    bool equal:n
    [
        var l := T<literal>(n) \ back:nil.
        if (nil == l)
        [
            ^ false.
        ];
        [
            ^ self equal(l).
        ]
    ]

    bool less:o
        <= less(T<literal>(o)).
        
    bool equal(BaseVariable v)
        <= equal(v value).
        
    bool less(BaseVariable v)
        <= less(v value).
        
    /// Compares the values
    bool equal(literal aLiteral)
        => system'core_routines'__literal.

    /// Compares the values
    bool less(literal aLiteral)
        => system'core_routines'__literal.

    /// Compares the values
    bool equal(wide aLiteral)
        = T<wide>(self) equal(aLiteral).

    /// Compares the values
    bool less(wide aLiteral)
        = T<wide>(self) less(aLiteral).

    /// Compares the values
    bool equal(char aChar)
        <= equal(T<literal>(aChar)).

    /// Compares the values
    bool less(char aChar)
        <= less(T<literal>(aChar)).

    /// Returns the character at the specified index
    char getAt(int anIndex)
    [
        char aChar := $0.
        self read(anIndex, &aChar).
        
        ^ CharValue new(aChar).
    ]

    literal add : anObject 
        <= add(T<literal>(anObject)).

    /// Concatinates the values
    literal add(BaseVariable v)
        <= add(v value).
        
    /// Concatinates the values
    literal add(literal aLiteral)
        => system'core_routines'__literal.

    /// Concatinates the values
    stacksafe literal add(char aChar)
        = self add(T<literal>(aChar)).

    // Reads the character at the specified index and saves into an output parameter
    stacksafe read(int anIndex, ref<char> aChar)
        => system'core_routines'__literal.

    // Reads the byte at the specified index and saves into an output parameter
    stacksafe read(int anIndex, ref<byte> anInt)
        => system'core_routines'__literal.

    /// Saves the index of the first occurence of the specified substring into an output parameter.
    ///
    /// If the substring is not found. returns -1
    stacksafe indexOf(int anIndex, literal aLiteral, ref<int> aRetVal)
        => system'core_routines'__literal.

    /// Saves the index of the first occurence of the specified symbol into an output parameter.
    ///
    /// If the substring is not found. returns -1
    stacksafe indexOf(int anIndex, char aChar, ref<int> aRetVal)
        => system'core_routines'__literal.

    /// Returns the index of the first occurence of the specified symbol.
    /// 
    /// If the substring is not found. returns -1
    stacksafe indexOf(char aChar) at(int anIndex)
    [
        int aRetVal := 0.
        self indexOf(anIndex, aChar, &aRetVal).
        
        ^ IntNumber new(aRetVal).
    ]

    /// Returns the index of the first occurence of the specified substring.
    ///
    /// If the substring is not found. returns -1
    stacksafe indexOf(literal aLiteral) at(int anIndex)
    [
        int aRetVal := 0.
        self indexOf(anIndex, aLiteral, &aRetVal).
        
        ^ IntNumber new(aRetVal).
    ]

    /// Inserts the substring into and returns the new one
    stacksafe literal insert(int anIndex, literal aLiteral)
        => system'core_routines'__literal.

    /// Inserts a character into and returns the new one
    stacksafe literal insert(int anIndex, char aChar)
        = self insert(anIndex, T<literal>(aChar)).

    /// Inserts an object into and returns the new one
    insert : anObject at:anIndex
        = self insert(anIndex, anObject).

    /// Deletes the substring from the object and returns the new one
    delete:aLength at:anIndex
        = self delete(T<int>(anIndex), T<int>(aLength)).

    /// Deletes the substring from the object and returns the new one
    stacksafe literal delete(int anIndex, int aLength)
        => system'core_routines'__literal.

    /// Returns the substring
    Substring : aLength at:anIndex
        = self Substring(T<int>(anIndex), T<int>(aLength)).

    /// Returns the substring
    stacksafe literal Substring(int anIndex, int aLength)
        => system'core_routines'__literal.

    /// Reads the literal length into an output parameter
    stacksafe readLengthTo(ref<int> aLength)
        => system'core_routines'__literal.
        
    int rvprop length :: readLengthTo.

    /// Saves the literal into a UTF-32 character array
    stacksafe save(int anIndex, CharArray anArray, ref<int> aRetVal)
        => system'core_routines'__literal.

    stacksafe internal saveToByteArray(int anIndex, ByteArray anArray, int aRetVal)
        => system'core_routines'__literal.

    /// Returns the string enumerator
    Enumerator<char> enumerator = LiteralEnumerator new(self).

//    /// Returns the string indexer
//    indexer = LiteralIndexer new literal:$self.
    
    /// Clones the value
    clone = LiteralValue new(self).

    /// Reads the object hash code into the output parameter
    stacksafe readHashCodeTo(ref<int> retVal)
    [
        int len := self length.
        
        int h := system'external core_utf8hashcode(self, len).
        
        retVal value := h.
    ]    
        
    int rvprop hash_code :: readHashCodeTo.
}

// --- emptyWideLiteralValue ---

/// An empty wide literal value
const wide emptyWideLiteral = ""w.

// --- WideLiteralValue ---

/// A UTF-18 literal value
public sealed __string WideLiteralValue :: BaseValue
{
    dynamic<short> theArray.

    /// return the default value
    constructor
        = emptyWideLiteral.

    /// Creates the empty literal
    constructor min
        = emptyWideLiteral.
        
    /// Creates the literal string with specified length
    constructor new(int size)
        = dynamic<short>(size + 1).

    /// Creates the literal string with specified length and copies the parameter value
    constructor new(int aSize, wide aLiteral)
        <= new(aSize);
    [
        system'core_routines'__wide copy(aLiteral, self).
    ]

    /// Creates the literal string with specified length fills with spciefied character value
    constructor new(int aSize, char aChar)
        => system'core_routines'__wide.

    /// Creates the literal string copy
    constructor new(wide aLiteral)
        <= new(aLiteral length, aLiteral).
        
    /// Creates the copy of a subarray
    stacksafe constructor new(int anIndex, int aSize, CharArray anArray)
        => system'core_routines'__wide.

    internal stacksafe constructor fromShortArray(int anIndex, int aSize, ShortArray anArray)
        <= new(aSize);
    [
        system'core_routines'__wide copy(self, anIndex, aSize, anArray).
    ]

    /// Converts to UTF-8 literal
    cast T<literal> = literalConvertor convert(self).

    T<literal> literal = literalConvertor convert(self).
    
    bool equal:n
    [
        var l := T<wide>(n) \ back:nil.
        if (nil == l)
        [
            ^ false.
        ];
        [
            ^ self equal(l).
        ]
    ]

    bool less:n
        <= less(T<wide>(n)).
        
    bool equal(BaseVariable v)
        <= equal(v value).
        
    bool less(BaseVariable v)
        <= less(v value).

    /// Compares the values
    bool equal(wide aLiteral)
        => system'core_routines'__wide.

    /// Compares the values
    bool less(wide aLiteral)
        => system'core_routines'__wide.

    /// Compares the values
    bool equal(char aChar)
        <= equal(T<wide>(aChar)).

    /// Compares the values
    bool less(char aChar)
        <= less(T<wide>(aChar)).

    /// Returns the character at the specified index
    getAt : aParam
        <= getAt(T<int>(aParam)).

    /// Returns the character at the specified index
    char getAt(int anIndex)
    [
        char aChar := $0.
        self read(anIndex, &aChar).
        
        ^ CharValue new(aChar)
    ]

    wide add : anObject 
        <= add(T<literal>(anObject)).

    /// Concatinates the values
    wide add(BaseVariable v)
        <= add(v value).
    
    /// Concatinates the values
    wide add(wide aLiteral)
        => system'core_routines'__wide.

    /// Concatinates the values
    wide add(char aChar)
        = self add(wideConvertor convert(aChar)).

    /// Concatinates the values
    wide add(literal aLiteral)
        = self add(T<wide>(aLiteral)).

    // Reads the character at the specified index and saves into an output parameter
    read(int anIndex, ref<char> aChar)
        => system'core_routines'__wide.

    // Reads the short integer at the specified index and saves into an output parameter
    read(int anIndex, ref<short>aValue)
        => system'core_routines'__wide.

    /// Saves the index of the first occurence of the specified substring into an output parameter.
    ///
    /// If the substring is not found. returns -1
    indexOf(int anIndex, wide aLiteral, ref<int> aRetVal)
        => system'core_routines'__wide.

    /// Saves the index of the first occurence of the specified character into an output parameter.
    ///
    /// If the substring is not found. returns -1
    indexOf(int anIndex, char aChar, ref<int> aRetVal)
        => system'core_routines'__wide.

    /// Returns the index of the first occurence of the specified character.
    ///
    /// If the substring is not found. returns -1
    indexOf(char aChar) at(int anIndex)
    [
        int aRetVal := 0.
        self indexOf(anIndex, aChar, &aRetVal).
        
        ^ IntNumber new(aRetVal).
    ]

    /// Returns the index of the first occurence of the specified substring.
    ///
    /// If the substring is not found. returns -1
    indexOf(wide aLiteral) at(int anIndex)
    [
        int aRetVal := 0.
        self indexOf(anIndex, aLiteral, &aRetVal).
        
        ^ IntNumber new(aRetVal).
    ]

    /// Inserts the substring into and returns the new one
    wide insert(int anIndex, wide aLiteral)
        => system'core_routines'__wide.

    /// Inserts the substring into and returns the new one
    wide insert(int anIndex, literal aLiteral)
        = self insert(anIndex, T<wide>(aLiteral)).

    /// Inserts a character into and returns the new one
    wide insert(int anIndex, char aChar)
        = self insert(anIndex, wideConvertor convert(aChar)).

    /// Inserts an object into and returns the new one
    insert : anObject at:anIndex
        = self insert(anIndex, anObject).

    /// Deletes the substring from the object and returns the new one
    delete:aLength at:anIndex
        = self delete(T<int>(anIndex), T<int>(aLength)).
            
    /// Deletes the substring from the object and returns the new one            
    wide delete(int anIndex, int aLength)
        => system'core_routines'__wide.

    /// Returns the substring
    wide Substring(int anIndex, int aLength)
        => system'core_routines'__wide.

    wide Substring:aLength at:anIndex
        <= Substring(T<int>(anIndex), T<int>(aLength)).

    /// Reads the literal length into an output parameter
    readLengthTo(ref<int> aLength)
        =>system'core_routines'__wide.
        
    int rvprop length :: readLengthTo.

    /// Saves the literal into a UTF-32 character array
    save(int anIndex, CharArray anArray, ref<int> aLength)
        => system'core_routines'__wide.

    internal saveToShortArray(int anIndex, ShortArray anArray, int aRetVal)
        => system'core_routines'__wide.

    /// Returns the object enumerator
    Enumerator<char> enumerator = WideLiteralEnumerator new(self).
    
//    /// Returns the object indexer
//    indexer = WideLiteralIndexer new wide:$self.
    
    /// Clones the value
    clone = WideLiteralValue new(self).

    stacksafe readHashCodeTo(ref<int> retVal)
    [
        int len := self length.
        
        int h := system'external core_utf16hashcode(self, len).
        
        retVal value := h.
    ]    
        
    int rvprop hash_code :: readHashCodeTo.
}

// --- Variant ---

/// A basic type variant class
public sealed class Variant
{
    object theValue.
    
    constructor new : object
    [
        theValue := object.
    ]
    
    /// Tries to convert the value to a byte number
    cast T<byte>
        = byteConvertor convert:theValue.

    /// Tries to convert the value to a short integer number
    cast T<short>
        = shortConvertor convert:theValue.

    /// Tries to convert the value to a character
    cast T<char>
        = charConvertor convert:theValue.

    /// Tries to convert the literal value to the integer number
    cast T<int>
        = intConvertor convert:theValue.

    /// Tries to convert the literal value to the long integer number
    cast T<long>
        = longConvertor convert:theValue.

    /// Tries to convert the literal value to the real number
    cast T<real>
        = realConvertor convert:theValue.

    /// Returns a literal value
    cast T<literal>
        = literalConvertor convert:theValue.
    
    /// Returns a wide literal value
    cast T<wide>
        = wideConvertor convert:theValue.

    T<literal> literal => theValue.

    /// Converts the parameter into a literal string
    set value(int aValue)
    [
        theValue := aValue.
    ]    

    /// Converts the parameter into a literal string
    set value(short aValue)
    [
        theValue := aValue.
    ]    

    /// Converts the parameter into a literal string
    set value(byte aValue)
    [
        theValue := aValue.
    ]    

    /// Converts the parameter into a literal string
    set value(long aValue)
    [
        theValue := aValue.
    ]    

    /// Converts the parameter into a literal string
    set value(real aValue)
    [
        theValue := aValue.
    ]    

    private castValueTo:aTarget
        = {
            eval:o
                = o literal.
            
            eval(BaseVariable v)
                <= eval(v value).
            
            eval(int aValue)
                = T<int>(self).
                
            eval(byte aValue)
                = T<byte>(self).
                
            eval(short aValue)
                = T<short>(self).
                
            eval(long aValue)
                = T<long>(self).
                
            eval(real aValue)
                = T<real>(self).
                
            eval(literal aValue)
                = T<literal>(self).
                
            eval(wide aValue)
                = T<wide>(self).
        } eval:aTarget.

    saveTo : aTarget
    [
        aTarget value := self castValueTo:aTarget.
        
        ^ aTarget
    ]

//    add : aTarget
//        = aTarget add($self $castValueTo:aTarget).
//
//    subtract : aTarget
//        = $self $castValueTo:aTarget; subtract:aTarget.
//
//    multiply : aTarget
//        = $self $castValueTo:aTarget; multiply:aTarget.
//
//    divide : aTarget
//        = $self $castValueTo:aTarget; divide:aTarget.
//
//    bool equal : aTarget
//        = $self $castValueTo:aTarget; equal:aTarget.
}

// --- Array ---

/// An array
public sealed Array :: Indexable, Array<object>
{
    /// Creates an empty array
    constructor min
        = dynamic<object>(0).
    
    /// Creates the array with the specified length
    constructor new : aLength
        <= new(T<int>(aLength)).
    
    /// Creates the array with the specified length
    constructor new(int aLength)
        = dynamic<object>(aLength).

    /// Creates a copy of the provided array
    constructor new(Array anArray)
        <= new(0, anArray length, anArray).
    
    /// Creates a copy of the provided sub array
    constructor new(int anIndex, int aLength, Array anArray)
        <= new(aLength);
    [
        system'core_routines'__array copy(self, anIndex, aLength, anArray).
    ]
        
//    constructor new(object singleItem)
//        <= new(1);
//    [
//        self setAt(0, singleItem).
//    ]

    /// Concatinates the arrays    
    add(Array anArray)
    [
        int aLength1 := self length.
        int aLength2 := anArray length.
        
        auto aRetVal := Array new(aLength1 + aLength2).

        int anIndex := 0.
        while(anIndex < aLength1)
        [
            aRetVal setAt(anIndex, self getAt(anIndex)).
            
            anIndex := anIndex + 1.
        ].

        anIndex := 0.
        while(anIndex < aLength2)
        [
            aRetVal setAt(aLength1 + anIndex, anArray getAt(anIndex)).
            
            anIndex := anIndex + 1.
        ].
            
        ^ aRetVal
    ]

    /// Reads the array length into an output parameter
    readLengthTo(ref<int> aLength)
        => system'core_routines'__array.

    /// Sets the specified array member
    setAt(int anIndex, object anObject)
        => system'core_routines'__array.

    /// Returns the specified array member
    object getAt(int anIndex)
        => system'core_routines'__array.
 
//    /// Returns the array enumerator
//    enumerator = ArrayEnumerator new array:$self.

    /// Returns the array indexer
    T<Indexer> indexer = ArrayIndexer new(self).

    /// Clones the array
    T<Array> clone 
        = Array new(self).
}

// --- paramsOp ---

extension<__args> paramsOp
{
    stacksafe readLengthTo(ref<int> aLength)
        => system'core_routines'__args.            
        
    int rvprop length :: readLengthTo.
}

// === Convertors ===

// --- literalConvertor ---

/// A literal convertor
public class literalConvertor =
{
    stacksafe literal convert(BaseVariable v)
        <= convert(v value).
    
    stacksafe literal convert(BaseVariable v, int radix)
        <= convert(v value, radix).
    
    /// Converts the parameter to the literal value
    stacksafe convert(char aValue, ByteArray buffer, ref<int> length)
        => system'core_routines'__literalconvertor.
    
    stacksafe literal convert(char aValue)
    [
        T<byte,4> buffer.
        int       length := 0.
        
        self convert(aValue, T<ByteArray>(buffer), &length).
        
        ^ LiteralValue fromByteArray(0, length, T<ByteArray>(buffer))
    ]
    
    stacksafe convert(int aNumber, ByteArray buffer, ref<int> length)
        => system'core_routines'__literalconvertor.
    
    stacksafe convert(uint aNumber, ByteArray buffer, ref<int> length)
        => system'core_routines'__literalconvertor.
    
    /// Converts the parameter to the literal value
    stacksafe literal convert(int aNumber)
    [
        T<byte,12> buffer.
        int        length := 0.
        
        self convert(aNumber, T<ByteArray>(buffer), &length).
        
        ^ LiteralValue fromByteArray(0, length, T<ByteArray>(buffer))
    ]
    
    /// Converts the parameter to the literal value
    stacksafe literal convert(uint aNumber)
    [
        T<byte,12> buffer.
        int        length := 0.
        
        self convert(aNumber, T<ByteArray>(buffer), &length).
        
        ^ LiteralValue fromByteArray(0, length, T<ByteArray>(buffer))
    ]

    /// Converts the parameter to the literal value
    stacksafe convert(short aNumber, ByteArray buffer, ref<int> length)
        => system'core_routines'__literalconvertor.
    
    /// Converts the parameter to the literal value
    stacksafe literal convert(short aNumber)
    [
        T<byte,12> buffer.
        int        length := 0.
        
        self convert(aNumber, T<ByteArray>(buffer), &length).
        
        ^ LiteralValue fromByteArray(0, length, T<ByteArray>(buffer))
    ]

    /// Converts the parameter to the literal value
    stacksafe convert(byte aNumber, ByteArray buffer, ref<int> length)
        => system'core_routines'__literalconvertor.
    
    /// Converts the parameter to the literal value
    stacksafe literal convert(byte aNumber)
    [
        T<byte,4> buffer.
        int       length := 0.
        
        self convert(aNumber, T<ByteArray>(buffer), &length).
        
        ^ LiteralValue fromByteArray(0, length, T<ByteArray>(buffer))
    ]

    /// Converts the parameter to the literal value
    stacksafe convert(long aNumber, ByteArray buffer, ref<int> length)
        => system'core_routines'__literalconvertor.

    /// Converts the parameter to the literal value
    stacksafe literal convert(long aNumber)
    [
        T<byte, 64> buffer.
        int         length := 0.
        
        self convert(aNumber, T<ByteArray>(buffer), &length).
        
        ^ LiteralValue fromByteArray(0, length, T<ByteArray>(buffer)).
    ]

    /// Converts the parameter to the literal value with the provided radix
    stacksafe convert(int aNumber, int aRadix, ByteArray buffer, ref<int> length)
        => system'core_routines'__literalconvertor.

    /// Converts the parameter to the literal value with the provided radix
    stacksafe convert(uint aNumber, int aRadix, ByteArray buffer, ref<int> length)
        => system'core_routines'__literalconvertor.

    /// Converts the parameter to the literal value with the provided radix
    stacksafe literal convert(int aNumber, int aRadix)
    [
        T<byte, 48> buffer.
        int         length := 0.
        
        self convert(aNumber, aRadix, T<ByteArray>(buffer), &length).
        
        ^ LiteralValue fromByteArray(0, length, T<ByteArray>(buffer))
    ]

    /// Converts the parameter to the literal value with the provided radix
    stacksafe literal convert(uint aNumber, int aRadix)
    [
        T<byte,48> buffer.
        int        length := 0.
        
        self convert(aNumber, aRadix, T<ByteArray>(buffer), &length).
        
        ^ LiteralValue fromByteArray(0, length, T<ByteArray>(buffer))
    ]

    /// Converts the parameter to the literal value with the provided radix
    stacksafe convert(short aNumber, int aRadix, ByteArray buffer, ref<int> length)
        => system'core_routines'__literalconvertor.

    /// Converts the parameter to the literal value with the provided radix
    stacksafe literal convert(short aNumber, int aRadix)
    [
        T<byte,32> buffer.
        int        length := 0.
        
        self convert(aNumber, aRadix, T<ByteArray>(buffer), &length).
        
        ^ LiteralValue fromByteArray(0, length, T<ByteArray>(buffer))
    ]

    /// Converts the parameter to the literal value with the provided radix
    stacksafe literal convert(byte aNumber, int aRadix, ByteArray buffer, ref<int> length)
        => system'core_routines'__literalconvertor.

    /// Converts the parameter to the literal value with the provided radix
    stacksafe literal convert(byte aNumber, int aRadix)
    [
        T<byte,8> buffer.
        int       length := 0.
        
        self convert(aNumber, aRadix, T<ByteArray>(buffer), &length).
        
        ^ LiteralValue fromByteArray(0, length, T<ByteArray>(buffer)).
    ]

    /// Converts the parameter to the literal value with the provided radix
    stacksafe convert(long aNumber, int aRadix, ByteArray buffer, ref<int> length)
        => system'core_routines'__literalconvertor.

    /// Converts the parameter to the literal value
    stacksafe literal convert(long aNumber, int radix)
    [
        T<byte,64> buffer.
        int        length := 0.
        
        self convert(aNumber, radix, T<ByteArray>(buffer), &length).
        
        ^ LiteralValue fromByteArray(0, length, T<ByteArray>(buffer)).
    ]

    /// Converts the parameter to the literal value
    stacksafe convert(real aNumber, ByteArray buffer, ref<int> length)
        => system'core_routines'__literalconvertor.

    /// Converts the parameter to the literal value
    stacksafe literal convert(real aNumber)
    [
        T<byte,32> buffer.
        int        length := 0.
        
        self convert(aNumber, T<ByteArray>(buffer), &length).
        
        ^ LiteralValue fromByteArray(0, length, T<ByteArray>(buffer)).
    ]

    /// Converts UTF-16 literal to UTF-8 one
    stacksafe convert(wide aLiteral, ByteArray buffer, ref<int> length)
        => system'core_routines'__literalconvertor.

    stacksafe literal convert(wide aLiteral)
    [
        int       i := 0.
        int       m := 0.
        int       page := 0.
        int       l := aLiteral length.
        T<byte, 256> buffer.
        if (l < 128)
        [
            self convert(aLiteral, T<ByteArray>(buffer), &m).
            
            ^ LiteralValue fromByteArray(0, m, T<ByteArray>(buffer))
        ].

        literal retVal := emptyLiteral.
        while(l > 0)
        [
            page := l.
            if (page > 127)
            [
               page := 127.
            ].
            
            self convert(aLiteral Substring(i, page), T<ByteArray>(buffer), &m).
            
            retVal := retVal add(LiteralValue fromByteArray(0, m, T<ByteArray>(buffer))).
            
            i := i + page.
            l := l - page.
        ].
        
        ^ retVal
    ]

    /// Returns a parameter                
    literal convert(literal aLiteral)
        = aLiteral.
        
    /// Returns the literal presentation of the parameter
    literal convert : anObject
        = anObject literal.
}.

// --- wideConvertor ---

/// A wide literal convertor
public class wideConvertor =
{
    stacksafe wide convert(BaseVariable v)
        <= convert(v value).
    
    /// Converts the parameter to the literal value
    stacksafe convert(char aValue, ShortArray buffer, ref<int> length)
        => system'core_routines'__wideconvertor.
    
    stacksafe wide convert(char aValue)
    [
        T<short,2> buffer.
        int        length := 0.
        
        self convert(aValue, T<ShortArray>(buffer), &length).
        
        ^ WideLiteralValue fromShortArray(0, length, T<ShortArray>(buffer))
    ]    
    
    /// Converts the parameter to the literal value
    stacksafe convert(int aNumber, ShortArray buffer, ref<int> length)
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value
    stacksafe convert(uint aNumber, ShortArray buffer, ref<int> length)
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value
    stacksafe wide convert(int aNumber)
    [
        T<short,12> buffer.
        int         length := 0.
        
        self convert(aNumber, T<ShortArray>(buffer), &length).
        
        ^ WideLiteralValue fromShortArray(0, length, T<ShortArray>(buffer))
    ]

    /// Converts the parameter to the literal value
    stacksafe wide convert(uint aNumber)
    [
        T<short,12> buffer.
        int         length := 0.
        
        self convert(aNumber, buffer, T<ShortArray>(buffer)).
        
        ^ WideLiteralValue fromShortArray(0, length, T<ShortArray>(buffer))
    ]

    /// Converts the parameter to the literal value
    stacksafe convert(short aNumber, ShortArray buffer, ref<int> length)
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value
    stacksafe wide convert(short aNumber)
    [
        T<short,12> buffer.
        int         length := 0.
        
        self convert(aNumber, T<ShortArray>(buffer), &length).
        
        ^ WideLiteralValue fromShortArray(0, length, T<ShortArray>(buffer))
    ]

    /// Converts the parameter to the literal value
    stacksafe convert(byte aNumber, ShortArray buffer, ref<int> length)
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value
    stacksafe wide convert(byte aNumber)
    [
        T<short,12> buffer.
        int         length := 0.
        
        self convert(aNumber, T<ShortArray>(buffer), &length).
        
        ^ WideLiteralValue fromShortArray(0, length, T<ShortArray>(buffer))
    ]

    /// Converts the parameter to the literal value
    stacksafe convert(long aNumber, ShortArray buffer, ref<int> length)
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value
    stacksafe wide convert(long aNumber)
    [
        T<short,64> buffer.
        int         length := 0.
        
        self convert(aNumber, T<ShortArray>(buffer), &length).
        
        ^ WideLiteralValue fromShortArray(0, length, T<ShortArray>(buffer)).
    ]

    /// Converts the parameter to the literal value with the provided radix
    stacksafe convert(int aNumber, int aRadix, ShortArray buffer, ref<int> length)
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value with the provided radix
    stacksafe convert(uint aNumber, int aRadix, ShortArray buffer, ref<int> length)
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value with the provided radix
    stacksafe wide convert(int aNumber, int aRadix)
    [
        T<short,32> buffer.
        int         length := 0.
        
        self convert(aNumber, aRadix, T<ShortArray>(buffer), &length).
        
        ^ WideLiteralValue fromShortArray(0, length, T<ShortArray>(buffer))
    ]

    /// Converts the parameter to the literal value with the provided radix
    stacksafe wide convert(uint aNumber, int aRadix)
    [
        T<short,32> buffer.
        int         length := 0.
        
        self convert(aNumber, aRadix, T<ShortArray>(buffer), &length).
        
        ^ WideLiteralValue fromShortArray(0, length, T<ShortArray>(buffer))
    ]

    /// Converts the parameter to the literal value with the provided radix
    stacksafe convert(short aNumber, int aRadix, ShortArray buffer, ref<int> length)
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value with the provided radix
    stacksafe wide convert(short aNumber, int aRadix)
    [
        T<short,16> buffer.
        int         length := 0.
        
        self convert(aNumber, aRadix, T<ShortArray>(buffer), &length).
        
        ^ WideLiteralValue fromShortArray(0, length, T<ShortArray>(buffer))
    ]

    /// Converts the parameter to the literal value with the provided radix
    stacksafe convert(byte aNumber, int aRadix, ShortArray buffer, ref<int> length)
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value with the provided radix
    stacksafe wide convert(byte aNumber, int aRadix)
    [
        T<short,12> buffer.
        int         length := 0.
        
        self convert(aNumber, aRadix, T<ShortArray>(buffer), &length).
        
        ^ WideLiteralValue fromShortArray(0, length, T<ShortArray>(buffer))
    ]

    /// Converts the parameter to the literal value with the provided radix
    stacksafe convert(long aNumber, int aRadix, ShortArray buffer, ref<int> length)
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value with the provided radix
    stacksafe wide convert(long aNumber, int aRadix)
    [
        T<short,64> buffer.
        int         length := 0.
        
        self convert(aNumber, aRadix, T<ShortArray>(buffer), &length).
        
        ^ WideLiteralValue fromShortArray(0, length, T<ShortArray>(buffer))
    ]

    /// Converts the parameter to the literal value
    stacksafe convert(real aNumber, ShortArray buffer, ref<int> length)
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value
    stacksafe wide convert(real aNumber)
    [
        T<short,32> buffer.
        int         length := 0.
        
        self convert(aNumber, T<ShortArray>(buffer), &length).
        
        ^ WideLiteralValue fromShortArray(0, length, T<ShortArray>(buffer))
    ]

    /// Converts UTF-8 literal to UTF-16e
    stacksafe convert(literal aLiteral, ShortArray buffer, ref<int> length)
        => system'core_routines'__wideconvertor.

    /// Converts UTF-8 literal to UTF-16e
    stacksafe wide convert(literal aLiteral)
    [
        int        i := 0.
        int        m := 0.
        int        page := 0.
        int        l := aLiteral length.
        T<short,256> buffer.
        if (l < 256)
        [
            self convert(aLiteral, T<ShortArray>(buffer), &m).
            
            ^ WideLiteralValue fromShortArray(0, m, T<ShortArray>(buffer))
        ].
        
        wide retVal := emptyWideLiteral.
        while(l > 0)
        [
            page := l.
            if (page > 256)
            [
                page := 256
            ].
            
            self convert(aLiteral Substring(i,page), T<ShortArray>(buffer), &m).
            
            retVal := retVal add(WideLiteralValue fromShortArray(0, m, T<ShortArray>(buffer))).
            
            i := i + page.
            l := l - page
        ].
        
        ^ retVal
    ]        
        
    /// Returns a parameter
    wide convert(wide aLiteral)
        = aLiteral.

    /// Returns the literal presentation of the parameter
    wide convert : anObject
        = T<wide>(anObject literal).
}.

// === Enumerators / Indexers

// --- LiteralEnumerator ---

/// A literal enumerator
class LiteralEnumerator :: Enumerator<char>
{
    literal  theLiteral.
    ref<int> theIndex.
    ref<int> theDiff.
    char     theChar.
    
    /// Creates an enumerator
    constructor new : anObject
        <= new(T<literal>(anObject)).
    
    /// Creates an enumerator
    constructor new(literal aLiteral)
    [
        theLiteral := aLiteral.
        theIndex := -1.
        theDiff := 1.
        theChar := $0.
    ]

    /// Resets the enumerator
    reset
    [
        theIndex value := -1.
        theDiff value := 1.
    ]

    /// Goes to the next enumeration member and returns true if it is not the last member
    bool next
    [
        int aLength := theLiteral length.
        
        theIndex append(T<int>(theDiff)).
        
        if (theIndex < aLength)
            [
                theChar := theLiteral getAt(T<int>(theIndex)).
                
                theChar readLengthTo(theDiff).
                
                ^ true
            ].

        theDiff := 0.
        theChar := $0.

        ^ false
    ]
    
    cast T<Enumerator> = Enumerator::
    {
        embeddable dispatch => self.
    }.
    
    enumerable = theLiteral.
    
    /// Returns the current character
    char get = theChar.
}

//// --- LiteralIndexer ---
//
///// A literal value indexer
//class LiteralIndexer :: Indexer
//{
//    literal theLiteral.
//    vint    theIndex.
//    
//    /// Creates an indexer
//    constructor new : anObject
//        <= new literal:anObject.
//    
//    /// Creates an indexer
//    constructor new literal:aLiteral
//    [
//        theLiteral := aLiteral.
//        theIndex := 0.
//    ]
//    
//    /// Returns false if the index out of range
//    bool available
//    [
//        int aLength := theLiteral length.
//        
//        ^ theIndex < aLength.
//    ]
//
//    /// Returns the current character
//    get
//    [
//        char aChar := $0.
//        theLiteral read int:theIndex vchar:aChar.
//        
//        ^ CharValue new char:aChar.
//    ]
//    
//    /// Sets the index by specified value
//    set : anObject 
//    [
//        system'NotSupportedException new; raise.
//    ]
//    
//    /// Reads the current index and saves it into an output parameter
//    readIndexTo vint:anIndex
//    [
//        anIndex int := theIndex.
//    ]
//    
//    /// Sets the index by specified value
//    writeIndex int:anIndex 
//    [
//        theIndex set int:anIndex.
//    ]
//    
//    /// Reads the length to the output parameter    
//    readLengthTo vint:anOutput
//        => theLiteral.
//}

// --- WideLiteralEnumerator ---

/// A literal enumerator
class WideLiteralEnumerator :: Enumerator<char>
{
    wide      theLiteral.
    ref<int>  theIndex.
    ref<int>  theDiff.
    ref<char> theChar.
    
    /// Creates an enumerator
    constructor new : anObject
        <= new(T<wide>(anObject)).
    
    /// Creates an enumerator
    constructor new(wide aLiteral)
    [
        theLiteral := aLiteral.
        theIndex := -1.
        theDiff := 1.
        theChar := $0.
    ]

    /// Resets the enumerator
    reset
    [
        theIndex value := -1.
        theDiff value := 1.
    ]

    /// Goes to the next enumeration member and returns true if it is not the last member
    bool next
    [
        int aLength := theLiteral length.

        int diff := theDiff int.
        theIndex append(diff).
        
        if (theIndex < aLength)
            [
                theChar := theLiteral getAt(T<int>(theIndex)).
                
                theChar readLengthTo(theDiff).
                theDiff append:1.
                theDiff value := theDiff / 2.
                
                ^ true
            ].

        theDiff value := 0.
        theChar := $0.

        ^ false
    ]
    
    cast T<Enumerator> = Enumerator::
    {
        embeddable dispatch => self.
    }.
    
    enumerable = theLiteral.
    
    /// Returns the current character
    char get = theChar.
}

//// --- WideLiteralInexer ---
//
///// A wide literal enumerator
//class WideLiteralIndexer :: Indexer
//{
//    wide theLiteral.
//    vint theIndex.
//    
//    /// Creates an indexer
//    constructor new : anObject
//        <= new wide:anObject.
//    
//    /// Creates an indexer
//    constructor new wide:aLiteral
//    [
//        theLiteral := aLiteral.
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
//    get
//        = theLiteral getAt int:theIndex.
//    
//    set : anObject 
//    [
//        system'NotSupportedException new; raise.
//    ]
//    
//    /// Reads the current index and saves it into an output parameter
//    readIndexTo vint:anIndex
//    [
//        anIndex int := theIndex.
//    ]
//    
//    /// Sets the index by specified value
//    writeIndex int:anIndex 
//    [
//        theIndex set int:anIndex.
//    ]
//    
//    /// Reads the length to the output parameter    
//    readLengthTo vint:anOutput
//        => theLiteral.
//}

// --- ArrayIndexer ---

/// An array indexer
public class ArrayIndexer :: Indexer
{
    T<Array> theArray.
    ref<int> theIndex.

    /// Returns the parameter indexer
    constructor new : anArray
        <= new(T<Array>(anArray)).
    
    /// Returns the parameter indexer
    constructor new(Array anArray)
    [
        theArray := anArray.
        theIndex := 0.
    ]
    
    /// Returns false if the index out of range
    bool available
    [
        int aLength := 0.
        theArray readLengthTo(&aLength).
        
        ^ theIndex < aLength.
    ]

    /// Returns the current member
    get
        = theArray getAt(theIndex).
    
    /// Sets the current member
    set : anObject
    [
        theArray setAt(theIndex, anObject).
    ]
    
    /// Reads the current index and saves it into an output parameter
    readIndexTo(ref<int> retVal)
    [
        theIndex readValueTo(retVal).
    ]
    
    /// Sets the index by specified value
    writeIndex(int anIndex)
    [
        theIndex value := anIndex.
    ]
    
    /// Sets the index by specified value
    appendIndex(int anIndex)
    [
        theIndex append(anIndex).
    ]
    
    /// Reads the length to the output parameter    
    readLengthTo(ref<int> anOutput)
        => theArray.
}

//// --- ArrayEnumerator ---
//
///// An array enumerator
//class ArrayEnumerator :: Enumerator
//{
//    array theArray.
//    vint  theIndex.
//    
//    /// Creates an array enumerator
//    constructor new : anArray
//        <= new array:anArray.
//    
//    /// Creates an array enumerator
//    constructor new array:anArray
//    [
//        theArray := anArray.
//        theIndex := -1.
//    ]
//
//    /// Resets the enumerator
//    reset
//    [
//        theIndex set int:-1.
//    ]
//
//    /// Goes to the next enumeration member and returns true if it is not the last member
//    bool next
//    [
//        int aLength := theArray length.
//        
//        theIndex append int:1.
//
//        ^ theIndex < aLength.
//    ]
//    
//    /// Returns the current object
//    get
//        = theArray getAt int:theIndex.
//}
//
//// --- BitArrayIndexer ---
//
///// An array indexer
//class BitArrayIndexer :: Indexer
//{
//    bitarray theArray.
//    vint  theIndex.
//
//    /// Returns the parameter indexer
//    constructor new : anArray
//        <= new bitarray:anArray.
//    
//    /// Returns the parameter indexer
//    constructor new bitarray:anArray
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
//        ^ theIndex < aLength.
//    ]
//
//    /// Returns the current member
//    bool get
//        = theArray getAt int:theIndex.
//    
//    /// Sets the current member
//    set : anObject
//    [
//        theArray setAt int:theIndex bool:anObject.
//    ]
//    
//    /// Reads the current index and saves it into an output parameter
//    readIndexTo vint:anIndex
//    [
//        anIndex int := theIndex.
//    ]
//    
//    /// Sets the index by specified value
//    writeIndex int:anIndex 
//    [
//        theIndex set int:anIndex.
//    ]
//    
//    /// Reads the length to the output parameter    
//    readLengthTo vint:anOutput
//        => theArray.
//}
//
//// --- BitArrayEnumerator ---
//
///// An array enumerator
//class BitArrayEnumerator :: Enumerator
//{
//    bitarray theArray.
//    vint     theIndex.
//    
//    /// Creates an array enumerator
//    constructor new : anArray
//        <= new bitarray:anArray.
//    
//    /// Creates an array enumerator
//    constructor new bitarray:anArray
//    [
//        theArray := anArray.
//        theIndex := -1.
//    ]
//
//    /// Resets the enumerator
//    reset
//    [
//        theIndex set int:-1.
//    ]
//
//    /// Goes to the next enumeration member and returns true if it is not the last member
//    bool next
//    [
//        int aLength := theArray length.
//        
//        theIndex append int:1.
//
//        ^ theIndex < aLength.
//    ]
//    
//    /// Returns the current object
//    bool get
//        = theArray getAt int:theIndex.
//}
//
//// --- BitArray32Indexer ---
//
///// An array indexer
//class BitArray32Indexer :: Indexer
//{
//    bitset theArray.
//    vint   theIndex.
//
//    /// Returns the parameter indexer
//    constructor new : anArray
//        <= new bitset:anArray.
//    
//    /// Returns the parameter indexer
//    constructor new bitset:anArray
//    [
//        theArray := anArray.
//        theIndex := 0.
//    ]
//    
//    /// Returns false if the index out of range
//    bool available
//    [
//        ^ theIndex < 32
//    ]
//
//    /// Returns the current member
//    bool get
//        = theArray getAt int:theIndex.
//    
//    /// Sets the current member
//    set : anObject
//    [
//        theArray setAt int:theIndex bool:anObject.
//    ]
//    
//    /// Reads the current index and saves it into an output parameter
//    readIndexTo  vint:anIndex
//    [
//        anIndex int := theIndex.
//    ]
//    
//    /// Sets the index by specified value
//    writeIndex int:anIndex 
//    [
//        theIndex set int:anIndex.
//    ]
//    
//    /// Reads the length to the output parameter    
//    readLengthTo vint:anOutput
//        => theArray.
//}
//
//// --- BitArray32Enumerator ---
//
///// An array enumerator
//class BitArray32Enumerator :: Enumerator
//{
//    bitset theArray.
//    vint   theIndex.
//    
//    /// Creates an array enumerator
//    constructor new : anArray
//        <= new bitset:anArray.
//    
//    /// Creates an array enumerator
//    constructor new bitset:anArray
//    [
//        theArray := anArray.
//        theIndex := -1.
//    ]
//
//    /// Resets the enumerator
//    reset
//    [
//        theIndex set int:-1.
//    ]
//
//    /// Goes to the next enumeration member and returns true if it is not the last member
//    bool next
//    [
//        theIndex append int:1.
//
//        ^ theIndex < 32.
//    ]
//    
//    /// Returns the current object
//    get
//        = theArray getAt int:theIndex.
//}