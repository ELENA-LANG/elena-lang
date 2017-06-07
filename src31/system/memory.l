// --- arrayof ---

dynamic template arrayof :: template_subj
{
    template_subj $target.
    
    enumerator 
    [
        vint index := -1.
        int length := 0.
        $target >> length.
        
        ^ Enumerator::
          {
              get = $owner getAt int:index.
              
              enumerable = $owner.
              
              reset [ index int := -1 ]
              
              next
              [
                  index append int:1.
                  
                  ^ index < length
              ]
          }
    ]
        
    stacksafe readLengthTo vint:retVal
    [
        int len := 0.
        $target >> len.
        
        retVal int := len
    ]
    
    int length :: outnprop(readLengthTo).

    stacksafe template_subj getAt int:n
    [
        if (n < 0)
            [ InvalidArgumentException new; raise ].
        
        int len := 0.
        $target >> len.

        if (n < len)
            [ ^ $target @ n ];
            [ InvalidArgumentException new; raise ]
    ]

    stacksafe setAt int:n template_subj:item
    [
        if (n < 0)
            [ InvalidArgumentException new; raise ].
        
        int len := 0.
        $target >> len.

        if (n < len)
            [ $target @ n := item ];
            [ InvalidArgumentException new; raise ]
    ]
                
    /// Returns the item at the specified index
    template_subj getAt : aParam
        <= getAt int:aParam.
        
    /// Returns the item at the specified index
    setAt : index : value
        <= setAt int:index template_subj:value.
}
//
//// --- matrixof ---
//
//#template matrixof(template_subj)
//{
//    arrayof(template_subj) theArray.
//    int                    theRows.
//    int                    theColumns.
//    
//    implicit object:params
//    [
//        theRows := params rows.
//        theColumns := params columns.
//        
//        int len := theRows * theColumns.
//        
//        theArray := template_subj(/*theRows * theColumns*/len).
//        
//        var it := params indexer2d \ back:$nil.
//        ($nil == it)
//            ! [
//                int i := 0.
//                int j := 0.
//                
//                while (i < theRows)
//                [
//                    j := 0.
//                    while (j < theColumns)
//                    [
//                        it writeIndex1 &int:i.
//                        it writeIndex2 &int:j.
//                        
//                        var val := it get.
//                        
//                        $self setAt &int:i &int:j &template_subj:val.
//                        
//                        j += 1.
//                    ].
//                    
//                    i += 1.
//                ].
//            ].
//    ]
//    
//    /// Reads the matrix lengths to the output parameter
//    stacksafe readLengths &vint:Rows &vint:Columns
//    [
//        Rows := theRows.
//        Columns := theColumns.
//    ]
//
//    stacksafe outnprop(length) readLength &vint:retVal
//    [
//        retVal := theRows.
//    ]
//
//    stacksafe outnprop(columns) readColumns &vint:retVal
//    [
//        retVal := theColumns.
//    ]
//
//    stacksafe outnprop(rows) readRows &vint:retVal
//    [
//        retVal := theRows.
//    ]
//
//    /// Writes an integer value at the specified position
//    stacksafe setAt &int:i &int:j &template_subj:aValue
//    [
//        theArray setAt &int:(i * theColumns + j) &template_subj:aValue.
//    ]
//
//    embeddable template_subj getAt &int:i &int:j
//        = theArray getAt &int:(i * theColumns + j).
//
//    getAt : i : j
//        = $self getAt &int:i &int:j.
//
//    setAt : i : j : aValue
//    [
//        $self setAt &int:i &int:j &template_subj:aValue.    
//    ]
//
//    /// Returns a row
//    getAt : i =
//    {
//        length = theColumns int.
//        
//        getAt : j
//            = $owner getAt:i:j.
//        
//        setAt : j : aValue
//        [
//            $owner setAt:i:j:aValue.
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
//        ^ Enumerator
//          {
//              get = $owner getAt &int:i &int:j.
//              
//              enumerable = $owner.
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
//                  j += 1.
//                  
//                  ifnot (j < length2)
//                  [
//                      j := -1.
//                      i += 1.
//                      
//                      ^ false.
//                  ].
//                  
//                  ^ true.
//              ]
//          }.
//    ]
//}

// === basic memory classes ===

// --- ByteArray ---

/// A byte array
sealed ByteArray :: dispatchable(bytearray), arrayof(byte)
{
    /// Creates the byte array with the specified length
    stacksafe constructor new int:len
        = byte(len).

    /// Creates the byte array with the specified length
    constructor new : aLength
        <= new int:aLength.
    
    /// Creates the copy of a byte subarray
    stacksafe constructor new int:anIndex int:aLength bytearray:anArray
        <= new int:aLength;
    [
        anArray read int:anIndex int:aLength bytearray:$self.
    ]

    /// Creates the byte array copy
    stacksafe constructor new bytearray:anArray
        <= new int:0 int:(anArray length) bytearray:anArray.

    /// Reads the array length to the output parameter
    stacksafe readLengthTo vint:aLength
        => system'core_routines'__bytearray.

    /// Fills the sub array with a specified value
    stacksafe fill int:anIndex int:aLength byte:aValue
    [
        int aTotalLength := $self length.
        
        int i := anIndex.
        int m := anIndex + aLength.
        
        if (i > aTotalLength)
            [ system'InvalidArgumentException new; raise ].
        
        if (m > aTotalLength)
            [ system'InvalidArgumentException new; raise ].
            
        while(i < m)
        [
            $self write int:i byte:aValue.
            
            i := i + 1
        ]
    ]

    /// Copies a sub array to the provided one at the specified position
    stacksafe read int:anIndex int:aLength bytearray:aDump
        => system'core_routines'__bytearray.

    /// Copies the byte array at the specified position
    stacksafe write int:anIndex int:aLength bytearray:aDump
        => system'core_routines'__bytearray.

    /// Writes the byte value at specified position
    stacksafe write int:anIndex byte:anInt
        => system'core_routines'__bytearray.

    /// Reads a byte value at the specified position and saves it into an output variable
    stacksafe read int:anIndex vbyte:anInt
        => system'core_routines'__bytearray.

    embeddable getAt int:n
    [
        byte retVal := 0.
        
        $self read int:n vbyte:retVal.
        
        ^ retVal
    ]

    /// Writes a short integer value at the specified position
    stacksafe write int:anIndex short:anInt
        => system'core_routines'__bytearray.

    /// Reads a short integer value at the specified position and saves it into an output variable
    stacksafe read int:anIndex vshort:aChar
        => system'core_routines'__bytearray.

    /// Writes a character value at the specified position
    stacksafe write int:anIndex char:anInt
        => system'core_routines'__bytearray.

    /// Reads a character value at the specified position and saves it into an output variable
    stacksafe read int:anIndex vchar:aChar
        => system'core_routines'__bytearray.

    /// Writes an integer value at the specified position
    stacksafe write int:anIndex int:anInt
        => system'core_routines'__bytearray.

    /// Reads an integer value at the specified position and saves it into an output variable
    stacksafe read int:anIndex vint:anInt
        => system'core_routines'__bytearray.

    /// Writes a long integer value at the specified position
    stacksafe write int:anIndex long:aLong
        => system'core_routines'__bytearray.

    /// Reads a long integer value at the specified position and saves it into an output variable
    stacksafe read int:anIndex vlong:aLong
        => system'core_routines'__bytearray.

    /// Writes a real value at the specified position
    stacksafe write int:anIndex real:aLong
        => system'core_routines'__bytearray.

    /// Reads a real value at the specified position and saves it into an output variable
    stacksafe read int:anIndex vreal:aLong
        => system'core_routines'__bytearray.

    stacksafe move int:anIndex int:aLength int:anOffset
        => system'core_routines'__bytearray.
    
    /// Concatinates the arrays    
    stacksafe bytearray add int:aLength2 bytearray:anArray
    [
        int aLength1 := $self length.
        
        bytearray aRetVal := ByteArray new int:(aLength1 + aLength2).

        byte tmp := 0.
        int anIndex := 0.
        while(anIndex < aLength1)
        [
            $self read int:anIndex vbyte:tmp.
            aRetVal write int:anIndex byte:tmp.
            
            anIndex := anIndex + 1
        ].

        anIndex := 0.
        while(anIndex < aLength2)
        [
            anArray read int:anIndex vbyte:tmp.
            aRetVal write int:(aLength1 + anIndex) byte:tmp.
            
            anIndex := anIndex + 1
        ].
            
        ^ aRetVal
    ]    

    /// Creates the object copy
    clone = ByteArray new bytearray:$self.
}

// --- ShortArray ---

/// A short integer array
sealed ShortArray :: dispatchable(shortarray), arrayof(short)
{
    /// Creates the short array with the specified length
    stacksafe constructor new int:aLength
        = short(aLength).
    
    /// Creates the short array with the specified length
    constructor new : aLength
        <= new int:aLength.
    
    /// Creates the copy of a short subarray
    stacksafe constructor new int:anIndex int:aLength shortarray:anArray
        <= new int:aLength;
    [
        anArray read int:anIndex int:aLength shortarray:$self.
    ]

    /// Creates the short array copy
    constructor new shortarray:anArray
        <= new int:0 int:(anArray length) shortarray:anArray.

    /// Reads the array length to the output parameter
    stacksafe readLengthTo vint:aLength
        => system'core_routines'__shortarray.

    /// Fills the sub array with a specified value
    stacksafe fill int:anIndex int:aLength short:aValue
    [
        int aTotalLength := $self length.
        
        int i := anIndex.
        int m := anIndex + aLength.
        
        if (i > aTotalLength)
            [ system'InvalidArgumentException new; raise ].
        
        if (m > aTotalLength)
            [ system'InvalidArgumentException new; raise ].
            
        while(i < m)
        [
            $self write int:i short:aValue.
            
            i := i + 1
        ]
    ]
    
    /// Copies a sub array to the provided one at the specified position
    stacksafe read int:anIndex int:aLength shortarray:anArray
        => system'core_routines'__shortarray.

    /// Copies the short array at the specified position   
    stacksafe write int:anIndex int:aLength shortarray:anArray
        => system'core_routines'__shortarray.

    /// Writes a short integer value at the specified position
    stacksafe write int:anIndex short:aValue
        => system'core_routines'__shortarray.

    embeddable getAt int:n
    [
        short retVal := 0.
        
        $self read int:n vshort:retVal.
        
        ^ retVal
    ]

    /// Reads a short integer value at the specified position and saves it into an output variable
    stacksafe read int:anIndex vshort:aValue
        => system'core_routines'__shortarray.

    /// Creates the object copy
    clone = ShortArray new shortarray:$self.
}

// --- IntArray ---

/// An array of integers
sealed IntArray :: dispatchable(intarray), arrayof(int)
{
    /// Creates the array with the specified length
    stacksafe constructor new int : aLength
        = int(aLength).

    /// Creates the array with the specified length
    constructor new : aLength
        <= new int:aLength.

    /// Creates the copy of a subarray
    stacksafe constructor new int:anIndex int:aLength intarray:anArray
        <= new int:aLength;
    [
        anArray read int:anIndex int:aLength intarray:$self.
    ]

    /// Creates the short array copy
    constructor new intarray:anArray
        <= new int:0 int:(anArray length) intarray:anArray.
    
    /// Reads the array length to the output parameter
    stacksafe readLengthTo vint:aLength
        => system'core_routines'__intarray.

    /// Fills the sub array with a specified value
    stacksafe fill int:anIndex int:aLength int:aValue
    [
        int aTotalLength := $self length.
        
        int i := anIndex.
        int m := anIndex + aLength.
        
        if (i > aTotalLength)
            [ system'InvalidArgumentException new; raise ].
        
        if (m > aTotalLength)
            [ system'InvalidArgumentException new; raise ].
            
        while(i < m)
        [
            $self write int:i int:aValue.
            
            i := i + 1
        ]
    ]

    /// Copies a sub array to the provided one at the specified position
    stacksafe read int:anIndex int:aLength intarray:anArray
        => system'core_routines'__intarray.

    /// Copies the short array at the specified position
    stacksafe write int:anIndex int:aLength intarray:anArray
        => system'core_routines'__intarray.

    /// Writes an integer value at the specified position
    stacksafe write int:anIndex int:aValue
        => system'core_routines'__intarray.

    /// Reads an integer value at the specified position and saves it into an output variable
    stacksafe read int:anIndex vint:anInt
        => system'core_routines'__intarray.

    embeddable getAt int:n
    [
        int retVal := 0.
        
        $self read int:n vint:retVal.
        
        ^ retVal
    ]

    /// Creates the object copy
    clone = IntArray new intarray:$self.
}

// --- BitArray ---

sealed class BitArray :: dispatchable(bitarray)
{
    intarray theArray.
    int      theLength.
    
    /// Creates the array with the specified length
    stacksafe constructor new int:length
    [
        theLength := length.
        theArray := IntArray new int:(length / 32 + 1).
    ]
    
    stacksafe readLengthTo vint:retVal
    [
        retVal int := theLength.
    ]
    
    int length :: outnprop(readLengthTo).
    
    getAt:index
        = $self getAt int:index.
    
    stacksafe bool getAt int:index
    [
        int arr_index := index / 32.
        int bit_index := arr_index * 32.
        bit_index := index - arr_index.
        
        int bit_mask := 1.
        if (bit_index != 0)
            [ bit_mask := bit_mask << bit_index ].
        
        int bits := 0.
        theArray read int:arr_index vint:bits.
        
        ^ (bits && bit_mask) != 0
    ]
    
    setAt:index:value
    [
        $self setAt int:index bool:value.
    ]
    
    stacksafe setAt int:index bool:value
    [
        int arr_index := index / 32.
        int bit_index := arr_index * 32.
        bit_index := index - arr_index.
        
        int bit_mask := 1.
        if (bit_index != 0)
            [ bit_mask := bit_mask << bit_index ].
        
        int bits := 0.
        theArray read int:arr_index vint:bits.
        
        value
            ? [ bits := bits || bit_mask. ]
            ! [  
                bit_mask := bit_mask inverted.
                
                bits := bits && bit_mask
            ].
            
        theArray write int:arr_index int:bits
    ]
    
    /// Returns the array enumerator
    enumerator = BitArrayEnumerator new bitarray:$self.

    /// Returns the array indexer
    indexer = BitArrayIndexer new bitarray:$self.    
}

// --- BitArray32 ---

sealed embeddable class BitArray32 :: dispatchable(bitset)
{
    int theValue.
    
    constructor new int:value
    [
        theValue := value.
    ]
    
    constructor new:value
        <= new int:value.
    
    int length = 32.
    
    write int:value
    [
        theValue := value.
    ]
    
    write : value
    [
        value cast:%write to:$self.
    ]
    
    getAt:index
        = $self getAt int:index.
    
    stacksafe bool getAt int:index
    [
        int bit_mask := 1.
        if (index != 0)
            [ bit_mask := bit_mask << index ].
        
        ^ (theValue && bit_mask) != 0
    ]
    
    setAt:index:value
    [
        $self setAt int:index bool:value.
    ]
    
    stacksafe setAt int:index bool:value
    [
        int bit_mask := 1.
        if (index != 0)
            [ bit_mask := bit_mask << index ].
        
        if(value)
            [ theValue := theValue || bit_mask ];
            [  
                bit_mask := bit_mask inverted.
                
                theValue := theValue && bit_mask
            ].
    ]
    
    /// Returns the array enumerator
    enumerator = BitArray32Enumerator new bitset:$self.

    /// Returns the array indexer
    indexer = BitArray32Indexer new bitset:$self.
}

// --- IntMatrix ---

sealed class IntMatrix :: dispatchable(intmatrix)
{
    intarray theArray.
    int      theRows.
    int      theColumns.
    
    constructor new int:Rows int:Columns
    [
        theRows := Rows.
        theColumns := Columns.
        
        theArray := IntArray new int:(Columns * Rows).        
    ]
        
    constructor new : Rows : Columns
        <= new int:Rows int:Columns.
        
    /// Reads the matrix lengths to the output parameter
    stacksafe readLengthsTo vint:Rows vint:Columns
    [
        Rows int := theRows.
        Columns int := theColumns
    ]

    stacksafe readLengthTo vint:retVal
    [
        retVal int := theRows
    ]
    
    int length :: outnprop(readLengthTo).

    stacksafe readColumnsTo vint:retVal
    [
        retVal int := theColumns
    ]
    
    int columns :: outnprop(readColumnsTo).

    stacksafe readRowsTo vint:retVal
    [
        retVal int := theRows
    ]
    
    int rows :: outnprop(readRowsTo).

    /// Writes an integer value at the specified position
    stacksafe write int:i int:j int:aValue
    [
        theArray write int:(i * theColumns + j) int:aValue
    ]

    /// Reads an integer value at the specified position and saves it into an output variable
    stacksafe read int:i int:j vint:anInt
    [
        theArray read int:(i * theColumns + j) vint:anInt
    ]

    embeddable int getAt int:i int:j
    [
        int retVal := 0.
        $self read int:i int:j vint:retVal.
        ^ retVal
    ]

    getAt : i : j
    [
        int aValue := 0.
        $self read int:i int:j vint:aValue.
        
        ^ aValue
    ]

    setAt : i : j : aValue
    [
        $self write int:i int:j int:aValue.    
    ]

    /// Returns a row
    getAt : i =
    {
        length = theColumns.
        
        getAt : j
            = $owner getAt:i:j.
        
        setAt : j : aValue
        [
            $owner setAt:i:j:aValue.
        ]
    }.
}

// --- RealArray ---

/// An array of real numbers
sealed RealArray :: dispatchable(realarray), arrayof(real)
{
    /// Creates the array with the specified length
    stacksafe constructor new int:aLength
        = real(aLength).

    /// Creates the array with the specified length
    constructor new : aLength
        <= new int:aLength.

    /// Creates the copy of a subarray
    stacksafe constructor new int:anIndex int:aLength realarray:anArray
        <= new int:aLength;
    [
        anArray read int:anIndex int:aLength realarray:$self
    ]

    /// Creates the array copy
    constructor new realarray:anArray
        <= new int:0 int:(anArray length) realarray:anArray.

    /// Fills the sub array with a specified value
    stacksafe fill int:anIndex int:aLength real:aValue
    [
        int aTotalLength := $self length.
        
        int i := anIndex.
        int m := anIndex + aLength.
        
        if (i > aTotalLength)
            [ system'InvalidArgumentException new; raise ].
        
        if (m > aTotalLength)
            [ system'InvalidArgumentException new; raise ].
          
        while(i < m)
        [
            $self setAt int:i real:aValue.
            
            i := i + 1.
        ].
    ]

    /// Copies a sub array to the provided one at the specified position
    stacksafe read int:anIndex int:aLength realarray:anArray
        => system'core_routines'__realarray.

    /// Copies the short array at the specified position
    stacksafe write int:anIndex int:aLength realarray:anArray
        => system'core_routines'__realarray.

    /// Reads a real value at the specified position and saves it into an output variable
    stacksafe read int:anIndex vreal:aReal
        => system'core_routines'__realarray.

    embeddable getAt int:n
    [
        real retVal := 0.
        
        $self read int:n vreal:retVal.
        
        ^ retVal.
    ]

    /// Creates the object copy
    clone = RealArray new realarray:$self.
}

// --- RealMatrix ---

sealed class RealMatrix :: dispatchable(realmatrix)
{
    realarray theArray.
    int       theRows.
    int       theColumns.
    
    stacksafe constructor new int:Rows int:Columns
    [
        theArray := RealArray new int:(Columns * Rows).
        
        theRows := IntNumber new int:Rows.
        theColumns := IntNumber new int:Columns.
    ]
        
    constructor new : Rows : Columns
        <= new int:Columns int:Rows.

    /// Reads the matrix lengths to the output parameter
    stacksafe readLengthsTo vint:Rows vint:Columns
    [
        Columns int := theColumns.
        Rows int := theRows.
    ]

    stacksafe readLengthTo vint:retVal
    [
        retVal int := theRows.
    ]
    
    int length :: outnprop(readLengthTo).

    stacksafe readColumnsTo vint:retVal
    [
        retVal int := theColumns.
    ]
    
    int columns :: outnprop(readColumnsTo).

    stacksafe readRowsTo vint:retVal
    [
        retVal int := theRows
    ]
    
    int rows :: outnprop(readRowsTo).

    /// Writes an integer value at the specified position
    stacksafe write int:i int:j real:aValue
    [
        int anIndex := i * theColumns + j.
        
        theArray setAt int:anIndex real:aValue
    ]

    /// Reads an integer value at the specified position and saves it into an output variable
    stacksafe read int:i int:j vreal:aValue
    [
        int anIndex := i * theColumns + j.
        
        theArray read int:anIndex vreal:aValue
    ]

    embeddable real getAt int:i int:j
    [
        real retVal := 0.0r.
        $self read int:i int:j vreal:retVal.
        
        ^ retVal
    ]

    getAt : i : j
    [
        real aValue := 0.0r.
        $self read int:i int:j vreal:aValue.
        
        ^ RealNumber new real:aValue
    ]

    setAt : i : j : aValue
    [
        $self write int:i int:j real:aValue
    ]

    /// Returns a row
    getAt : i =
    {
        length = theColumns.
        
        getAt : j
            = $owner getAt:i:j.
        
        setAt : j : aValue
        [
            $owner setAt:i:j:aValue
        ]
    }.
}

// --- CharArray ---

/// An array of unicode characters (UTF-32)
sealed CharArray :: dispatchable(chararray), arrayof(char)
{
    /// Creates the new array with a specified length
    stacksafe constructor new int : len
        = char(len).

    /// Creates the new array with a specified length
    constructor new : aLength
        <= new int:aLength.

    /// Creates the copy of a subarray
    stacksafe constructor new int:anIndex int:aLength chararray:anArray
        <= new int:aLength;
    [
        anArray read int:anIndex int:aLength chararray:$self.
    ]
     
    /// Creates the short array copy
    constructor new chararray:anArray
        <= new int:0 int:(anArray length) chararray:anArray.
    
    /// Reads the array length to the output parameter
    stacksafe readLengthTo vint:aLength
        => system'core_routines'__intarray.

    /// Fills the sub array with a specified value
    stacksafe fill int:anIndex int:aLength char:aValue
    [
        int aTotalLength := $self length.
        
        int i := anIndex.
        int m := anIndex + aLength.
        
        if (i > aTotalLength)
            [ system'InvalidArgumentException new; raise ].
        
        if (m > aTotalLength)
            [ system'InvalidArgumentException new; raise ].
            
        while(i < m)
        [
            $self write int:i char:aValue.
            
            i := i + 1
        ]
    ]

    /// Copies a sub array to the provided one at the specified position
    stacksafe read int:anIndex int:aLength chararray:anArray
        => system'core_routines'__chararray.

    /// Copies the array at the specified position
    stacksafe write int:anIndex int:aLength chararray:anArray
        => system'core_routines'__chararray.

    /// Writes a character value at the specified position
    stacksafe write int:anIndex char:aValue
        => system'core_routines'__chararray.

    /// Reads a character value at the specified position and saves it into an output variable
    stacksafe read int:anIndex vchar:anInt
        => system'core_routines'__chararray.

    embeddable getAt int:n
    [
        char retVal := $0.
        
        $self read int:n vchar:retVal.
        
        ^ retVal
    ]

    stacksafe move int:anIndex int:aLength int:anOffset
        => system'core_routines'__chararray.

    /// Creates the object copy
    clone = CharArray new chararray:$self.
}
        
// --- emptyliteralvalue ---

/// An empty literal value
const literal emptyLiteralValue = "".

// --- LiteralValue ---

/// A UTF-8 literal value
sealed dynamic string LiteralValue :: BaseValue, dispatchable(literal,int,char,wide)
{
    byte $target.

    /// Creates the empty literal
    constructor min
        = emptyLiteralValue.
        
    /// Creates the literal string with specified length        
    stacksafe constructor new int:size
        = byte(size + 1).

    /// Creates the literal string with specified length and copies the parameter value
    stacksafe constructor new int:aSize literal:aLiteral
        <= new int:aSize;
    [
        system'core_routines'__literal copy literal:$self literal:aLiteral.
    ]

    /// Creates the literal string with specified length fills with spciefied character value        
    stacksafe constructor new int:aSize char:aChar
        => system'core_routines'__literal.

    /// Creates the literal string copy              
    constructor new literal:aLiteral
        <= new int:(aLiteral length) literal:aLiteral.
        
    /// Creates the copy of a subarray        
    stacksafe constructor new int:anIndex int:aSize chararray:anArray
        => system'core_routines'__literal.

    stacksafe constructor $new int:anIndex int:aSize bytearray:anArray
        <= new int:aSize;
    [
        system'core_routines'__literal copy literal:$self int:anIndex int:aSize bytearray:anArray.
    ]
    
    /// Converts to UTF-16 literal
    wide = wideConvertor convert literal:$self.

    /// Compares the values
    bool equal literal : aLiteral
        => system'core_routines'__literal.

    /// Compares the values
    bool less literal : aLiteral
        => system'core_routines'__literal.

    /// Compares the values
    bool equal wide : aLiteral
        = $self wide; equal wide:aLiteral.

    /// Compares the values
    bool less wide : aLiteral
        = $self wide; less wide:aLiteral.

    /// Compares the values
    bool equal char : aChar
        <= equal literal:aChar.

    /// Compares the values
    bool less char : aChar
        <= less literal:aChar.

    /// Concatinates the values
    literal add literal : aLiteral
        => system'core_routines'__literal.

    add : anObject = anObject cast:%add to:$self.

    /// Returns the character at the specified index
    getAt : aParam
        <= getAt int:aParam.

    /// Returns the character at the specified index
    stacksafe char getAt int:anIndex
    [
        char aChar := $0.
        $self read int:anIndex vchar:aChar.
        
        ^ CharValue new char:aChar.
    ]
    
    /// Concatinates the values
    stacksafe literal add char:aChar
        = $self add literal:aChar.

    // Reads the character at the specified index and saves into an output parameter
    stacksafe read int:anIndex vchar:aChar
        => system'core_routines'__literal.

    // Reads the byte at the specified index and saves into an output parameter
    stacksafe read int:anIndex vbyte:anInt
        => system'core_routines'__literal.

    /// Saves the index of the first occurence of the specified substring into an output parameter.
    ///
    /// If the substring is not found. returns -1
    stacksafe indexOf int:anIndex literal:aLiteral vint:aRetVal
        => system'core_routines'__literal.

    /// Saves the index of the first occurence of the specified symbol into an output parameter.
    ///
    /// If the substring is not found. returns -1
    stacksafe indexOf int:anIndex char:aChar vint:aRetVal
        => system'core_routines'__literal.

    /// Returns the index of the first occurence of the specified object.
    /// 
    /// If the object is not found. returns -1
    indexOf : aSearchObject at:anIndex
        = aSearchObject cast:%eval to:
            {
               eval:target literal:aLiteral
                  = target indexOf literal:aLiteral at:anIndex.
    
               eval:target wide:aLiteral
                  = target indexOf literal:aLiteral at:anIndex.
    
               eval:target char:aChar
                  = target indexOf char:aChar at:anIndex.
            } with:$self.

    /// obsolete            
    indexOf:anIndex:aSearchObject
        <= indexOf:aSearchObject at:anIndex.

    /// Returns the index of the first occurence of the specified symbol.
    /// 
    /// If the substring is not found. returns -1
    indexOf char:aChar at:anIndex
    [
        int aRetVal := 0.
        $self indexOf int:anIndex char:aChar vint:aRetVal.
        
        ^ IntNumber new int:aRetVal.
    ]

    /// Returns the index of the first occurence of the specified substring.
    ///
    /// If the substring is not found. returns -1
    indexOf literal:aLiteral at:anIndex
    [
        int aRetVal := 0.
        $self indexOf int:anIndex literal:aLiteral vint:aRetVal.
        
        ^ IntNumber new int:aRetVal.
    ]

    /// Inserts the substring into and returns the new one
    stacksafe literal insert int:anIndex literal:aLiteral
        => system'core_routines'__literal.

    /// Inserts a character into and returns the new one
    stacksafe literal insert int:anIndex char:aChar
        = $self insert int:anIndex literal:aChar.

    /// Inserts an object into and returns the new one
    insert : anObject at:anIndex
        = anObject cast:%eval to:
            {
               eval:target literal:aLiteral
                  = target insert int:anIndex literal:aLiteral.
    
               eval:target wide:aLiteral
                  = target insert int:anIndex literal:aLiteral.
    
               eval:target char:aChar
                  = target insert int:anIndex char:aChar.
            } with:$self.

    /// Deletes the substring from the object and returns the new one
    delete:aLength at:anIndex
        = $self delete int:anIndex int:aLength.

    /// Deletes the substring from the object and returns the new one
    stacksafe literal delete int:anIndex int:aLength
        => system'core_routines'__literal.

    /// Returns the substring
    Substring : aLength at:anIndex
        = $self Substring int:anIndex int:aLength.

    /// Returns the substring
    stacksafe literal Substring int:anIndex int:aLength
        => system'core_routines'__literal.

    /// Reads the literal length into an output parameter
    stacksafe readLengthTo vint:aLength
        => system'core_routines'__literal.
        
    int length :: outnprop(readLengthTo).        

    /// Saves the literal into a UTF-32 character array
    stacksafe save int:anIndex chararray:anArray vint:aRetVal
        => system'core_routines'__literal.

    stacksafe $save int:anIndex bytearray:anArray int:aRetVal
        => system'core_routines'__literal.

    /// Returns the string enumerator
    enumerator = LiteralEnumerator new literal:$self.

    /// Returns the string indexer
    indexer = LiteralIndexer new literal:$self.
    
    /// Clones the value
    clone = LiteralValue new literal:$self.

    /// Reads the object hash code into the output parameter
    stacksafe readHashCodeTo vint:aCode 
        => system'core_routines'__literal.
        
    int hash_code :: outnprop(readHashCodeTo).
}

// --- emptyWideLiteralValue ---

/// An empty wide literal value
const wide emptyWideLiteralValue = ""w.

// --- WideLiteralValue ---

/// A UTF-18 literal value
sealed dynamic string WideLiteralValue :: BaseValue, dispatchable(wide,int,char,literal)
{
    short $target.

    /// Creates the empty literal
    constructor min
        = emptyWideLiteralValue.
        
    /// Creates the literal string with specified length
    stacksafe constructor new int:size
        = short(size + 1).

    /// Creates the literal string with specified length and copies the parameter value
    stacksafe constructor new int:aSize wide:aLiteral
        <= new int:aSize;
    [
        system'core_routines'__wide copy wide:$self wide:aLiteral.
    ]

    /// Creates the literal string with specified length fills with spciefied character value
    stacksafe constructor new int:aSize char:aChar
        => system'core_routines'__wide.

    /// Creates the literal string copy
    constructor new wide:aLiteral
        <= new int:(aLiteral length) wide:aLiteral.
        
    /// Creates the copy of a subarray
    stacksafe constructor new int:anIndex int:aSize chararray:anArray
        => system'core_routines'__wide.

    stacksafe constructor $new int:anIndex int:aSize shortarray:anArray
        <= new int:aSize;
    [
        system'core_routines'__wide copy wide:$self int:anIndex int:aSize shortarray:anArray.
    ]

    /// Converts to UTF-8 literal
    literal = literalConvertor convert wide:$self.

    /// Compares the values
    bool equal wide : aLiteral
        => system'core_routines'__wide.

    /// Compares the values
    bool less wide : aLiteral
        => system'core_routines'__wide.

    /// Compares the values
    bool equal char : aChar
        <= equal wide:aChar.

    /// Compares the values
    bool less char : aChar
        <= less wide:aChar.

    add : anObject = anObject cast:%add to:$self.

    /// Returns the character at the specified index
    getAt : aParam
        <= getAt int:aParam.

    /// Returns the character at the specified index
    stacksafe char getAt int:anIndex
    [
        char aChar := $0.
        $self read int:anIndex vchar:aChar.
        
        ^ CharValue new char:aChar
    ]

    /// Concatinates the values
    stacksafe wide add wide : aLiteral
        => system'core_routines'__wide.

    /// Concatinates the values
    stacksafe wide add char:aChar
        = $self add wide:(wideConvertor convert char:aChar).

    /// Concatinates the values
    wide add literal:aLiteral
        = $self add wide:aLiteral.

    // Reads the character at the specified index and saves into an output parameter
    stacksafe read int:anIndex vchar:aChar
        => system'core_routines'__wide.

    // Reads the short integer at the specified index and saves into an output parameter
    stacksafe read int:anIndex vshort:aValue
        => system'core_routines'__wide.

    /// Saves the index of the first occurence of the specified substring into an output parameter.
    ///
    /// If the substring is not found. returns -1
    stacksafe indexOf int:anIndex wide:aLiteral vint:aRetVal
        => system'core_routines'__wide.

    /// Saves the index of the first occurence of the specified character into an output parameter.
    ///
    /// If the substring is not found. returns -1
    stacksafe indexOf int:anIndex char:aChar vint:aRetVal
        => system'core_routines'__wide.

    /// Returns the index of the first occurence of the specified character.
    ///
    /// If the substring is not found. returns -1
    stacksafe indexOf int:anIndex char:aChar
    [
        int aRetVal := 0.
        $self indexOf int:anIndex char:aChar vint:aRetVal.
        
        ^ IntNumber new int:aRetVal.
    ]

    /// Returns the index of the first occurence of the specified substring.
    ///
    /// If the substring is not found. returns -1
    stacksafe indexOf int:anIndex wide:aLiteral
    [
        int aRetVal := 0.
        $self indexOf int:anIndex wide:aLiteral vint:aRetVal.
        
        ^ IntNumber new int:aRetVal.
    ]

    /// Returns the index of the first occurence of the specified object.
    ///
    /// If the substring is not found. returns -1
    indexOf : anObject at:anIndex
        = anObject cast:%eval to:
            {
                eval:target literal:aLiteral = target indexOf int:anIndex wide:aLiteral.
                
                eval:target wide:aLiteral = target indexOf int:anIndex wide:aLiteral.
                
                eval:target char:aChar = target indexOf int:anIndex char:aChar.
            } with:$self.

    /// Inserts the substring into and returns the new one
    stacksafe wide insert int:anIndex wide:aLiteral
        => system'core_routines'__wide.

    /// Inserts the substring into and returns the new one
    stacksafe wide insert int:anIndex literal:aLiteral
        = $self insert int:anIndex wide:aLiteral.

    /// Inserts a character into and returns the new one
    stacksafe wide insert int:anIndex char:aChar
        = $self insert int:anIndex wide:(wideConvertor convert char:aChar).

    /// Inserts an object into and returns the new one
    insert : anObject at:anIndex
        = anObject cast:%eval to:
            {
               eval:target wide:aLiteral
                  = target insert int:anIndex wide:aLiteral.
    
               eval:target literal:aLiteral
                  = target insert int:anIndex wide:aLiteral.
    
               eval:target char:aChar
                  = target insert int:anIndex char:aChar.
            } with:$self.

    /// Deletes the substring from the object and returns the new one
    delete:aLength at:anIndex
        = $self delete int:anIndex int:aLength.
            
    /// Deletes the substring from the object and returns the new one            
    stacksafe wide delete int:anIndex int:aLength
        => system'core_routines'__wide.

    /// Returns the substring
    stacksafe wide Substring int:anIndex int:aLength
        => system'core_routines'__wide.

    wide Substring : aLength at:anIndex
        <= Substring int:anIndex int:aLength.

    /// Reads the literal length into an output parameter
    stacksafe readLengthTo vint:aLength
        => system'core_routines'__wide.
        
    int length :: outnprop(readLengthTo).

    /// Saves the literal into a UTF-32 character array
    stacksafe save int:anIndex chararray:anArray vint:aLength
        => system'core_routines'__wide.

    stacksafe $save int:anIndex shortarray:anArray int:aRetVal
        => system'core_routines'__wide.

    /// Returns the object enumerator
    enumerator = WideLiteralEnumerator new wide:$self.
    
    /// Returns the object indexer
    indexer = WideLiteralIndexer new wide:$self.
    
    /// Clones the value
    clone = WideLiteralValue new wide:$self.

    // Reads the object hash code into the output parameter
    stacksafe readHashCodeTo vint:aCode 
        => system'core_routines'__wide.
        
    int hash_code :: outnprop(readHashCodeTo).
}

// --- Variant ---

/// A basic type variant class
class Variant
{
    object theValue.
    
    constructor new : object
    [
        theValue := object.
    ]
    
    /// Tries to convert the value to a byte number
    byte
        = byteConvertor convert:theValue.

    /// Tries to convert the value to a short integer number
    short
        = shortConvertor convert:theValue.

    /// Tries to convert the value to a character
    char
        = charConvertor convert:theValue.

    /// Tries to convert the literal value to the integer number
    int
        = intConvertor convert:theValue.

    /// Tries to convert the literal value to the long integer number
    long
        = longConvertor convert:theValue.

    /// Tries to convert the literal value to the real number
    real
        = realConvertor convert:theValue.

    /// Returns a literal value
    literal 
        = literalConvertor convert:theValue.
    
    /// Returns a wide literal value
    wide 
        = wideConvertor convert:theValue.

    /// Converts the parameter into a literal string
    set int:aValue
    [
        theValue := aValue.
    ]    

    /// Converts the parameter into a literal string
    set short:aValue
    [
        theValue := aValue.
    ]    

    /// Converts the parameter into a literal string
    write byte:aValue
    [
        theValue := aValue.
    ]    

    /// Converts the parameter into a literal string
    write long:aValue
    [
        theValue := aValue.
    ]    

    /// Converts the parameter into a literal string
    write real:aValue
    [
        theValue := aValue.
    ]    

    cast : aVerb to:aTarget
    [
        var aValue := aTarget cast:%eval to:
        {
            eval : aVariant int:aValue
                = aVariant int.
                
            eval : aVariant byte:aValue
                = aVariant byte.
                
            eval : aVariant short:aValue
                = aVariant short.
                
            eval : aVariant long:aValue
                = aVariant long.
                
            eval : aVariant real:aValue
                = aVariant real.
                
            eval : aVariant literal:aValue
                = aVariant literal.
                
            eval : aVariant wide:aValue
                = aVariant wide.
        }
        with:$self.
        
        ^ aValue cast:aVerb to:aTarget.
    ]

    equal : anObject
        = anObject equal:$self.
}

// --- Array ---

/// An array
sealed Array :: dispatchable(array), arrayof(object)
{
    /// Creates an empty array
    constructor min
        = object(0).
    
    /// Creates the array with the specified length
    constructor new : aLength
        <= new int:aLength.
    
    /// Creates the array with the specified length
    ///
    /// Obsolete
    constructor new length:aLength
        <= new int:aLength.
    
    /// Creates the array with the specified length
    stacksafe constructor new int:aLength
        = object(aLength).

    /// Creates a copy of the provided array
    constructor new array:anArray
        <= new int:0 int:(anArray length) array:anArray.
    
    /// Creates a copy of the provided sub array
    stacksafe constructor new int:anIndex int:aLength array:anArray
        <= new int:aLength;
    [
        system'core_routines'__array copy array:$self int:anIndex int:aLength array:anArray.
    ]
        
    constructor new object:singleItem
        <= new int:1;
    [
        $self setAt int:0 object:singleItem.
    ]

    /// Concatinates the arrays    
    add : anObject
        = $self add array:anObject.

    /// Concatinates the arrays    
    add array:anArray
    [
        int aLength1 := $self length.
        int aLength2 := anArray length.
        
        array aRetVal := Array new int:(aLength1 + aLength2).

        int anIndex := 0.
        while(anIndex < aLength1)
        [
            aRetVal setAt int:anIndex object:($self getAt int:anIndex).
            
            anIndex := anIndex + 1.
        ].

        anIndex := 0.
        while(anIndex < aLength2)
        [
            aRetVal setAt int:(aLength1 + anIndex) object:(anArray getAt int:anIndex).
            
            anIndex := anIndex + 1.
        ].
            
        ^ aRetVal
    ]

    /// Reads the array length into an output parameter
    stacksafe readLengthTo vint:aLength
        => system'core_routines'__array.

    /// Sets the specified array member
    stacksafe setAt int:anIndex object:anObject
        => system'core_routines'__array.

    /// Returns the specified array member
    stacksafe getAt int:anIndex
        => system'core_routines'__array.
        
    /// Returns the array enumerator
    enumerator = ArrayEnumerator new array:$self.

    /// Returns the array indexer
    indexer = ArrayIndexer new array:$self.

    /// Clones the array
    array clone 
        = Array new array:$self.
}

// --- $paramsOp ---

args extension $paramsOp
{
    stacksafe readLengthTo vint:aLength
        => system'core_routines'__args.            
        
    int length :: outnprop(readLengthTo).
}

// === Convertors ===

// --- literalConvertor ---

/// A literal convertor
class literalConvertor =
{
    /// Converts the parameter to the literal value
    stacksafe convert char:aValue bytearray:buffer vint:length
        => system'core_routines'__literalconvertor.
    
    stacksafe literal convert char:aValue
    [
        byte buffer(4).
        int  length := 0.
        
        $self convert char:aValue bytearray:buffer vint:length.
        
        ^ LiteralValue $new int:0 int:length bytearray:buffer
    ]
    
    stacksafe convert int:aNumber bytearray:buffer vint:length
        => system'core_routines'__literalconvertor.
    
    /// Converts the parameter to the literal value
    stacksafe literal convert int:aNumber
    [
        byte buffer(12).
        int  length := 0.
        
        $self convert int:aNumber bytearray:buffer vint:length.
        
        ^ LiteralValue $new int:0 int:length bytearray:buffer
    ]

    /// Converts the parameter to the literal value
    stacksafe convert short:aNumber bytearray:buffer vint:length
        => system'core_routines'__literalconvertor.
    
    /// Converts the parameter to the literal value
    stacksafe literal convert short:aNumber
    [
        byte buffer(12).
        int  length := 0.
        
        $self convert short:aNumber bytearray:buffer vint:length.
        
        ^ LiteralValue $new int:0 int:length bytearray:buffer
    ]

    /// Converts the parameter to the literal value
    stacksafe convert byte:aNumber bytearray:buffer vint:length
        => system'core_routines'__literalconvertor.
    
    /// Converts the parameter to the literal value
    stacksafe literal convert byte:aNumber
    [
        byte buffer(4).
        int  length := 0.
        
        $self convert byte:aNumber bytearray:buffer vint:length.
        
        ^ LiteralValue $new int:0 int:length bytearray:buffer
    ]

    /// Converts the parameter to the literal value
    stacksafe convert long:aNumber bytearray:buffer vint:length
        => system'core_routines'__literalconvertor.

    /// Converts the parameter to the literal value
    stacksafe literal convert long:aNumber
    [
        byte buffer(64).
        int  length := 0.
        
        $self convert long:aNumber bytearray:buffer vint:length.
        
        ^ LiteralValue $new int:0 int:length bytearray:buffer.
    ]

    /// Converts the parameter to the literal value with the provided radix
    stacksafe convert int:aNumber int:aRadix bytearray:buffer vint:length
        => system'core_routines'__literalconvertor.

    /// Converts the parameter to the literal value with the provided radix
    stacksafe literal convert int:aNumber int:aRadix
    [
        byte buffer(48).
        int  length := 0.
        
        $self convert int:aNumber int:aRadix bytearray:buffer vint:length.
        
        ^ LiteralValue $new int:0 int:length bytearray:buffer
    ]

    /// Converts the parameter to the literal value with the provided radix
    stacksafe convert short:aNumber int:aRadix bytearray:buffer vint:length
        => system'core_routines'__literalconvertor.

    /// Converts the parameter to the literal value with the provided radix
    stacksafe literal convert short:aNumber int:aRadix
    [
        byte buffer(32).
        int  length := 0.
        
        $self convert short:aNumber int:aRadix bytearray:buffer vint:length.
        
        ^ LiteralValue $new int:0 int:length bytearray:buffer
    ]

    /// Converts the parameter to the literal value with the provided radix
    stacksafe literal convert byte:aNumber int:aRadix bytearray:buffer vint:length
        => system'core_routines'__literalconvertor.

    /// Converts the parameter to the literal value with the provided radix
    stacksafe literal convert byte:aNumber int:aRadix
    [
        byte buffer(8).
        int  length := 0.
        
        $self convert byte:aNumber int:aRadix bytearray:buffer vint:length.
        
        ^ LiteralValue $new int:0 int:length bytearray:buffer.
    ]

    /// Converts the parameter to the literal value with the provided radix
    stacksafe convert long:aNumber int:aRadix bytearray:buffer vint:length
        => system'core_routines'__literalconvertor.

    /// Converts the parameter to the literal value
    stacksafe literal convert long:aNumber int:radix
    [
        byte buffer(64).
        int  length := 0.
        
        $self convert long:aNumber int:radix bytearray:buffer vint:length.
        
        ^ LiteralValue $new int:0 int:length bytearray:buffer.
    ]

    /// Converts the parameter to the literal value
    stacksafe convert real:aNumber bytearray:buffer vint:length
        => system'core_routines'__literalconvertor.

    /// Converts the parameter to the literal value
    stacksafe literal convert real:aNumber
    [
        byte buffer(32).
        int  length := 0.
        
        $self convert real:aNumber bytearray:buffer vint:length.
        
        ^ LiteralValue $new int:0 int:length bytearray:buffer.
    ]

    /// Converts UTF-16 literal to UTF-8 one
    stacksafe convert wide:aLiteral bytearray:buffer vint:length
        => system'core_routines'__literalconvertor.

    stacksafe literal convert wide:aLiteral
    [
        int  i := 0.
        int  m := 0.
        int  page := 0.
        int  l := aLiteral length.
        byte buffer(256).
        if (l < 128)
        [
            $self convert wide:aLiteral bytearray:buffer vint:m.
            
            ^ LiteralValue $new int:0 int:m bytearray:buffer
        ].

        literal retVal := emptyLiteralValue.
        while(l > 0)
        [
            page := l.
            if (page > 127)
            [
               page := 127.
            ].
            
            $self convert wide:(aLiteral Substring int:i int:page) bytearray:buffer vint:m.
            
            retVal := retVal add literal:(LiteralValue $new int:0 int:m bytearray:buffer).
            
            i := i + page.
            l := l - page.
        ].
        
        ^ retVal
    ]

    /// Returns the literal presentation of the parameter
    stacksafe literal convert : anObject
        = anObject literal.
//
    /// Returns a parameter                
    stacksafe literal convert literal:aLiteral
        = aLiteral.
}.

// --- wideConvertor ---

/// A wide literal convertor
class wideConvertor =
{
    /// Converts the parameter to the literal value
    stacksafe convert char:aValue shortarray:buffer vint:length
        => system'core_routines'__wideconvertor.
    
    stacksafe wide convert char:aValue
    [
        short buffer(2).
        int   length := 0.
        
        $self convert char:aValue shortarray:buffer vint:length.
        
        ^ WideLiteralValue $new int:0 int:length shortarray:buffer
    ]    
    
    /// Converts the parameter to the literal value
    stacksafe convert int:aNumber shortarray:buffer vint:length
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value
    stacksafe wide convert int:aNumber
    [
        short buffer(12).
        int   length := 0.
        
        $self convert int:aNumber shortarray:buffer vint:length.
        
        ^ WideLiteralValue $new int:0 int:length shortarray:buffer
    ]

    /// Converts the parameter to the literal value
    stacksafe convert short:aNumber shortarray:buffer vint:length
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value
    stacksafe wide convert short:aNumber
    [
        short buffer(12).
        int   length := 0.
        
        $self convert short:aNumber shortarray:buffer vint:length.
        
        ^ WideLiteralValue $new int:0 int:length shortarray:buffer
    ]

    /// Converts the parameter to the literal value
    stacksafe convert byte:aNumber shortarray:buffer vint:length
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value
    stacksafe wide convert byte:aNumber
    [
        short buffer(12).
        int   length := 0.
        
        $self convert byte:aNumber shortarray:buffer vint:length.
        
        ^ WideLiteralValue $new int:0 int:length shortarray:buffer
    ]

    /// Converts the parameter to the literal value
    stacksafe convert long:aNumber shortarray:buffer vint:length
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value
    stacksafe wide convert long:aNumber
    [
        short buffer(64).
        int   length := 0.
        
        $self convert long:aNumber shortarray:buffer vint:length.
        
        ^ WideLiteralValue $new int:0 int:length shortarray:buffer.
    ]

    /// Converts the parameter to the literal value with the provided radix
    stacksafe convert int:aNumber int:aRadix shortarray:buffer vint:length
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value with the provided radix
    stacksafe wide convert int:aNumber int:aRadix
    [
        short buffer(32).
        int   length := 0.
        
        $self convert int:aNumber int:aRadix shortarray:buffer vint:length.
        
        ^ WideLiteralValue $new int:0 int:length shortarray:buffer
    ]

    /// Converts the parameter to the literal value with the provided radix
    stacksafe convert short:aNumber int:aRadix shortarray:buffer vint:length
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value with the provided radix
    stacksafe wide convert short:aNumber int:aRadix
    [
        short buffer(16).
        int   length := 0.
        
        $self convert short:aNumber int:aRadix shortarray:buffer vint:length.
        
        ^ WideLiteralValue $new int:0 int:length shortarray:buffer
    ]

    /// Converts the parameter to the literal value with the provided radix
    stacksafe convert byte:aNumber int:aRadix shortarray:buffer vint:length
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value with the provided radix
    stacksafe wide convert byte:aNumber int:aRadix
    [
        short buffer(12).
        int   length := 0.
        
        $self convert byte:aNumber int:aRadix shortarray:buffer vint:length.
        
        ^ WideLiteralValue $new int:0 int:length shortarray:buffer
    ]

    /// Converts the parameter to the literal value with the provided radix
    stacksafe convert long:aNumber int:aRadix shortarray:buffer vint:length
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value with the provided radix
    stacksafe wide convert long:aNumber int:aRadix
    [
        short buffer(64).
        int   length := 0.
        
        $self convert long:aNumber int:aRadix shortarray:buffer vint:length.
        
        ^ WideLiteralValue $new int:0 int:length shortarray:buffer
    ]

    /// Converts the parameter to the literal value
    stacksafe convert real:aNumber shortarray:buffer vint:length
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value
    stacksafe wide convert real:aNumber
    [
        short buffer(32).
        int   length := 0.
        
        $self convert real:aNumber shortarray:buffer vint:length.
        
        ^ WideLiteralValue $new int:0 int:length shortarray:buffer
    ]

    /// Converts UTF-8 literal to UTF-16e
    stacksafe convert literal:aLiteral shortarray:buffer vint:length
        => system'core_routines'__wideconvertor.

    /// Converts UTF-8 literal to UTF-16e
    stacksafe wide convert literal:aLiteral
    [
        int   i := 0.
        int   m := 0.
        int   page := 0.
        int   l := aLiteral length.
        short buffer(256).
        if (l < 256)
        [
            $self convert literal:aLiteral shortarray:buffer vint:m.
            
            ^ WideLiteralValue $new int:0 int:m shortarray:buffer
        ].
        
        wide retVal := emptyWideLiteralValue.
        while(l > 0)
        [
            page := l.
            if (page > 256)
            [
                page := 256
            ].
            
            $self convert literal:(aLiteral Substring int:i int:page) shortarray:buffer vint:m.
            
            retVal := retVal add wide:(WideLiteralValue $new int:0 int:m shortarray:buffer).
            
            i := i + page.
            l := l - page
        ].
        
        ^ retVal
    ]        

    /// Returns the literal presentation of the parameter
    stacksafe wide convert : anObject
        = anObject wide.
        
    /// Returns a parameter
    stacksafe wide convert wide:aLiteral
        = aLiteral.
}.

// === Enumerators / Indexers

// --- LiteralEnumerator ---

/// A literal enumerator
class LiteralEnumerator :: Enumerator
{
    literal theLiteral.
    vint    theIndex.
    vint    theDiff.
    char    theChar.
    
    /// Creates an enumerator
    constructor new : anObject
        <= new literal:anObject.
    
    /// Creates an enumerator
    constructor new literal:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := -1.
        theDiff := 1.
        theChar := $0.
    ]

    /// Resets the enumerator
    reset
    [
        theIndex := -1.
        theDiff := 1.
    ]

    /// Goes to the next enumeration member and returns true if it is not the last member
    next
    [
        int aLength := theLiteral length.
        
        theIndex append int:theDiff.
        
        if (theIndex < aLength)
            [
                theChar := theLiteral getAt int:theIndex.                
                
                theChar readLengthTo vint:theDiff.
                
                ^ true
            ].

        theDiff := 0.
        theChar := $0.

        ^ false
    ]
    
    /// Returns the current character
    get = theChar.
}

// --- LiteralIndexer ---

/// A literal value indexer
class LiteralIndexer :: Indexer
{
    literal theLiteral.
    vint    theIndex.
    
    /// Creates an indexer
    constructor new : anObject
        <= new literal:anObject.
    
    /// Creates an indexer
    constructor new literal:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := 0.
    ]
    
    /// Returns false if the index out of range
    available
    [
        int aLength := theLiteral length.
        
        ^ theIndex < aLength.
    ]

    /// Returns the current character
    get
    [
        char aChar := $0.
        theLiteral read int:theIndex vchar:aChar.
        
        ^ CharValue new char:aChar.
    ]
    
    /// Sets the index by specified value
    set : anObject 
    [
        system'NotSupportedException new; raise.
    ]
    
    /// Reads the current index and saves it into an output parameter
    readIndexTo vint:anIndex
    [
        anIndex int := theIndex.
    ]
    
    /// Sets the index by specified value
    writeIndex int:anIndex 
    [
        theIndex set int:anIndex.
    ]
    
    /// Reads the length to the output parameter    
    readLengthTo vint:anOutput
        => theLiteral.
}

// --- WideLiteralEnumerator ---

/// A literal enumerator
class WideLiteralEnumerator :: Enumerator
{
    wide theLiteral.
    vint theIndex.
    vint theDiff.
    char theChar.
    
    /// Creates an enumerator
    constructor new : anObject
        <= new wide:anObject.
    
    /// Creates an enumerator
    constructor new wide:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := -1.
        theDiff := 1.
        theChar := $0.
    ]

    /// Resets the enumerator
    reset
    [
        theIndex set int:-1.
        theDiff set int:1.
    ]

    /// Goes to the next enumeration member and returns true if it is not the last member
    next
    [
        int aLength := theLiteral length.

        theIndex append int:theDiff.
        
        if (theIndex < aLength)
            [
                theChar := theLiteral getAt int:theIndex.
                
                theChar readLengthTo vint:theDiff.
                theDiff append int:1.
                theDiff int := theDiff / 2.
                
                ^ true
            ].

        theDiff set int:0.
        theChar := $0.

        ^ false
    ]
    
    /// Returns the current character
    get = theChar.
}

// --- WideLiteralInexer ---

/// A wide literal enumerator
class WideLiteralIndexer :: Indexer
{
    wide theLiteral.
    vint theIndex.
    
    /// Creates an indexer
    constructor new : anObject
        <= new wide:anObject.
    
    /// Creates an indexer
    constructor new wide:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := 0.
    ]

    /// Returns false if the index out of range    
    available
    [
        int aLength := theLiteral length.
        
        ^ theIndex < aLength
    ]

    /// Returns the current character
    get
        = theLiteral getAt int:theIndex.
    
    set : anObject 
    [
        system'NotSupportedException new; raise.
    ]
    
    /// Reads the current index and saves it into an output parameter
    readIndexTo vint:anIndex
    [
        anIndex int := theIndex.
    ]
    
    /// Sets the index by specified value
    writeIndex int:anIndex 
    [
        theIndex set int:anIndex.
    ]
    
    /// Reads the length to the output parameter    
    readLengthTo vint:anOutput
        => theLiteral.
}

// --- ArrayIndexer ---

/// An array indexer
class ArrayIndexer :: Indexer
{
    array theArray.
    vint  theIndex.

    /// Returns the parameter indexer
    constructor new : anArray
        <= new array:anArray.
    
    /// Returns the parameter indexer
    constructor new array:anArray
    [
        theArray := anArray.
        theIndex := 0.
    ]
    
    /// Returns false if the index out of range
    available
    [
        int aLength := 0.
        theArray readLengthTo vint:aLength.
        
        ^ theIndex < aLength.
    ]

    /// Returns the current member
    get
        = theArray getAt int:theIndex.
    
    /// Sets the current member
    set : anObject
    [
        theArray setAt int:theIndex object:anObject.
    ]
    
    /// Reads the current index and saves it into an output parameter
    readIndexTo vint:anIndex
    [
        anIndex int := theIndex.
    ]
    
    /// Sets the index by specified value
    writeIndex int:anIndex 
    [
        theIndex set int:anIndex.
    ]
    
    /// Reads the length to the output parameter    
    readLengthTo vint:anOutput
        => theArray.
}

// --- ArrayEnumerator ---

/// An array enumerator
class ArrayEnumerator :: Enumerator
{
    array theArray.
    vint  theIndex.
    
    /// Creates an array enumerator
    constructor new : anArray
        <= new array:anArray.
    
    /// Creates an array enumerator
    constructor new array:anArray
    [
        theArray := anArray.
        theIndex := -1.
    ]

    /// Resets the enumerator
    reset
    [
        theIndex set int:-1.
    ]

    /// Goes to the next enumeration member and returns true if it is not the last member
    next
    [
        int aLength := theArray length.
        
        theIndex append int:1.

        ^ theIndex < aLength.
    ]
    
    /// Returns the current object
    get
        = theArray getAt int:theIndex.
}

// --- Func1Group ---

//#subject func1array = arrayof:func1.

//#class Function1Group :: BaseFunction1
//{
//    #field(func1array) theHandlers.
//    
//    #constructor new &args:Handlers
//    [
//        #var(int)i.
//        #var(int)len := Handlers length.
//        
//        theHandlers := func1:len.
//        
//        #loop (i < len)?
//        [
//            theHandlers setAt &int:i &func1:((Handlers@i) func1).
//            
//            i += 1.
//        ].
//    ]
//    
//    #method eval : anObject
//    [
//        #var(int)i.
//        #var(int)len := theHandlers length.
//        
//        #loop (i < len)?
//        [
//            theHandlers getAt &int:i eval:anObject.
//            
//            i += 1.
//        ].
//    ]
//}

// --- BitArrayIndexer ---

/// An array indexer
class BitArrayIndexer :: Indexer
{
    bitarray theArray.
    vint  theIndex.

    /// Returns the parameter indexer
    constructor new : anArray
        <= new bitarray:anArray.
    
    /// Returns the parameter indexer
    constructor new bitarray:anArray
    [
        theArray := anArray.
        theIndex := 0.
    ]
    
    /// Returns false if the index out of range
    available
    [
        int aLength := theArray length.
        
        ^ theIndex < aLength.
    ]

    /// Returns the current member
    bool get
        = theArray getAt int:theIndex.
    
    /// Sets the current member
    set : anObject
    [
        theArray setAt int:theIndex bool:anObject.
    ]
    
    /// Reads the current index and saves it into an output parameter
    readIndexTo vint:anIndex
    [
        anIndex int := theIndex.
    ]
    
    /// Sets the index by specified value
    writeIndex int:anIndex 
    [
        theIndex set int:anIndex.
    ]
    
    /// Reads the length to the output parameter    
    readLengthTo vint:anOutput
        => theArray.
}

// --- BitArrayEnumerator ---

/// An array enumerator
class BitArrayEnumerator :: Enumerator
{
    bitarray theArray.
    vint     theIndex.
    
    /// Creates an array enumerator
    constructor new : anArray
        <= new bitarray:anArray.
    
    /// Creates an array enumerator
    constructor new bitarray:anArray
    [
        theArray := anArray.
        theIndex := -1.
    ]

    /// Resets the enumerator
    reset
    [
        theIndex set int:-1.
    ]

    /// Goes to the next enumeration member and returns true if it is not the last member
    next
    [
        int aLength := theArray length.
        
        theIndex append int:1.

        ^ theIndex < aLength.
    ]
    
    /// Returns the current object
    bool get
        = theArray getAt int:theIndex.
}

// --- BitArray32Indexer ---

/// An array indexer
class BitArray32Indexer :: Indexer
{
    bitset theArray.
    vint   theIndex.

    /// Returns the parameter indexer
    constructor new : anArray
        <= new bitset:anArray.
    
    /// Returns the parameter indexer
    constructor new bitset:anArray
    [
        theArray := anArray.
        theIndex := 0.
    ]
    
    /// Returns false if the index out of range
    available
    [
        ^ theIndex < 32
    ]

    /// Returns the current member
    bool get
        = theArray getAt int:theIndex.
    
    /// Sets the current member
    set : anObject
    [
        theArray setAt int:theIndex bool:anObject.
    ]
    
    /// Reads the current index and saves it into an output parameter
    readIndexTo  vint:anIndex
    [
        anIndex int := theIndex.
    ]
    
    /// Sets the index by specified value
    writeIndex int:anIndex 
    [
        theIndex set int:anIndex.
    ]
    
    /// Reads the length to the output parameter    
    readLengthTo vint:anOutput
        => theArray.
}

// --- BitArray32Enumerator ---

/// An array enumerator
class BitArray32Enumerator :: Enumerator
{
    bitset theArray.
    vint   theIndex.
    
    /// Creates an array enumerator
    constructor new : anArray
        <= new bitset:anArray.
    
    /// Creates an array enumerator
    constructor new bitset:anArray
    [
        theArray := anArray.
        theIndex := -1.
    ]

    /// Resets the enumerator
    reset
    [
        theIndex set int:-1.
    ]

    /// Goes to the next enumeration member and returns true if it is not the last member
    next
    [
        theIndex append int:1.

        ^ theIndex < 32.
    ]
    
    /// Returns the current object
    get
        = theArray getAt int:theIndex.
}
