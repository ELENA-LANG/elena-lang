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

// --- BitArray ---

#class(sealed,dispatchable:bitarray) BitArray
{
    #field(intarray) theArray.
    #field(int)      theLength.
    
    /// Creates the array with the specified length
    #constructor(stacksafe,suppress:w3) new &int:length
    [
        theLength := length.
        theArray := IntArray new &int:(length / 32 + 1).
    ]
    
    #method(stacksafe,outnprop:length)readLength &vint:retVal
    [
        retVal := theLength.
    ]
    
    #method getAt:index
        = $self getAt &int:(index int).
    
    #method(bool,stacksafe)getAt &int:index
    [
        #var(int)arr_index := index / 32.
        #var(int)bit_index := arr_index * 32.
        bit_index := index - arr_index.
        
        #var(int)bit_mask := 1.
        (bit_index != 0)
            ? [ bit_mask := bit_mask << bit_index. ].
        
        #var(int)bits.
        theArray read &int:arr_index &vint:bits.
        
        ^ (bits && bit_mask) != 0.
    ]
    
    #method setAt:index:value
    [
        $self setAt &int:(index int) &bool:(value bool).
    ]
    
    #method(stacksafe) setAt &int:index &bool:value
    [
        #var(int)arr_index := index / 32.
        #var(int)bit_index := arr_index * 32.
        bit_index := index - arr_index.
        
        #var(int)bit_mask := 1.
        (bit_index != 0)
            ? [ bit_mask := bit_mask << bit_index. ].
        
        #var(int)bits.
        theArray read &int:arr_index &vint:bits.
        
        value
            ? [ bits := bits || bit_mask. ]
            ! [  
                bit_mask := bit_mask inverted.
                
                bits := bits && bit_mask.
            ].
            
        theArray write &int:arr_index &int:bits.            
    ]
    
    /// Returns the array enumerator
    #method enumerator = BitArrayEnumerator new &bitarray:$self.

    /// Returns the array iterator
    #method iterator = BitArrayIterator new &bitarray:$self.    
}

// --- BitArray32 ---

#class(sealed,struct:embeddable,dispatchable:bitset) BitArray32
{
    #field(int)theValue.
    
    #constructor new &int:value
    [
        theValue := value.
    ]
    
    #constructor new:value
        <= new &int:(value int).
    
    #method(int) length = 32.
    
    #method write &int:value
    [
        theValue := value.
    ]
    
    #method write : value
    [
        value cast:%write &to:$self.
    ]
    
    #method getAt:index
        = $self getAt &int:(index int).
    
    #method(bool,stacksafe)getAt &int:index
    [
        #var(int)bit_mask := 1.
        (index != 0)
            ? [ bit_mask := bit_mask << index. ].
        
        ^ (theValue && bit_mask) != 0.
    ]
    
    #method setAt:index:value
    [
        $self setAt &int:(index int) &bool:(value bool).
    ]
    
    #method(stacksafe) setAt &int:index &bool:value
    [
        #var(int)bit_mask := 1.
        (index != 0)
            ? [ bit_mask := bit_mask << index. ].
        
        value
            ? [ theValue := theValue || bit_mask. ]
            ! [  
                bit_mask := bit_mask inverted.
                
                theValue := theValue && bit_mask.
            ].
    ]
    
    /// Returns the array enumerator
    #method enumerator = BitArray32Enumerator new &bitset:$self.

    /// Returns the array iterator
    #method iterator = BitArray32Iterator new &bitset:$self.
}

// --- IntMatrix ---

#class(sealed,dispatchable:intmatrix)IntMatrix
{
    #field(intarray)theArray.
    #field(int)theRows.
    #field(int)theColumns.
    
    #constructor new &int:Columns &int:Rows
    [
        theRows := Rows.
        theColumns := Columns.
        
        theArray := IntArray new &int:(Columns * Rows).        
    ]
        
    #constructor new : Columns : Rows
        <= new &int:(Columns int) &int:(Rows int).
        
    /// Reads the matrix lengths to the output parameter
    #method(stacksafe) readLengths &vint:Columns &vint:Rows
    [
        Columns := theColumns.
        Rows := theRows.
    ]

    #method(int) length = theRows.

    /// Writes an integer value at the specified position
    #method(stacksafe) write &int:i &int:j &int:aValue
    [
        theArray write &int:(i * theColumns + j) &int:aValue.
    ]

    /// Reads an integer value at the specified position and saves it into an output variable
    #method(stacksafe) read &int:i &int:j &vint:anInt
    [
        theArray read &int:(i * theColumns + j) &vint:anInt.
    ]

    #method getAt : i : j
    [
        #var(int)aValue.            
        $self read &int:(i int) &int:(j int) &vint:aValue.
        
        ^ IntNumber new &int:aValue.
    ]

    #method setAt : i : j : aValue
    [
        $self write &int:(i int) &int:(j int) &int:(aValue int).    
    ]

    /// Returns a row
    #method getAt : i =
    {
        length = theColumns int.
        
        getAt : j
            = self getAt:i:j.
        
        setAt : j : aValue
        [
            self setAt:i:j:aValue.
        ]
    }.
}

// --- RealArray ---

/// An array of real numbers
#class(sealed,arrayof:real,dispatchable:realarray) RealArray
{
    /// Creates the array with the specified length
    #constructor(stacksafe) new &int:aLength
        = real:aLength.

    /// Creates the array with the specified length
    #constructor new : aLength
        <= new &int:(aLength int).

    /// Creates the copy of a subarray
    #constructor(stacksafe) new &int:anIndex &int:aLength &realarray:anArray
        <= (new &int:aLength)
    [
        anArray read &int:anIndex &int:aLength &realarray:$self.
    ]

    /// Creates the array copy
    #constructor new &realarray:anArray
        <= new &int:0 &int:(anArray length) &realarray:anArray.

    /// Fills the sub array with a specified value
    #method(stacksafe) fill &int:anIndex &int:aLength &real:aValue
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
            $self setAt &int:i &real:aValue.
            
            i := i + 1.
        ].
    ]

    /// Copies a sub array to the provided one at the specified position
    #method(stacksafe) read &int:anIndex &int:aLength &realarray:anArray
        => system'core_routines'__realarray.

    /// Copies the short array at the specified position
    #method(stacksafe) write &int:anIndex &int:aLength &realarray:anArray
        => system'core_routines'__realarray.

    /// Reads a real value at the specified position and saves it into an output variable
    #method(stacksafe) read &int:anIndex &vreal:aReal
        => system'core_routines'__realarray.

    /// Creates the object copy
    #method clone = RealArray new &realarray:$self.
}

// --- RealMatrix ---

#class(sealed,dispatchable:realmatrix)RealMatrix
{
    #field(realarray)theArray.
    #field(int)theRows.
    #field(int)theColumns.
    
    #constructor(stacksafe) new &int:Rows &int:Columns
    [
        theArray := RealArray new &int:(Columns * Rows).
        
        theRows := IntNumber new &int:Rows.
        theColumns := IntNumber new &int:Columns.
    ]
        
    #constructor new : Rows : Columns
        <= new &int:(Columns int) &int:(Rows int).

    /// Reads the matrix lengths to the output parameter
    #method(stacksafe) readLengths &vint:Rows &vint:Columns
    [
        Columns := theColumns.
        Rows := theRows.
    ]

    #method(int) length = theRows.

    /// Writes an integer value at the specified position
    #method(stacksafe) write &int:i &int:j &real:aValue
    [
        #var(int)anIndex := i * theColumns + j.
        
        theArray setAt &int:anIndex &real:aValue.
    ]

    /// Reads an integer value at the specified position and saves it into an output variable
    #method(stacksafe) read &int:i &int:j &vreal:aValue
    [
        #var(int)anIndex := i * theColumns + j.
        
        theArray read &int:anIndex &vreal:aValue.
    ]

    #method getAt : i : j
    [
        #var(real)aValue.            
        $self read &int:(i int) &int:(j int) &vreal:aValue.
        
        ^ RealNumber new &real:aValue.
    ]

    #method setAt : i : j : aValue
    [
        $self write &int:(i int) &int:(j int) &real:(aValue real).    
    ]

    /// Returns a row
    #method getAt : i =
    {
        length = theColumns.
        
        getAt : j
            = self getAt:i:j.
        
        setAt : j : aValue
        [
            self setAt:i:j:aValue.
        ]
    }.
}

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
        => system'core_routines'__intarray.

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

// --- Variant ---

/// A basic type variant class
#class Variant
{
    #field theValue.
    
    #constructor new : object
    [
        theValue := object.
    ]
    
    /// Tries to convert the value to a byte number
    #method byte
        = byteConvertor convert:theValue.

    /// Tries to convert the value to a short integer number
    #method short
        = shortConvertor convert:theValue.

    /// Tries to convert the value to a character
    #method char
        = charConvertor convert:theValue.

    /// Tries to convert the literal value to the integer number
    #method int
        = intConvertor convert:theValue.

    /// Tries to convert the literal value to the long integer number
    #method long
        = longConvertor convert:theValue.

    /// Tries to convert the literal value to the real number
    #method real
        = realConvertor convert:theValue.

    /// Returns a literal value
    #method literal 
        = literalConvertor convert:theValue.
    
    /// Returns a wide literal value
    #method wide 
        = wideConvertor convert:theValue.

    /// Converts the parameter into a literal string
    #method set &int:aValue
    [
        theValue := aValue.
    ]    

    /// Converts the parameter into a literal string
    #method set &short:aValue
    [
        theValue := aValue.
    ]    

    /// Converts the parameter into a literal string
    #method write &byte:aValue
    [
        theValue := aValue.
    ]    

    /// Converts the parameter into a literal string
    #method write &long:aValue
    [
        theValue := aValue.
    ]    

    /// Converts the parameter into a literal string
    #method write &real:aValue
    [
        theValue := aValue.
    ]    

    #method cast : aVerb &to:aTarget
    [
        #var aValue := aTarget cast:%eval &to:
        {
            eval : aVariant &int:aValue
                = aVariant int.
                
            eval : aVariant &byte:aValue
                = aVariant byte.
                
            eval : aVariant &short:aValue
                = aVariant short.
                
            eval : aVariant &long:aValue
                = aVariant long.
                
            eval : aVariant &real:aValue
                = aVariant real.
                
            eval : aVariant &literal:aValue
                = aVariant literal.
                
            eval : aVariant &wide:aValue
                = aVariant wide.
        }
        &with:$self.
        
        ^ aValue cast:aVerb &to:aTarget.
    ]

    #method equal : anObject
        = anObject equal:$self bool.
}

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
        
    #constructor new &object:singleItem
        <= (new &int:1)
    [
        $self setAt &int:0 &object:singleItem.
    ]

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

// --- LiteralIterator ---

/// A literal value iterator
#class LiteralIterator :: Iterator
{
    #field(literal) theLiteral.
    #field(vint)    theIndex.
    
    /// Creates an iterator
    #constructor new : anObject
        <= new &literal:(anObject literal).
    
    /// Creates an iterator
    #constructor(suppress:w3) new &literal:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := 0.
    ]
    
    /// Returns false if the index out of range
    #method available
    [
        #var(int)aLength := theLiteral length.
        
        ^ theIndex < aLength.
    ]

    /// Returns the current character
    #method get
    [
        #var(char)aChar.
        theLiteral read &int:theIndex &vchar:aChar.
        
        ^ CharValue new &char:aChar.
    ]
    
    /// Sets the index by specified value
    #method set : anObject 
    [
        #throw system'NotSupportedException new.
    ]
    
    /// Reads the current index and saves it into an output parameter
    #method readIndex &vint:anIndex
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
        => theLiteral.
}

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

// --- WideLiteralIterator ---

/// A wide literal enumerator
#class WideLiteralIterator :: Iterator
{
    #field(wide) theLiteral.
    #field(vint) theIndex.
    
    /// Creates an iterator
    #constructor new : anObject
        <= new &wide:(anObject wide).
    
    /// Creates an iterator
    #constructor(suppress:w3) new &wide:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := 0.
    ]

    /// Returns false if the index out of range    
    #method available
    [
        #var(int)aLength := theLiteral length.
        
        ^ theIndex < aLength.
    ]

    /// Returns the current character
    #method get
        = theLiteral getAt &int:theIndex.
    
    #method set : anObject 
    [
        #throw system'NotSupportedException new.
    ]
    
    /// Reads the current index and saves it into an output parameter
    #method readIndex &vint:anIndex
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
        => theLiteral.
}

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

// --- Func1Group ---

#subject func1array = arrayof:func1.

#class Function1Group :: BaseFunction1
{
    #field(func1array) theHandlers.
    
    #constructor new &args:Handlers
    [
        #var(int)i.
        #var(int)len := Handlers length.
        
        theHandlers := func1:len.
        
        #loop (i < len)?
        [
            theHandlers setAt &int:i &func1:((Handlers@i) func1).
            
            i += 1.
        ].
    ]
    
    #method eval : anObject
    [
        #var(int)i.
        #var(int)len := theHandlers length.
        
        #loop (i < len)?
        [
            theHandlers getAt &int:i eval:anObject.
            
            i += 1.
        ].
    ]
}

// --- BitArrayIterator ---

/// An array iterator
#class BitArrayIterator :: Iterator
{
    #field(bitarray) theArray.
    #field(vint)  theIndex.

    /// Returns the parameter iterator
    #constructor new : anArray
        <= new &bitarray:(anArray bitarray).
    
    /// Returns the parameter iterator
    #constructor(suppress:w3) new &bitarray:anArray
    [
        theArray := anArray.
        theIndex := 0.
    ]
    
    /// Returns false if the index out of range
    #method available
    [
        #var(int)aLength := theArray length.
        
        ^ theIndex < aLength.
    ]

    /// Returns the current member
    #method(bool) get
        = theArray getAt &int:theIndex.
    
    /// Sets the current member
    #method set : anObject
    [
        theArray setAt &int:theIndex &bool:(anObject bool).
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

// --- BitArrayEnumerator ---

/// An array enumerator
#class BitArrayEnumerator :: Enumerator
{
    #field(bitarray) theArray.
    #field(vint)  theIndex.
    
    /// Creates an array enumerator
    #constructor new : anArray
        <= new &bitarray:(anArray bitarray).
    
    /// Creates an array enumerator
    #constructor(suppress:w3) new &bitarray:anArray
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
        
        theIndex append &int:1.

        ^ theIndex < aLength.
    ]
    
    /// Returns the current object
    #method(bool) get
        = theArray getAt &int:theIndex.
}

// --- BitArray32Iterator ---

/// An array iterator
#class BitArray32Iterator :: Iterator
{
    #field(bitset) theArray.
    #field(vint)  theIndex.

    /// Returns the parameter iterator
    #constructor new : anArray
        <= new &bitset:(anArray bitset).
    
    /// Returns the parameter iterator
    #constructor(suppress:w3) new &bitset:anArray
    [
        theArray := anArray.
        theIndex := 0.
    ]
    
    /// Returns false if the index out of range
    #method available
    [
        ^ theIndex < 32.
    ]

    /// Returns the current member
    #method(bool) get
        = theArray getAt &int:theIndex.
    
    /// Sets the current member
    #method set : anObject
    [
        theArray setAt &int:theIndex &bool:(anObject bool).
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

// --- BitArray32Enumerator ---

/// An array enumerator
#class BitArray32Enumerator :: Enumerator
{
    #field(bitset) theArray.
    #field(vint)  theIndex.
    
    /// Creates an array enumerator
    #constructor new : anArray
        <= new &bitset:(anArray bitset).
    
    /// Creates an array enumerator
    #constructor(suppress:w3) new &bitset:anArray
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
        theIndex append &int:1.

        ^ theIndex < 32.
    ]
    
    /// Returns the current object
    #method get
        = theArray getAt &int:theIndex.
}
