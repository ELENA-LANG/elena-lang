// ==== ver 3.4.1 ===

// === basic memory classes ===

// --- BaseByteArray ---

/// A base byte array
#class(stringof:1) BaseByteArray
{
    /// Creates the byte array with the specified length
    #constructor(stacksafe) new &int:aLength
        => system'internal'bs_create.
}

// --- ByteArray ---

/// A byte array
#class(sealed) ByteArray :: BaseByteArray
{
    /// Creates the byte array with the specified length
    #constructor(stacksafe) new &int:aLength
        => system'internal'bs_create.

    /// Creates the byte array with the specified length
    #constructor new : aLength
        <= new &int:(aLength int).
    
    /// Creates the copy of a byte subarray
    #constructor(stacksafe) new &int:anIndex &int:aLength &bytearray:anArray
        => system'internal'bs_copysubarr.
    
    /// Creates the byte array copy
    #constructor(stacksafe) new &bytearray:anArray
        <= new &int:0 &int:(anArray length int) &bytearray:anArray.
    
    /// Returns itself
    #method bytearray = $self.

    /// Reads the array length to the output parameter
    #method(stacksafe) readLength &vint:aLength
        => system'internal'bs_getlen.
        
    /// Returns the byte array length        
    #method(stacksafe,type:int,embeddable,suppress:w3) length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ aLength.
    ]

    /// Fills the sub array with a specified value
    #method(stacksafe) fill &int:anIndex &int:aLength &byte:aValue
    [
        #var(type:int)aTotalLength.
        $self readLength &vint:aTotalLength.
        
        #var(type:int)i := anIndex.
        #var(type:int)m := anIndex + aLength.
        
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
        => system'internal'bs_read.

    /// Copies the byte array at the specified position
    #method(stacksafe) write &int:anIndex &int:aLength &bytearray:aDump
        => system'internal'bs_write.

    /// Writes the byte value at specified position
    #method(stacksafe) write &int:anIndex &byte:anInt
        => system'internal'bs_writebyte.

    /// Reads a byte value at the specified position and saves it into an output variable
    #method(stacksafe) read &int:anIndex &vbyte:anInt
        => system'internal'bs_readbyte.

    /// Writes a short integer value at the specified position
    #method(stacksafe) write &int:anIndex &short:anInt
        => system'internal'bs_writeword.

    /// Reads a short integer value at the specified position and saves it into an output variable
    #method(stacksafe) read &int:anIndex &vshort:aChar
        => system'internal'bs_readword.

    /// Writes a character value at the specified position
    #method(stacksafe) write &int:anIndex &char:anInt
        => system'internal'bs_writedword.

    /// Reads a character value at the specified position and saves it into an output variable
    #method(stacksafe) read &int:anIndex &vchar:aChar
        => system'internal'bs_readdword.

    /// Writes an integer value at the specified position
    #method(stacksafe) write &int:anIndex &int:anInt
        => system'internal'bs_writedword.

    /// Reads an integer value at the specified position and saves it into an output variable
    #method(stacksafe) read &int:anIndex &vint:anInt
        => system'internal'bs_readdword.

    /// Writes a long integer value at the specified position
    #method(stacksafe) write &int:anIndex &long:aLong
        => system'internal'bs_writeqword.

    /// Reads a long integer value at the specified position and saves it into an output variable
    #method(stacksafe) read &int:anIndex &vlong:aLong
        => system'internal'bs_readqword.

    /// Writes a real value at the specified position
    #method(stacksafe) write &int:anIndex &real:aLong
        => system'internal'bs_writeqword.

    /// Reads a real value at the specified position and saves it into an output variable
    #method(stacksafe) read &int:anIndex &vreal:aLong
        => system'internal'bs_readqword.

    /// Returns a byte at the specified position
    #method getAt : anIndex
    [
        #var(type:byte)aRetVal.
        $self read &int:(anIndex int) &vbyte:aRetVal.
        
        ^ ByteNumber new &byte:aRetVal.
    ]        

    /// Sets a byte at the specified position
    #method setAt : anIndex : aValue
    [
        $self write &int:(anIndex int) &byte:(aValue byte).
    ]
    
    /// Concatinates the arrays    
    #method(stacksafe,type:bytearray) add &int:aLength2 &bytearray:anArray
    [
        #var(type:int)aLength1 := $self length.
        
        #var(type:bytearray)aRetVal := ByteArray new &int:(aLength1 + aLength2).

        #var(type:byte)tmp.
        #var(type:int)anIndex := 0.
        #loop (anIndex < aLength1) ?
        [
            $self read &int:anIndex &vbyte:tmp.
            aRetVal write &int:anIndex &byte:tmp.
            
            anIndex := anIndex + 1.
        ].

        anIndex := 0.
        #loop (anIndex < aLength2) ?
        [
            anArray read &int:anIndex &vbyte:tmp.
            aRetVal write &int:(aLength1 + anIndex) &byte:tmp.
            
            anIndex := anIndex + 1.
        ].
            
        ^ aRetVal.
    ]    
                        
    /// Creates the object copy
    #method clone = ByteArray new &bytearray:$self.
    
    /// Dispatches the verb to the target object with bytearray subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &bytearray:$self.
    
    /// Dispatches the verb to the target object with bytearray subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &bytearray:$self.
}

// --- ShortArray ---

/// A short integer array
#class(sealed,stringof:2) ShortArray
{
    /// Creates the short array with the specified length
    #constructor(stacksafe) new &int:aLength
        => system'internal'sa_create.
    
    /// Creates the short array with the specified length
    #constructor new : aLength
        <= new &int:(aLength int).
    
    /// Creates the copy of a short subarray
    #constructor(stacksafe) new &int:anIndex &int:aLength &shortarray:anArray
        => system'internal'sa_copysubarr.

    /// Creates the short array copy
    #constructor new &shortarray:anArray
        <= new &int:0 &int:(anArray length int) &shortarray:anArray.
    
    /// Returns itself
    #method shortarray = $self.

    /// Reads the array length to the output parameter
    #method(stacksafe) readLength &vint:aLength
        => system'internal'sa_getlen.

    /// Returns the array length        
    #method(stacksafe,type:int,embeddable,suppress:w3) length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ aLength.
    ]

    /// Fills the sub array with a specified value
    #method(stacksafe) fill &int:anIndex &int:aLength &short:aValue
    [
        #var(type:int)aTotalLength := $self length.
        
        #var(type:int)i := anIndex.
        #var(type:int)m := anIndex + aLength.
        
        (i > aTotalLength)
            ? [ #throw system'InvalidArgumentException new. ].
        
        (m > aTotalLength)
            ? [ #throw system'InvalidArgumentException new. ].
            
        #loop (i < m)?
        [
            $self write &int:i &short:aValue.
            
            i := i + 1.
        ].
    ]
    
    /// Copies a sub array to the provided one at the specified position
    #method(stacksafe) read &int:anIndex &int:aLength &shortarray:anArray
        => system'internal'sa_read.

    /// Copies the short array at the specified position   
    #method(stacksafe) write &int:anIndex &int:aLength &shortarray:anArray
        => system'internal'sa_write.

    /// Writes a short integer value at the specified position
    #method(stacksafe) write &int:anIndex &short:aValue
        => system'internal'sa_writeword.

    /// Reads a short integer value at the specified position and saves it into an output variable
    #method(stacksafe) read &int:anIndex &vshort:aValue
        => system'internal'sa_readword.

    /// Returns a short integer at the specified position
    #method getAt : anIndex
    [
        #var(type:short)aRetVal.
        $self read &int:(anIndex int) &vshort:aRetVal.
        
        ^ ShortNumber new &short:aRetVal.
    ]        

    /// Sets a short integer at the specified position
    #method setAt : anIndex : aValue
    [
        $self write &int:(anIndex int) &short:(aValue short).
    ]

    /// Creates the object copy
    #method clone = ShortArray new &shortarray:$self.
    
    /// Dispatches the verb to the target object with shortarray subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &shortarray:$self.
    
    /// Dispatches the verb to the target object with shortarray subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &shortarray:$self.
}

// --- BaseIntArray ---

/// A base integer array
#class(stringof:4) BaseIntArray
{
    /// Creates the integer array with the specified length
    #constructor(stacksafe) new &int : aLength
        => system'internal'ns_create.
}

// --- IntArray ---

/// An array of integers
#class(sealed) IntArray :: BaseIntArray
{
    /// Creates the array with the specified length
    #constructor(stacksafe) new &int : aLength
        => system'internal'ns_create.

    /// Creates the array with the specified length
    #constructor new : aLength
        <= new &int:(aLength int).

    /// Creates the copy of a subarray
    #constructor(stacksafe) new &int:anIndex &int:aLength &intarray:anArray
        => system'internal'ns_copysubarr.
        
    /// Creates the short array copy
    #constructor new &intarray:anArray
        <= new &int:0 &int:(anArray length int) &intarray:anArray.
    
    /// Reads the array length to the output parameter
    #method(stacksafe) readLength &vint:aLength
        => system'internal'ns_getlen.

    /// Returns the array length
    #method(stacksafe,type:int,embeddable,suppress:w3) length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ aLength.
    ]
    
    /// Returns itself
    #method intarray = $self.

    /// Fills the sub array with a specified value
    #method(stacksafe) fill &int:anIndex &int:aLength &int:aValue
    [
        #var(type:int)aTotalLength := $self length.
        
        #var(type:int)i := anIndex.
        #var(type:int)m := anIndex + aLength.
        
        (i > aTotalLength)
            ? [ #throw system'InvalidArgumentException new. ].
        
        (m > aTotalLength)
            ? [ #throw system'InvalidArgumentException new. ].
            
        #loop (i < m)?
        [
            $self write &int:i &int:aValue.
            
            i := i + 1.
        ].
    ]

    /// Copies a sub array to the provided one at the specified position
    #method(stacksafe) read &int:anIndex &int:aLength &intarray:anArray
        => system'internal'ns_read.

    /// Copies the short array at the specified position
    #method(stacksafe) write &int:anIndex &int:aLength &intarray:anArray
        => system'internal'ns_write.

    /// Writes an integer value at the specified position
    #method(stacksafe) write &int:anIndex &int:aValue
        => system'internal'ns_writedword.

    /// Reads an integer value at the specified position and saves it into an output variable
    #method(stacksafe) read &int:anIndex &vint:anInt
        => system'internal'ns_readdword.

    /// Returns an integer at the specified position
    #method getAt : anIndex
    [
        #var(type:int)aRetVal.
        $self read &int:(anIndex int) &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]        

    /// Sets an integer at the specified position
    #method setAt : anIndex : aValue
    [
        $self write &int:(anIndex int) &int:(aValue int).
    ]

    /// Creates the object copy
    #method clone = IntArray new &intarray:$self.
    
    /// Dispatches the verb to the target object with intarray subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &intarray:$self.
    
    /// Dispatches the verb to the target object with intarray subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &intarray:$self.    
}

// --- IntMatrix ---

#class(sealed)IntMatrix
{
    #field(type:intarray)theArray.
    #field(type:int)theRows.
    #field(type:int)theColumns.
    
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
        Columns << theColumns.
        Rows << theRows.
    ]

    #method(type:int) length = theRows.
    
    /// Returns itself
    #method intmatrix = $self.

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
        #var(type:int)aValue.            
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

    /// Dispatches the verb to the target object with intarray subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &intmatrix:$self.
    
    /// Dispatches the verb to the target object with intarray subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &intmatrix:$self.
}

// --- RealArray ---

/// An array of real numbers
#class(sealed,nonstructural) RealArray
{
    #field(type:bytearray)theBinaryArray.
    
    /// Creates the array with the specified length
    #constructor(stacksafe) new &int:aLength
    [
        theBinaryArray := ByteArray new &int:(aLength * 8).
    ]

    /// Creates the array with the specified length
    #constructor new : aLength
        <= new &int:(aLength int).

    /// Creates the copy of a subarray
    #constructor(stacksafe) new &int:anIndex &int:aLength &realarray:anArray
    [
        theBinaryArray := ByteArray new &int:(aLength * 8).
        
        theBinaryArray write &int:anIndex &int:aLength &bytearray:(anArray bytearray).
    ]
    
    #method bytearray = theBinaryArray.
    
    /// Creates the array copy
    #constructor new &realarray:anArray
        <= new &int:0 &int:(anArray length int) &realarray:anArray.
    
    /// Reads the array length to the output parameter
    #method(stacksafe) readLength &vint:aLength
    [
        #var(type:int)aBinLen.
        theBinaryArray readLength &vint:aBinLen.
        
        aBinLen shift &int:3.
        
        aLength << aBinLen.
    ]

    /// Returns the array length
    #method(stacksafe,type:int,embeddable,suppress:w3) length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ aLength.
    ]
    
    /// Returns itself
    #method realarray = $self.

    /// Fills the sub array with a specified value
    #method(stacksafe) fill &int:anIndex &int:aLength &real:aValue
    [
        #var(type:int)aTotalLength := $self length.
        
        #var(type:int)i := anIndex.
        #var(type:int)m := anIndex + aLength.
        
        (i > aTotalLength)
            ? [ #throw system'InvalidArgumentException new. ].
        
        (m > aTotalLength)
            ? [ #throw system'InvalidArgumentException new. ].
            
        #loop (i < m)?
        [
            $self write &int:i &real:aValue.
            
            i := i + 1.
        ].
    ]

    /// Copies a sub array to the provided one at the specified position
    #method(stacksafe) read &int:anIndex &int:aLength &realarray:anArray
    [
        theBinaryArray read &int:(anIndex * 8) &int:(aLength * 8) &bytearray:(anArray bytearray).
    ]
    
    /// Copies the short array at the specified position
    #method(stacksafe) write &int:anIndex &int:aLength &realarray:anArray
    [
        theBinaryArray write &int:(anIndex * 8) &int:(aLength * 8) &bytearray:(anArray bytearray).
    ]

    /// Writes a real value at the specified position
    #method(stacksafe) write &int:anIndex &real:aValue
    [
        #var(type:int)aBinIndex := anIndex shift &int:(-3).
        
        theBinaryArray write &int:aBinIndex &real:aValue.
    ]
    
    /// Reads a real value at the specified position and saves it into an output variable
    #method(stacksafe) read &int:anIndex &vreal:aReal
    [
        #var(type:int)aBinIndex := anIndex shift &int:(-3).
        
        theBinaryArray read &int:aBinIndex &vreal:aReal.
    ]

    /// Returns a real number at the specified position
    #method getAt : anIndex
    [
        #var(type:real)aRetVal.
        $self read &int:(anIndex int) &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]        

    /// Sets a real number at the specified position
    #method setAt : anIndex : aValue
    [
        $self write &int:(anIndex int) &real:(aValue real).
    ]

    /// Creates the object copy
    #method clone = RealArray new &realarray:$self.
    
    /// Dispatches the verb to the target object with intarray subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &realarray:$self.
    
    /// Dispatches the verb to the target object with intarray subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &realarray:$self.    
}

// --- RealMatrix ---

#class(sealed)RealMatrix
{
    #field(type:bytearray)theArray.
    #field(type:int)theRows.
    #field(type:int)theColumns.
    
    #constructor(stacksafe) new &int:Rows &int:Columns
    [
        theArray := ByteArray new &int:(Columns * Rows * 8).
        
        theRows := IntNumber new &int:Rows.
        theColumns := IntNumber new &int:Columns.
    ]
        
    #constructor new : Rows : Columns
        <= new &int:(Columns int) &int:(Rows int).
        
    /// Reads the matrix lengths to the output parameter
    #method(stacksafe) readLengths &vint:Rows &vint:Columns
    [
        Columns << theColumns.
        Rows << theRows.
    ]

    #method(type:int) length = theRows.
    
    /// Returns itself
    #method realmatrix = $self.

    /// Writes an integer value at the specified position
    #method(stacksafe) write &int:i &int:j &real:aValue
    [
        #var(type:int)anIndex := i * theColumns + j.
        
        anIndex := anIndex shift &int:(-3).
        
        theArray write &int:anIndex &real:aValue.
    ]

    /// Reads an integer value at the specified position and saves it into an output variable
    #method(stacksafe) read &int:i &int:j &vreal:aValue
    [
        #var(type:int)anIndex := i * theColumns + j.
        
        anIndex := anIndex shift &int:(-3).
        
        theArray read &int:anIndex &vreal:aValue.
    ]

    #method getAt : i : j
    [
        #var(type:real)aValue.            
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
        length = theColumns int.
        
        getAt : j
            = self getAt:i:j.
        
        setAt : j : aValue
        [
            self setAt:i:j:aValue.
        ]
    }.

    /// Dispatches the verb to the target object with intarray subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &realmatrix:$self.
    
    /// Dispatches the verb to the target object with intarray subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &realmatrix:$self.
}

// --- CharArray ---

/// An array of unicode characters (UTF-32)
#class(sealed) CharArray :: BaseIntArray
{
    /// Creates the new array with a specified length
    #constructor(stacksafe) new &int : aLength
        => system'internal'ns_create.

    /// Creates the new array with a specified length
    #constructor new : aLength
        <= new &int:(aLength int).

    /// Creates the copy of a subarray
    #constructor(stacksafe) new &int:anIndex &int:aLength &chararray:anArray
        => system'internal'ns_copysubarr.
        
    /// Creates the short array copy
    #constructor new &chararray:anArray
        <= new &int:0 &int:(anArray length int) &chararray:anArray.
    
    /// Reads the array length to the output parameter
    #method(stacksafe) readLength &vint:aLength
        => system'internal'ns_getlen.

    /// Returns the array length        
    #method(stacksafe,type:int,embeddable,suppress:w3) length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ aLength.
    ]

    /// Fills the sub array with a specified value
    #method(stacksafe) fill &int:anIndex &int:aLength &char:aValue
    [
        #var(type:int)aTotalLength := $self length.
        
        #var(type:int)i := anIndex.
        #var(type:int)m := anIndex + aLength.
        
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
        
    /// Returns itself
    #method chararray = $self.

    /// Copies a sub array to the provided one at the specified position
    #method(stacksafe) read &int:anIndex &int:aLength &chararray:anArray
        => system'internal'ns_read.

    /// Copies the array at the specified position
    #method(stacksafe) write &int:anIndex &int:aLength &chararray:anArray
        => system'internal'ns_write.

    /// Writes a character value at the specified position
    #method(stacksafe) write &int:anIndex &char:aValue
        => system'internal'ns_writedword.

    /// Reads a character value at the specified position and saves it into an output variable
    #method(stacksafe) read &int:anIndex &vchar:anInt
        => system'internal'ns_readdword.

    #method(stacksafe) move &int:anIndex &int:aLength &int:anOffset
        => system'internal'ns_move.

    /// Returns a character at the specified position
    #method getAt : anIndex
    [
        #var(type:char)aRetVal.
        $self read &int:(anIndex int) &vchar:aRetVal.
        
        ^ CharValue new &char:aRetVal.
    ]        

    /// Sets a character at the specified position
    #method setAt : anIndex : aValue
    [
        $self write &int:(anIndex int) &char:(aValue char).
    ]

    /// Creates the object copy
    #method clone = CharArray new &chararray:$self.
    
    /// Dispatches the verb to the target object with chararray subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &chararray:$self.
    
    /// Dispatches the verb to the target object with chararray subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &chararray:$self.    
}
        
// --- emptyliteralvalue ---

/// An empty literal value
#symbol(const,type:literal) emptyLiteralValue = "".

// --- LiteralValue ---

/// A UTF-8 literal value
#class(sealed,string) LiteralValue :: BaseValue
{
    #field(type:bytearray)theValue.

    /// Creates the empty literal
    #constructor min
        => system'internal's_empty.
        
    /// Creates the literal string with specified length        
    #constructor(stacksafe) new &int:aSize 
        => system'internal's_create.

    /// Creates the literal string with specified length and copies the parameter value
    #constructor(stacksafe) new &int:aSize &literal:aLiteral
        => system'internal's_copy.

    /// Creates the literal string with specified length fills with spciefied character value        
    #constructor(stacksafe) new &int:aSize &char:aChar
        => system'internal's_fill.

    /// Creates the literal string copy              
    #constructor new &literal:aLiteral
        <= %new &int:(aLiteral length int) &literal:aLiteral.
        
    /// Creates the copy of a subarray        
    #constructor(stacksafe) new &int:anIndex &int:aSize &chararray:anArray
        => system'internal's_copysubarr.

    #constructor(stacksafe) $new &int:anIndex &int:aSize &bytearray:anArray
        => system'internal's_copybuf.

    /// Returns itself
    #method literal = $self.
    
    /// Converts to UTF-16 literal
    #method wide = wideConvertor convert &literal:$self.

    /// Compares the values
    #method(type:bool) equal &literal : aLiteral
        => system'internal's_equal.

    /// Compares the values
    #method(type:bool) less &literal : aLiteral
        => system'internal's_less.

    /// Compares the values
    #method(type:bool) equal &char : aChar
        <= %equal &literal:(aChar literal).

    /// Compares the values
    #method(type:bool) less &char : aChar
        <= %less &literal:(aChar literal).

    /// Concatinates the values
    #method(type:literal) add &literal : aLiteral
        => system'internal's_add.

    /// Concatinates the values
    #method(type:literal,stacksafe) add &char:aChar
        = $self add &literal:(aChar literal).

    /// Concatinates the values
    #method add : anObject
        = anObject cast:%add &to:$self.

    /// Returns the character at the specified index
    #method getAt : aParam
        <= %getAt &int:(aParam int).

    // Reads the character at the specified index and saves into an output parameter
    #method(stacksafe) read &int:anIndex &vchar:aChar
        => system'internal's_getat.

    // Reads the byte at the specified index and saves into an output parameter
    #method(stacksafe) read &int:anIndex &vbyte:anInt
        => system'internal'bs_readbyte.
        
    /// Returns the character at the specified index
    #method(type:char,stacksafe) getAt &int:anIndex
    [
        #var(type:char)aChar.
        $self read &int:anIndex &vchar:aChar.
        
        ^ CharValue new &char:aChar.
    ]

    /// Saves the index of the first occurence of the specified substring into an output parameter.
    ///
    /// If the substring is not found. returns -1
    #method(stacksafe) indexOf &int:anIndex &literal:aLiteral &vint:aRetVal
        => system'internal's_indexof.

    /// Saves the index of the first occurence of the specified symbol into an output parameter.
    ///
    /// If the substring is not found. returns -1
    #method(stacksafe) indexOf &int:anIndex &char:aChar &vint:aRetVal
        => system'internal's_indexof_char.        

    /// Returns the index of the first occurence of the specified object.
    /// 
    /// If the object is not found. returns -1
    #method indexOf : aSearchObject &at:anIndex
        = aSearchObject cast:%eval &to:
            {
               eval:anIndex &literal:aLiteral
                  = self indexOf &literal:aLiteral &at:anIndex.
    
               eval:anIndex &wide:aLiteral
                  = self indexOf &literal:(aLiteral literal) &at:anIndex.
    
               eval:anIndex &char:aChar
                  = self indexOf &char:aChar &at:anIndex.
            } &with:anIndex.

    /// obsolete            
    #method indexOf:anIndex:aSearchObject
        <= indexOf:aSearchObject &at:anIndex.

    /// Returns the index of the first occurence of the specified symbol.
    /// 
    /// If the substring is not found. returns -1
    #method indexOf &char:aChar &at:anIndex
    [
        #var(type:int)aRetVal.
        $self indexOf &int:(anIndex int) &char:aChar &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    /// Returns the index of the first occurence of the specified substring.
    ///
    /// If the substring is not found. returns -1
    #method indexOf &literal:aLiteral &at:anIndex
    [
        #var(type:int)aRetVal.
        $self indexOf &int:(anIndex int) &literal:aLiteral &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    /// Inserts the substring into and returns the new one
    #method(type:literal,stacksafe) insert &int:anIndex &literal:aLiteral
        => system'internal's_insert.

    /// Inserts a character into and returns the new one
    #method(type:literal,stacksafe) insert &int:anIndex &char:aChar
        = $self insert &int:anIndex &literal:(aChar literal).

    /// Inserts an object into and returns the new one
    #method insert : anObject &at:anIndex
        = anObject cast:%eval &to:
            {
               eval &literal:aLiteral
                  = self insert &int:(anIndex int) &literal:aLiteral.
    
               eval &wide:aLiteral
                  = self insert &int:(anIndex int) &literal:(aLiteral literal).
    
               eval &char:aChar
                  = self insert &int:(anIndex int) &char:aChar.
            }.

    /// Deletes the substring from the object and returns the new one
    #method delete:aLength &at:anIndex
        = $self delete &int:(anIndex int) &int:(aLength int).

    /// Deletes the substring from the object and returns the new one
    #method(type:literal,stacksafe) delete &int:anIndex &int:aLength
        => system'internal's_delete.

    /// Returns the substring
    #method Substring : aLength &at:anIndex
        = $self Substring &int:(anIndex int) &int:(aLength int).

    /// Returns the substring
    #method(type:literal,stacksafe) Substring &int:anIndex &int:aLength
        => system'internal's_subs.

    /// Reads the literal length into an output parameter
    #method(stacksafe) readLength &vint:aLength
        => system'internal's_getlen.

    /// Saves the literal into a UTF-32 character array
    #method(stacksafe) save &int:anIndex &chararray:anArray &vint:aRetVal
        => system'internal's_save.

    #method(stacksafe) $save &int:anIndex &bytearray:anArray &int:aRetVal
        => system'internal's_saveraw.

    /// Returns the string length
    #method(type:int,embeddable,suppress:w3) length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ aLength.
    ]

    /// Returns the string enumerator
    #method enumerator = LiteralEnumerator new &literal:$self.
    
    /// Returns the string iterator
    #method iterator = LiteralIterator new &literal:$self.
    
    /// Dispatches the verb to the target object with literal subject
    #method cast:aVerb &to:aTarget = aTarget::aVerb eval &literal:$self.
    
    /// Dispatches the verb to the target object with literal subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &literal:$self.
    
    /// Clones the value
    #method clone = LiteralValue new &literal:$self.

    // Reads the object hash code into the output parameter
    #method(stacksafe) readHashCode &vint:aCode => system'internal'int_hashcode.
    
    #method(type:int,embeddable,suppress:w3) hash_code
    [
        #var(type:int)aCode.
        $self readHashCode &vint:aCode.
        
        ^ aCode.
    ]
}

// --- String ---

/// A literal variable. 
///
/// Extends system'LiteralValue value
#class String :: BaseValue
{
    #field(type:literal)theValue.

    /// Creates an empty literal variable 
    #constructor new
    [
        theValue := emptyLiteralValue.
    ]

    /// Creates a literal variable with specified value
    #constructor new : aValue
    [
        $self write:aValue.
    ]
    
    /// Creates a literal variable with specified value
    #constructor new &literal:aValue
    [
        theValue := aValue.
    ]
    
    /// Creates a literal variable with specified value
    #constructor new &char:aValue
    [
        theValue := aValue literal.
    ]
    
    /// Creates a literal variable with specified value
    #constructor new &wide:aValue
    [
        theValue := aValue literal.
    ]
    
    /// Writes the value
    #method write : anOperand
    [
        anOperand cast:%write &to:$self.
    ]
    
    /// Writes the value
    #method write &literal:aLiteral
    [
        theValue := aLiteral.
    ]
    
    /// Writes the value
    #method write &wide:aLiteral
    [
        theValue := aLiteral literal.
    ]
    
    /// Writes the value
    #method write &char:aChar
    [
        theValue := aChar literal.
    ]

    /// Appends the value    
    #method append : anOperand
    [
        anOperand cast:%append &to:$self.
    ]
    
    /// Appends the value    
    #method append &literal:anOperand
    [
        theValue := theValue add &literal:anOperand.
    ]
    
    /// Appends the value    
    #method append &wide:anOperand
    [
        theValue := theValue add &literal:(anOperand literal).
    ]
    
    /// Appends the value    
    #method append &char:anOperand
    [
        theValue := theValue add &literal:(anOperand literal).
    ]

    /// Inserts a sub string
    #method insert &int:anIndex &literal:aLiteral
    [
        (anIndex == 0)
            ? [ theValue := aLiteral add &literal:theValue. ]
            ! [ theValue := theValue insert &int:anIndex &literal:aLiteral. ].
    ]
    
    /// Inserts a character
    #method insert &int:anIndex &char:aChar
    [
        (anIndex == 0)
            ? [ theValue := aChar literal add &literal:theValue. ]
            ! [ theValue := theValue insert &int:anIndex &literal:(aChar literal). ].
    ]
    
    /// Inserts a wide literal
    #method insert &int:anIndex &wide:aLiteral
    [
        theValue := theValue insert &int:anIndex &literal:(aLiteral literal).
    ]

    /// Inserts an object at the specified position
    #method insert : anObject &at:anIndex
    [
        anObject cast:%eval &to:
        {
            eval &char:aChar [ self insert &int:(anIndex int) &char:aChar. ]
            
            eval &literal:aLiteral [ self insert &int:(anIndex int) &literal:aLiteral. ]
            
            eval &wide:aLiteral [ self insert &int:(anIndex int) &wide:aLiteral. ]
        }.
    ]            
                                    
    /// Deletes a sub string
    #method delete &int:anIndex &int:aLength
    [
        theValue := theValue delete &int:anIndex &int:aLength.
    ]

    /// Deletes a sub string
    #method delete : aLength &at:anIndex
        <= delete &int:(anIndex int) &int:(aLength int).

    /// Clears the variable value
    #method clear
    [
        theValue := emptyLiteralValue.
    ]

    /// Clones the object    
    #method clone = String new &literal:theValue.
                
    /// Dispatches the verb to the target object with literal subject
    #method cast:aVerb &to:aTarget = aTarget::aVerb eval &literal:theValue.
    
    /// Dispatches the verb to the target object with literal subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &literal:theValue.

    /// Returns a literal value        
    #method literal = theValue.
    
    /// Returns a literal value        
    #method value = theValue.
    
    /// Redirects other messages to the literal value
    #method => theValue.
}

// --- emptyWideLiteralValue ---

/// An empty wide literal value
#symbol(type:wide) emptyWideLiteralValue = #0 wide. // !! temporal

// --- WideLiteralValue ---

/// A UTF-18 literal value
#class(sealed,widestring) WideLiteralValue :: BaseValue
{
    #field(type:shortarray)theValue.

    /// Creates the empty literal
    #constructor min
        => system'internal'ws_empty.
        
    /// Creates the literal string with specified length
    #constructor(stacksafe) new &int:aSize 
        => system'internal'ws_create.

    /// Creates the literal string with specified length and copies the parameter value
    #constructor(stacksafe) new &int:aSize &wide:aLiteral
        => system'internal'ws_copy.
    
    /// Creates the literal string with specified length fills with spciefied character value
    #constructor(stacksafe) new &int:aSize &char:aChar
        => system'internal'ws_fill.

    /// Creates the literal string copy
    #constructor new &wide:aLiteral
        <= %new &int:(aLiteral length int) &wide:aLiteral.
        
    /// Creates the copy of a subarray
    #constructor(stacksafe) new &int:anIndex &int:aSize &chararray:anArray
        => system'internal'ws_copysubarr.

    #constructor(stacksafe) $new &int:anIndex &int:aSize &shortarray:anArray
        => system'internal'ws_copybuf.

    /// Returns itself
    #method wide = $self.

    /// Converts to UTF-8 literal
    #method literal = literalConvertor convert &wide:$self.

    /// Compares the values
    #method(type:bool) equal &wide : aLiteral
        => system'internal'ws_equal.

    /// Compares the values
    #method(type:bool) less &wide : aLiteral
        => system'internal'ws_less.

    /// Compares the values
    #method(type:bool) equal &char : aChar
        <= %equal &wide:(aChar wide).

    /// Compares the values
    #method(type:bool) less &char : aChar
        <= %less &wide:(aChar wide).

    /// Concatinates the values
    #method(type:wide,stacksafe) add &wide : aLiteral
        => system'internal'ws_add.

    /// Concatinates the values
    #method(type:wide,stacksafe) add &char:aChar
        = $self add &wide:(aChar wide).

    /// Concatinates the values
    #method(type:wide) add &literal:aLiteral
        = $self add &wide:(aLiteral wide).

    /// Concatinates the values
    #method add : anObject
        = anObject cast:%add &to:$self.

    /// Returns the character at specified index
    #method getAt : aParam
        <= %getAt &int:(aParam int).

    // Reads the character at the specified index and saves into an output parameter
    #method(stacksafe) read &int:anIndex &vchar:aChar
        => system'internal'ws_getat.

    // Reads the short integer at the specified index and saves into an output parameter
    #method(stacksafe) read &int:anIndex &vshort:aValue
        => system'internal'sa_readword.
        
    /// Returns the character at specified index
    #method(type:char,stacksafe) getAt &int:anIndex
    [
        #var(type:char)aChar.
        $self read &int:anIndex &vchar:aChar.
        
        ^ CharValue new &char:aChar.
    ]

    /// Saves the index of the first occurence of the specified substring into an output parameter.
    ///
    /// If the substring is not found. returns -1
    #method(stacksafe) indexOf &int:anIndex &wide:aLiteral &vint:aRetVal
        => system'internal'ws_indexof.

    /// Saves the index of the first occurence of the specified character into an output parameter.
    ///
    /// If the substring is not found. returns -1
    #method(stacksafe) indexOf &int:anIndex &char:aChar &vint:aRetVal
        => system'internal'ws_indexof_char.

    /// Returns the index of the first occurence of the specified character.
    ///
    /// If the substring is not found. returns -1
    #method(stacksafe) indexOf &int:anIndex &char:aChar
    [
        #var(type:int)aRetVal.
        $self indexOf &int:anIndex &char:aChar &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    /// Returns the index of the first occurence of the specified substring.
    ///
    /// If the substring is not found. returns -1
    #method(stacksafe) indexOf &int:anIndex &wide:aLiteral
    [
        #var(type:int)aRetVal.
        $self indexOf &int:anIndex &wide:aLiteral &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    /// Returns the index of the first occurence of the specified object.
    ///
    /// If the substring is not found. returns -1
    #method indexOf : anObject &at:anIndex
        = anObject cast:%eval &to:
            {
                eval &literal:aLiteral = self indexOf &int:(anIndex int) &wide:(aLiteral wide).
                
                eval &wide:aLiteral = self indexOf &int:(anIndex int) &wide:aLiteral.
                
                eval &char:aChar = self indexOf &int:(anIndex int) &char:aChar.
            }.

    /// Inserts the substring into and returns the new one
    #method(type:wide,stacksafe) insert &int:anIndex &wide:aLiteral
        => system'internal'ws_insert.

    /// Inserts the substring into and returns the new one
    #method(type:wide,stacksafe) insert &int:anIndex &literal:aLiteral
        = $self insert &int:anIndex &wide:(aLiteral wide).

    /// Inserts a character into and returns the new one
    #method(type:wide,stacksafe) insert &int:anIndex &char:aChar
        = $self insert &int:anIndex &wide:(aChar wide).

    /// Inserts an object into and returns the new one
    #method insert : anObject &at:anIndex
        = anObject cast:%eval &to:
            {
               eval &wide:aLiteral
                  = self insert &int:(anIndex int) &wide:aLiteral.
    
               eval &literal:aLiteral
                  = self insert &int:(anIndex int) &wide:(aLiteral wide).
    
               eval &char:aChar
                  = self insert &int:(anIndex int) &char:aChar.
            }.

    /// Deletes the substring from the object and returns the new one
    #method delete:aLength &at:anIndex
        = $self delete &int:(anIndex int) &int:(aLength int).
            
    /// Deletes the substring from the object and returns the new one            
    #method(type:wide,stacksafe) delete &int:anIndex &int:aLength
        => system'internal'ws_delete.

    /// Returns the substring
    #method(type:wide,stacksafe) Substring &int:anIndex &int:aLength
        => system'internal'ws_subs.

    #method(type:wide) Substring : aLength &at:anIndex
        <= Substring &int:(anIndex int) &int:(aLength int).

    /// Reads the literal length into an output parameter
    #method(stacksafe) readLength &vint:aLength
        => system'internal'ws_getlen.

    /// Returns the string length
    #method(stacksafe,type:int,embeddable,suppress:w3) length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ aLength.
    ]

    /// Saves the literal into a UTF-32 character array
    #method(stacksafe) save &int:anIndex &chararray:anArray &vint:aLength
        => system'internal'ws_save.

    #method(stacksafe) $save &int:anIndex &shortarray:anArray &int:aRetVal
        => system'internal'ws_saveraw.

    /// Returns the object enumerator
    #method enumerator = WideLiteralEnumerator new &wide:$self.
    
    /// Returns the object iterator
    #method iterator = WideLiteralIterator new &wide:$self.
    
    /// Dispatches the verb to the target object with wide subject
    #method cast:aVerb &to:aTarget = aTarget::aVerb eval &wide:$self.
    
    /// Dispatches the verb to the target object with wide subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &wide:$self.
    
    /// Clones the value
    #method clone = WideLiteralValue new &wide:$self.

    // Reads the object hash code into the output parameter
    #method(stacksafe) readHashCode &vint:aCode => system'internal'int_hashcode.
    
    #method(type:int,embeddable,suppress:w3) hash_code
    [
        #var(type:int)aCode.
        $self readHashCode &vint:aCode.
        
        ^ aCode.
    ]
}

// --- WideString ---

/// A wide literal variable. 
///
/// Extends system'WideLiteralValue value
#class WideString :: BaseValue
{
    #field(type:wide)theValue.

    /// Creates an empty literal variable 
    #constructor new
    [
        theValue := emptyWideLiteralValue.
    ]
            
    /// Creates a literal variable with specified value
    #constructor new : aValue
    [
        $self write:aValue.
    ]
    
    /// Creates a literal variable with specified value
    #constructor new &wide:aValue
    [
        theValue := aValue.
    ]
    
    /// Creates a literal variable with specified value
    #constructor new &literal:aValue
    [
        theValue := aValue wide.
    ]
    
    /// Creates a literal variable with specified value
    #constructor new &char:aValue
    [
        theValue := aValue wide.
    ]

    /// Writes the value    
    #method write : anOperand
    [
        anOperand cast:%write &to:$self.
    ]
    
    /// Writes the value    
    #method write &wide:aLiteral
    [
        theValue := aLiteral.
    ]
    
    /// Writes the value    
    #method write &literal:aLiteral
    [
        theValue := aLiteral wide.
    ]
    
    /// Writes the value    
    #method write &char:aChar
    [
        theValue := aChar wide.
    ]

    /// Appends the value    
    #method append : anOperand
    [
        anOperand cast:%append &to:$self.
    ]
    
    /// Appends the value    
    #method append &wide:anOperand
    [
        theValue := theValue add &wide:anOperand.
    ]
    
    /// Appends the value    
    #method append &literal:anOperand
    [
        theValue := theValue add &literal:anOperand.
    ]
    
    /// Appends the value    
    #method append &char:anOperand
    [
        theValue := theValue add &wide:(anOperand wide).
    ]
    
    /// Inserts a sub string
    #method insert &int:anIndex &wide:aLiteral
    [
        (anIndex == 0)
            ? [ theValue := aLiteral add &wide:theValue. ]
            ! [ theValue := theValue insert &int:anIndex &wide:aLiteral. ].
    ]

    /// Inserts a character    
    #method insert &int:anIndex &char:aChar
    [
        (anIndex == 0)
            ? [ theValue := aChar wide add &wide:theValue. ]
            ! [ theValue := theValue insert &int:anIndex &wide:(aChar wide). ].
    ]
    
    /// Inserts a sub string
    #method insert &int:anIndex &literal:aLiteral
    [
        theValue := theValue insert &int:anIndex &wide:(aLiteral wide).
    ]

    /// Inserts an object at the specified position
    #method insert : anObject &at:anIndex
    [
        anObject cast:%eval &to:
        {
            eval &char:aChar [ self insert &int:(anIndex int) &char:aChar. ]
            
            eval &literal:aLiteral [ self insert &int:(anIndex int) &literal:aLiteral. ]
            
            eval &wide:aLiteral [ self insert &int:(anIndex int) &wide:aLiteral. ]
        }.
    ]            
        
    /// Deletes a sub string
    #method delete &int:anIndex &int:aLength
    [
        theValue := theValue delete &int:anIndex &int:aLength.
    ]
        
    /// Deletes a sub string                
    #method delete : aLength &at:anIndex
        <= delete &int:(anIndex int) &int:(aLength int).
                        
    /// Clears the variable value
    #method clear
    [
        theValue := emptyWideLiteralValue.
    ]

    /// Clones the object    
    #method clone = WideString new &wide:theValue.
                
    /// Dispatches the verb to the target object with wide subject
    #method cast:aVerb &to:aTarget = aTarget::aVerb eval &wide:theValue.
    
    /// Dispatches the verb to the target object with wide subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &wide:theValue.

    /// Returns the literal value    
    #method wide = theValue.
    
    /// Returns the literal value
    #method value = theValue.
    
    /// Redirects other messages to the literal value
    #method => theValue.
}

// --- Variant ---

/// A basic type variant class
#class Variant :: String
{
    /// Tries to convert the literal value to a byte number
    #method byte
        = byteConvertor convert &literal:theValue.

    /// Tries to convert the literal value to a short integer number
    #method short
        = shortConvertor convert &literal:theValue.

    /// Tries to convert the literal value to a character
    #method char
    [
        #var(type:char)retVal.
        charConvertor $convert &int:0 &literal:theValue &vchar:retVal.
        
        ^ CharValue new &char:retVal.
    ]

    /// Tries to convert the literal value to the integer number
    #method int
        = intConvertor convert &literal:theValue.

    /// Tries to convert the literal value to the long integer number
    #method long
        = longConvertor convert &literal:theValue.

    /// Tries to convert the literal value to the real number
    #method real
        = realConvertor convert &literal:theValue.

    /// Converts the parameter into a literal string
    #method write &int:aValue
    [
        theValue := aValue literal.
    ]    

    /// Converts the parameter into a literal string
    #method write &short:aValue
    [
        theValue := aValue literal.
    ]    

    /// Converts the parameter into a literal string
    #method write &byte:aValue
    [
        theValue := aValue literal.
    ]    

    /// Converts the parameter into a literal string
    #method write &long:aValue
    [
        theValue := aValue literal.
    ]    

    /// Converts the parameter into a literal string
    #method write &real:aValue
    [
        theValue := aValue literal.
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

    /// Returns a literal value
    #method literal = theValue.
    
    /// Returns a wide literal value
    #method wide = theValue wide.
}

// --- BaseArray ---

/// A base array
#class(dynamic) BaseArray
{
    /// Creates the array with the specified length
    #constructor(stacksafe) new &int:aLength
        => system'internal'arr_create.
}

// --- Array ---

/// An array
#class(sealed) Array :: BaseArray
{
    /// Creates an empty array
    #constructor min
        => system'internal'arr_empty.
    
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
        => system'internal'arr_create.
    
    /// Creates a copy of the provided array
    #constructor new &array:anArray
        <= %new &int:0 &int:(anArray length int) &array:anArray.
    
    /// Creates a copy of the provided sub array
    #constructor(stacksafe) new &int:anIndex &int:aLength &array:anArray
        => system'internal'arr_copysubarr.
        
    /// Returns the specified array member
    #method getAt : anIndex
        = $self getAt &int:(anIndex int).

    /// Returns the specified array member
    #method(stacksafe) getAt &int:anIndex
        => system'internal'arr_getat.

    /// Sets the specified array member
    #method setAt:anIndex:anObject
    [
        $self setAt &int:(anIndex int) &object:anObject.
    ]
    
    /// Sets the specified array member
    #method(stacksafe) setAt &int:anIndex &object:anObject
        => system'internal'arr_setat.

    /// Concatinates the arrays    
    #method add : anObject
        = $self add &array:(anObject array).

    /// Concatinates the arrays    
    #method add &array:anArray
    [
        #var(type:int)aLength1.
        #var(type:int)aLength2.
        
        $self readLength &vint:aLength1.        
        anArray readLength &vint:aLength2.
        
        #var(type:array)aRetVal := Array new &int:(aLength1 + aLength2).

        #var(type:int)anIndex := 0.
        #loop (anIndex < aLength1) ?
        [
            aRetVal setAt &int:anIndex &object:($self getAt &int:anIndex).
            
            anIndex := anIndex + 1.
        ].

        anIndex := 0.
        #loop (anIndex < aLength2) ?
        [
            aRetVal setAt &int:(aLength1 + anIndex) &object:(anArray getAt &int:anIndex).
            
            anIndex := anIndex + 1.
        ].
            
        ^ aRetVal.
    ]

    /// Returns itself
    #method array = $self.

    /// Reads the array length into an output parameter
    #method(stacksafe) readLength &vint:aLength
        => system'internal'arr_getlen.

    /// Returns the array length
    #method(stacksafe,type:int,embeddable,suppress:w3) length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ aLength.
    ]

    /// Returns the array enumerator
    #method enumerator = ArrayEnumerator new &array:$self.

    /// Returns the array iterator
    #method iterator = ArrayIterator new &array:$self.

    /// Dispatches the verb to the target object with array subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &array:$self.
    
    /// Dispatches the verb to the target object with array subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &array:$self.

    /// Clones the array
    #method(type:array) clone 
        = Array new &array:$self.
}

// --- OpenArgs ---

#class(sealed,dynamic)OpenArgs
{
    /// Returns the specified array member
    #method getAt : anIndex
        = $self getAt &int:(anIndex int).

    /// Returns the specified array member
    #method(stacksafe) getAt &int:anIndex
        => system'internal'arr_getat.

    #method(stacksafe) readLength &vint:aLength
        => system'internal'args_getlen.
        
    #method(stacksafe,type:int,embeddable,suppress:w3) length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ aLength.
    ]
}

// === Enumerators / Indexers

// --- LiteralEnumerator ---

/// A literal enumerator
#class LiteralEnumerator :: Enumerator
{
    #field(type:literal) theLiteral.
    #field(type:vint)    theIndex.
    #field(type:vint)    theDiff.
    #field(type:char)    theChar.
    
    /// Creates an enumerator
    #constructor new : anObject
        <= new &literal:(anObject literal).
    
    /// Creates an enumerator
    #constructor new &literal:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := Integer new &int:(-1).
        theDiff := Integer new &int:1.
        theChar := #0.
    ]

    /// Resets the enumerator
    #method reset
    [
        theIndex write &int:-1.
        theDiff write &int:1.
    ]

    /// Goes to the next enumeration member and returns true if it is not the last member
    #method next
    [
        #var(type:int)aLength.       
        theLiteral readLength &vint:aLength.
        
        theIndex += theDiff.
        
        (theIndex < aLength)
            ? [
                theChar := theLiteral getAt &int:theIndex.                
                
                theChar readLength &vint:theDiff.
                
                ^ true.
            ].

        theDiff write &int:0.
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
    #field(type:literal) theLiteral.
    #field(type:vint)    theIndex.
    
    /// Creates an iterator
    #constructor new : anObject
        <= new &literal:(anObject literal).
    
    /// Creates an iterator
    #constructor new &literal:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := Integer new.
    ]
    
    /// Returns false if the index out of range
    #method available
    [
        #var(type:int)aLength.
        theLiteral readLength &vint:aLength.
        
        ^ theIndex < aLength.
    ]

    /// Returns the current character
    #method get
    [
        #var(type:char)aChar.
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
        anIndex << theIndex.
    ]
    
    /// Sets the index by specified value
    #method writeIndex &int:anIndex 
    [
        theIndex write &int:anIndex.
    ]
    
    /// Reads the length to the output parameter    
    #method readLength &vint:anOutput
        => theLiteral.
}

// --- WideLiteralEnumerator ---

/// A literal enumerator
#class WideLiteralEnumerator :: Enumerator
{
    #field(type:wide) theLiteral.
    #field(type:vint) theIndex.
    #field(type:vint) theDiff.
    #field(type:char) theChar.
    
    /// Creates an enumerator
    #constructor new : anObject
        <= new &wide:(anObject wide).
    
    /// Creates an enumerator
    #constructor new &wide:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := Integer new &int:(-1).
        theDiff := Integer new &int:1.
        theChar := #0.
    ]

    /// Resets the enumerator
    #method reset
    [
        theIndex write &int:-1.
        theDiff write &int:1.
    ]

    /// Goes to the next enumeration member and returns true if it is not the last member
    #method next
    [
        #var(type:int)aLength.
        
        theLiteral readLength &vint:aLength.

        theIndex += theDiff.
        
        (theIndex < aLength)
            ? [
                theChar := theLiteral getAt &int:theIndex.
                
                theChar readLength &vint:theDiff.
                theDiff += 1.
                theDiff /= 2.
                
                ^ true.
            ].

        theDiff write &int:0.
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
    #field(type:wide) theLiteral.
    #field(type:vint) theIndex.
    
    /// Creates an iterator
    #constructor new : anObject
        <= new &wide:(anObject wide).
    
    /// Creates an iterator
    #constructor new &wide:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := Integer new.
    ]

    /// Returns false if the index out of range    
    #method available
    [
        #var(type:int)aLength.
        theLiteral readLength &vint:aLength.
        
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
        anIndex << theIndex.
    ]
    
    /// Sets the index by specified value
    #method writeIndex &int:anIndex 
    [
        theIndex write &int:anIndex.
    ]
    
    /// Reads the length to the output parameter    
    #method readLength &vint:anOutput
        => theLiteral.
}

// --- ArrayIterator ---

/// An array iterator
#class ArrayIterator :: Iterator
{
    #field(type:array) theArray.
    #field(type:vint)  theIndex.

    /// Returns the parameter iterator
    #constructor new : anArray
        <= new &array:(anArray array).
    
    /// Returns the parameter iterator
    #constructor new &array:anArray
    [
        theArray := anArray.
        theIndex := Integer new.
    ]
    
    /// Returns false if the index out of range
    #method available
    [
        #var(type:int)aLength.
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
        anIndex << theIndex.
    ]
    
    /// Sets the index by specified value
    #method writeIndex &int:anIndex 
    [
        theIndex write &int:anIndex.
    ]
    
    /// Reads the length to the output parameter    
    #method readLength &vint:anOutput
        => theArray.
}

// --- ArrayEnumerator ---

/// An array enumerator
#class ArrayEnumerator :: Enumerator
{
    #field(type:array) theArray.
    #field(type:vint)  theIndex.
    
    /// Creates an array enumerator
    #constructor new : anArray
        <= new &array:(anArray array).
    
    /// Creates an array enumerator
    #constructor new &array:anArray
    [
        theArray := anArray.
        theIndex := Integer new &int:(-1).
    ]

    /// Resets the enumerator
    #method reset
    [
        theIndex write &int:-1.
    ]

    /// Goes to the next enumeration member and returns true if it is not the last member
    #method next
    [
        #var(type:int)aLength.
        theArray readLength &vint:aLength.
        
        theIndex append &int:1.

        ^ theIndex < aLength.
    ]
    
    /// Returns the current object
    #method get
        = theArray getAt &int:theIndex.
}

// --- Func1Group ---

#subject(class:system'Func1Array)func1array.

#class(arrayof:func1,sealed) Func1Array
{
    #constructor(stacksafe) new &int:aLength
        => system'internal'arr_create.
        
    #method(stacksafe) readLength &vint:aLength
        => system'internal'arr_getlen.

    #method getAt : anIndex
        = $self getAt &int:(anIndex int).

    #method(stacksafe) getAt &int:anIndex
        => system'internal'arr_getat.

    #method setAt:anIndex:anObject
    [
        $self setAt &int:(anIndex int) &object:anObject.
    ]
    
    #method(stacksafe) setAt &int:anIndex &object:anObject
        => system'internal'arr_setat.

    #method(stacksafe,type:int,embeddable,suppress:w3) length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ aLength.
    ]        
}

#class Function1Group :: BaseFunction1
{
    #field(type:func1array) theHandlers.
    
    #constructor new &args:Handlers
    [
        #var(type:int)i.
        #var(type:int)len.
        Handlers readLength &vint:len.
        
        theHandlers := Func1Array new &int:len.
        
        #loop (i < len)?
        [
            theHandlers@i := (Handlers@i) func1.
            
            i := i + 1.
        ].
    ]
    
    #method eval : anObject
    [
        #var(type:int)i.
        #var(type:int)len.
        theHandlers readLength &vint:len.
        
        #loop (i < len)?
        [
            theHandlers@i eval:anObject.
            
            i := i + 1.
        ].
    ]
}
