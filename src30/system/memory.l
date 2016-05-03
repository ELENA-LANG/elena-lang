// --- arrayof ---

#template(dynamic) arrayof : template_subj
{
    #field(template_subj)$target.

    #method(suppress:w3) enumerator 
    [
        #var(vint)index := -1.
        #var(int)length := 0.
        $target >> length.
        
        ^ Enumerator
          {
              get = $owner getAt &int:index.
              
              enumerable = $owner.
              
              reset [ index := -1. ]
              
              next
              [
                  index += 1.
                  
                  ^ index < length.
              ]
          }.
    ]
        
    #method(outnprop:length,stacksafe)readLength &vint:retVal
    [
        #var(int)len.
        $target >> len.
        
        retVal := len.
    ]

    #method(suppress:w3,stacksafe,template_subj) getAt &int:n
    [
        (n < 0)
            ? [ #throw InvalidArgumentException new. ].
        
        #var(int)len.
        $target >> len.

        (n < len)
            ? [ ^ $target @ n. ]
            ! [ #throw InvalidArgumentException new. ].
            
    ]

    #method(suppress:w3,stacksafe) setAt &int:n &template_subj:item
    [
        (n < 0)
            ? [ #throw InvalidArgumentException new. ].
        
        #var(int)len.
        $target >> len.

        (n < len)
            ? [ $target @ n := item. ]
            ! [ #throw InvalidArgumentException new. ].            
    ]
                
    /// Returns the item at the specified index
    #method(template_subj) getAt : aParam
        <= %getAt &int:(aParam int).
        
    /// Returns the item at the specified index
    #method(suppress:w2) setAt : index : value
        <= %setAt &int:(index int) &template_subj:value.
}

// === basic memory classes ===

// --- ByteArray ---

/// A byte array
#class(sealed,arrayof:byte,dispatchable:bytearray) ByteArray
{
    /// Creates the byte array with the specified length
    #constructor(stacksafe) new &int:len
        = byte:len.

    /// Creates the byte array with the specified length
    #constructor new : aLength
        <= new &int:(aLength int).
    
    /// Creates the copy of a byte subarray
    #constructor(stacksafe) new &int:anIndex &int:aLength &bytearray:anArray
        <= (new &int:aLength)
    [
        anArray read &int:anIndex &int:aLength &bytearray:$self.
    ]
    
    /// Creates the byte array copy
    #constructor(stacksafe) new &bytearray:anArray
        <= new &int:0 &int:(anArray length) &bytearray:anArray.

    /// Reads the array length to the output parameter
    #method(stacksafe) readLength &vint:aLength
        => system'core_routines'__bytearray.

    /// Fills the sub array with a specified value
    #method(stacksafe) fill &int:anIndex &int:aLength &byte:aValue
    [
        #var(int)aTotalLength := $self length.
        
        #var(int)i := anIndex.
        #var(int)m := anIndex + aLength.
        
        (i > aTotalLength)
            ? [ #throw system'InvalidArgumentException new. ].
        
        (m > aTotalLength)
            ? [ #throw system'InvalidArgumentException new. ].
            
        #loop (i < m)?
        [
            $self write &int:i &byte:aValue.
            
            i := i + 1.
        ].
    ]

    /// Copies a sub array to the provided one at the specified position
    #method(stacksafe) read &int:anIndex &int:aLength &bytearray:aDump
        => system'core_routines'__bytearray.

    /// Copies the byte array at the specified position
    #method(stacksafe) write &int:anIndex &int:aLength &bytearray:aDump
        => system'core_routines'__bytearray.

    /// Writes the byte value at specified position
    #method(stacksafe) write &int:anIndex &byte:anInt
        => system'core_routines'__bytearray.

    /// Reads a byte value at the specified position and saves it into an output variable
    #method(stacksafe) read &int:anIndex &vbyte:anInt
        => system'core_routines'__bytearray.

    /// Writes a short integer value at the specified position
    #method(stacksafe) write &int:anIndex &short:anInt
        => system'core_routines'__bytearray.

    /// Reads a short integer value at the specified position and saves it into an output variable
    #method(stacksafe) read &int:anIndex &vshort:aChar
        => system'core_routines'__bytearray.

    /// Writes a character value at the specified position
    #method(stacksafe) write &int:anIndex &char:anInt
        => system'core_routines'__bytearray.

    /// Reads a character value at the specified position and saves it into an output variable
    #method(stacksafe) read &int:anIndex &vchar:aChar
        => system'core_routines'__bytearray.

    /// Writes an integer value at the specified position
    #method(stacksafe) write &int:anIndex &int:anInt
        => system'core_routines'__bytearray.

    /// Reads an integer value at the specified position and saves it into an output variable
    #method(stacksafe) read &int:anIndex &vint:anInt
        => system'core_routines'__bytearray.

    /// Writes a long integer value at the specified position
    #method(stacksafe) write &int:anIndex &long:aLong
        => system'core_routines'__bytearray.

    /// Reads a long integer value at the specified position and saves it into an output variable
    #method(stacksafe) read &int:anIndex &vlong:aLong
        => system'core_routines'__bytearray.

    /// Writes a real value at the specified position
    #method(stacksafe) write &int:anIndex &real:aLong
        => system'core_routines'__bytearray.

    /// Reads a real value at the specified position and saves it into an output variable
    #method(stacksafe) read &int:anIndex &vreal:aLong
        => system'core_routines'__bytearray.

    #method(stacksafe) move &int:anIndex &int:aLength &int:anOffset
        => system'core_routines'__bytearray.
    
    /// Concatinates the arrays    
    #method(stacksafe,bytearray) add &int:aLength2 &bytearray:anArray
    [
        #var(int)aLength1 := $self length.
        
        #var(bytearray)aRetVal := ByteArray new &int:(aLength1 + aLength2).

        #var(byte)tmp.
        #var(int)anIndex := 0.
        #loop (anIndex < aLength1) ?
        [
            $self read &int:anIndex &vbyte:tmp.
            aRetVal write &int:anIndex &byte:tmp.
            
            anIndex += 1.
        ].

        anIndex := 0.
        #loop (anIndex < aLength2) ?
        [
            anArray read &int:anIndex &vbyte:tmp.
            aRetVal write &int:(aLength1 + anIndex) &byte:tmp.
            
            anIndex += 1.
        ].
            
        ^ aRetVal.
    ]    

    /// Creates the object copy
    #method clone = ByteArray new &bytearray:$self.
}

// --- ShortArray ---

/// A short integer array
#class(sealed,arrayof:short,dispatchable:shortarray) ShortArray
{
    /// Creates the short array with the specified length
    #constructor(stacksafe) new &int:aLength
        = short:aLength.
    
    /// Creates the short array with the specified length
    #constructor new : aLength
        <= new &int:(aLength int).
    
    /// Creates the copy of a short subarray
    #constructor(stacksafe) new &int:anIndex &int:aLength &shortarray:anArray
        <= (new &int:aLength)
    [
        anArray read &int:anIndex &int:aLength &shortarray:$self.
    ]

    /// Creates the short array copy
    #constructor new &shortarray:anArray
        <= new &int:0 &int:(anArray length) &shortarray:anArray.

    /// Reads the array length to the output parameter
    #method(stacksafe) readLength &vint:aLength
        => system'core_routines'__shortarray.

    /// Fills the sub array with a specified value
    #method(stacksafe) fill &int:anIndex &int:aLength &short:aValue
    [
        #var(int)aTotalLength := $self length.
        
        #var(int)i := anIndex.
        #var(int)m := anIndex + aLength.
        
        (i > aTotalLength)
            ? [ #throw system'InvalidArgumentException new. ].
        
        (m > aTotalLength)
            ? [ #throw system'InvalidArgumentException new. ].
            
        #loop (i < m)?
        [
            $self write &int:i &short:aValue.
            
            i += 1.
        ].
    ]
    
    /// Copies a sub array to the provided one at the specified position
    #method(stacksafe) read &int:anIndex &int:aLength &shortarray:anArray
        => system'core_routines'__shortarray.

    /// Copies the short array at the specified position   
    #method(stacksafe) write &int:anIndex &int:aLength &shortarray:anArray
        => system'core_routines'__shortarray.

    /// Writes a short integer value at the specified position
    #method(stacksafe) write &int:anIndex &short:aValue
        => system'core_routines'__shortarray.

    /// Reads a short integer value at the specified position and saves it into an output variable
    #method(stacksafe) read &int:anIndex &vshort:aValue
        => system'core_routines'__shortarray.

    /// Creates the object copy
    #method clone = ShortArray new &shortarray:$self.
}

// --- IntArray ---

/// An array of integers
#class(sealed,arrayof:int,dispatchable:intarray) IntArray
{
    /// Creates the array with the specified length
    #constructor(stacksafe) new &int : aLength
        = int:aLength.

    /// Creates the array with the specified length
    #constructor new : aLength
        <= new &int:(aLength int).

    /// Creates the copy of a subarray
    #constructor(stacksafe) new &int:anIndex &int:aLength &intarray:anArray
        <= (new &int:aLength)
    [
        anArray read &int:anIndex &int:aLength &intarray:$self.
    ]

    /// Creates the short array copy
    #constructor new &intarray:anArray
        <= new &int:0 &int:(anArray length) &intarray:anArray.
    
    /// Reads the array length to the output parameter
    #method(stacksafe) readLength &vint:aLength
        => system'core_routines'__intarray.

    /// Fills the sub array with a specified value
    #method(stacksafe) fill &int:anIndex &int:aLength &int:aValue
    [
        #var(int)aTotalLength := $self length.
        
        #var(int)i := anIndex.
        #var(int)m := anIndex + aLength.
        
        (i > aTotalLength)
            ? [ #throw system'InvalidArgumentException new. ].
        
        (m > aTotalLength)
            ? [ #throw system'InvalidArgumentException new. ].
            
        #loop (i < m)?
        [
            $self write &int:i &int:aValue.
            
            i += 1.
        ].
    ]

    /// Copies a sub array to the provided one at the specified position
    #method(stacksafe) read &int:anIndex &int:aLength &intarray:anArray
        => system'core_routines'__intarray.

    /// Copies the short array at the specified position
    #method(stacksafe) write &int:anIndex &int:aLength &intarray:anArray
        => system'core_routines'__intarray.

    /// Writes an integer value at the specified position
    #method(stacksafe) write &int:anIndex &int:aValue
        => system'core_routines'__intarray.

    /// Reads an integer value at the specified position and saves it into an output variable
    #method(stacksafe) read &int:anIndex &vint:anInt
        => system'core_routines'__intarray.

    /// Creates the object copy
    #method clone = IntArray new &intarray:$self.
}

//// --- BitArray ---
//
//#class(sealed) BitArray
//{
//    #field(type:intarray) theArray.
//    #field(type:int)      theLength.
//    
//    /// Creates the array with the specified length
//    #constructor(stacksafe) new &int:length
//    [
//        theLength := length.
//        theArray := IntArray new &int:(length / 32 + 1).
//    ]
//    
//    #method(stacksafe)readLength &vint:retVal
//    [
//        retVal << theLength.
//    ]
//    
//    #method(stacksafe,type:int,embeddable,suppress:w3)length
//    [
//        #var(type:int)retVal.
//        $self readLength &vint:retVal.
//        
//        ^ retVal.
//    ]
//    
//    #method getAt:index
//        = $self getAt &int:(index int).
//    
//    #method(type:bool,stacksafe)getAt &int:index
//    [
//        #var(type:int)arr_index := index / 32.
//        #var(type:int)bit_index := arr_index * 32.
//        bit_index := index - arr_index.
//        bit_index := 0 - bit_index.
//        
//        #var(type:int)bit_mask := 1.
//        (bit_index != 0)
//            ? [ bit_mask shift &int:bit_index &vint:bit_mask. ].
//        
//        #var(type:int)bits.
//        theArray read &int:arr_index &vint:bits.
//        
//        ^ (bits && bit_mask) != 0.
//    ]
//    
//    #method setAt:index:value
//    [
//        $self setAt &int:(index int) &bool:(value bool).
//    ]
//    
//    #method setAt &int:index &bool:value
//    [
//        #var(type:int)arr_index := index / 32.
//        #var(type:int)bit_index := arr_index * 32.
//        bit_index := index - arr_index.
//        bit_index := 0 - bit_index.
//        
//        #var(type:int)bit_mask := 1.
//        (bit_index != 0)
//            ? [ bit_mask shift &int:bit_index &vint:bit_mask. ].
//        
//        #var(type:int)bits.
//        theArray read &int:arr_index &vint:bits.
//        
//        value
//            ? [ bits := bits || bit_mask. ]
//            ! [  
//                bit_mask := bit_mask not.
//                
//                bits := bits && bit_mask.
//            ].
//            
//        theArray write &int:arr_index &int:bits.            
//    ]
//
//    #method bitarray = $self.
//        
//    /// Dispatches the verb to the target object with intarray subject
//    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &bitarray:$self.
//    
//    /// Dispatches the verb to the target object with intarray subject
//    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &bitarray:$self.    
//    
//    /// Returns the array enumerator
//    #method enumerator = BitArrayEnumerator new &bitarray:$self.
//
//    /// Returns the array iterator
//    #method iterator = BitArrayIterator new &bitarray:$self.    
//}
//
//// --- BitArray32 ---
//
//#class(sealed,struct:embeddable) BitArray32
//{
//    #field(type:int)theValue.
//    
//    #constructor new &int:value
//    [
//        theValue := value.
//    ]
//    
//    #constructor new:value
//        <= new &int:(value int).
//    
//    #method(type:int) length = 32.
//    
//    #method write &int:value
//    [
//        theValue := value.
//    ]
//    
//    #method write : value
//    [
//        value cast:%write &to:$self.
//    ]
//    
//    #method getAt:index
//        = $self getAt &int:(index int).
//    
//    #method(type:bool,stacksafe)getAt &int:index
//    [
//        #var(type:int)bit_index := 0 - index.
//        
//        #var(type:int)bit_mask := 1.
//        (bit_index != 0)
//            ? [ bit_mask shift &int:bit_index &vint:bit_mask. ].
//        
//        ^ (theValue && bit_mask) != 0.
//    ]
//    
//    #method setAt:index:value
//    [
//        $self setAt &int:(index int) &bool:(value bool).
//    ]
//    
//    #method setAt &int:index &bool:value
//    [
//        #var(type:int)bit_index := 0 - index.
//        
//        #var(type:int)bit_mask := 1.
//        (bit_index != 0)
//            ? [ bit_mask shift &int:bit_index &vint:bit_mask. ].
//        
//        value
//            ? [ theValue := theValue || bit_mask. ]
//            ! [  
//                bit_mask := bit_mask not.
//                
//                theValue := theValue && bit_mask.
//            ].
//    ]
//    
//    #method bitset = $self.
//        
//    /// Dispatches the verb to the target object with intarray subject
//    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &bitset:$self.
//    
//    /// Dispatches the verb to the target object with intarray subject
//    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &bitset:$self.
//    
//    /// Returns the array enumerator
//    #method enumerator = BitArray32Enumerator new &bitset:$self.
//
//    /// Returns the array iterator
//    #method iterator = BitArray32Iterator new &bitset:$self.
//}
//
//// --- IntMatrix ---
//
//#class(sealed)IntMatrix
//{
//    #field(type:intarray)theArray.
//    #field(type:int)theRows.
//    #field(type:int)theColumns.
//    
//    #constructor new &int:Columns &int:Rows
//    [
//        theRows := Rows.
//        theColumns := Columns.
//        
//        theArray := IntArray new &int:(Columns * Rows).        
//    ]
//        
//    #constructor new : Columns : Rows
//        <= new &int:(Columns int) &int:(Rows int).
//        
//    /// Reads the matrix lengths to the output parameter
//    #method(stacksafe) readLengths &vint:Columns &vint:Rows
//    [
//        Columns << theColumns.
//        Rows << theRows.
//    ]
//
//    #method(type:int) length = theRows.
//    
//    /// Returns itself
//    #method intmatrix = $self.
//
//    /// Writes an integer value at the specified position
//    #method(stacksafe) write &int:i &int:j &int:aValue
//    [
//        theArray write &int:(i * theColumns + j) &int:aValue.
//    ]
//
//    /// Reads an integer value at the specified position and saves it into an output variable
//    #method(stacksafe) read &int:i &int:j &vint:anInt
//    [
//        theArray read &int:(i * theColumns + j) &vint:anInt.
//    ]
//
//    #method getAt : i : j
//    [
//        #var(type:int)aValue.            
//        $self read &int:(i int) &int:(j int) &vint:aValue.
//        
//        ^ IntNumber new &int:aValue.
//    ]
//
//    #method setAt : i : j : aValue
//    [
//        $self write &int:(i int) &int:(j int) &int:(aValue int).    
//    ]
//
//    /// Returns a row
//    #method getAt : i =
//    {
//        length = theColumns int.
//        
//        getAt : j
//            = self getAt:i:j.
//        
//        setAt : j : aValue
//        [
//            self setAt:i:j:aValue.
//        ]
//    }.
//
//    /// Dispatches the verb to the target object with intarray subject
//    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &intmatrix:$self.
//    
//    /// Dispatches the verb to the target object with intarray subject
//    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &intmatrix:$self.
//}
//
//// --- RealArray ---
//
///// An array of real numbers
//#class(sealed,nonstructural) RealArray
//{
//    #field(type:bytearray)theBinaryArray.
//    
//    /// Creates the array with the specified length
//    #constructor(stacksafe) new &int:aLength
//    [
//        theBinaryArray := ByteArray new &int:(aLength * 8).
//    ]
//
//    /// Creates the array with the specified length
//    #constructor new : aLength
//        <= new &int:(aLength int).
//
//    /// Creates the copy of a subarray
//    #constructor(stacksafe) new &int:anIndex &int:aLength &realarray:anArray
//    [
//        theBinaryArray := ByteArray new &int:(aLength * 8).
//        
//        theBinaryArray write &int:anIndex &int:aLength &bytearray:(anArray bytearray).
//    ]
//    
//    #method bytearray = theBinaryArray.
//    
//    /// Creates the array copy
//    #constructor new &realarray:anArray
//        <= new &int:0 &int:(anArray length int) &realarray:anArray.
//    
//    /// Reads the array length to the output parameter
//    #method(stacksafe) readLength &vint:aLength
//    [
//        #var(type:int)aBinLen.
//        theBinaryArray readLength &vint:aBinLen.
//        
//        aBinLen shift &int:3 &vint:aLength.
//    ]
//
//    /// Returns the array length
//    #method(stacksafe,type:int,embeddable,suppress:w3) length
//    [
//        #var(type:int)aLength.
//        $self readLength &vint:aLength.
//        
//        ^ aLength.
//    ]
//    
//    /// Returns itself
//    #method realarray = $self.
//
//    /// Fills the sub array with a specified value
//    #method(stacksafe) fill &int:anIndex &int:aLength &real:aValue
//    [
//        #var(type:int)aTotalLength := $self length.
//        
//        #var(type:int)i := anIndex.
//        #var(type:int)m := anIndex + aLength.
//        
//        (i > aTotalLength)
//            ? [ #throw system'InvalidArgumentException new. ].
//        
//        (m > aTotalLength)
//            ? [ #throw system'InvalidArgumentException new. ].
//            
//        #loop (i < m)?
//        [
//            $self write &int:i &real:aValue.
//            
//            i := i + 1.
//        ].
//    ]
//
//    /// Copies a sub array to the provided one at the specified position
//    #method(stacksafe) read &int:anIndex &int:aLength &realarray:anArray
//    [
//        theBinaryArray read &int:(anIndex * 8) &int:(aLength * 8) &bytearray:(anArray bytearray).
//    ]
//    
//    /// Copies the short array at the specified position
//    #method(stacksafe) write &int:anIndex &int:aLength &realarray:anArray
//    [
//        theBinaryArray write &int:(anIndex * 8) &int:(aLength * 8) &bytearray:(anArray bytearray).
//    ]
//
//    /// Writes a real value at the specified position
//    #method(stacksafe) write &int:anIndex &real:aValue
//    [
//        #var(type:int)aBinIndex := anIndex shift &int:(-3).
//        
//        theBinaryArray write &int:aBinIndex &real:aValue.
//    ]
//    
//    /// Reads a real value at the specified position and saves it into an output variable
//    #method(stacksafe) read &int:anIndex &vreal:aReal
//    [
//        #var(type:int)aBinIndex := anIndex shift &int:(-3).
//        
//        theBinaryArray read &int:aBinIndex &vreal:aReal.
//    ]
//
//    /// Returns a real number at the specified position
//    #method getAt : anIndex
//    [
//        #var(type:real)aRetVal.
//        $self read &int:(anIndex int) &vreal:aRetVal.
//        
//        ^ RealNumber new &real:aRetVal.
//    ]        
//
//    /// Sets a real number at the specified position
//    #method setAt : anIndex : aValue
//    [
//        $self write &int:(anIndex int) &real:(aValue real).
//    ]
//
//    /// Creates the object copy
//    #method clone = RealArray new &realarray:$self.
//    
//    /// Dispatches the verb to the target object with intarray subject
//    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &realarray:$self.
//    
//    /// Dispatches the verb to the target object with intarray subject
//    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &realarray:$self.    
//}
//
//// --- RealMatrix ---
//
//#class(sealed)RealMatrix
//{
//    #field(type:bytearray)theArray.
//    #field(type:int)theRows.
//    #field(type:int)theColumns.
//    
//    #constructor(stacksafe) new &int:Rows &int:Columns
//    [
//        theArray := ByteArray new &int:(Columns * Rows * 8).
//        
//        theRows := IntNumber new &int:Rows.
//        theColumns := IntNumber new &int:Columns.
//    ]
//        
//    #constructor new : Rows : Columns
//        <= new &int:(Columns int) &int:(Rows int).
//        
//    /// Reads the matrix lengths to the output parameter
//    #method(stacksafe) readLengths &vint:Rows &vint:Columns
//    [
//        Columns << theColumns.
//        Rows << theRows.
//    ]
//
//    #method(type:int) length = theRows.
//    
//    /// Returns itself
//    #method realmatrix = $self.
//
//    /// Writes an integer value at the specified position
//    #method(stacksafe) write &int:i &int:j &real:aValue
//    [
//        #var(type:int)anIndex := i * theColumns + j.
//        
//        anIndex := anIndex shift &int:(-3).
//        
//        theArray write &int:anIndex &real:aValue.
//    ]
//
//    /// Reads an integer value at the specified position and saves it into an output variable
//    #method(stacksafe) read &int:i &int:j &vreal:aValue
//    [
//        #var(type:int)anIndex := i * theColumns + j.
//        
//        anIndex := anIndex shift &int:(-3).
//        
//        theArray read &int:anIndex &vreal:aValue.
//    ]
//
//    #method getAt : i : j
//    [
//        #var(type:real)aValue.            
//        $self read &int:(i int) &int:(j int) &vreal:aValue.
//        
//        ^ RealNumber new &real:aValue.
//    ]
//
//    #method setAt : i : j : aValue
//    [
//        $self write &int:(i int) &int:(j int) &real:(aValue real).    
//    ]
//
//    /// Returns a row
//    #method getAt : i =
//    {
//        length = theColumns int.
//        
//        getAt : j
//            = self getAt:i:j.
//        
//        setAt : j : aValue
//        [
//            self setAt:i:j:aValue.
//        ]
//    }.
//
//    /// Dispatches the verb to the target object with intarray subject
//    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &realmatrix:$self.
//    
//    /// Dispatches the verb to the target object with intarray subject
//    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &realmatrix:$self.
//}

// --- CharArray ---

/// An array of unicode characters (UTF-32)
#class(sealed,arrayof:char,dispatchable:chararray) CharArray
{
    /// Creates the new array with a specified length
    #constructor(stacksafe) new &int : len
        = char:len.

    /// Creates the new array with a specified length
    #constructor new : aLength
        <= new &int:(aLength int).

    /// Creates the copy of a subarray
    #constructor(stacksafe) new &int:anIndex &int:aLength &chararray:anArray
        <= (new &int:aLength)
    [
        anArray read &int:anIndex &int:aLength &chararray:$self.
    ]
     
    /// Creates the short array copy
    #constructor new &chararray:anArray
        <= new &int:0 &int:(anArray length) &chararray:anArray.
    
    /// Reads the array length to the output parameter
    #method(stacksafe) readLength &vint:aLength
        => system'core_routines'__array.

    /// Fills the sub array with a specified value
    #method(stacksafe) fill &int:anIndex &int:aLength &char:aValue
    [
        #var(int)aTotalLength := $self length.
        
        #var(int)i := anIndex.
        #var(int)m := anIndex + aLength.
        
        (i > aTotalLength)
            ? [ #throw system'InvalidArgumentException new. ].
        
        (m > aTotalLength)
            ? [ #throw system'InvalidArgumentException new. ].
            
        #loop (i < m)?
        [
            $self write &int:i &char:aValue.
            
            i := i + 1.
        ].
    ]

    /// Copies a sub array to the provided one at the specified position
    #method(stacksafe) read &int:anIndex &int:aLength &chararray:anArray
        => system'core_routines'__chararray.

    /// Copies the array at the specified position
    #method(stacksafe) write &int:anIndex &int:aLength &chararray:anArray
        => system'core_routines'__chararray.

    /// Writes a character value at the specified position
    #method(stacksafe) write &int:anIndex &char:aValue
        => system'core_routines'__chararray.

    /// Reads a character value at the specified position and saves it into an output variable
    #method(stacksafe) read &int:anIndex &vchar:anInt
        => system'core_routines'__chararray.

    #method(stacksafe) move &int:anIndex &int:aLength &int:anOffset
        => system'core_routines'__chararray.

    /// Creates the object copy
    #method clone = CharArray new &chararray:$self.
}
        
// --- emptyliteralvalue ---

/// An empty literal value
#symbol(const,literal) emptyLiteralValue = "".

// --- LiteralValue ---

/// A UTF-8 literal value
#class(sealed,dispatchable:literal,string) LiteralValue :: BaseValue
{
    #field(byte)$target.
    
    /// Creates the empty literal
    #constructor min
        = emptyLiteralValue.
        
    /// Creates the literal string with specified length        
    #constructor(stacksafe) new &int:size
        = byte:(size + 1).

    /// Creates the literal string with specified length and copies the parameter value
    #constructor(stacksafe) new &int:aSize &literal:aLiteral
        <= (new &int:aSize)
    [
        system'core_routines'__literal copy &literal:$self &literal:aLiteral.
    ]

//    /// Creates the literal string with specified length fills with spciefied character value        
//    #constructor(stacksafe) new &int:aSize &char:aChar
//        => system'core_routines's_fill.

    /// Creates the literal string copy              
    #constructor new &literal:aLiteral
        <= new &int:(aLiteral length) &literal:aLiteral.
        
    /// Creates the copy of a subarray        
    #constructor(stacksafe) new &int:anIndex &int:aSize &chararray:anArray
        => system'core_routines'__literal.

    #constructor(stacksafe) $new &int:anIndex &int:aSize &bytearray:anArray
        <= (new &int:aSize)
    [
        system'core_routines'__literal copy &literal:$self &int:anIndex &int:aSize &bytearray:anArray.
    ]
    
    /// Converts to UTF-16 literal
    #method wide = wideConvertor convert &literal:$self.

    /// Compares the values
    #method(bool) equal &literal : aLiteral
        => system'core_routines'__literal.

    /// Compares the values
    #method(bool) less &literal : aLiteral
        => system'core_routines'__literal.

    /// Compares the values
    #method(bool) equal &char : aChar
        <= %equal &literal:(aChar literal).

    /// Compares the values
    #method(bool) less &char : aChar
        <= %less &literal:(aChar literal).

    /// Concatinates the values
    #method(literal) add &literal : aLiteral
        => system'core_routines'__literal.

    #method add : anObject = anObject cast:%add &to:$self.

    /// Returns the character at the specified index
    #method getAt : aParam
        <= %getAt &int:(aParam int).

    /// Returns the character at the specified index
    #method(char,stacksafe) getAt &int:anIndex
    [
        #var(char)aChar.
        $self read &int:anIndex &vchar:aChar.
        
        ^ CharValue new &char:aChar.
    ]
    
    /// Concatinates the values
    #method(literal) add &char:aChar
        = $self add &literal:(aChar literal).

    // Reads the character at the specified index and saves into an output parameter
    #method(stacksafe) read &int:anIndex &vchar:aChar
        => system'core_routines'__literal.

//    // Reads the byte at the specified index and saves into an output parameter
//    #method(stacksafe) read &int:anIndex &vbyte:anInt
//        => system'core_routines'bs_readbyte.

    /// Saves the index of the first occurence of the specified substring into an output parameter.
    ///
    /// If the substring is not found. returns -1
    #method(stacksafe) indexOf &int:anIndex &literal:aLiteral &vint:aRetVal
        => system'core_routines'__literal.

    /// Saves the index of the first occurence of the specified symbol into an output parameter.
    ///
    /// If the substring is not found. returns -1
    #method(stacksafe) indexOf &int:anIndex &char:aChar &vint:aRetVal
        => system'core_routines'__literal.

    /// Returns the index of the first occurence of the specified object.
    /// 
    /// If the object is not found. returns -1
    #method indexOf : aSearchObject &at:anIndex
        = aSearchObject cast:%eval &to:
            {
               eval:target &literal:aLiteral
                  = target indexOf &literal:aLiteral &at:anIndex.
    
               eval:target &wide:aLiteral
                  = target indexOf &literal:(aLiteral literal) &at:anIndex.
    
               eval:target &char:aChar
                  = target indexOf &char:aChar &at:anIndex.
            } &with:$self.

    /// obsolete            
    #method indexOf:anIndex:aSearchObject
        <= indexOf:aSearchObject &at:anIndex.

    /// Returns the index of the first occurence of the specified symbol.
    /// 
    /// If the substring is not found. returns -1
    #method indexOf &char:aChar &at:anIndex
    [
        #var(int)aRetVal.
        $self indexOf &int:(anIndex int) &char:aChar &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    /// Returns the index of the first occurence of the specified substring.
    ///
    /// If the substring is not found. returns -1
    #method indexOf &literal:aLiteral &at:anIndex
    [
        #var(int)aRetVal.
        $self indexOf &int:(anIndex int) &literal:aLiteral &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    /// Inserts the substring into and returns the new one
    #method(literal,stacksafe) insert &int:anIndex &literal:aLiteral
        => system'core_routines'__literal.

    /// Inserts a character into and returns the new one
    #method(literal,stacksafe) insert &int:anIndex &char:aChar
        = $self insert &int:anIndex &literal:(aChar literal).

    /// Inserts an object into and returns the new one
    #method insert : anObject &at:anIndex
        = anObject cast:%eval &to:
            {
               eval:target &literal:aLiteral
                  = target insert &int:(anIndex int) &literal:aLiteral.
    
               eval:target &wide:aLiteral
                  = target insert &int:(anIndex int) &literal:(aLiteral literal).
    
               eval:target &char:aChar
                  = target insert &int:(anIndex int) &char:aChar.
            } &with:$self.

    /// Deletes the substring from the object and returns the new one
    #method delete:aLength &at:anIndex
        = $self delete &int:(anIndex int) &int:(aLength int).

    /// Deletes the substring from the object and returns the new one
    #method(literal,stacksafe) delete &int:anIndex &int:aLength
        => system'core_routines'__literal.

    /// Returns the substring
    #method Substring : aLength &at:anIndex
        = $self Substring &int:(anIndex int) &int:(aLength int).

    /// Returns the substring
    #method(literal,stacksafe) Substring &int:anIndex &int:aLength
        => system'core_routines'__literal.

    /// Reads the literal length into an output parameter
    #method(outnprop:length,stacksafe) readLength &vint:aLength
        => system'core_routines'__literal.

    /// Saves the literal into a UTF-32 character array
    #method(stacksafe) save &int:anIndex &chararray:anArray &vint:aRetVal
        => system'core_routines'__literal.

    #method(stacksafe) $save &int:anIndex &bytearray:anArray &int:aRetVal
        => system'core_routines'__literal.

    /// Returns the string enumerator
    #method enumerator = LiteralEnumerator new &literal:$self.

//    /// Returns the string iterator
//    #method iterator = LiteralIterator new &literal:$self.
//    
    /// Clones the value
    #method clone = LiteralValue new &literal:$self.

    /// Reads the object hash code into the output parameter
    #method(stacksafe,outnprop:hash_code) readHashCode &vint:aCode 
        => system'core_routines'__literal.
}

// --- emptyWideLiteralValue ---

/// An empty wide literal value
#symbol(const,wide) emptyWideLiteralValue = ""w.

// --- WideLiteralValue ---

/// A UTF-18 literal value
#class(sealed,dispatchable:wide,string) WideLiteralValue :: BaseValue
{
    #field(short)$target.

    /// Creates the empty literal
    #constructor min
        = emptyWideLiteralValue.
        
    /// Creates the literal string with specified length
    #constructor(stacksafe) new &int:size
        = short:(size + 1).

    /// Creates the literal string with specified length and copies the parameter value
    #constructor(stacksafe) new &int:aSize &wide:aLiteral
        <= (new &int:aSize)
    [
        system'core_routines'__wide copy &wide:$self &wide:aLiteral.
    ]

//    /// Creates the literal string with specified length fills with spciefied character value
//    #constructor(stacksafe) new &int:aSize &char:aChar
//        => system'core_routines'ws_fill.

    /// Creates the literal string copy
    #constructor new &wide:aLiteral
        <= %new &int:(aLiteral length) &wide:aLiteral.
        
    /// Creates the copy of a subarray
    #constructor(stacksafe) new &int:anIndex &int:aSize &chararray:anArray
        => system'core_routines'__wide.

//    #constructor(stacksafe) $new &int:anIndex &int:aSize &shortarray:anArray
//        => system'core_routines'ws_copybuf.

    /// Converts to UTF-8 literal
    #method literal = literalConvertor convert &wide:$self.

    /// Compares the values
    #method(bool) equal &wide : aLiteral
        => system'core_routines'__wide.

    /// Compares the values
    #method(bool) less &wide : aLiteral
        => system'core_routines'__wide.

    /// Compares the values
    #method(bool) equal &char : aChar
        <= %equal &wide:(aChar wide).

    /// Compares the values
    #method(bool) less &char : aChar
        <= %less &wide:(aChar wide).

    #method add : anObject = anObject cast:%add &to:$self.

    /// Returns the character at the specified index
    #method getAt : aParam
        <= %getAt &int:(aParam int).

    /// Returns the character at the specified index
    #method(char,stacksafe) getAt &int:anIndex
    [
        #var(char)aChar.
        $self read &int:anIndex &vchar:aChar.
        
        ^ CharValue new &char:aChar.
    ]

    /// Concatinates the values
    #method(wide,stacksafe) add &wide : aLiteral
        => system'core_routines'__wide.

    /// Concatinates the values
    #method(wide,stacksafe) add &char:aChar
        = $self add &wide:(aChar wide).

    /// Concatinates the values
    #method(wide) add &literal:aLiteral
        = $self add &wide:(aLiteral wide).

    // Reads the character at the specified index and saves into an output parameter
    #method(stacksafe) read &int:anIndex &vchar:aChar
        => system'core_routines'__wide.

//    // Reads the short integer at the specified index and saves into an output parameter
//    #method(stacksafe) read &int:anIndex &vshort:aValue
//        => system'core_routines'__wide.

    /// Saves the index of the first occurence of the specified substring into an output parameter.
    ///
    /// If the substring is not found. returns -1
    #method(stacksafe) indexOf &int:anIndex &wide:aLiteral &vint:aRetVal
        => system'core_routines'__wide.

    /// Saves the index of the first occurence of the specified character into an output parameter.
    ///
    /// If the substring is not found. returns -1
    #method(stacksafe) indexOf &int:anIndex &char:aChar &vint:aRetVal
        => system'core_routines'__wide.

    /// Returns the index of the first occurence of the specified character.
    ///
    /// If the substring is not found. returns -1
    #method(stacksafe) indexOf &int:anIndex &char:aChar
    [
        #var(int)aRetVal.
        $self indexOf &int:anIndex &char:aChar &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    /// Returns the index of the first occurence of the specified substring.
    ///
    /// If the substring is not found. returns -1
    #method(stacksafe) indexOf &int:anIndex &wide:aLiteral
    [
        #var(int)aRetVal.
        $self indexOf &int:anIndex &wide:aLiteral &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    /// Returns the index of the first occurence of the specified object.
    ///
    /// If the substring is not found. returns -1
    #method indexOf : anObject &at:anIndex
        = anObject cast:%eval &to:
            {
                eval:target &literal:aLiteral = target indexOf &int:(anIndex int) &wide:(aLiteral wide).
                
                eval:target &wide:aLiteral = target indexOf &int:(anIndex int) &wide:aLiteral.
                
                eval:target &char:aChar = target indexOf &int:(anIndex int) &char:aChar.
            } &with:$self.

    /// Inserts the substring into and returns the new one
    #method(wide,stacksafe) insert &int:anIndex &wide:aLiteral
        => system'core_routines'__wide.

    /// Inserts the substring into and returns the new one
    #method(wide,stacksafe) insert &int:anIndex &literal:aLiteral
        = $self insert &int:anIndex &wide:(aLiteral wide).

    /// Inserts a character into and returns the new one
    #method(wide,stacksafe) insert &int:anIndex &char:aChar
        = $self insert &int:anIndex &wide:(aChar wide).

    /// Inserts an object into and returns the new one
    #method insert : anObject &at:anIndex
        = anObject cast:%eval &to:
            {
               eval:target &wide:aLiteral
                  = target insert &int:(anIndex int) &wide:aLiteral.
    
               eval:target &literal:aLiteral
                  = target insert &int:(anIndex int) &wide:(aLiteral wide).
    
               eval:target &char:aChar
                  = target insert &int:(anIndex int) &char:aChar.
            } &with:$self.

    /// Deletes the substring from the object and returns the new one
    #method delete:aLength &at:anIndex
        = $self delete &int:(anIndex int) &int:(aLength int).
            
    /// Deletes the substring from the object and returns the new one            
    #method(wide,stacksafe) delete &int:anIndex &int:aLength
        => system'core_routines'__wide.

    /// Returns the substring
    #method(wide,stacksafe) Substring &int:anIndex &int:aLength
        => system'core_routines'__wide.

    #method(wide) Substring : aLength &at:anIndex
        <= Substring &int:(anIndex int) &int:(aLength int).

    /// Reads the literal length into an output parameter
    #method(outnprop:length,stacksafe) readLength &vint:aLength
        => system'core_routines'__wide.

    /// Saves the literal into a UTF-32 character array
    #method(stacksafe) save &int:anIndex &chararray:anArray &vint:aLength
        => system'core_routines'__wide.

    #method(stacksafe) $save &int:anIndex &shortarray:anArray &int:aRetVal
        => system'core_routines'__wide.

    /// Returns the object enumerator
    #method enumerator = WideLiteralEnumerator new &wide:$self.
    
//    /// Returns the object iterator
//    #method iterator = WideLiteralIterator new &wide:$self.
    
    /// Clones the value
    #method clone = WideLiteralValue new &wide:$self.

    // Reads the object hash code into the output parameter
    #method(stacksafe,outnprop:hash_code) readHashCode &vint:aCode 
        => system'core_routines'__wide.
}
//
//// --- Variant ---
//
///// A basic type variant class
//#class Variant :: String
//{
//    /// Tries to convert the literal value to a byte number
//    #method byte
//        = byteConvertor convert &literal:theValue.
//
//    /// Tries to convert the literal value to a short integer number
//    #method short
//        = shortConvertor convert &literal:theValue.
//
//    /// Tries to convert the literal value to a character
//    #method char
//    [
//        #var(type:char)retVal.
//        charConvertor $convert &int:0 &literal:theValue &vchar:retVal.
//        
//        ^ CharValue new &char:retVal.
//    ]
//
//    /// Tries to convert the literal value to the integer number
//    #method int
//        = intConvertor convert &literal:theValue.
//
//    /// Tries to convert the literal value to the long integer number
//    #method long
//        = longConvertor convert &literal:theValue.
//
//    /// Tries to convert the literal value to the real number
//    #method real
//        = realConvertor convert &literal:theValue.
//
//    /// Converts the parameter into a literal string
//    #method write &int:aValue
//    [
//        theValue := aValue literal.
//    ]    
//
//    /// Converts the parameter into a literal string
//    #method write &short:aValue
//    [
//        theValue := aValue literal.
//    ]    
//
//    /// Converts the parameter into a literal string
//    #method write &byte:aValue
//    [
//        theValue := aValue literal.
//    ]    
//
//    /// Converts the parameter into a literal string
//    #method write &long:aValue
//    [
//        theValue := aValue literal.
//    ]    
//
//    /// Converts the parameter into a literal string
//    #method write &real:aValue
//    [
//        theValue := aValue literal.
//    ]    
//
//    #method cast : aVerb &to:aTarget
//    [
//        #var aValue := aTarget cast:%eval &to:
//        {
//            eval : aVariant &int:aValue
//                = aVariant int.
//                
//            eval : aVariant &byte:aValue
//                = aVariant byte.
//                
//            eval : aVariant &short:aValue
//                = aVariant short.
//                
//            eval : aVariant &long:aValue
//                = aVariant long.
//                
//            eval : aVariant &real:aValue
//                = aVariant real.
//                
//            eval : aVariant &literal:aValue
//                = aVariant literal.
//                
//            eval : aVariant &wide:aValue
//                = aVariant wide.
//        }
//        &with:$self.
//        
//        ^ aValue cast:aVerb &to:aTarget.
//    ]
//
//    #method equal : anObject
//        = anObject equal:$self bool.
//
//    /// Returns a literal value
//    #method literal = theValue.
//    
//    /// Returns a wide literal value
//    #method wide = theValue wide.
//}

// --- Array ---

/// An array
#class(sealed,arrayof:object,dispatchable:array) Array
{
    /// Creates an empty array
    #constructor min
        = object:0.
    
    /// Creates the array with the specified length
    #constructor new : aLength
        <= new &int:(aLength int).
    
    /// Creates the array with the specified length
    ///
    /// Obsolete
    #constructor new &length:aLength
        <= new &int:(aLength int).
    
    /// Creates the array with the specified length
    #constructor(stacksafe) new &int:aLength
        = object:aLength.

    /// Creates a copy of the provided array
    #constructor new &array:anArray
        <= %new &int:0 &int:(anArray length int) &array:anArray.
    
    /// Creates a copy of the provided sub array
    #constructor(stacksafe) new &int:anIndex &int:aLength &array:anArray
        <= (new &int:aLength)
    [
        system'core_routines'__array copy &array:$self &int:anIndex &int:aLength &array:anArray.
    ]
//        
//    #constructor new &object:singleItem
//        <= (new &int:1)
//    [
//        $self setAt &int:0 &object:singleItem.
//    ]

    /// Concatinates the arrays    
    #method add : anObject
        = $self add &array:(anObject array).

    /// Concatinates the arrays    
    #method add &array:anArray
    [
        #var(int)aLength1 := $self length.
        #var(int)aLength2 := anArray length.
        
        #var(array)aRetVal := Array new &int:(aLength1 + aLength2).

        #var(int)anIndex := 0.
        #loop (anIndex < aLength1) ?
        [
            aRetVal setAt &int:anIndex &object:($self getAt &int:anIndex).
            
            anIndex += 1.
        ].

        anIndex := 0.
        #loop (anIndex < aLength2) ?
        [
            aRetVal setAt &int:(aLength1 + anIndex) &object:(anArray getAt &int:anIndex).
            
            anIndex += 1.
        ].
            
        ^ aRetVal.
    ]

    /// Reads the array length into an output parameter
    #method(stacksafe) readLength &vint:aLength
        => system'core_routines'__array.

    /// Sets the specified array member
    #method(stacksafe) setAt &int:anIndex &object:anObject
        => system'core_routines'__array.

    /// Returns the specified array member
    #method(stacksafe) getAt &int:anIndex
        => system'core_routines'__array.
        
    /// Returns the array enumerator
    #method enumerator = ArrayEnumerator new &array:$self.

    /// Returns the array iterator
    #method iterator = ArrayIterator new &array:$self.

    /// Clones the array
    #method(array) clone 
        = Array new &array:$self.
}

// --- OpenArgs ---

#class(sealed,dynamic)OpenArgs
{
    #field $target.
    
    /// Returns the specified array member
    #method getAt : anIndex
        = $self getAt &int:(anIndex int).

    /// Returns the specified array member
    #method(stacksafe) getAt &int:anIndex
        = $self@anIndex.

    #method(stacksafe,outnprop:length) readLength &vint:aLength
        => system'core_routines'__args.
}

// === Enumerators / Indexers

// --- LiteralEnumerator ---

/// A literal enumerator
#class LiteralEnumerator :: Enumerator
{
    #field(literal) theLiteral.
    #field(vint)    theIndex.
    #field(vint)    theDiff.
    #field(char)    theChar.
    
    /// Creates an enumerator
    #constructor new : anObject
        <= new &literal:(anObject literal).
    
    /// Creates an enumerator
    #constructor(suppress:w3) new &literal:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := -1.
        theDiff := 1.
        theChar := #0.
    ]

    /// Resets the enumerator
    #method reset
    [
        theIndex set &int:(-1).
        theDiff set &int:1.
    ]

    /// Goes to the next enumeration member and returns true if it is not the last member
    #method next
    [
        #var(int)aLength := theLiteral length.
        
        theIndex += theDiff.
        
        (theIndex < aLength)
            ? [
                theChar := theLiteral getAt &int:theIndex.                
                
                theChar readLength &vint:theDiff.
                
                ^ true.
            ].

        theDiff set &int:0.
        theChar := #0.

        ^ false.
    ]
    
    /// Returns the current character
    #method get = theChar.
}

//// --- LiteralIterator ---
//
///// A literal value iterator
//#class LiteralIterator :: Iterator
//{
//    #field(type:literal) theLiteral.
//    #field(type:vint)    theIndex.
//    
//    /// Creates an iterator
//    #constructor new : anObject
//        <= new &literal:(anObject literal).
//    
//    /// Creates an iterator
//    #constructor new &literal:aLiteral
//    [
//        theLiteral := aLiteral.
//        theIndex := Integer new.
//    ]
//    
//    /// Returns false if the index out of range
//    #method available
//    [
//        #var(type:int)aLength.
//        theLiteral readLength &vint:aLength.
//        
//        ^ theIndex < aLength.
//    ]
//
//    /// Returns the current character
//    #method get
//    [
//        #var(type:char)aChar.
//        theLiteral read &int:theIndex &vchar:aChar.
//        
//        ^ CharValue new &char:aChar.
//    ]
//    
//    /// Sets the index by specified value
//    #method set : anObject 
//    [
//        #throw system'NotSupportedException new.
//    ]
//    
//    /// Reads the current index and saves it into an output parameter
//    #method readIndex &vint:anIndex
//    [
//        anIndex << theIndex.
//    ]
//    
//    /// Sets the index by specified value
//    #method writeIndex &int:anIndex 
//    [
//        theIndex write &int:anIndex.
//    ]
//    
//    /// Reads the length to the output parameter    
//    #method readLength &vint:anOutput
//        => theLiteral.
//}

// --- WideLiteralEnumerator ---

/// A literal enumerator
#class WideLiteralEnumerator :: Enumerator
{
    #field(wide) theLiteral.
    #field(vint) theIndex.
    #field(vint) theDiff.
    #field(char) theChar.
    
    /// Creates an enumerator
    #constructor new : anObject
        <= new &wide:(anObject wide).
    
    /// Creates an enumerator
    #constructor(suppress:w3) new &wide:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := -1.
        theDiff := 1.
        theChar := #0.
    ]

    /// Resets the enumerator
    #method reset
    [
        theIndex set &int:-1.
        theDiff set &int:1.
    ]

    /// Goes to the next enumeration member and returns true if it is not the last member
    #method next
    [
        #var(int)aLength := theLiteral length.

        theIndex += theDiff.
        
        (theIndex < aLength)
            ? [
                theChar := theLiteral getAt &int:theIndex.
                
                theChar readLength &vint:theDiff.
                theDiff += 1.
                theDiff /= 2.
                
                ^ true.
            ].

        theDiff set &int:0.
        theChar := #0.

        ^ false.
    ]
    
    /// Returns the current character
    #method get = theChar.
}

//// --- WideLiteralIterator ---
//
///// A wide literal enumerator
//#class WideLiteralIterator :: Iterator
//{
//    #field(type:wide) theLiteral.
//    #field(type:vint) theIndex.
//    
//    /// Creates an iterator
//    #constructor new : anObject
//        <= new &wide:(anObject wide).
//    
//    /// Creates an iterator
//    #constructor new &wide:aLiteral
//    [
//        theLiteral := aLiteral.
//        theIndex := Integer new.
//    ]
//
//    /// Returns false if the index out of range    
//    #method available
//    [
//        #var(type:int)aLength.
//        theLiteral readLength &vint:aLength.
//        
//        ^ theIndex < aLength.
//    ]
//
//    /// Returns the current character
//    #method get
//        = theLiteral getAt &int:theIndex.
//    
//    #method set : anObject 
//    [
//        #throw system'NotSupportedException new.
//    ]
//    
//    /// Reads the current index and saves it into an output parameter
//    #method readIndex &vint:anIndex
//    [
//        anIndex << theIndex.
//    ]
//    
//    /// Sets the index by specified value
//    #method writeIndex &int:anIndex 
//    [
//        theIndex write &int:anIndex.
//    ]
//    
//    /// Reads the length to the output parameter    
//    #method readLength &vint:anOutput
//        => theLiteral.
//}

// --- ArrayIterator ---

/// An array iterator
#class ArrayIterator :: Iterator
{
    #field(array) theArray.
    #field(vint)  theIndex.

    /// Returns the parameter iterator
    #constructor new : anArray
        <= new &array:(anArray array).
    
    /// Returns the parameter iterator
    #constructor(suppress:w3) new &array:anArray
    [
        theArray := anArray.
        theIndex := 0.
    ]
    
    /// Returns false if the index out of range
    #method available
    [
        #var(int)aLength.
        theArray readLength &vint:aLength.
        
        ^ theIndex < aLength.
    ]

    /// Returns the current member
    #method get
        = theArray getAt &int:theIndex.
    
    /// Sets the current member
    #method set : anObject
    [
        theArray setAt &int:theIndex &object:anObject.
    ]
    
    /// Reads the current index and saves it into an output parameter
    #method readIndex  &vint:anIndex
    [
        anIndex := theIndex.
    ]
    
    /// Sets the index by specified value
    #method writeIndex &int:anIndex 
    [
        theIndex set &int:anIndex.
    ]
    
    /// Reads the length to the output parameter    
    #method readLength &vint:anOutput
        => theArray.
}

// --- ArrayEnumerator ---

/// An array enumerator
#class ArrayEnumerator :: Enumerator
{
    #field(array) theArray.
    #field(vint)  theIndex.
    
    /// Creates an array enumerator
    #constructor new : anArray
        <= new &array:(anArray array).
    
    /// Creates an array enumerator
    #constructor(suppress:w3) new &array:anArray
    [
        theArray := anArray.
        theIndex := -1.
    ]

    /// Resets the enumerator
    #method reset
    [
        theIndex set &int:-1.
    ]

    /// Goes to the next enumeration member and returns true if it is not the last member
    #method next
    [
        #var(int)aLength := theArray length.
        
        theIndex += 1.

        ^ theIndex < aLength.
    ]
    
    /// Returns the current object
    #method get
        = theArray getAt &int:theIndex.
}

//// --- Func1Group ---
//
//#subject(class:system'Func1Array)func1array.

//#class Function1Group :: BaseFunction1
//{
//    #field(type:func1array) theHandlers.
//    
//    #constructor new &args:Handlers
//    [
//        #var(type:int)i.
//        #var(type:int)len.
//        Handlers readLength &vint:len.
//        
//        theHandlers := Func1Array new &int:len.
//        
//        #loop (i < len)?
//        [
//            theHandlers@i := (Handlers@i) func1.
//            
//            i := i + 1.
//        ].
//    ]
//    
//    #method eval : anObject
//    [
//        #var(type:int)i.
//        #var(type:int)len.
//        theHandlers readLength &vint:len.
//        
//        #loop (i < len)?
//        [
//            theHandlers@i eval:anObject.
//            
//            i := i + 1.
//        ].
//    ]
//}
//
//// --- BitArrayIterator ---
//
///// An array iterator
//#class BitArrayIterator :: Iterator
//{
//    #field(type:bitarray) theArray.
//    #field(type:vint)  theIndex.
//
//    /// Returns the parameter iterator
//    #constructor new : anArray
//        <= new &bitarray:(anArray bitarray).
//    
//    /// Returns the parameter iterator
//    #constructor new &bitarray:anArray
//    [
//        theArray := anArray.
//        theIndex := Integer new.
//    ]
//    
//    /// Returns false if the index out of range
//    #method available
//    [
//        #var(type:int)aLength.
//        theArray readLength &vint:aLength.
//        
//        ^ theIndex < aLength.
//    ]
//
//    /// Returns the current member
//    #method(type:bool) get
//        = theArray getAt &int:theIndex.
//    
//    /// Sets the current member
//    #method set : anObject
//    [
//        theArray setAt &int:theIndex &bool:(anObject bool).
//    ]
//    
//    /// Reads the current index and saves it into an output parameter
//    #method readIndex  &vint:anIndex
//    [
//        anIndex << theIndex.
//    ]
//    
//    /// Sets the index by specified value
//    #method writeIndex &int:anIndex 
//    [
//        theIndex write &int:anIndex.
//    ]
//    
//    /// Reads the length to the output parameter    
//    #method readLength &vint:anOutput
//        => theArray.
//}
//
//// --- BitArrayEnumerator ---
//
///// An array enumerator
//#class BitArrayEnumerator :: Enumerator
//{
//    #field(type:bitarray) theArray.
//    #field(type:vint)  theIndex.
//    
//    /// Creates an array enumerator
//    #constructor new : anArray
//        <= new &bitarray:(anArray bitarray).
//    
//    /// Creates an array enumerator
//    #constructor new &bitarray:anArray
//    [
//        theArray := anArray.
//        theIndex := Integer new &int:(-1).
//    ]
//
//    /// Resets the enumerator
//    #method reset
//    [
//        theIndex write &int:-1.
//    ]
//
//    /// Goes to the next enumeration member and returns true if it is not the last member
//    #method next
//    [
//        #var(type:int)aLength.
//        theArray readLength &vint:aLength.
//        
//        theIndex append &int:1.
//
//        ^ theIndex < aLength.
//    ]
//    
//    /// Returns the current object
//    #method(type:bool) get
//        = theArray getAt &int:theIndex.
//}
//
//// --- BitArray32Iterator ---
//
///// An array iterator
//#class BitArray32Iterator :: Iterator
//{
//    #field(type:bitset) theArray.
//    #field(type:vint)  theIndex.
//
//    /// Returns the parameter iterator
//    #constructor new : anArray
//        <= new &bitset:(anArray bitset).
//    
//    /// Returns the parameter iterator
//    #constructor new &bitset:anArray
//    [
//        theArray := anArray.
//        theIndex := Integer new.
//    ]
//    
//    /// Returns false if the index out of range
//    #method available
//    [
//        ^ theIndex < 32.
//    ]
//
//    /// Returns the current member
//    #method(type:bool) get
//        = theArray getAt &int:theIndex.
//    
//    /// Sets the current member
//    #method set : anObject
//    [
//        theArray setAt &int:theIndex &bool:(anObject bool).
//    ]
//    
//    /// Reads the current index and saves it into an output parameter
//    #method readIndex  &vint:anIndex
//    [
//        anIndex << theIndex.
//    ]
//    
//    /// Sets the index by specified value
//    #method writeIndex &int:anIndex 
//    [
//        theIndex write &int:anIndex.
//    ]
//    
//    /// Reads the length to the output parameter    
//    #method readLength &vint:anOutput
//        => theArray.
//}
//
//// --- BitArray32Enumerator ---
//
///// An array enumerator
//#class BitArray32Enumerator :: Enumerator
//{
//    #field(type:bitset) theArray.
//    #field(type:vint)  theIndex.
//    
//    /// Creates an array enumerator
//    #constructor new : anArray
//        <= new &bitset:(anArray bitset).
//    
//    /// Creates an array enumerator
//    #constructor new &bitset:anArray
//    [
//        theArray := anArray.
//        theIndex := Integer new &int:(-1).
//    ]
//
//    /// Resets the enumerator
//    #method reset
//    [
//        theIndex write &int:-1.
//    ]
//
//    /// Goes to the next enumeration member and returns true if it is not the last member
//    #method next
//    [
//        theIndex append &int:1.
//
//        ^ theIndex < 32.
//    ]
//    
//    /// Returns the current object
//    #method get
//        = theArray getAt &int:theIndex.
//}
