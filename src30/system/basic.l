// === basic data classes ===

// --- BaseValue ---

/// A base value
#class BaseValue
{
    /// Compares the values
    #method equal : anObject = anObject cast:%equal &to:$self bool.

    /// Compares the values
    #method(type:bool) less : anObject = anObject cast:%less &to:$self bool.

    /// Compares the values
    #method(type:bool) greater : anObject = anObject less:$self bool.
}

// --- BaseNumber ---

/// A base numeric value
#class BaseNumber :: BaseValue
{
    /// Adds the value
    #method add : anObject = anObject cast:%add &to:$self.

    /// Subtracts the value
    #method subtract : anObject = anObject cast:%subtract &to:$self.

    /// Multiply by the value
    #method multiply : anObject = anObject cast:%multiply &to:$self.

    /// Divides into the value
    #method divide : anObject = anObject cast:%divide &to:$self.
}

// --- IntBaseNumber ---

/// A base integer value
#class IntBaseNumber :: BaseNumber
{
    /// Execute bitwise AND operation
    #method and : anObject = anObject cast:%and &to:$self.
    
    /// Execute bitwise OR operation
    #method or : anObject = anObject cast:%or &to:$self.
    
    /// Execute bitwise XOR operation
    #method xor : anObject = anObject cast:%xor &to:$self.
    
    /// Returns true if all the mask bits are set
    #method allMask : anOperand
        = $self and:anOperand == anOperand.
        
    /// Returns true if any of the mask bits are set    
    #method anyMask : anOperand
        = 0 != $self and:anOperand.
        
    /// Returns the negated value        
    #method negative = 0 subtract:$self.
}

// --- IntBaseVariable ---

/// A base integer variable
#class IntBaseVariable :: IntBaseNumber
{
    /// Assigns the numeric value
    #method write : anOperand
    [
        anOperand cast:%write &to:$self.
    ]
    
    /// Appends the numeric value
    #method append : anOperand
    [
        anOperand cast:%append &to:$self.
    ]
    
    /// Subtracts the numeric value and reassigns the value
    #method reduce : anOperand
    [
        anOperand cast:%reduce &to:$self.
    ]
    
    /// Multiplies by the numeric value and reassigns the value
    #method multiplyBy : anOperand
    [
        anOperand cast:%multiplyBy &to:$self.
    ]
    
    /// Divides into the numeric value and reassigns the value
    #method divideInto : anOperand
    [
        anOperand cast:%divideInto &to:$self.
    ]
}

// --- BaseNumber ---

/// A base numeric variable
#class BaseVariable :: BaseNumber
{
    /// Assigns the value
    #method write : anOperand
    [
        anOperand cast:%write &to:$self.
    ]
    
    /// Appends the value
    #method append : anOperand
    [
        anOperand cast:%append &to:$self.
    ]
    
    /// Subtracts the value and reassigns the value
    #method reduce : anOperand
    [
        anOperand cast:%reduce &to:$self.
    ]
    
    /// Multiplies by the value and reassigns the value
    #method multiplyBy : anOperand
    [
        anOperand cast:%multiplyBy &to:$self.
    ]
    
    /// Divides into the value and reassigns the value
    #method divideInto : anOperand
    [
        anOperand cast:%divideInto &to:$self.
    ]
}

// --- ByteNumber ---

/// An unsigned 8 bit integer
#class(sealed,integer:1) ByteNumber :: IntBaseNumber
{
    /// Returns the minimal value (0)
    #constructor min
        => system'internal'byte_min.
    
    /// Returns the maxial value (255)
    #constructor max
        => system'internal'byte_max.
    
    /// Creates the object with a specified value
    #constructor new &byte:anOperand
        => system'internal'int_copy.

    /// Creates the object with a specified value
    #constructor new : aValue
        <= new &byte:(aValue byte).

    /// Creates the object with a default value (0)
    #constructor new
        => system'internal'int_init.

    /// Returns itself
    #method byte = $self.

    /// Returns the value as a short integer
    #method short = ShortNumber new &byte:$self.

    /// Returns the value as an integer
    #method int = IntNumber new &byte:$self.

    /// Returns the value as an integer
    #method index = IntNumber new &byte:$self.

    /// Returns the value as an integer
    #method length = IntNumber new &byte:$self.

    /// Returns the value as a long integer
    #method long = LongNumber new &byte:$self.

    /// Returns the value as a real number
    #method real = RealNumber new &byte:$self.

    #method(type:bool) equal &byte:anOperand
        => system'internal'int_equal.
        
    #method(type:bool) less &byte:anOperand
        => system'internal'int_less.

    #method(type:bool) equal &short:anOperand
        => system'internal'int_equal.
        
    #method(type:bool) less &short:anOperand
        = ShortNumber new &byte:$self less &short:anOperand.

    /// Compares the value
    #method(type:bool) equal &int:anOperand
        => system'internal'int_equal.
        
    /// Compares the value
    #method(type:bool) less &int:anOperand
        = IntNumber new &byte:$self less &int:anOperand.

    /// Compares the value
    #method(type:bool) equal &long:anOperand
        = LongNumber new &byte:$self equal &long:anOperand.
        
    /// Compares the value
    #method(type:bool) less &long:anOperand
        = LongNumber new &byte:$self less &long:anOperand.

    /// Compares the value
    #method(type:bool) equal &real:anOperand
        = RealNumber new &byte:$self equal &real:anOperand.
        
    /// Compares the value
    #method(type:bool) less &real:anOperand
        = RealNumber new &byte:$self less &real:anOperand.

    /// Returns the sum
    #method(type:byte) add &byte:anOperand
        => system'internal'byte_add.

    /// Returns the difference
    #method(type:byte) subtract &byte:anOperand
        => system'internal'byte_sub.

    /// Returns the product
    #method(type:byte) multiply &byte:anOperand
        => system'internal'byte_mul.

    /// Returns the result of integer division
    #method(type:byte) divide &byte:anOperand
        => system'internal'byte_div.

    /// Returns the sum
    #method add &short:anOperand
        = ShortNumber new &byte:$self add &short:anOperand.

    /// Returns the difference
    #method subtract &short:anOperand
        = ShortNumber new &byte:$self subtract &short:anOperand.

    /// Returns the product
    #method multiply &short:anOperand
        = ShortNumber new &byte:$self multiply &short:anOperand.

    /// Returns the result of integer division
    #method divide &short:anOperand
        = ShortNumber new &byte:$self divide &short:anOperand.

    /// Returns the sum
    #method add &int:anOperand
        = IntNumber new &byte:$self add &int:anOperand.

    /// Returns the difference
    #method subtract &int:anOperand
        = IntNumber new &byte:$self subtract &int:anOperand.

    /// Returns the product
    #method multiply &int:anOperand
        = IntNumber new &byte:$self multiply &int:anOperand.

    /// Returns the result of integer division
    #method divide &int:anOperand
        = IntNumber new &byte:$self divide &int:anOperand.

    /// Returns the sum
    #method add &long:anOperand
        = LongNumber new &byte:$self add &long:anOperand.

    /// Returns the difference
    #method subtract &long:anOperand
        = LongNumber new &byte:$self subtract &long:anOperand.

    /// Returns the product
    #method multiply &long:anOperand
        = LongNumber new &byte:$self multiply &long:anOperand.

    /// Returns the result of integer division
    #method divide &long:anOperand
        = LongNumber new &byte:$self divide &long:anOperand.

    /// Returns the sum
    #method add &real:anOperand
        = RealNumber new &byte:$self add &real:anOperand.

    /// Returns the difference
    #method subtract &real:anOperand
        = RealNumber new &byte:$self subtract &real:anOperand.

    /// Returns the product
    #method multiply &real:anOperand
        = RealNumber new &byte:$self multiply &real:anOperand.

    /// Returns the result of integer division
    #method divide &real:anOperand
        = RealNumber new &byte:$self divide &real:anOperand.

    /// Returns the result of bitwise AND operation
    #method(type:byte) and &byte:anOperand
        => system'internal'byte_and.

    /// Returns the result of bitwise OR operation
    #method(type:byte) or &byte:anOperand
        => system'internal'byte_or.

    /// Returns the result of bitwise XOR operation
    #method(type:byte) xor &byte:anOperand
        => system'internal'byte_xor.

    /// Returns the result of bitwise AND operation
    #method and &short:anOperand
        = ShortNumber new &byte:$self and &short:anOperand.

    /// Returns the result of bitwise OR operation
    #method or &short:anOperand
        = ShortNumber new &byte:$self or &short:anOperand.

    /// Returns the result of bitwise XOR operation
    #method xor &short:anOperand
        = ShortNumber new &byte:$self xor &short:anOperand.

    /// Returns the result of bitwise AND operation
    #method(type:int) and &int:anOperand
        => system'internal'int_and.

    /// Returns the result of bitwise OR operation
    #method(type:int) or &int:anOperand
        => system'internal'int_or.

    /// Returns the result of bitwise XOR operation
    #method(type:int) xor &int:anOperand
        => system'internal'int_xor.

    /// Returns the result of bitwise AND operation
    #method(type:long) and &long:anOperand
        => system'internal'long_and.

    /// Returns the result of bitwise OR operation
    #method(type:long) or &long:anOperand
        => system'internal'long_or.

    /// Returns the result of bitwise XOR operation
    #method(type:long) xor &long:anOperand
        => system'internal'long_xor.

    /// Bitwise shift operation
    #method(type:byte) shift &index:anOrder
        => system'internal'byte_shift.

    /// Returns the literal presentation
    #method literal = literalConvertor convert &byte:$self.

    /// Returns the literal presentation
    #method wide = wideConvertor convert &byte:$self.

    /// Bitwise inversion
    #method(type:byte) not
        => system'internal'byte_not.

    /// Clones the value
    #method clone = ByteNumber new &byte:$self.
    
    /// Dispatches the verb to the target object with byte subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &byte:$self.
    
    /// Dispatches the verb to the target object with byte subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &byte:$self.

    // Reads the object hash code into the output parameter
    #method readHashCode &vint:aCode => system'internal'int_hashcode.
    
    #method hash_code
    [
        #var(type:int)aCode.
        $self readHashCode &vint:aCode.
        
        ^ IntNumber new &int:aCode.
    ]
}

// --- ShortNumber ---

/// A signed 16 bit integer
#class(sealed,integer:2) ShortNumber :: IntBaseNumber
{
    /// Returns the minimal value (-32768)
    #constructor min
        => system'internal'short_min.
    
    /// Returns the maxial value (32767)
    #constructor max
        => system'internal'short_max.
    
    /// Creates the object with specified value
    #constructor new &byte:anOperand
        => system'internal'int_copy.
        
    /// Creates the object with specified value
    #constructor new &short:anOperand
        => system'internal'int_copy.

    /// Creates the object with a default value (0)
    #constructor new
        => system'internal'int_init.
        
    /// Creates the object with specified value
    #constructor new : anObject
        <= new &short:(anObject short).
        
    /// Returns the byte value        
    #method byte = byteConvertor convert &short:$self.
                
    /// Returns itself
    #method short = $self.
    
    /// Returns the integer value
    #method int = IntNumber new &short:$self.
    
    /// Returns the long integer value
    #method long = LongNumber new &short:$self.
    
    /// Returns the value as a real number
    #method real = RealNumber new &short:$self.
    
    /// Returns the integer value
    #method index = IntNumber new &short:$self.
    
    /// Returns the integer value
    #method length = IntNumber new &short:$self.
    
    /// Compares the value
    #method(type:bool) equal &int:anOperand 
        => system'internal'int_equal.
        
    /// Compares the value
    #method(type:bool) less &int:anOperand
        => system'internal'int_less.
    
    /// Compares the value
    #method(type:bool) equal &short:anOperand 
        => system'internal'int_equal.
        
    /// Compares the value
    #method(type:bool) less &short:anOperand
        => system'internal'int_less.
    
    /// Compares the value
    #method(type:bool) equal &byte:anOperand 
        => system'internal'int_equal.
        
    /// Compares the value
    #method(type:bool) less &byte:anOperand
        => system'internal'int_less.
    
    /// Compares the value
    #method(type:bool) equal &long:anOperand 
        = LongNumber new &short:$self equal &long:anOperand.
        
    /// Compares the value
    #method(type:bool) less &long:anOperand
        = LongNumber new &short:$self less &long:anOperand.
    
    /// Compares the value
    #method(type:bool) equal &real:anOperand 
        = RealNumber new &short:$self equal &real:anOperand.
        
    /// Compares the value
    #method(type:bool) less &real:anOperand
        = RealNumber new &short:$self less &real:anOperand.

    /// Returns the sum
    #method(type:short) add &short:anOperand
        => system'internal'short_add.
    
    /// Returns the difference
    #method(type:short) subtract &short:anOperand
        => system'internal'short_sub.

    /// Returns the product
    #method(type:short) multiply &short:anOperand
        => system'internal'short_mul.

    /// Returns the result of integer division
    #method(type:short) divide &short:anOperand
        => system'internal'short_div.

    /// Returns the sum
    #method add &byte:anOperand
        = $self add &short:(ShortNumber new &byte:anOperand).

    /// Returns the difference
    #method subtract &byte:anOperand
        = $self subtract &short:(ShortNumber new &byte:anOperand).

    /// Returns the product
    #method multiply &byte:anOperand
        = $self multiply &short:(ShortNumber new &byte:anOperand).

    /// Returns the result of integer division
    #method divide &byte:anOperand
        = $self divide &short:(ShortNumber new &byte:anOperand).

    /// Returns the sum
    #method add &int:anOperand
        = IntNumber new &short:$self add &int:anOperand.

    /// Returns the difference
    #method subtract &int:anOperand
        = IntNumber new &short:$self subtract &int:anOperand.

    /// Returns the product
    #method multiply &int:anOperand
        = IntNumber new &short:$self multiply &int:anOperand.
        
    /// Returns the result of integer division
    #method divide &int:anOperand
        = IntNumber new &short:$self divide &int:anOperand.

    /// Returns the sum
    #method add &long:anOperand
        = LongNumber new &short:$self add &long:anOperand.

    /// Returns the difference
    #method subtract &long:anOperand
        = LongNumber new &short:$self subtract &long:anOperand.

    /// Returns the product
    #method multiply &long:anOperand
        = LongNumber new &short:$self multiply &long:anOperand.
        
    /// Returns the result of integer division
    #method divide &long:anOperand
        = LongNumber new &short:$self divide &long:anOperand.

    /// Returns the sum
    #method add &real:anOperand
        = RealNumber new &short:$self add &real:anOperand.

    /// Returns the difference
    #method subtract &real:anOperand
        = RealNumber new &short:$self subtract &real:anOperand.

    /// Returns the product
    #method multiply &real:anOperand
        = RealNumber new &short:$self multiply &real:anOperand.
        
    /// Returns the result of integer division
    #method divide &real:anOperand
        = RealNumber new &short:$self divide &real:anOperand.

    /// Returns the result of bitwise AND operation
    #method(type:short) and &short:anOperand
        => system'internal'short_and.

    /// Returns the result of bitwise OR operation
    #method(type:short) or &short:anOperand
        => system'internal'short_or.

    /// Returns the result of bitwise XOR operation
    #method(type:short) xor &short:anOperand
        => system'internal'short_xor.

    /// Returns the result of bitwise AND operation
    #method and &byte:anOperand
        => system'internal'short_and.

    /// Returns the result of bitwise OR operation
    #method or &byte:anOperand
        => system'internal'short_or.

    /// Returns the result of bitwise XOR operation
    #method xor &byte:anOperand
        => system'internal'short_xor.
        
    /// Returns the result of bitwise AND operation
    #method and &int:anOperand
        = IntNumber new &short:$self and &int:anOperand.        
        
    /// Returns the result of bitwise OR operation
    #method or &int:anOperand
        = IntNumber new &short:$self or &int:anOperand.        
        
    /// Returns the result of bitwise XOR operation
    #method xor &int:anOperand
        = IntNumber new &short:$self xor &int:anOperand.        
        
    /// Returns the result of bitwise AND operation
    #method and &long:anOperand
        = LongNumber new &short:$self and &long:anOperand.        
        
    /// Returns the result of bitwise OR operation
    #method or &long:anOperand
        = LongNumber new &short:$self or &long:anOperand.        
        
    /// Returns the result of bitwise XOR operation
    #method xor &long:anOperand
        = LongNumber new &short:$self xor &long:anOperand.        

    /// Bitwise shift operation
    #method(type:short) shift &index:anOrder
        => system'internal'short_shift.

    /// Returns the literal presentation
    #method literal = literalConvertor convert &short:$self.

    /// Returns the literal presentation
    #method wide = wideConvertor convert &short:$self.

    /// Bitwise inversion
    #method(type:byte) not
        => system'internal'short_not.

    /// Clones the value
    #method clone = ShortNumber new &short:$self.    
                
    /// Dispatches the verb to the target object with short subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &short:$self.
    
    /// Dispatches the verb to the target object with short subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &short:$self.

    // Reads the object hash code into the output parameter
    #method readHashCode &vint:aCode => system'internal'int_hashcode.
    
    #method hash_code
    [
        #var(type:int)aCode.
        $self readHashCode &vint:aCode.
        
        ^ IntNumber new &int:aCode.
    ]
}

// --- IntNumber ---

/// A signed 32 bit integer
#class(sealed,integer:4) IntNumber :: IntBaseNumber
{
    /// Creates the object with the minimal value
    #constructor min
        => system'internal'int_min.
    
    /// Creates the object with the maximal value
    #constructor max
        => system'internal'int_max.
    
    /// Creates the object with specified value
    #constructor new &int:anOperand
        => system'internal'int_copy.

    /// Creates the object with specified value
    #constructor new &short:anOperand
        => system'internal'int_copyshort.

    /// Creates the object with specified value
    #constructor new &byte:anOperand
        => system'internal'int_copy.

    /// Creates the object with a default value (0)
    #constructor new
        => system'internal'int_init.
        
    /// Creates the object with specified value
    #constructor new : anObject
        <= new &int:(anObject int).

    /// Returns itself
    #method int = $self.
    
    /// Returns itself
    #method index = $self.
    
    /// Returns itself
    #method length = $self.

    /// Returns the value as a short integer    
    #method short = shortConvertor convert &int:$self.

    /// Returns the byte value    
    #method byte = byteConvertor convert &int:$self.

    /// Returns the long integer
    #method long = LongNumber new &int:$self.

    /// Returns the real number
    #method real = RealNumber new &int:$self.

    /// Compares the integer value
    #method(type:bool) equal &int:anOperand 
        => system'internal'int_equal.
        
    /// Compares the integer value
    #method(type:bool) equal &short:anOperand 
        => system'internal'int_equal.
        
    /// Compares the integer value
    #method(type:bool) equal &byte:anOperand 
        => system'internal'int_equal.
        
    /// Compares the integer value
    #method equal &long:anOperand 
        = LongNumber new &int:$self equal &long:anOperand.

    /// Compares the integer value
    #method equal &real:anOperand 
        = RealNumber new &int:$self equal &real:anOperand.

    /// Compares the integer value
    #method(type:bool) less &int:anOperand
        => system'internal'int_less.

    /// Compares the integer value
    #method(type:bool) less &short:anOperand
        => system'internal'int_less.

    /// Compares the integer value
    #method(type:bool) less &byte:anOperand
        => system'internal'int_less.
        
    /// Compares the integer value
    #method less &long:anOperand 
        = LongNumber new &int:$self less &long:anOperand.

    /// Compares the integer value
    #method less &real:anOperand 
        = RealNumber new &int:$self less &real:anOperand.

    /// Returns the sum of integers
    #method(type:int) add &int:anOperand
        => system'internal'int_add.
        
    /// Returns the sum of integers
    #method(type:int) add &short:anOperand
        => system'internal'int_add.

    /// Returns the sum of integers
    #method(type:int) add &byte:anOperand
        => system'internal'int_add.

    /// Returns the sum of integers
    #method add &long:anOperand
        = LongNumber new &int:$self add &long:anOperand.

    /// Returns the sum of integers
    #method add &real:anOperand
        = RealNumber new &int:$self add &real:anOperand.

    /// Returns the difference of integers
    #method(type:int) subtract &int:anOperand
        => system'internal'int_sub.

    /// Returns the difference of integers
    #method(type:int) subtract &short:anOperand
        => system'internal'int_sub.

    /// Returns the difference of integers
    #method(type:int) subtract &byte:anOperand
        => system'internal'int_sub.

    /// Returns the difference of integers
    #method subtract &long:anOperand
        = LongNumber new &int:$self subtract &long:anOperand.

    /// Returns the difference of integers
    #method subtract &real:anOperand
        = RealNumber new &int:$self subtract &real:anOperand.

    /// Returns the product of integers
    #method(type:int) multiply &int:anOperand
        => system'internal'int_mul.
        
    /// Returns the product of integers
    #method(type:int) multiply &short:anOperand
        => system'internal'int_mul.

    /// Returns the product of integers
    #method(type:int) multiply &byte:anOperand
        => system'internal'int_mul.

    /// Returns the product of integers
    #method multiply &long:anOperand
        = LongNumber new &int:$self multiply &long:anOperand.

    /// Returns the product of integers
    #method multiply &real:anOperand
        = RealNumber new &int:$self multiply &real:anOperand.

    /// Returns the result of integer division
    #method(type:int) divide &int:anOperand
        => system'internal'int_div.

    /// Returns the result of integer division
    #method(type:int) divide &short:anOperand
        => system'internal'int_div.

    /// Returns the result of integer division
    #method(type:int) divide &byte:anOperand
        => system'internal'int_div.

    /// Returns the result of integer division
    #method divide &long:anOperand
        = LongNumber new &int:$self divide &long:anOperand.

    /// Returns the result of integer division
    #method divide &real:anOperand
        = RealNumber new &int:$self divide &real:anOperand.

    /// Returns the result of bitwise AND operation
    #method(type:int) and &int:anOperand
        => system'internal'int_and.

    /// Returns the result of bitwise AND operation
    #method(type:int) and &short:anOperand
        => system'internal'int_and.

    /// Returns the result of bitwise AND operation
    #method(type:int) and &byte:anOperand
        => system'internal'int_and.

    /// Returns the result of bitwise AND operation
    #method and &long:anOperand
        = LongNumber new &int:$self and &long:anOperand.

    /// Returns the result of bitwise OR operation
    #method(type:int) or &int:anOperand
        => system'internal'int_or.

    /// Returns the result of bitwise OR operation
    #method(type:int) or &short:anOperand
        => system'internal'int_or.

    /// Returns the result of bitwise OR operation
    #method(type:int) or &byte:anOperand
        => system'internal'int_or.

    /// Returns the result of bitwise OR operation
    #method or &long:anOperand
        = LongNumber new &int:$self or &long:anOperand.

    /// Returns the result of bitwise XOR operation
    #method(type:int) xor &int:anOperand
        => system'internal'int_xor.

    /// Returns the result of bitwise XOR operation
    #method(type:int) xor &short:anOperand
        => system'internal'int_xor.

    /// Returns the result of bitwise XOR operation
    #method(type:int) xor &byte:anOperand
        => system'internal'int_xor.

    /// Returns the result of bitwise XOR operation
    #method xor &long:anOperand
        = LongNumber new &int:$self xor &long:anOperand.

    /// Bitwise shift operation
    #method(type:int) shift &index:anOrder
        => system'internal'int_shift.

    /// Bitwise shift operation
    #method shift &index:anOrder &vint:aResult
        => system'internal'int_shift2.

    /// Returns the literal presentation
    #method literal = literalConvertor convert &int:$self.

    /// Returns the literal presentation
    #method wide = wideConvertor convert &int:$self.

    /// Bitwise inversion
    #method(type:int) not
        => system'internal'int_not.

    /// Negates the value
    #method negative
        = 0 - $self.

    /// Clones the value
    #method clone = IntNumber new &int:$self.
    
    /// Dispatches the verb to the target object with int subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &int:$self.
    
    /// Dispatches the verb to the target object with int subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &int:$self.

    // Reads the object hash code into the output parameter
    #method readHashCode &vint:aCode => system'internal'int_hashcode.
    
    #method hash_code
    [
        #var(type:int)aCode.
        $self readHashCode &vint:aCode.
        
        ^ IntNumber new &int:aCode.
    ]
}

// --- longnumber ---

/// A signed 64 bit integer value
#class(sealed,integer:8) LongNumber :: IntBaseNumber
{
    /// Returns the minimal value
    #constructor min
        => system'internal'long_min.
    
    /// Returns the maximal value
    #constructor max
        => system'internal'long_max.
    
    /// Creates the object with a default value (0)
    #constructor new
        <= new &long:0l.

    /// Creates the object with specified value
    #constructor new &long:anOperand 
        => system'internal'long_copy.

    /// Creates the object with specified value
    #constructor new &int:anOperand 
        => system'internal'long_copyint.

    /// Creates the object with specified value
    #constructor new &short:anOperand 
        => system'internal'long_copyint.

    /// Creates the object with specified value
    #constructor new &byte:anOperand 
        => system'internal'long_copyint.
        
    /// Creates the object with specified value
    #constructor new : anObject
        <= new &long:(anObject long).

    /// Returns an integer value
    #method int = intConvertor convert &long:$self.
    
    /// Returns an integer value
    #method index = intConvertor convert &long:$self.
    
    /// Returns an integer value
    #method length = intConvertor convert &long:$self.

    /// Returns the value as a short integer    
    #method short = shortConvertor convert &long:$self.
    
    /// Returns the byte value
    #method byte = byteConvertor convert &long:$self.

    /// Returns itself
    #method long = $self.

    /// Returns a real value
    #method real = RealNumber new &long:$self.

    /// Compares the integer value
    #method(type:bool) equal &long:anOperand
        => system'internal'long_equal.

    /// Compares the integer value
    #method(type:bool) equal &int:anOperand 
        = $self equal &long:(system'LongNumber new &int:anOperand).

    /// Compares the integer value
    #method(type:bool) equal &short:anOperand 
        = $self equal &long:(system'LongNumber new &short:anOperand).

    /// Compares the integer value
    #method(type:bool) equal &byte:anOperand 
        = $self equal &long:(system'LongNumber new &byte:anOperand).

    /// Compares the integer value
    #method equal &real:anOperand 
        = RealNumber new &long:$self equal &real:anOperand.

    /// Compares the integer value
    #method(type:bool) less &long:anOperand
        => system'internal'long_less.

    /// Compares the integer value
    #method(type:bool) less &int:anOperand 
        = $self less &long:(system'LongNumber new &int:anOperand).

    /// Compares the integer value
    #method(type:bool) less &short:anOperand 
        = $self less &long:(system'LongNumber new &short:anOperand).

    /// Compares the integer value
    #method(type:bool) less &byte:anOperand 
        = $self less &long:(system'LongNumber new &byte:anOperand).

    /// Compares the integer value
    #method less &real:anOperand 
        = RealNumber new &long:$self less &real:anOperand.

    /// Returns the sum of integers
    #method(type:long) add &long:anOperand
        => system'internal'long_add.

    /// Returns the sum of integers
    #method add &int:anOperand
        = $self add &long:(system'LongNumber new &int:anOperand).

    /// Returns the sum of integers
    #method add &short:anOperand
        = $self add &long:(system'LongNumber new &short:anOperand).

    /// Returns the sum of integers
    #method add &byte:anOperand
        = $self add &long:(system'LongNumber new &byte:anOperand).

    /// Returns the sum of integers
    #method add &real:anOperand
        = RealNumber new &long:$self add &real:anOperand.

    /// Returns the difference of integers
    #method(type:long) subtract &long:anOperand
        => system'internal'long_sub.

    /// Returns the difference of integers
    #method subtract &int:anOperand
        = $self subtract &long:(system'LongNumber new &int:anOperand).

    /// Returns the difference of integers
    #method subtract &short:anOperand
        = $self subtract &long:(system'LongNumber new &short:anOperand).

    /// Returns the difference of integers
    #method subtract &byte:anOperand
        = $self subtract &long:(system'LongNumber new &byte:anOperand).

    /// Returns the difference of integers
    #method subtract &real:anOperand
        = RealNumber new &long:$self subtract &real:anOperand.

    /// Returns the product of integers
    #method(type:long) multiply &long:anOperand
        => system'internal'long_mul.

    /// Returns the product of integers
    #method multiply &real:anOperand
        = RealNumber new &long:$self multiply &real:anOperand.

    /// Returns the product of integers
    #method multiply &int:anOperand
        = $self multiply &long:(system'LongNumber new &int:anOperand).

    /// Returns the product of integers
    #method multiply &byte:anOperand
        = $self multiply &long:(system'LongNumber new &byte:anOperand).

    /// Returns the product of integers
    #method multiply &short:anOperand
        = $self multiply &long:(system'LongNumber new &short:anOperand).

    /// Returns the result of integer division
    #method(type:long) divide &long:anOperand
        => system'internal'long_div.

    /// Returns the result of integer division
    #method divide &int:anOperand
        = $self divide &long:(system'LongNumber new &int:anOperand).

    /// Returns the result of integer division
    #method divide &short:anOperand
        = $self divide &long:(system'LongNumber new &short:anOperand).

    /// Returns the result of integer division
    #method divide &byte:anOperand
        = $self divide &long:(system'LongNumber new &byte:anOperand).

    /// Returns the result of integer division
    #method divide &real:anOperand
        = RealNumber new &long:$self divide &real:anOperand.
        
    /// Returns the result of bitwise AND operation
    #method(type:long) and &long:anOperand
        => system'internal'long_and.

    /// Returns the result of bitwise AND operation
    #method and &int:anOperand
        = $self and &long:(system'LongNumber new &int:anOperand).

    /// Returns the result of bitwise AND operation
    #method and &short:anOperand
        = $self and &long:(system'LongNumber new &short:anOperand).

    /// Returns the result of bitwise AND operation
    #method and &byte:anOperand
        = $self and &long:(system'LongNumber new &byte:anOperand).

    /// Returns the result of bitwise OR operation
    #method(type:long) or &long:anOperand
        => system'internal'long_or.

    /// Returns the result of bitwise OR operation
    #method or &int:anOperand
        = $self or &long:(system'LongNumber new &int:anOperand).

    /// Returns the result of bitwise OR operation
    #method or &short:anOperand
        = $self or &long:(system'LongNumber new &short:anOperand).

    /// Returns the result of bitwise OR operation
    #method or &byte:anOperand
        = $self or &long:(system'LongNumber new &byte:anOperand).        

    /// Returns the result of bitwise XOR operation
    #method(type:long) xor &long:anOperand
        => system'internal'long_xor.

    /// Returns the result of bitwise XOR operation
    #method xor &int:anOperand
        = $self xor &long:(system'LongNumber new &int:anOperand).

    /// Returns the result of bitwise XOR operation
    #method xor &short:anOperand
        = $self xor &long:(system'LongNumber new &short:anOperand).

    /// Returns the result of bitwise XOR operation
    #method xor &byte:anOperand
        = $self xor &long:(system'LongNumber new &byte:anOperand).

    /// Returns the result of bitwise shift operation
    #method(type:long) shift &index:anOrder
        => system'internal'long_shift.

    /// Returns the literal presentation
    #method literal = literalConvertor convert &long:$self.

    /// Returns the literal presentation
    #method wide = wideConvertor convert &long:$self.

    /// Negates the value
    #method(type:long) negative = 0l - $self.

    /// Bitwise inversion
    #method(type:long) not
        => system'internal'long_not.

    /// Clones the value
    #method clone = LongNumber new &long:$self.
    
    /// Dispatches the verb to the target object with long subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &long:$self.
    
    /// Dispatches the verb to the target object with long subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &long:$self.

    // Reads the object hash code into the output parameter
    #method readHashCode &vint:aCode => system'internal'int_hashcode.
    
    #method hash_code
    [
        #var(type:int)aCode.
        $self readHashCode &vint:aCode.
        
        ^ IntNumber new &int:aCode.
    ]
}

// --- realNumber ---

/// A 64bit float numeric value
#class(sealed,floating:8) RealNumber :: BaseNumber
{
    /// Returns the minimal value
    #constructor min
        <= new &real: -1.7976931348623157e+308r.
    
    /// Returns the maximal value
    #constructor max
        <= new &real: 1.7976931348623157e+308r.
    
    /// Creates the object with a default value (0)
    #constructor new
        <= new &real:0.0r.

    /// Creates the object with specified value
    #constructor new &real:anOperand 
        => system'internal'long_copy.
  
    /// Creates the object with specified value
    #constructor new &int:anOperand 
        => system'internal'real_copyint.

    /// Creates the object with specified value
    #constructor new &short:anOperand 
        => system'internal'real_copyint.

    /// Creates the object with specified value
    #constructor new &byte:anOperand 
        => system'internal'real_copyint.

    /// Creates the object with specified value
    #constructor new &long:anOperand 
        => system'internal'real_copylong.
        
    /// Creates the object with specified value
    #constructor new : anObject
        <= new &real:(anObject real).
        
    /// Returns itself        
    #method real = $self.
    
    /// Compares the numeric values
    #method(type:bool) equal &real:anOperand
        => system'internal'real_equal.

    /// Compares the numeric values
    #method(type:bool) equal &int:anOperand
        = $self equal &real:(system'RealNumber new &int:anOperand).

    /// Compares the numeric values
    #method(type:bool) equal &byte:anOperand
        = $self equal &real:(system'RealNumber new &byte:anOperand).

    /// Compares the numeric values
    #method(type:bool) equal &short:anOperand
        = $self equal &real:(system'RealNumber new &short:anOperand).

    /// Compares the numeric values
    #method(type:bool) equal &long:anOperand
        = $self equal &real:(system'RealNumber new &long:anOperand).

    /// Compares the numeric values
    #method(type:bool) less &real:anOperand
        => system'internal'real_less.

    /// Compares the numeric values
    #method(type:bool) less &int:anOperand
        = $self less &real:(system'RealNumber new &int:anOperand).

    /// Compares the numeric values
    #method(type:bool) less &byte:anOperand
        = $self less &real:(system'RealNumber new &byte:anOperand).

    /// Compares the numeric values
    #method(type:bool) less &short:anOperand
        = $self less &real:(system'RealNumber new &short:anOperand).

    /// Compares the numeric values
    #method(type:bool) less &long:anOperand
        = $self less &real:(system'RealNumber new &long:anOperand).

    /// Returns the sum of numbers
    #method(type:real) add &real:anOperand
        => system'internal'real_add.

    /// Returns the sum of numbers
    #method(type:real) add &int:anOperand
        = $self add &real:(system'RealNumber new &int:anOperand).

    /// Returns the sum of numbers
    #method(type:real) add &byte:anOperand
        = $self add &real:(system'RealNumber new &byte:anOperand).

    /// Returns the sum of numbers
    #method(type:real) add &short:anOperand
        = $self add &real:(system'RealNumber new &short:anOperand).

    /// Returns the sum of numbers
    #method(type:real) add &long:anOperand
        = $self add &real:(system'RealNumber new &long:anOperand).

    /// Returns the difference of numbers
    #method(type:real) subtract &real:anOperand
        => system'internal'real_sub.

    /// Returns the difference of numbers
    #method(type:real) subtract &int:anOperand
        = $self subtract &real:(system'RealNumber new &int:anOperand).

    /// Returns the difference of numbers
    #method(type:real) subtract &byte:anOperand
        = $self subtract &real:(system'RealNumber new &byte:anOperand).

    /// Returns the difference of numbers
    #method(type:real) subtract &short:anOperand
        = $self subtract &real:(system'RealNumber new &short:anOperand).

    /// Returns the difference of numbers
    #method(type:real) subtract &long:anOperand
        = $self subtract &real:(system'RealNumber new &long:anOperand).

    /// Returns the product of numbers
    #method(type:real) multiply &real:anOperand
        => system'internal'real_mul.

    /// Returns the product of numbers
    #method(type:real) multiply &int:anOperand
        = $self multiply &real:(system'RealNumber new &int:anOperand).

    /// Returns the product of numbers
    #method(type:real) multiply &byte:anOperand
        = $self multiply &real:(system'RealNumber new &byte:anOperand).

    /// Returns the product of numbers
    #method(type:real) multiply &short:anOperand
        = $self multiply &real:(system'RealNumber new &short:anOperand).

    /// Returns the product of numbers
    #method(type:real) multiply &long:anOperand
        = $self multiply &real:(system'RealNumber new &long:anOperand).

    /// Returns the result of the division
    #method(type:real) divide &real:anOperand
        => system'internal'real_div.

    /// Returns the result of the division
    #method(type:real) divide &int:anOperand
        = $self divide &real:(system'RealNumber new &int:anOperand).

    /// Returns the result of the division
    #method(type:real) divide &byte:anOperand
        = $self divide &real:(system'RealNumber new &byte:anOperand).
        
    /// Returns the result of the division
    #method(type:real) divide &short:anOperand
        = $self divide &real:(system'RealNumber new &short:anOperand).

    /// Returns the result of the division
    #method(type:real) divide &long:anOperand
        = $self divide &real:(system'RealNumber new &long:anOperand).

    /// Negates the value
    #method negative = 0r - $self.

    /// Returns the literal presentation
    #method literal = literalConvertor convert &real:$self.
    
    /// Returns the literal presentation
    #method wide = wideConvertor convert &real:$self.

    /// Clones the value    
    #method clone = RealNumber new &real:$self.
    
    /// Dispatches the verb to the target object with real subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &real:$self.
    
    /// Dispatches the verb to the target object with real subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &real:$self.

    // Reads the object hash code into the output parameter
    #method readHashCode &vint:aCode => system'internal'int_hashcode.
    
    #method hash_code
    [
        #var(type:int)aCode.
        $self readHashCode &vint:aCode.
        
        ^ IntNumber new &int:aCode.
    ]
}

// --- Byte ---

/// A 8bit unsigned integer variable.
///
/// It extends system'ByteNumber class
#class(sealed,struct:embeddable) Byte :: IntBaseVariable
{
    #field(type:byte)theValue.
   
    /// Creates an integer variable with default value (0) 
    #constructor new
    [
        theValue := 0.
    ]
    
    /// Creates an integer variable with specified value
    #constructor new : aValue
    [
        theValue := aValue byte.
    ]
    
    /// Creates an integer variable with specified value
    #constructor new &byte:aValue
    [
        theValue := aValue.
    ]
    
    /// Returns itself
    #method vbyte = $self.
    
    /// Returns the byte value
    #method byte = ByteNumber new &byte:theValue.
                                                
    /// Returns the short integer value
    #method short = ShortNumber new &byte:theValue.
                                                
    /// Returns the integer value
    #method int = IntNumber new &byte:theValue.
                         
    /// Returns the value
    #method real = RealNumber new &byte:theValue.
    
    /// Returns the long integer value
    #method long = LongNumber new &byte:theValue.
    
    /// Copies the integer value
    #method write &byte:anOperand
    [
        theValue := anOperand.
    ]

    /// Reads the integer value
    #method read &vbyte:aRetVal
    [
        aRetVal << theValue.
    ]

    /// Appends the integer value
    #method append &byte:anOperand
    [
        theValue := theValue + anOperand.
    ]
        
    /// Subtracts the integer value        
    #method reduce &byte:anOperand
    [
        theValue := theValue - anOperand.
    ]
        
    /// Multiplies by the integer value        
    #method multiplyBy &byte:anOperand
    [
        theValue := theValue * anOperand.
    ]        

    /// Divides into the integer value
    #method divideInto &byte:anOperand
    [
        theValue := theValue / anOperand.
    ]
    
    /// Compares the integer value
    #method(type:bool) equal &byte:anOperand 
        = theValue == anOperand.

    /// Compares the integer value
    #method(type:bool) less &byte:anOperand
        = theValue < anOperand.

    /// Returns the sum of integers                
    #method(type:byte) add &byte:anOperand
        => system'internal'byte_add.

    /// Returns the difference of integers    
    #method(type:byte) subtract &byte:anOperand
        => system'internal'byte_sub.
            
    /// Returns the product of integers            
    #method(type:byte) multiply &byte:anOperand
        => system'internal'byte_mul.
    
    /// Returns the result of integer division
    #method(type:byte) divide &byte:anOperand
        => system'internal'byte_div.
        
    /// Returns the result of bitwise AND operation        
    #method(type:byte) and &byte:anOperand
        => system'internal'byte_and.
        
    /// Returns the result of bitwise OR operation        
    #method(type:byte) or &byte:anOperand
        => system'internal'byte_or.
        
    /// Returns the result of bitwise XOR operation        
    #method(type:byte) xor &byte:anOperand
        => system'internal'byte_xor.

    /// Returns the integer value
    #method value = ByteNumber new &byte:theValue.

    /// Returns the literal presentation
    #method literal = literalConvertor convert &byte:theValue.

    /// Returns the literal presentation
    #method wide = wideConvertor convert &byte:theValue.

    /// Clones the variable
    #method clone = Byte new &byte:theValue.
    
    /// Dispatches the verb to the target object with byte subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &byte:(ByteNumber new &byte:theValue).
    
    /// Dispatches the verb to the target object with byte subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &byte:(ByteNumber new &byte:theValue).

    /// Expands a byte integer
    #method => theValue.    
}

// --- Short ---

/// A 16bit signed integer variable.
///
/// It extends system'ShortNumber class
#class(sealed,struct:embeddable) Short :: IntBaseVariable
{
    #field(type:short)theValue.
    
    /// Creates an integer variable with default value (0) 
    #constructor new
    [
        theValue := 0.
    ]
    
    /// Creates an integer variable with specified value
    #constructor new : aValue
    [
        theValue := aValue short.
    ]
    
    /// Creates an integer variable with specified value
    #constructor new &short:aValue
    [
        theValue := aValue.
    ]
    
    /// Creates an integer variable with specified value
    #constructor new &byte:aValue
    [
        theValue := aValue short.
    ]

    /// Returns itself    
    #method vshort = $self.
    
    /// Returns the short integer value
    #method short = ShortNumber new &short:theValue.
                                                
    /// Returns the integer value
    #method int = IntNumber new &short:theValue.
                                                
    /// Returns the real number value
    #method real = RealNumber new &short:theValue.
    
    /// Returns the long integer value
    #method long = LongNumber new &short:theValue.
    
    /// Reads the integer value
    #method read &vshort:aRetVal
    [
        aRetVal << theValue.
    ]

    /// Copies the integer value    
    #method write &short:anOperand
    [
        theValue := anOperand.
    ]

    /// Copies the integer value    
    #method write &byte:anOperand
    [
        theValue := anOperand short.
    ]

    /// Appends the integer value
    #method append &short:anOperand
    [
        theValue := theValue + anOperand.
    ]

    /// Appends the integer value
    #method append &byte:anOperand
    [
        theValue := theValue + anOperand short.
    ]
        
    /// Subtracts the integer value        
    #method reduce &short:anOperand
    [
        theValue := theValue - anOperand.
    ]
        
    /// Subtracts the integer value        
    #method reduce &byte:anOperand
    [
        theValue := theValue - anOperand short.
    ]

    /// Multiplies by the integer value
    #method multiplyBy &short:anOperand
    [
        theValue := theValue * anOperand.
    ]        
        
    /// Multiplies by the integer value
    #method multiplyBy &byte:anOperand
    [
        theValue := theValue * anOperand short.
    ]        

    /// Divides into the integer value        
    #method divideInto &short:anOperand
    [
        theValue := theValue / anOperand.
    ]
        
    /// Divides into the integer value        
    #method divideInto &byte:anOperand
    [
        theValue := theValue / anOperand short.
    ]

    /// Compares the integer value    
    #method(type:bool) equal &short:anOperand 
        = theValue == anOperand.

    /// Compares the integer value    
    #method(type:bool) less &short:anOperand
        = theValue < anOperand.

    /// Returns the sum of integers                
    #method(type:short) add &short:anOperand
        => system'internal'short_add.
    
    /// Returns the difference of integers
    #method(type:short) subtract &short:anOperand
        => system'internal'short_sub.
    
    /// Returns the product of integers
    #method(type:short) multiply &short:anOperand
        => system'internal'short_mul.
    
    /// Returns the result of integer division
    #method(type:short) divide &short:anOperand
        => system'internal'short_div.
    
    /// Returns the result of bitwise AND operation
    #method(type:short) and &short:anOperand
        => system'internal'short_and.
        
    /// Returns the result of bitwise OR operation
    #method(type:short) or &short:anOperand
        => system'internal'short_or.
        
    /// Returns the result of bitwise XOR operation
    #method(type:short) xor &short:anOperand
        => system'internal'short_xor.

    /// Returns the integer value
    #method value = ShortNumber new &short:theValue.

    /// Returns the literal presentation
    #method literal = literalConvertor convert &short:theValue.

    /// Returns the literal presentation
    #method wide = wideConvertor convert &short:theValue.

    /// Clones the variable
    #method clone = Short new &short:theValue.
        
    /// Dispatches the verb to the target object with short subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &short:(ShortNumber new &short:theValue).
    
    /// Dispatches the verb to the target object with short subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &short:(ShortNumber new &short:theValue).

    /// Expands a short integer
    #method => theValue.    
}

// --- Integer ---

/// A 32bit signed integer variable. It extends system'IntNumber class
#class(sealed,struct:embeddable) Integer :: IntBaseVariable
{
    #field(type:int)theValue.
    
    /// Creates an integer variable with a default value (0)
    #constructor new
    [
        theValue := 0.
    ]
    
    /// Creates an integer variable
    #constructor new : aValue
    [
        theValue := aValue int.
    ]
    
    /// Creates an integer variable
    #constructor new &int:aValue
    [
        theValue := aValue.
    ]
    
    /// Creates an integer variable
    #constructor new &short:aValue
    [
        theValue := aValue int.
    ]
    
    /// Creates an integer variable
    #constructor new &byte:aValue
    [
        theValue := aValue int.
    ]
    
    /// Returns itself
    #method vint = $self.
    
    /// Returns the integer value
    #method int = IntNumber new &int:theValue.
                                      
    /// Returns the real number
    #method real = RealNumber new &int:theValue.
    
    /// Returns the long integer value
    #method long = LongNumber new &int:theValue.
    
    /// Copies the integer value
    #method write &int:anOperand
    [
        theValue := anOperand.
    ]

    /// Reads the integer value
    #method read &vint:aRetVal
    [
        aRetVal << theValue.
    ]

    /// Copies the integer value
    #method write &short:anOperand
    [
        theValue := anOperand int.
    ]

    /// Copies the integer value
    #method write &byte:anOperand
    [
        theValue := anOperand int.
    ]

    /// Appends the integer value
    #method append &int:anOperand
    [
        theValue := theValue + anOperand.
    ]

    /// Appends the integer value
    #method append &short:anOperand
    [
        theValue := theValue + anOperand int.
    ]

    /// Appends the integer value
    #method append &byte:anOperand
    [
        theValue := theValue + anOperand int.
    ]

    /// Reduces the integer value        
    #method reduce &int:anOperand
    [
        theValue := theValue - anOperand.
    ]
        
    /// Reduces the integer value        
    #method reduce &short:anOperand
    [
        theValue := theValue - anOperand int.
    ]
        
    /// Reduces the integer value        
    #method reduce &byte:anOperand
    [
        theValue := theValue - anOperand int.
    ]

    /// Multiplies by the integer value        
    #method multiplyBy &int:anOperand
    [
        theValue := theValue * anOperand.
    ]        
        
    /// Multiplies by the integer value        
    #method multiplyBy &short:anOperand
    [
        theValue := theValue * anOperand int.
    ]        
        
    /// Multiplies by the integer value        
    #method multiplyBy &byte:anOperand
    [
        theValue := theValue * anOperand int.
    ]        

    /// Divides into the integer value        
    #method divideInto &int:anOperand
    [
        theValue := theValue / anOperand.
    ]
        
    /// Divides into the integer value        
    #method divideInto &short:anOperand
    [
        theValue := theValue / anOperand int.
    ]
        
    /// Divides into the integer value        
    #method divideInto &byte:anOperand
    [
        theValue := theValue / anOperand int.
    ]

    /// Compares the integer value    
    #method(type:bool) equal &int:anOperand 
        = theValue == anOperand.

    /// Compares the integer value    
    #method(type:bool) less &int:anOperand
        = theValue < anOperand.

    /// Returns the sum of integers                
    #method(type:int) add &int:anOperand
        = IntNumber new &int:(theValue + anOperand).

    /// Returns the difference of integers    
    #method(type:int) subtract &int:anOperand
        = IntNumber new &int:(theValue - anOperand).
    
    /// Returns the product of integers
    #method(type:int) multiply &int:anOperand
        = IntNumber new &int:(theValue * anOperand).
    
    /// Returns the result of integer division
    #method(type:int) divide &int:anOperand
        = IntNumber new &int:(theValue + anOperand).
    
    /// Returns the result of bitwise AND operation
    #method(type:int) and &int:anOperand
        = IntNumber new &int:(theValue && anOperand).
        
    /// Returns the result of bitwise OR operation
    #method(type:int) or &int:anOperand
        = IntNumber new &int:(theValue || anOperand).
        
    /// Returns the result of bitwise XOR operation
    #method(type:int) xor &int:anOperand
        = IntNumber new &int:(theValue ^^ anOperand).

    /// Returns the integer value
    #method value = IntNumber new &int:theValue.

    /// Returns the literal presentation
    #method literal = literalConvertor convert &int:theValue.

    /// Returns the literal presentation
    #method wide = wideConvertor convert &int:theValue.

    /// Clones the variable
    #method clone = Integer new &int:theValue.
    
    /// Dispatches the verb to the target object with int subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &int:(IntNumber new &int:theValue).
    
    /// Dispatches the verb to the target object with int subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &int:(IntNumber new &int:theValue).

    /// Expands a short integer
    #method => theValue.    
}

// --- Long ---

/// A 64bit signed integer variable. 
///
/// It extends system'LongNumber class
#class(sealed,struct:embeddable) Long :: IntBaseVariable
{
    #field(type:long)theValue.
    
    /// Creates a long integer variable with a default value (0)
    #constructor new
    [
        theValue := 0l.
    ]
    
    /// Creates a long integer variable
    #constructor new : aValue
    [
        theValue := aValue long.
    ]
    
    /// Creates a long integer variable
    #constructor new &long:aValue
    [
        theValue := aValue.
    ]
    
    /// Creates a long integer variable
    #constructor new &int:aValue
        => system'internal'long_copyint.
    
    /// Creates a long integer variable
    #constructor new &short:aValue
    [
        theValue := aValue long.
    ]
    
    /// Creates a long integer variable
    #constructor new &byte:aValue
    [
        theValue := aValue long.
    ]

    /// Returns itself    
    #method vlong = $self.
            
    /// Returns the value
    #method long = LongNumber new &long:theValue.
    
    /// Returns the value
    #method real = RealNumber new &long:theValue.

    /// Copies the integer value
    #method write &long:anOperand
    [
        theValue := anOperand.
    ]

    /// Reads the integer value
    #method read &vlong:aRetVal
    [
        aRetVal << theValue.
    ]

    /// Copies the integer value
    #method write &int:anOperand
    [
        theValue := anOperand long.
    ]

    /// Copies the integer value
    #method write &short:anOperand
    [
        theValue := anOperand long.
    ]

    /// Copies the integer value
    #method write &byte:anOperand
    [
        theValue := anOperand long.
    ]
             
    /// Appends the integer value           
    #method append &long:anOperand    
    [
        theValue := theValue + anOperand.
    ]
                        
    /// Appends the integer value           
    #method append &int:anOperand    
    [
        theValue := theValue + anOperand long.
    ]
                        
    /// Appends the integer value           
    #method append &short:anOperand    
    [
        theValue := theValue + anOperand long.
    ]
                        
    /// Appends the integer value           
    #method append &byte:anOperand    
    [
        theValue := theValue + anOperand long.
    ]

    /// Reduces the integer value
    #method reduce &long:anOperand
    [
        theValue := theValue - anOperand.
    ]
        
    /// Reduces the integer value
    #method reduce &int:anOperand
    [
        theValue := theValue - anOperand long.
    ]
        
    /// Reduces the integer value
    #method reduce &short:anOperand
    [
        theValue := theValue - anOperand long.
    ]
        
    /// Reduces the integer value
    #method reduce &byte:anOperand
    [
        theValue := theValue - anOperand long.
    ]

    /// Multiplies by the integer value
    #method multiplyBy &long:anOperand
    [
        theValue := theValue * anOperand.
    ]        
        
    /// Multiplies by the integer value
    #method multiplyBy &int:anOperand
    [
        theValue := theValue * LongNumber new &int:anOperand.
    ]        
        
    /// Multiplies by the integer value
    #method multiplyBy &short:anOperand
    [
        theValue := theValue * anOperand long.
    ]        
        
    /// Multiplies by the integer value
    #method multiplyBy &byte:anOperand
    [
        theValue := theValue * anOperand long.
    ]        

    /// Divides into the integer value
    #method divideInto &long:anOperand
    [
        theValue := theValue / anOperand.
    ]
        
    /// Divides into the integer value
    #method divideInto &int:anOperand
    [
        theValue := theValue / LongNumber new &int:anOperand.
    ]
        
    /// Divides into the integer value
    #method divideInto &short:anOperand
    [
        theValue := theValue / anOperand long.
    ]
        
    /// Divides into the integer value
    #method divideInto &byte:anOperand
    [
        theValue := theValue / anOperand long.
    ]

    /// Compares the integer value
    #method(type:bool) equal &long:anOperand 
        = theValue == anOperand.

    /// Compares the integer value
    #method(type:bool) less &long:anOperand
        = theValue < anOperand.

    /// Compares the integer value
    #method(type:bool) equal &int:anOperand 
        = $self equal &long:(LongNumber new &int:anOperand).

    /// Compares the integer value
    #method(type:bool) less &int:anOperand
        = $self less &long:(LongNumber new &int:anOperand).
        
    /// Returns the sum of integers
    #method(type:long) add &long:anOperand
        = LongNumber new &long:(theValue + anOperand).

    /// Returns the difference of integers
    #method(type:long) subtract &long:anOperand
        = LongNumber new &long:(theValue - anOperand).

    /// Returns the product of integers
    #method(type:long) multiply &long:anOperand
        = LongNumber new &long:(theValue * anOperand).

    /// Returns the result of integer division
    #method(type:long) divide &long:anOperand
        = LongNumber new &long:(theValue + anOperand).

    /// Returns the result of bitwise AND operation        
    #method(type:long) and &long:anOperand
        = LongNumber new &long:(theValue && anOperand).
        
    /// Returns the result of bitwise OR operation        
    #method(type:long) or &long:anOperand
        = LongNumber new &long:(theValue || anOperand).
        
    /// Returns the result of bitwise XOR operation        
    #method(type:long) xor &long:anOperand
        = LongNumber new &long:(theValue ^^ anOperand).

    /// Returns the integer value
    #method value = LongNumber new &long:theValue.

    /// Returns the literal presentation
    #method literal = literalConvertor convert &long:theValue.

    /// Returns the literal presentation
    #method wide = wideConvertor convert &long:theValue.

    /// Clones the value
    #method clone = Long new &long:theValue.
    
    /// Dispatches the verb to the target object with long subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &long:(LongNumber new &long:theValue).
    
    /// Dispatches the verb to the target object with long subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &long:(LongNumber new &long:theValue).

    /// Expands a long integer value    
    #method => theValue.    
}

// --- Real ---

/// A 64bit real number variable. 
///
/// It extends system'RealNumber class
#class(sealed,struct:embeddable) Real :: BaseVariable
{
    #field(type:real)theValue.
    
    /// Creates a real variable
    #constructor new
    [
        theValue := 0.0r.
    ]

    /// Creates a real variable
    #constructor new : aValue
    [
        theValue := aValue real.
    ]
    
    /// Creates a real variable
    #constructor new &real:aValue
    [
        theValue := aValue.
    ]
    
    /// Creates a real variable
    #constructor new &long:aValue
    [
        theValue := aValue real.
    ]
    
    /// Creates a real variable
    #constructor new &int:aValue
        => system'internal'real_copyint.
    
    /// Creates a real variable
    #constructor new &short:aValue
        => system'internal'real_copyint.
    
    /// Creates a real variable
    #constructor new &byte:aValue
        => system'internal'real_copyint.

    /// Returns itself    
    #method vreal = $self.
    
    /// Returns the real value
    #method real = RealNumber new &real:theValue.
        
    /// Copies the numeric value
    #method write &real:anOperand
    [
        theValue := anOperand.
    ]
    
    /// Reads the numeric value
    #method read &vreal:aRetVal
    [
        aRetVal << theValue.
    ]    
    
    /// Copies the numeric value
    #method write &int:anOperand
    [
        theValue := RealNumber new &int:anOperand.
    ]
    
    /// Copies the numeric value
    #method write &long:anOperand
    [
        theValue := RealNumber new &long:anOperand.
    ]
    
    /// Copies the numeric value
    #method write &short:anOperand
    [
        theValue := RealNumber new &short:anOperand.
    ]
    
    /// Copies the numeric value
    #method write &byte:anOperand
    [
        theValue := RealNumber new &byte:anOperand.
    ]
        
    /// Appends the real value
    #method append &real:anOperand    
    [
        theValue := theValue + anOperand.
    ]
    
    /// Appends the integer value
    #method append &int:anOperand    
    [
        theValue := theValue add &int:anOperand.
    ]
    
    /// Appends the integer value
    #method append &long:anOperand    
    [
        theValue := theValue add &long:anOperand.
    ]
        
    /// Appends the integer value
    #method append &short:anOperand    
    [
        theValue := theValue + anOperand real.
    ]
        
    /// Appends the integer value
    #method append &byte:anOperand    
    [
        theValue := theValue + anOperand real.
    ]
    
    /// Reduces the real value
    #method reduce &real:anOperand
    [
        theValue := theValue - anOperand.
    ]
        
    /// Reduces the integer value
    #method reduce &int:anOperand
    [
        theValue := theValue subtract &int:anOperand.
    ]    
        
    /// Reduces the integer value
    #method reduce &long:anOperand
    [
        theValue := theValue - anOperand real.
    ]
        
    /// Reduces the integer value
    #method reduce &short:anOperand
    [
        theValue := theValue - anOperand real.
    ]
        
    /// Reduces the integer value
    #method reduce &byte:anOperand
    [
        theValue := theValue - anOperand real.
    ]
        
    /// Multiplies by the real numeric value
    #method multiplyBy &real:anOperand
    [
        theValue := theValue * anOperand.
    ]        
        
    /// Multiplies by the integer value
    #method multiplyBy &int:anOperand
    [
        theValue := theValue multiply &int:anOperand.
    ]        
        
    /// Multiplies by the integer value
    #method multiplyBy &long:anOperand
    [
        theValue := theValue * anOperand real.
    ]        
        
    /// Multiplies by the integer value
    #method multiplyBy &short:anOperand
    [
        theValue := theValue * anOperand real.
    ]        
        
    /// Multiplies by the integer value
    #method multiplyBy &byte:anOperand
    [
        theValue := theValue * anOperand real.
    ]        

    /// Divides into the numeric real value        
    #method divideInto &real:anOperand
    [
        theValue := theValue / anOperand.
    ]
        
    /// Divides into the integer value
    #method divideInto &int:anOperand
    [
        theValue := theValue divide &int:anOperand.
    ]
        
    /// Divides into the integer value
    #method divideInto &long:anOperand
    [
        theValue := theValue / anOperand real.
    ]
        
    /// Divides into the integer value
    #method divideInto &short:anOperand
    [
        theValue := theValue / anOperand real.
    ]
        
    /// Divides into the integer value
    #method divideInto &byte:anOperand
    [
        theValue := theValue / anOperand real.
    ]

    /// Compares the real value
    #method(type:bool) equal &real:anOperand 
        = theValue == anOperand.

    /// Compares the real value
    #method(type:bool) less &real:anOperand
        = theValue < anOperand.

    /// Returns the sum of numbers        
    #method add &real:anOperand
        = RealNumber new &real:(theValue + anOperand).
    
    /// Returns the difference
    #method subtract &real:anOperand
        = RealNumber new &real:(theValue - anOperand).
    
    /// Returns the product
    #method multiply &real:anOperand
        = RealNumber new &real:(theValue * anOperand).
    
    /// Returns the result of division
    #method divide &real:anOperand
        = RealNumber new &real:(theValue / anOperand).

    /// Returns the value
    #method value = RealNumber new &real:theValue.

    /// Returns the literal presentation
    #method literal = literalConvertor convert &real:theValue.

    /// Returns the literal presentation
    #method wide = wideConvertor convert &real:theValue.

    /// Clones the variable
    #method clone = Real new &real:theValue.
    
    /// Dispatches the verb to the target object with real subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &real:(RealNumber new &real:theValue).
    
    /// Dispatches the verb to the target object with real subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &real:(RealNumber new &real:theValue).
    
    /// Expands a real value
    #method => theValue.
}

// --- CharValue ---

/// An UTF-32 character symbol
#class(sealed,integer:4) CharValue :: BaseValue
{
    /// Creates the object with a default value (0)
    #constructor new
        => system'internal'int_init.        

    /// Creates the object with specified value
    #constructor new &char:anOperand
        => system'internal'int_copy.

    /// Creates the object with specified value
    #constructor new : anOperand
        <= new &char:(anOperand cast:%convert &to:charConvertor char).

    /// Creates the object with specified numeric value
    #constructor new &int:anOperand
        <= new &char:(charConvertor convert &int:anOperand).

    /// Returns the minimal possible character value (0)
    #constructor min
        <= %new &char:#0.

    /// Returns the maximal possible character value (010FFFFh)
    #constructor max
        <= %new &char:#010FFFFh.

    /// Returns itself
    #method char = $self.

    /// Returns the numeric value
    #method int = intConvertor convert &char:$self.
    
    /// Compares values
    #method equal &char:anOperand 
        => system'internal'int_equal.

    /// Compares values
    #method less &char:anOperand 
        => system'internal'int_less.

    /// Compares values
    #method equal &int:anOperand 
        => system'internal'int_equal.

    /// Compares values
    #method less &int:anOperand 
        => system'internal'int_less.

    /// Clones the value
    #method clone = CharValue new &char:$self.

    /// Dispatches the verb to the target object with char subject
    #method cast:aVerb &to:aTarget = aTarget::aVerb eval &char:$self.
    
    /// Dispatches the verb to the target object with char subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &char:$self.

    /// Returns the literal value
    #method literal = literalConvertor convert &char:$self.

    /// Returns the wide literal value
    #method wide = wideConvertor convert &char:$self.
    
    /// Reads the number of bytes to represent the symbol
    #method readLength &vint:aLength
        => system'internal'char_getlen.
        
    /// Retuns the number of bytes to represent the symbol        
    #method length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]
    
    /// Retuns the number of short integers to represent the symbol
    #method wideLength
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        aLength := aLength + 1.
        aLength := aLength / 2.
        
        ^ IntNumber new &int:aLength.
    ]

    // Reads the object hash code into the output parameter
    #method readHashCode &vint:aCode => system'internal'int_hashcode.
    
    #method hash_code
    [
        #var(type:int)aCode.
        $self readHashCode &vint:aCode.
        
        ^ IntNumber new &int:aCode.
    ]
}

// --- Char ---

/// An UTF-32 character variable
///
/// Extends CharValue class
#class(sealed,struct:embeddable) Char :: BaseVariable
{
    #field(type:char)theValue.
    
    /// Creates a variable with a default value (0)
    #constructor new
    [
        theValue := #0.
    ]
    
    /// Creates a variable with a specified value
    #constructor new : aValue   
    [
        theValue := aValue char.
    ]
    
    /// Creates a variable with a specified value
    #constructor new &char:aValue
    [
        theValue := aValue.
    ]
    
    /// Creates a variable with a specified value
    #constructor new &int:aValue
    [
        theValue := charConvertor convert &int:aValue.
    ]
    
    /// Returns itself
    #method vchar = $self.
    
    /// Returns the character value
    #method char = CharValue new &char:theValue.
    
    /// Copies the character value
    #method write &char:anOperand
    [
        theValue := anOperand.
    ]
    
    /// Reads the character value
    #method read &vchar:aRetVal
    [
        aRetVal << theValue.
    ]
    
    /// Compares values
    #method(type:bool) equal &char:anOperand 
        = theValue == anOperand.

    /// Compares values
    #method(type:bool) less &char:anOperand
        = theValue < anOperand.

    // Returns the value        
    #method value = CharValue new &char:theValue.

    /// Returns the literal value
    #method literal = literalConvertor convert &char:theValue.

    /// Returns the literal value
    #method wide = wideConvertor convert &char:theValue.

    /// Clones the variable
    #method clone = Char new &char:theValue.
    
    /// Reads the number of bytes to represent the symbol
    #method readLength &vint:aLength
        => system'internal'char_getlen.
        
    /// Retuns the number of bytes to represent the symbol        
    #method length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]
    
    /// Dispatches the verb to the target object with char subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &char:(CharValue new &char:theValue).
    
    /// Dispatches the verb to the target object with char subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &char:(CharValue new &char:theValue).

    /// Extends the character value
    #method => theValue.    
}

// === Basic Factory symbols ==

/// A generic variable. 
///
/// Extends an assigned value
#class Variable
{
    #field theValue.
    
    /// Creates the empty variable
    #constructor new
    [
    ]        
        
    /// Creates the variable
    #constructor new : aValue
    [
        theValue := aValue.
    ]
  
    /// Assigns the variable
    #method write : aValue
    [
        theValue := aValue.
    ]
        
    /// Returns the value
    #method value = theValue.        

    /// Appends the value
    #method append : aValue
    [
        ($nil == theValue)
            ? [ theValue := aValue. ]
            ! [ theValue := theValue + aValue. ].
    ]
    
    /// Subtracts the value
    #method reduce : aValue
    [
        theValue := theValue - aValue.
    ]

    /// Multiplies by the value
    #method multiplyBy : aValue
    [
        theValue := theValue * aValue.
    ]
    
    /// Divides into the value
    #method divideInto : aValue
    [
        theValue := theValue / aValue.
    ]
    
    /// Dispatches the verb to the target object
    #method cast : aVerb &to:aTarget => theValue.
    
    /// Dispatches the verb to the target object
    #method cast : aVerb &to:aTarget &with:aParam => theValue.

    /// Expands the value
    #method => theValue.
}

// --- KeyValue ---

/// A key - object pair
///
/// Extends the object with a key property
#class(sealed) KeyValue
{
    #field theKey.
    #field theObject.
    
    /// Creates a new objext
    #constructor new &key:aKey &object:anObject
    [
        theKey := aKey.
        theObject := anObject.
    ]
    
    /// Returns the key
    #method key = theKey.
    
    /// Returns the key
    #method object = theObject.

    #method set &object:anObject
    [
        theObject := anObject.
    ]
            
    /// Redirects the incoming messages to the object
    #method => theObject.
}

// === Basic Convertors ===

// --- convertor helpers ---

/// A byte convertor
#symbol byteConvertor =
{
    /// Converts a character into a byte value and saves the result into an output parameter
    #method convert &char:aValue &vbyte:aRetVal
        => system'internal'byte_copychar.
        
    /// Converts a short integer into a byte value and saves the result into an output parameter
    #method convert &short:aValue &vbyte:aRetVal
        => system'internal'byte_copyword.
        
    /// Converts an integer into a byte value and saves the result into an output parameter
    #method convert &int:aValue &vbyte:aRetVal
        => system'internal'byte_copyint.
        
    /// Converts a long integer into a byte value and saves the result into an output parameter
    #method convert &long:aValue &vbyte:aRetVal
        => system'internal'byte_copylong.
        
    /// Converts a real number into a byte value and saves the result into an output parameter
    #method convert &real:aValue &vbyte:aRetVal
        => system'internal'byte_copyreal.
        
    /// Converts a literal value into a byte value and saves the result into an output parameter
    #method convert &literal:aValue &vbyte:aRetVal
        => system'internal'byte_copystr.
        
    /// Converts a wide literal value into a byte value and saves the result into an output parameter
    #method convert &wide:aValue &vbyte:aRetVal
        => system'internal'byte_copywstr.
        
    /// Returns the parameter        
    #method(type:byte) convert &byte:anInt
        = ByteNumber new &byte:(anInt).
        
    /// Converts a character into a byte value
    #method(type:byte) convert &char:aValue
    [
        #var(type:byte)aRetVal.
        $self convert &char:aValue &vbyte:aRetVal.
        
        ^ ByteNumber new &byte:aRetVal.
    ]
        
    /// Converts a short integer into a byte value
    #method(type:byte) convert &short:aValue
    [
        #var(type:byte)aRetVal.
        $self convert &short:aValue &vbyte:aRetVal.
        
        ^ ByteNumber new &byte:aRetVal.
    ]
        
    /// Converts an integer into a byte value
    #method(type:byte) convert &int:aValue
    [
        #var(type:byte)aRetVal.
        $self convert &int:aValue &vbyte:aRetVal.
        
        ^ ByteNumber new &byte:aRetVal.
    ]
        
    /// Converts a long integer into a byte value
    #method(type:byte) convert &long:aValue
    [
        #var(type:byte)aRetVal.
        $self convert &long:aValue &vbyte:aRetVal.
        
        ^ ByteNumber new &byte:aRetVal.
    ]
        
    /// Converts a real number into a byte value
    #method(type:byte) convert &real:aValue
    [
        #var(type:byte)aRetVal.
        $self convert &real:aValue &vbyte:aRetVal.
        
        ^ ByteNumber new &byte:aRetVal.
    ]
                
    /// Converts a literal value into a byte value
    #method(type:byte) convert &literal:aLiteral
    [
        #var(type:byte)aValue.
        $self convert &literal:aLiteral &vbyte:aValue.
        
        ^ ByteNumber new &byte:aValue.
    ]
                
    /// Converts a wide literal value into a byte value
    #method(type:byte) convert &wide:aLiteral
    [
        #var(type:byte)aValue. 
        $self convert &wide:aLiteral &vbyte:aValue.
        
        ^ ByteNumber new &byte:aValue.
    ]
}.

/// A 16bit integer convertor
#symbol shortConvertor =
{
    /// Converts a character into a short integer value and saves the result into an output parameter
    #method convert &char:aValue &vshort:aRetVal
        => system'internal'word_copychar.
        
    /// Converts a byte value into a short integer value and saves the result into an output parameter
    #method convert &byte:aValue &vshort:aRetVal
        => system'internal'word_copybyte.
        
    /// Converts an integer into a short integer value and saves the result into an output parameter
    #method convert &int:aValue &vshort:aRetVal
        => system'internal'word_copyint.
        
    /// Converts a long integer into a short integer value and saves the result into an output parameter
    #method convert &long:aValue &vshort:aRetVal
        => system'internal'word_copylong.
        
    /// Converts a real number into a short integer value and saves the result into an output parameter
    #method convert &real:aValue &vshort:aRetVal
        => system'internal'word_copyreal.
        
    /// Converts a literal string into a short integer value and saves the result into an output parameter
    #method convert &literal:aValue &vshort:aRetVal
        => system'internal'word_copystr.
        
    /// Converts a wide literal string into a short integer value and saves the result into an output parameter
    #method convert &wide:aValue &vshort:aRetVal
        => system'internal'word_copywstr.
        
    /// Returns a short integer value        
    #method(type:short) convert &short:anInt
        = ShortNumber new &short:anInt.
        
    /// Converts a byte value into a short integer value
    #method(type:short) convert &byte:aValue
    [
        #var(type:short)aRetVal.
        $self convert &byte:aValue &vshort:aRetVal.
        
        ^ ShortNumber new &short:aRetVal.
    ]
        
    /// Converts an integer into a short integer value
    #method(type:short) convert &int:aValue
    [
        #var(type:short)aRetVal.
        $self convert &int:aValue &vshort:aRetVal.
        
        ^ ShortNumber new &short:aRetVal.
    ]
        
    /// Converts a long integer into a short integer value
    #method(type:short) convert &long:aValue
    [
        #var(type:short)aRetVal.
        $self convert &long:aValue &vshort:aRetVal.
        
        ^ ShortNumber new &short:aRetVal.
    ]
        
    /// Converts a real number into a short integer value
    #method(type:short) convert &real:aValue
    [
        #var(type:short)aRetVal.
        $self convert &real:aValue &vshort:aRetVal.
        
        ^ ShortNumber new &short:aRetVal.
    ]
        
    /// Converts a character into a short integer value
    #method(type:short) convert &char:aValue
    [
        #var(type:short)aRetVal.
        $self convert &char:aValue &vshort:aRetVal.
        
        ^ ShortNumber new &short:aRetVal.
    ]
        
    /// Converts a literal string into a short integer value
    #method(type:short) convert &literal:aValue
    [
        #var(type:short)aRetVal.
        $self convert &literal:aValue &vshort:aRetVal.
        
        ^ ShortNumber new &short:aRetVal.
    ]
        
    /// Converts a wide literal string into a short integer value
    #method(type:short) convert &wide:aValue
    [
        #var(type:short)aRetVal.
        $self convert &wide:aValue &vshort:aRetVal.
        
        ^ ShortNumber new &short:aRetVal.
    ]
}.

/// An Unicode character convertor
#symbol charConvertor =
{
    /// Converts a byte value into a character and saves the result into an output parameter
    #method convert &byte:aValue &vchar:aRetVal
        => system'internal'char_savebyte.
        
    /// Converts a short integer into a character and saves the result into an output parameter
    #method convert &short:aValue &vchar:aRetVal
        => system'internal'char_saveshort.
        
    /// Converts an integer into a character and saves the result into an output parameter
    #method convert &int:aValue &vchar:aRetVal
        => system'internal'char_saveint.
        
    /// Returns a parameter        
    #method(type:char)convert &char:aChar
        = CharValue new &char:aChar.
  
    #method(type:char) $convert &index:anIndex &literal:aLiteral &vchar:char
        => system'internal'char_copystr.
  
    #method(type:char) $convert &index:anIndex &bytearray:aLiteral &vchar:char
        => system'internal'char_copystr.
  
    #method(type:char) $convert &index:anIndex &wide:aLiteral &vchar:char
        => system'internal'char_copywstr.
        
    #method(type:char) $convert &index:anIndex &shortarray:aLiteral &vchar:char
        => system'internal'char_copywstr.

    #method $save &char:aChar &index:anIndex &shortarray:aCharArray &vlength:aLength
        => system'internal'sa_copychar.
    
    #method $save &char:aChar &index:anIndex &bytearray:aCharArray &vlength:aLength
        => system'internal'ba_copychar.

    /// Converts a literal string into a character                
    #method(type:char) convert &literal:aValue
    [
        #var(type:char)aRetVal.
        $self $convert &index:0 &literal:aValue &vchar:aRetVal.
        
        (aRetVal length != aValue length)
            ? [ #throw system'InvalidArgumentException new. ].
        
        ^ CharValue new &char:aRetVal.
    ]        
        
    /// Converts a wide literal string into a character                
    #method(type:char) convert &wide:aValue
    [
        #var(type:char)aRetVal.
        $self $convert &index:0 &wide:aValue &vchar:aRetVal.
        
        (aRetVal wideLength != aValue length)
            ? [ #throw system'InvalidArgumentException new. ].
        
        ^ CharValue new &char:aRetVal.
    ]        
        
    /// Converts a byte value into a character
    #method(type:char) convert &byte:aValue
    [
        #var(type:char)aRetVal.
        $self convert &byte:aValue &vchar:aRetVal.
        
        ^ CharValue new &char:aRetVal.
    ]        
        
    /// Converts a short integer into a character
    #method(type:char) convert &short:aValue
    [
        #var(type:char)aRetVal.
        $self convert &short:aValue &vchar:aRetVal.
        
        ^ CharValue new &char:aRetVal.
    ]        
        
    /// Converts an integer into a character
    #method(type:char) convert &int:aValue
    [
        #var(type:char)aRetVal.
        $self convert &int:aValue &vchar:aRetVal.
        
        ^ CharValue new &char:aRetVal.
    ]
}.

/// An integer convertor
#symbol intConvertor =
{
    /// Converts a literal string into an integer and saves the result into an output parameter
    convert &literal:aValue &vint:aRetVal
        => system'internal'int_copystr.

    /// Converts a wide literal string into an integer and saves the result into an output parameter
    convert &wide:aValue &vint:aRetVal
        => system'internal'int_copywstr.

    /// Converts a character value into an integer and saves the result into an output parameter
    convert &char:aValue &vint:aRetVal
        => system'internal'int_saveint.

    /// Converts a short integer value into an integer and saves the result into an output parameter
    convert &short:aValue &vint:aRetVal
        => system'internal'int_saveint.

    /// Converts a byte value into an integer and saves the result into an output parameter
    convert &byte:aValue &vint:aRetVal
        => system'internal'int_saveint.

    /// Converts a long integer into an integer and saves the result into an output parameter
    convert &long:aValue &vint:aRetVal
        => system'internal'int_savelong.
        
    /// Converts a real number into an integer and saves the result into an output parameter
    convert &real:aReal &vint:aRetVal
        => system'internal'int_copyreal.
        
    /// Converts a literal string into an integer
    #method(type:int) convert &literal:aLiteral
    [
        #var(type:int)aRetVal.
        $self convert &literal:aLiteral &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]
        
    /// Converts a wide literal string into an integer
    #method(type:int) convert &wide:aLiteral
    [
        #var(type:int)aRetVal.
        $self convert &wide:aLiteral &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]
        
    /// Converts a character value into an integer
    #method(type:int) convert &char:aValue
    [
        #var(type:int)aRetVal.
        $self convert &char:aValue &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]
        
    /// Converts a byte value into an integer
    #method(type:int) convert &byte:aValue
    [
        #var(type:int)aRetVal.
        $self convert &byte:aValue &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]
        
    /// Converts a short integer value into an integer
    #method(type:int) convert &short:aValue
    [
        #var(type:int)aRetVal.
        $self convert &short:aValue &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]
        
    /// Converts a long integer value into an integer and saves the result into an output parameter
    #method(type:int) convert &long:aValue
    [
        #var(type:int)aRetVal.
        $self convert &long:aValue &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    /// Converts a long integer into an integer
    #method(type:int) convert &real:aReal
    [
        #var(type:int)aRetVal.
        $self convert &real:aReal &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    /// Returns a parameter                                    
    #method(type:int) convert &int:anInt
        = IntNumber new &int:anInt.
}.

/// A long integer convertor
#symbol longConvertor =
{
    /// Converts a literal string into a long integer and saves the result into an output parameter
    convert &literal:aValue &vlong:aRetVal
        => system'internal'long_copystr.

    /// Converts a wide literal string into a long integer and saves the result into an output parameter
    convert &wide:aValue &vlong:aRetVal
        => system'internal'long_copywstr.

    /// Converts a real number into a long integer and saves the result into an output parameter
    convert &real:aValue &vlong:aRetVal
        => system'internal'long_copyreal.

    /// Converts an integer value into a long integer
    convert &int:aValue
        = LongNumber new &int:aValue.

    /// Converts a byte into a long integer
    convert &byte:aValue
        = LongNumber new &byte:aValue.

    /// Converts a short integer value into a long integer
    convert &short:aValue
        = LongNumber new &short:aValue.

    /// Converts a real number into a long integer
    convert &real:aValue
    [
        #var(type:long)aRetVal.
        $self convert &real:aValue &vlong:aRetVal.
        
        ^ LongNumber new &long:aRetVal.
    ]

    /// Converts a character into a long integer
    convert &char:aValue
    [
        #var(type:int)aRetVal.
        intConvertor convert &char:aValue &vint:aRetVal.
        
        ^ aRetVal long.
    ]

    /// Converts a literal string into a long integer
    #method(type:long) convert &literal:aLiteral
    [
        #var(type:long)aRetVal.
        $self convert &literal:aLiteral &vlong:aRetVal.
        
        ^ LongNumber new &long:aRetVal.
    ]

    /// Converts a wide literal string into a long integer
    #method(type:long) convert &wide:aLiteral
    [
        #var(type:long)aRetVal.
        $self convert &wide:aLiteral &vlong:aRetVal.
        
        ^ LongNumber new &long:aRetVal.
    ]
    
    /// Returns a parameter
    convert &long:aLong
        = LongNumber new &long:aLong.
}.

/// A real number convertor
#symbol realConvertor =
{
    /// Converts an integer into a real number and saves the result into an output parameter
    convert &int:aValue &vreal:aRetVal
        => system'internal'real_saveint.

    /// Converts a literal string into a real number and saves the result into an output parameter
    convert &literal:aValue &vreal:aRetVal
        => system'internal'real_copystr.
        
    /// Converts a wide literal string into a real number and saves the result into an output parameter
    convert &wide:aValue &vreal:aRetVal
        => system'internal'real_copywstr.
        
    /// Converts a literal string into a real number
    #method(type:real)convert &literal:aLiteral
    [
        #var(type:real)aRetVal.
        $self convert &literal:aLiteral &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]
        
    /// Converts a wide literal string into a real number
    #method(type:real)convert &wide:aLiteral
    [
        #var(type:real)aRetVal.
        $self convert &wide:aLiteral &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    /// Returns a parameter
    convert &real:aReal
        = RealNumber new &real:aReal.
        
    /// Converts an integer into a real number
    convert &int:anInt
        = RealNumber new &int:anInt.
        
    /// Converts a long integer into a real number
    convert &long:aLong
        = RealNumber new &long:aLong.
        
    /// Converts a byte into a real number
    convert &byte:aValue
        = RealNumber new &byte:aValue.
        
    /// Converts a short integer into a real number
    convert &short:aValue
        = RealNumber new &short:aValue.
}.

// --- literalConvertor ---

/// A literal convertor
#symbol literalConvertor =
{
    /// Converts the parameter to the literal value
    #method(type:literal) convert &char:aValue
        => system'internal's_copychar.
    
    /// Converts the parameter to the literal value
    #method(type:literal) convert &int:aNumber
        => system'internal's_copyint.

    /// Converts the parameter to the literal value
    #method(type:literal) convert &short:aNumber
        => system'internal's_copyint.

    /// Converts the parameter to the literal value
    #method(type:literal) convert &byte:aNumber
        => system'internal's_copyint.

    /// Converts the parameter to the literal value
    #method(type:literal) convert &long:aNumber
        => system'internal's_copylong.

    /// Converts the parameter to the literal value with the provided radix
    #method(type:literal) convert &int:aNumber &index:aRadix
        => system'internal's_copyint_radix.

    /// Converts the parameter to the literal value with the provided radix
    #method(type:literal) convert &short:aNumber &index:aRadix
        => system'internal's_copyint_radix.

    /// Converts the parameter to the literal value with the provided radix
    #method(type:literal) convert &byte:aNumber &index:aRadix
        => system'internal's_copyint_radix.

    /// Converts the parameter to the literal value with the provided radix
    #method(type:literal) convert &long:aNumber &index:aRadix
        => system'internal's_copylong_radix.

    /// Converts the parameter to the literal value
    #method(type:literal) convert &real:aNumber
        => system'internal's_copyreal.

    /// Converts UTF-16 literal to UTF-8 one
    #method(type:literal) convert &wide:aLiteral
        => system'internal'str_copywstr.

    /// Returns the literal presentation of the parameter
    #method(type:literal) convert : anObject
        = anObject literal.

    /// Returns a parameter                
    #method(type:literal) convert &literal:aLiteral
        = aLiteral.
}.

// --- wideConvertor ---

/// A wide literal convertor
#symbol wideConvertor =
{
    /// Converts the parameter to the literal value
    #method(type:wide) convert &char:aValue
        => system'internal'ws_copychar.
    
    /// Converts the parameter to the literal value
    #method(type:wide) convert &int:aNumber
        => system'internal'ws_copyint.

    /// Converts the parameter to the literal value
    #method(type:wide) convert &short:aNumber
        => system'internal'ws_copyint.

    /// Converts the parameter to the literal value
    #method(type:wide) convert &byte:aNumber
        => system'internal'ws_copyint.

    /// Converts the parameter to the literal value
    #method(type:wide) convert &long:aNumber
        => system'internal'ws_copylong.

    /// Converts the parameter to the literal value with the provided radix
    #method(type:wide) convert &int:aNumber &index:aRadix
        => system'internal'ws_copyint_radix.

    /// Converts the parameter to the literal value with the provided radix
    #method(type:wide) convert &short:aNumber &index:aRadix
        => system'internal'ws_copyint_radix.

    /// Converts the parameter to the literal value with the provided radix
    #method(type:wide) convert &byte:aNumber &index:aRadix
        => system'internal'ws_copyint_radix.

    /// Converts the parameter to the literal value with the provided radix
    #method(type:wide) convert &long:aNumber &index:aRadix
        => system'internal'ws_copylong_radix.

    /// Converts the parameter to the literal value
    #method(type:wide) convert &real:aNumber
        => system'internal'ws_copyreal.

    /// Converts UTF-8 literal to UTF-16
    #method(type:wide) convert &literal:aLiteral
        => system'internal'wstr_copystr.
        
    /// Returns the literal presentation of the parameter
    #method(type:wide) convert : anObject
        = anObject wide.
        
    /// Returns a parameter
    #method(type:wide) convert &wide:aLiteral
        = aLiteral.
}.

// === Typecaster ==

#symbol typeCaster
    = { ! : anObject = $subject. }.
