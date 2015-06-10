// === basic memory classes ===

// --- BaseByteArray ---

/// A base byte array
#class(stringof:1) BaseByteArray
{
    /// Creates the byte array with the specified length
    #constructor new &length : aLength
        => system'internal'bs_create.
}

// --- ByteArray ---

/// A byte array
#class(sealed) ByteArray :: BaseByteArray
{
    /// Creates the byte array with the specified length
    #constructor new &length : aLength
        => system'internal'bs_create.
    
    /// Creates the copy of a byte subarray
    #constructor new &length:aLength &index:anIndex &bytearray:anArray
        => system'internal'bs_copysubarr.
    
    /// Creates the byte array copy
    #constructor new &bytearray:anArray
        <= new &length:(anArray length) &index:0 &bytearray:anArray.
    
    /// Returns itself
    #method bytearray = $self.

    /// Reads the array length to the output parameter
    #method readLength &vint:aLength
        => system'internal'bs_getlen.
        
    /// Returns the byte array length        
    #method length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]

    /// Fills the sub array with a specified value
    #method fill &index:anIndex &length:aLength &byte:aValue
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
            $self write &index:i &byte:aValue.
            
            i := i + 1.
        ].
    ]

    /// Copies a sub array to the provided one at the specified position
    #method read &index:anIndex &length:aLength &bytearray:aDump
        => system'internal'bs_read.

    /// Copies the byte array at the specified position
    #method write &index:anIndex &length:aLength &bytearray:aDump
        => system'internal'bs_write.

    /// Writes the byte value at specified position
    #method write &index:anIndex &byte:anInt
        => system'internal'bs_writebyte.

    /// Reads a byte value at the specified position and saves it into an output variable
    #method read &index:anIndex &vbyte:anInt
        => system'internal'bs_readbyte.

    /// Writes a short integer value at the specified position
    #method write &index:anIndex &short:anInt
        => system'internal'bs_writeword.

    /// Reads a short integer value at the specified position and saves it into an output variable
    #method read &index:anIndex &vshort:aChar
        => system'internal'bs_readword.

    /// Writes a character value at the specified position
    #method write &index:anIndex &char:anInt
        => system'internal'bs_writedword.

    /// Reads a character value at the specified position and saves it into an output variable
    #method read &index:anIndex &vchar:aChar
        => system'internal'bs_readdword.

    /// Writes an integer value at the specified position
    #method write &index:anIndex &int:anInt
        => system'internal'bs_writedword.

    /// Reads an integer value at the specified position and saves it into an output variable
    #method read &index:anIndex &vint:anInt
        => system'internal'bs_readdword.

    /// Writes a long integer value at the specified position
    #method write &index:anIndex &long:aLong
        => system'internal'bs_writeqword.

    /// Reads a long integer value at the specified position and saves it into an output variable
    #method read &index:anIndex &vlong:aLong
        => system'internal'bs_readqword.

    /// Writes a real value at the specified position
    #method write &index:anIndex &real:aLong
        => system'internal'bs_writeqword.

    /// Reads a real value at the specified position and saves it into an output variable
    #method read &index:anIndex &vreal:aLong
        => system'internal'bs_readqword.

    /// Returns a byte at the specified position
    #method getAt : anIndex
    [
        #var(type:byte)aRetVal.
        $self read &index:(anIndex int) &vbyte:aRetVal.
        
        ^ ByteNumber new &byte:aRetVal.
    ]        

    /// Sets a byte at the specified position
    #method setAt : anIndex : aValue
    [
        $self write &index:(anIndex int) &byte:(aValue byte).
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
    #constructor new &length : aLength
        => system'internal'sa_create.
    
    /// Creates the copy of a short subarray
    #constructor new &length:aLength &index:anIndex &shortarray:anArray
        => system'internal'sa_copysubarr.

    /// Creates the short array copy
    #constructor new &shortarray:anArray
        <= new &length:(anArray length) &index:0 &shortarray:anArray.
    
    /// Returns itself
    #method shortarray = $self.

    /// Reads the array length to the output parameter
    #method readLength &vint:aLength
        => system'internal'sa_getlen.

    /// Returns the array length        
    #method length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]

    /// Fills the sub array with a specified value
    #method fill &index:anIndex &length:aLength &short:aValue
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
            $self write &index:i &short:aValue.
            
            i := i + 1.
        ].
    ]
    
    /// Copies a sub array to the provided one at the specified position
    #method read &index:anIndex &length:aLength &shortarray:anArray
        => system'internal'sa_read.

    /// Copies the short array at the specified position        
    #method write &index:anIndex &length:aLength &shortarray:anArray
        => system'internal'sa_write.

    /// Writes a short integer value at the specified position
    #method write &index:anIndex &short:aValue
        => system'internal'sa_writeword.

    /// Reads a short integer value at the specified position and saves it into an output variable
    #method read &index:anIndex &vshort:aValue
        => system'internal'sa_readword.

    /// Returns a short integer at the specified position
    #method getAt : anIndex
    [
        #var(type:short)aRetVal.
        $self read &index:(anIndex int) &vshort:aRetVal.
        
        ^ ShortNumber new &short:aRetVal.
    ]        

    /// Sets a short integer at the specified position
    #method setAt : anIndex : aValue
    [
        $self write &index:(anIndex int) &short:(aValue short).
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
    #constructor new &length : aLength
        => system'internal'ns_create.
}

// --- IntArray ---

/// An array of integers
#class(sealed) IntArray :: BaseIntArray
{
    /// Creates the array with the specified length
    #constructor new &length : aLength
        => system'internal'ns_create.

    /// Creates the copy of a subarray
    #constructor new  &length:aLength &index:anIndex &intarray:anArray
        => system'internal'ns_copysubarr.
        
    /// Creates the short array copy
    #constructor new &intarray:anArray
        <= new &length:(anArray length) &index:0 &intarray:anArray.
    
    /// Reads the array length to the output parameter
    #method readLength &vint:aLength
        => system'internal'ns_getlen.

    /// Returns the array length
    #method length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]
    
    /// Returns itself
    #method intarray = $self.

    /// Fills the sub array with a specified value
    #method fill &index:anIndex &length:aLength &int:aValue
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
            $self write &index:i &int:aValue.
            
            i := i + 1.
        ].
    ]

    /// Copies a sub array to the provided one at the specified position
    #method read &index:anIndex &length:aLength &intarray:anArray
        => system'internal'ns_read.

    /// Copies the short array at the specified position
    #method write &index:anIndex &length:aLength &intarray:anArray
        => system'internal'ns_write.

    /// Writes an integer value at the specified position
    #method write &index:anIndex &int:aValue
        => system'internal'ns_writedword.

    /// Reads an integer value at the specified position and saves it into an output variable
    #method read &index:anIndex &vint:anInt
        => system'internal'ns_readdword.

    /// Returns an integer at the specified position
    #method getAt : anIndex
    [
        #var(type:int)aRetVal.
        $self read &index:(anIndex int) &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]        

    /// Sets an integer at the specified position
    #method setAt : anIndex : aValue
    [
        $self write &index:(anIndex int) &int:(aValue int).
    ]

    /// Creates the object copy
    #method clone = IntArray new &intarray:$self.
    
    /// Dispatches the verb to the target object with intarray subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &intarray:$self.
    
    /// Dispatches the verb to the target object with intarray subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &intarray:$self.    
}

// --- CharArray ---

/// An array of unicode characters (UTF-32)
#class(sealed) CharArray :: BaseIntArray
{
    /// Creates the new array with a specified length
    #constructor new &length : aLength
        => system'internal'ns_create.

    /// Creates the copy of a subarray
    #constructor new  &length:aLength &index:anIndex &chararray:anArray
        => system'internal'ns_copysubarr.
        
    /// Creates the short array copy
    #constructor new &chararray:anArray
        <= new &length:(anArray length) &index:0 &chararray:anArray.
    
    /// Reads the array length to the output parameter
    #method readLength &vint:aLength
        => system'internal'ns_getlen.

    /// Returns the array length        
    #method length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]

    /// Fills the sub array with a specified value
    #method fill &index:anIndex &length:aLength &char:aValue
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
            $self write &index:i &char:aValue.
            
            i := i + 1.
        ].
    ]
        
    /// Returns itself
    #method chararray = $self.

    /// Copies a sub array to the provided one at the specified position
    #method read &index:anIndex &length:aLength &chararray:anArray
        => system'internal'ns_read.

    /// Copies the array at the specified position
    #method write &index:anIndex &length:aLength &chararray:anArray
        => system'internal'ns_write.

    /// Writes a character value at the specified position
    #method write &index:anIndex &char:aValue
        => system'internal'ns_writedword.

    /// Reads a character value at the specified position and saves it into an output variable
    #method read &index:anIndex &vchar:anInt
        => system'internal'ns_readdword.

    #method move &index:anIndex &length:aLength &offset:anOffset
        => system'internal'ns_move.

    /// Returns a character at the specified position
    #method getAt : anIndex
    [
        #var(type:char)aRetVal.
        $self read &index:(anIndex int) &vchar:aRetVal.
        
        ^ CharValue new &char:aRetVal.
    ]        

    /// Sets a character at the specified position
    #method setAt : anIndex : aValue
    [
        $self write &index:(anIndex int) &char:(aValue char).
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
    #constructor new &length:aSize 
        => system'internal's_create.

    /// Creates the literal string with specified length and copies the parameter value
    #constructor new &length:aSize &literal:aLiteral
        => system'internal's_copy.

    /// Creates the literal string with specified length fills with spciefied character value        
    #constructor new &length:aSize &char:aChar
        => system'internal's_fill.

    /// Creates the literal string copy              
    #constructor new &literal:aLiteral
        <= %new &length:(aLiteral length) &literal:aLiteral.
        
    /// Creates the copy of a subarray        
    #constructor new &length:aSize &index:anIndex &chararray:anArray
        => system'internal's_copysubarr.

    #constructor $new &length:aSize &index:anIndex &bytearray:anArray
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
    #method(type:literal) add &char:aChar
        = $self add &literal:(aChar literal).

    /// Concatinates the values
    #method add : anObject
        = anObject cast:%add &to:$self.

    /// Returns the character at the specified index
    #method getAt : aParam
        <= %getAt &index:(aParam int).

    // Reads the character at the specified index and saves into an output parameter
    #method read &index:anIndex &vchar:aChar
        => system'internal's_getat.

    // Reads the byte at the specified index and saves into an output parameter
    #method read &index:anIndex &vbyte:anInt
        => system'internal'bs_readbyte.
        
    /// Returns the character at the specified index
    #method(type:char) getAt &index:anIndex
    [
        #var(type:char)aChar.
        $self read &index:anIndex &vchar:aChar.
        
        ^ CharValue new &char:aChar.
    ]

    /// Saves the index of the first occurence of the specified substring into an output parameter.
    ///
    /// If the substring is not found. returns -1
    #method indexOf &index:anIndex &literal:aLiteral &vint:aRetVal
        => system'internal's_indexof.

    /// Saves the index of the first occurence of the specified symbol into an output parameter.
    ///
    /// If the substring is not found. returns -1
    #method indexOf &index:anIndex &char:aChar &vint:aRetVal
        => system'internal's_indexof_char.        

    /// Returns the index of the first occurence of the specified object.
    /// 
    /// If the object is not found. returns -1
    #method indexOf :anIndex :aSearchObject
        = aSearchObject cast:%eval &to:
            {
               eval &literal:aLiteral
                  = self indexOf &index:(anIndex int) &literal:aLiteral.
    
               eval &wide:aLiteral
                  = self indexOf &index:(anIndex int) &literal:(aLiteral literal).
    
               eval &char:aChar
                  = self indexOf &index:(anIndex int) &char:aChar.
            }.

    /// Returns the index of the first occurence of the specified symbol.
    /// 
    /// If the substring is not found. returns -1
    #method indexOf &index:anIndex &char:aChar
    [
        #var(type:int)aRetVal.
        $self indexOf &index:anIndex &char:aChar &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    /// Returns the index of the first occurence of the specified substring.
    ///
    /// If the substring is not found. returns -1
    #method indexOf &index:anIndex &literal:aLiteral
    [
        #var(type:int)aRetVal.
        $self indexOf &index:anIndex &literal:aLiteral &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    /// Inserts the substring into and returns the new one
    #method(type:literal) insert &index:anIndex &literal:aLiteral
        => system'internal's_insert.

    /// Inserts a character into and returns the new one
    #method(type:literal) insert &index:anIndex &char:aChar
        = $self insert &index:anIndex &literal:(aChar literal).

    /// Inserts an object into and returns the new one
    #method insert : anIndex : anObject
        = anObject cast:%eval &to:
            {
               eval &literal:aLiteral
                  = self insert &index:(anIndex int) &literal:aLiteral.
    
               eval &wide:aLiteral
                  = self insert &index:(anIndex int) &literal:(aLiteral literal).
    
               eval &char:aChar
                  = self insert &index:(anIndex int) &char:aChar.
            }.

    /// Deletes the substring from the object and returns the new one
    #method(type:literal) delete &index:anIndex &length:aLength
        => system'internal's_delete.

    /// Returns the substring
    #method(type:literal) Substring &index:anIndex &length:aLength
        => system'internal's_subs.

    /// Reads the literal length into an output parameter
    #method readLength &vint:aLength
        => system'internal's_getlen.

    /// Saves the literal into a UTF-32 character array
    #method save &index:anIndex &chararray:anArray &vlength:aRetVal
        => system'internal's_save.

    #method $save &index:anIndex &bytearray:anArray &length:aRetVal
        => system'internal's_saveraw.

    /// Returns the string length
    #method length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
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
    #method insert &index:anIndex &literal:aLiteral
    [
        (anIndex == 0)
            ? [ theValue := aLiteral add &literal:theValue. ]
            ! [ theValue := theValue insert &index:anIndex &literal:aLiteral. ].
    ]
    
    /// Inserts a character
    #method insert &index:anIndex &char:aChar
    [
        (anIndex == 0)
            ? [ theValue := aChar literal add &literal:theValue. ]
            ! [ theValue := theValue insert &index:anIndex &literal:(aChar literal). ].
    ]
    
    /// Inserts a wide literal
    #method insert &index:anIndex &wide:aLiteral
    [
        theValue := theValue insert &index:anIndex &literal:(aLiteral literal).
    ]

    /// Inserts an object at the specified position
    #method insert : anIndex : anObject
    [
        anObject cast:%eval &to:
        {
            eval &char:aChar [ self insert &index:(anIndex int) &char:aChar. ]
            
            eval &literal:aLiteral [ self insert &index:(anIndex int) &literal:aLiteral. ]
            
            eval &wide:aLiteral [ self insert &index:(anIndex int) &wide:aLiteral. ]
        }.
    ]            
                                    
    /// Deletes a sub string
    #method delete &index:anIndex &length:aLength
    [
        theValue := theValue delete &index:anIndex &length:aLength.
    ]

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
#symbol(const,type:wide) emptyWideLiteralValue = #0 wide. // !! temporal

// --- WideLiteralValue ---

/// A UTF-18 literal value
#class(sealed,widestring) WideLiteralValue :: BaseValue
{
    #field(type:shortarray)theValue.

    /// Creates the empty literal
    #constructor min
        => system'internal'ws_empty.
        
    /// Creates the literal string with specified length
    #constructor new &length:aSize 
        => system'internal'ws_create.

    /// Creates the literal string with specified length and copies the parameter value
    #constructor new &length:aSize &wide:aLiteral
        => system'internal'ws_copy.
    
    /// Creates the literal string with specified length fills with spciefied character value
    #constructor new &length:aSize &char:aChar
        => system'internal'ws_fill.

    /// Creates the literal string copy
    #constructor new &wide:aLiteral
        <= %new &length:(aLiteral length) &wide:aLiteral.
        
    /// Creates the copy of a subarray
    #constructor new &length:aSize &index:anIndex &chararray:anArray
        => system'internal'ws_copysubarr.

    #constructor $new &length:aSize &index:anIndex &shortarray:anArray
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
    #method(type:wide) add &wide : aLiteral
        => system'internal'ws_add.

    /// Concatinates the values
    #method(type:wide) add &char:aChar
        = $self add &wide:(aChar wide).

    /// Concatinates the values
    #method(type:wide) add &literal:aLiteral
        = $self add &wide:(aLiteral wide).

    /// Concatinates the values
    #method add : anObject
        = anObject cast:%add &to:$self.

    /// Returns the character at specified index
    #method getAt : aParam
        <= %getAt &index:(aParam int).

    // Reads the character at the specified index and saves into an output parameter
    #method read &index:anIndex &vchar:aChar
        => system'internal'ws_getat.

    // Reads the short integer at the specified index and saves into an output parameter
    #method read &index:anIndex &vshort:aValue
        => system'internal'sa_readword.
        
    /// Returns the character at specified index
    #method(type:char) getAt &index:anIndex
    [
        #var(type:char)aChar.
        $self read &index:anIndex &vchar:aChar.
        
        ^ CharValue new &char:aChar.
    ]

    /// Saves the index of the first occurence of the specified substring into an output parameter.
    ///
    /// If the substring is not found. returns -1
    #method indexOf &index:anIndex &wide:aLiteral &vint:aRetVal
        => system'internal'ws_indexof.

    /// Saves the index of the first occurence of the specified character into an output parameter.
    ///
    /// If the substring is not found. returns -1
    #method indexOf &index:anIndex &char:aChar &vint:aRetVal
        => system'internal'ws_indexof_char.

    /// Returns the index of the first occurence of the specified character.
    ///
    /// If the substring is not found. returns -1
    #method indexOf &index:anIndex &char:aChar
    [
        #var(type:int)aRetVal.
        $self indexOf &index:anIndex &char:aChar &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    /// Returns the index of the first occurence of the specified substring.
    ///
    /// If the substring is not found. returns -1
    #method indexOf &index:anIndex &wide:aLiteral
    [
        #var(type:int)aRetVal.
        $self indexOf &index:anIndex &wide:aLiteral &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    /// Returns the index of the first occurence of the specified object.
    ///
    /// If the substring is not found. returns -1
    #method indexOf : anIndex : anObject
        = anObject cast:%eval &to:
            {
                eval &literal:aLiteral = self indexOf &index:(anIndex int) &wide:(aLiteral wide).
                
                eval &wide:aLiteral = self indexOf &index:(anIndex int) &wide:aLiteral.
                
                eval &char:aChar = self indexOf &index:(anIndex int) &char:aChar.
            }.

    /// Inserts the substring into and returns the new one
    #method(type:wide) insert &index:anIndex &wide:aLiteral
        => system'internal'ws_insert.

    /// Inserts the substring into and returns the new one
    #method(type:wide) insert &index:anIndex &literal:aLiteral
        = $self insert &index:anIndex &wide:(aLiteral wide).

    /// Inserts a character into and returns the new one
    #method(type:wide) insert &index:anIndex &char:aChar
        = $self insert &index:anIndex &wide:(aChar wide).

    /// Inserts an object into and returns the new one
    #method insert : anIndex : anObject
        = anObject cast:%eval &to:
            {
               eval &wide:aLiteral
                  = self insert &index:(anIndex int) &wide:aLiteral.
    
               eval &literal:aLiteral
                  = self insert &index:(anIndex int) &wide:(aLiteral wide).
    
               eval &char:aChar
                  = self insert &index:(anIndex int) &char:aChar.
            }.
            
    /// Deletes the substring from the object and returns the new one            
    #method(type:wide) delete &index:anIndex &length:aLength
        => system'internal'ws_delete.

    /// Returns the substring
    #method(type:wide) Substring &index:anIndex &length:aLength
        => system'internal'ws_subs.

    /// Reads the literal length into an output parameter
    #method readLength &vint:aLength
        => system'internal'ws_getlen.

    /// Returns the string length
    #method length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]

    /// Saves the literal into a UTF-32 character array
    #method save &index:anIndex &chararray:anArray &vlength:aLength
        => system'internal'ws_save.

    #method $save &index:anIndex &shortarray:anArray &length:aRetVal
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
    #method insert &index:anIndex &wide:aLiteral
    [
        (anIndex == 0)
            ? [ theValue := aLiteral add &wide:theValue. ]
            ! [ theValue := theValue insert &index:anIndex &wide:aLiteral. ].
    ]

    /// Inserts a character    
    #method insert &index:anIndex &char:aChar
    [
        (anIndex == 0)
            ? [ theValue := aChar wide add &wide:theValue. ]
            ! [ theValue := theValue insert &index:anIndex &wide:(aChar wide). ].
    ]
    
    /// Inserts a sub string
    #method insert &index:anIndex &literal:aLiteral
    [
        theValue := theValue insert &index:anIndex &wide:(aLiteral wide).
    ]

    /// Inserts an object at the specified position
    #method insert : anIndex : anObject
    [
        anObject cast:%eval &to:
        {
            eval &char:aChar [ self insert &index:(anIndex int) &char:aChar. ]
            
            eval &literal:aLiteral [ self insert &index:(anIndex int) &literal:aLiteral. ]
            
            eval &wide:aLiteral [ self insert &index:(anIndex int) &wide:aLiteral. ]
        }.
    ]            
        
    /// Deletes a sub string                
    #method delete &index:anIndex &length:aLength
    [
        theValue := theValue delete &index:anIndex &length:aLength.
    ]
                        
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
        charConvertor $convert &index:0 &literal:theValue &vchar:retVal.
        
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

    /// Returns a literal value
    #method literal = theValue.
    
    /// Returns a wide literal value
    #method wide = theValue wide.
}

// --- BaseArray ---

#class(dynamic) BaseArray
{
    #constructor new &length:aLength
        => system'internal'arr_create.
}

// --- Array ---

#class(sealed) Array :: BaseArray
{
    #constructor min
        => system'internal'arr_empty.
    
    #constructor new &length:aLength
        => system'internal'arr_create.
    
    #constructor new &length:aLength &array:anArray
        <= (%new &length:aLength)
    [
        #var(type:int)anIndex := 0.
        #loop (anIndex < aLength) ?
        [
            $self setAt &index:anIndex &object:(anArray getAt &index:anIndex).
            
            anIndex := anIndex + 1.
        ].
    ]
    
    #constructor new &length:aLength &index:anIndex &array:anArray
        <= (%new &length:aLength)
    [
        #var(type:int)i := 0.
        #loop (i < aLength) ?
        [
            $self setAt &index:i &object:(anArray getAt &index:(anIndex + i)).
            
            i := i + 1.
        ].
    ]
    
    #constructor new &array:anArray
        <= %new &length:(anArray length) &array:anArray.
        
    #method getAt : anIndex
        = $self getAt &index:(anIndex int).

    #method getAt &index:anIndex
        => system'internal'arr_getat.

    #method setAt:anIndex:anObject
    [
        $self setAt &index:(anIndex int) &object:anObject.
    ]
    
    #method setAt &index:anIndex &object:anObject
        => system'internal'arr_setat.
    
    #method add : anObject
        = $self add &array:(anObject array).

    #method add &array:anArray
    [
        #var(type:int)aLength1.
        #var(type:int)aLength2.
        
        $self readLength &vint:aLength1.        
        anArray readLength &vint:aLength2.
        
        #var(type:array)aRetVal := Array new &length:(aLength1 + aLength2).

        #var(type:int)anIndex := 0.
        #loop (anIndex < aLength1) ?
        [
            aRetVal setAt &index:anIndex &object:($self getAt &index:anIndex).
            
            anIndex := anIndex + 1.
        ].

        anIndex := 0.
        #loop (anIndex < aLength2) ?
        [
            aRetVal setAt &index:(aLength1 + anIndex) &object:(anArray getAt &index:anIndex).
            
            anIndex := anIndex + 1.
        ].
            
        ^ aRetVal.
    ]

    #method array = $self.

    #method readLength &vint:aLength
        => system'internal'arr_getlen.

    #method length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]

    #method enumerator = ArrayEnumerator new &array:$self.

    #method iterator = ArrayIterator new &array:$self.

    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &array:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &array:$self.

    #method clone 
        = Array new &array:$self.
}

// --- OpenArgs ---

#class(sealed,dynamic)OpenArgs
{
    #method readLength &vint:aLength
        => system'internal'args_getlen.
        
    #method length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]
}

// === Enumerators / Indexers

// --- LiteralEnumerator ---

#class LiteralEnumerator :: Enumerator
{
    #field(type:literal) theLiteral.
    #field(type:vint)    theIndex.
    #field(type:vint)    theDiff.
    #field(type:char)    theChar.
    
    #constructor new &literal:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := Integer new &int:(-1).
        theDiff := Integer new &int:1.
        theChar := #0.
    ]

    #method reset
    [
        theIndex write &int:-1.
        theDiff write &int:1.
    ]

    #method next
    [
        #var(type:int)aLength.
        
        theLiteral readLength &vint:aLength.
        
        theIndex += theDiff.
        
        (theIndex < aLength)
            ? [
                theChar := theLiteral getAt &index:theIndex.                
                
                theChar readLength &vint:theDiff.
                
                ^ true.
            ].

        theDiff write &int:0.
        theChar := #0.

        ^ false.
    ]
    
    #method get = theChar.
}

// --- LiteralIterator ---

#class LiteralIterator :: Iterator
{
    #field(type:literal) theLiteral.
    #field(type:vint)    theIndex.
    
    #constructor new &literal:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := Integer new.
    ]
    
    #method available
    [
        #var(type:int)aLength.
        theLiteral readLength &vint:aLength.
        
        ^ theIndex < aLength.
    ]

    #method get
    [
        #var(type:char)aChar.
        theLiteral read &index:theIndex &vchar:aChar.
        
        ^ CharValue new &char:aChar.
    ]
    
    #method set : anObject 
    [
        #throw system'NotSupportedException new.
    ]
    
    #method readIndex &vint:anIndex
    [
        anIndex << theIndex.
    ]
    
    #method write &index:anIndex 
    [
        theIndex write &int:anIndex.
    ]
}

// --- LiteralEnumerator ---

#class WideLiteralEnumerator :: Enumerator
{
    #field(type:wide) theLiteral.
    #field(type:vint) theIndex.
    #field(type:vint) theDiff.
    #field(type:char) theChar.
    
    #constructor new &wide:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := Integer new &int:(-1).
        theDiff := Integer new &int:1.
        theChar := #0.
    ]

    #method reset
    [
        theIndex write &int:-1.
        theDiff write &int:1.
    ]

    #method next
    [
        #var(type:int)aLength.
        
        theLiteral readLength &vint:aLength.

        theIndex += theDiff.
        
        (theIndex < aLength)
            ? [
                theChar := theLiteral getAt &index:theIndex.
                
                theChar readLength &vint:theDiff.
                theDiff += 1.
                theDiff /= 2.
                
                ^ true.
            ].

        theDiff write &int:0.
        theChar := #0.

        ^ false.
    ]
    
    #method get = theChar.
}

// --- WideLiteralIterator ---

#class WideLiteralIterator :: Iterator
{
    #field(type:wide) theLiteral.
    #field(type:vint) theIndex.
    
    #constructor new &wide:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := Integer new.
    ]
    
    #method available
    [
        #var(type:int)aLength.
        theLiteral readLength &vint:aLength.
        
        ^ theIndex < aLength.
    ]

    #method get
        = theLiteral getAt &index:theIndex.
    
    #method set : anObject 
    [
        #throw system'NotSupportedException new.
    ]
    
    #method readIndex &vint:anIndex
    [
        anIndex << theIndex.
    ]
    
    #method write &index:anIndex 
    [
        theIndex write &int:anIndex.
    ]
}

// --- ArrayIterator ---

#class ArrayIterator :: Iterator
{
    #field(type:array) theArray.
    #field(type:vint)  theIndex.

    #constructor new &array:anArray
    [
        theArray := anArray.
        theIndex := Integer new.
    ]
    
    #method available
    [
        #var(type:int)aLength.
        theArray readLength &vint:aLength.
        
        ^ theIndex < aLength.
    ]

    #method get
        = theArray getAt &index:theIndex.
    
    #method set : anObject
    [
        theArray setAt &index:theIndex &object:anObject.
    ]
    
    #method readIndex  &vint:anIndex
    [
        anIndex << theIndex.
    ]
    
    #method write &index:anIndex 
    [
        theIndex write &int:anIndex.
    ]
}

// --- ArrayEnumerator ---

#class ArrayEnumerator :: Enumerator
{
    #field(type:array) theArray.
    #field(type:vint)  theIndex.
    
    #constructor new &array:anArray
    [
        theArray := anArray.
        theIndex := Integer new &int:(-1).
    ]

    #method reset
    [
        theIndex write &int:-1.
    ]

    #method next
    [
        #var(type:int)aLength.
        theArray readLength &vint:aLength.
        
        theIndex append &int:1.

        ^ theIndex < aLength.
    ]
    
    #method get
        = theArray getAt &index:theIndex.
}
