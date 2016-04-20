// === basic data classes ===

// === Numeric Variables ===

#subject vbyte  = varof:byte.
#subject vshort = varof:short.
#subject vint   = varof:int.
#subject vlong  = varof:long.
#subject vreal  = varof:real.

// --- BaseValue ---

// A base value
#class BaseValue
{
    /// Compares the values
    #method equal : anObject = anObject cast:%equal &to:$self bool.

    /// Compares the values
    #method(bool) less : anObject = anObject cast:%less &to:$self bool.

    /// Compares the values
    #method(bool) greater : anObject = anObject less:$self bool.
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

//// --- IntBaseVariable ---
//
///// A base integer variable
//#class IntBaseVariable :: IntBaseNumber
//{
//    /// Assigns the numeric value
//    #method write : anOperand
//    [
//        anOperand cast:%write &to:$self.
//    ]
//    
//    /// Appends the numeric value
//    #method append : anOperand
//    [
//        anOperand cast:%append &to:$self.
//    ]
//    
//    /// Subtracts the numeric value and reassigns the value
//    #method reduce : anOperand
//    [
//        anOperand cast:%reduce &to:$self.
//    ]
//    
//    /// Multiplies by the numeric value and reassigns the value
//    #method multiplyBy : anOperand
//    [
//        anOperand cast:%multiplyBy &to:$self.
//    ]
//    
//    /// Divides into the numeric value and reassigns the value
//    #method divideInto : anOperand
//    [
//        anOperand cast:%divideInto &to:$self.
//    ]
//}
//
//// --- BaseNumber ---
//
///// A base numeric variable
//#class BaseVariable :: BaseNumber
//{
//    /// Assigns the value
//    #method write : anOperand
//    [
//        anOperand cast:%write &to:$self.
//    ]
//    
//    /// Appends the value
//    #method append : anOperand
//    [
//        anOperand cast:%append &to:$self.
//    ]
//    
//    /// Subtracts the value and reassigns the value
//    #method reduce : anOperand
//    [
//        anOperand cast:%reduce &to:$self.
//    ]
//    
//    /// Multiplies by the value and reassigns the value
//    #method multiplyBy : anOperand
//    [
//        anOperand cast:%multiplyBy &to:$self.
//    ]
//    
//    /// Divides into the value and reassigns the value
//    #method divideInto : anOperand
//    [
//        anOperand cast:%divideInto &to:$self.
//    ]
//}

// --- ByteNumber ---

/// An unsigned 8 bit integer
#class(sealed,integerof:1,numeric:byte,dispatchable:byte) ByteNumber :: IntBaseNumber
{
    /// Returns the minimal value (0)
    #constructor min
        = 0.

    /// Returns the maxial value (255)
    #constructor max
        = 255.
    
    /// Creates the object with a specified value
    #constructor(stacksafe,suppress:w3) new &byte:value
        = value.

    /// Creates the object with a specified value
    #constructor new : aValue
        <= new &byte:(aValue byte).

    /// Creates the object with a default value (0)
    #constructor new
        = 0.

    /// Returns the value as a short integer
    #method short = ShortNumber new &byte:$self.

    /// Returns the value as an integer
    #method int = IntNumber new &byte:$self.

    /// Returns the value as a long integer
    #method long = LongNumber new &byte:$self.

    /// Returns the value as a real number
    #method real = RealNumber new &byte:$self.

    #method(bool,stacksafe) equal &short:anOperand
        = ShortNumber new &byte:$self equal &short:anOperand.

    #method(bool,stacksafe) less &short:anOperand
        = ShortNumber new &byte:$self less &short:anOperand.

    /// Compares the value
    #method(bool,stacksafe) equal &int:anOperand
        = IntNumber new &byte:$self less &int:anOperand.
        
    /// Compares the value
    #method(bool,stacksafe) less &int:anOperand
        = IntNumber new &byte:$self less &int:anOperand.

    /// Compares the value
    #method(bool,stacksafe) equal &long:anOperand
        = LongNumber new &byte:$self equal &long:anOperand.
        
    /// Compares the value
    #method(bool,stacksafe) less &long:anOperand
        = LongNumber new &byte:$self less &long:anOperand.

    /// Compares the value
    #method(bool,stacksafe) equal &real:anOperand
        = RealNumber new &byte:$self equal &real:anOperand.

    /// Compares the value
    #method(bool,stacksafe) less &real:anOperand
        = RealNumber new &byte:$self less &real:anOperand.

    /// Returns the sum
    #method(stacksafe) add &short:anOperand
        = ShortNumber new &byte:$self add &short:anOperand.

    /// Returns the difference
    #method(stacksafe) subtract &short:anOperand
        = ShortNumber new &byte:$self subtract &short:anOperand.

    /// Returns the product
    #method(stacksafe) multiply &short:anOperand
        = ShortNumber new &byte:$self multiply &short:anOperand.

    /// Returns the result of integer division
    #method(stacksafe) divide &short:anOperand
        = ShortNumber new &byte:$self divide &short:anOperand.

    /// Returns the sum
    #method(stacksafe) add &int:anOperand
        = IntNumber new &byte:$self add &int:anOperand.

    /// Returns the difference
    #method(stacksafe) subtract &int:anOperand
        = IntNumber new &byte:$self subtract &int:anOperand.

    /// Returns the product
    #method(stacksafe) multiply &int:anOperand
        = IntNumber new &byte:$self multiply &int:anOperand.

    /// Returns the result of integer division
    #method(stacksafe) divide &int:anOperand
        = IntNumber new &byte:$self divide &int:anOperand.

    /// Returns the sum
    #method(stacksafe) add &long:anOperand
        = LongNumber new &byte:$self add &long:anOperand.

    /// Returns the difference
    #method(stacksafe) subtract &long:anOperand
        = LongNumber new &byte:$self subtract &long:anOperand.

    /// Returns the product
    #method(stacksafe) multiply &long:anOperand
        = LongNumber new &byte:$self multiply &long:anOperand.

    /// Returns the result of integer division
    #method(stacksafe) divide &long:anOperand
        = LongNumber new &byte:$self divide &long:anOperand.

    /// Returns the sum
    #method(stacksafe) add &real:anOperand
        = RealNumber new &byte:$self add &real:anOperand.

    /// Returns the difference
    #method(stacksafe) subtract &real:anOperand
        = RealNumber new &byte:$self subtract &real:anOperand.

    /// Returns the product
    #method(stacksafe) multiply &real:anOperand
        = RealNumber new &byte:$self multiply &real:anOperand.

    /// Returns the result of integer division
    #method(stacksafe) divide &real:anOperand
        = RealNumber new &byte:$self divide &real:anOperand.

//    /// Returns the result of bitwise AND operation
//    #method(type:byte,stacksafe) and &byte:anOperand
//        => system'core_routines'byte_and.
//
//    /// Returns the result of bitwise OR operation
//    #method(type:byte,stacksafe) or &byte:anOperand
//        => system'core_routines'byte_or.
//
//    /// Returns the result of bitwise XOR operation
//    #method(type:byte,stacksafe) xor &byte:anOperand
//        => system'core_routines'byte_xor.
//
//    /// Returns the result of bitwise AND operation
//    #method(stacksafe) and &short:anOperand
//        = ShortNumber new &byte:$self and &short:anOperand.
//
//    /// Returns the result of bitwise OR operation
//    #method(stacksafe) or &short:anOperand
//        = ShortNumber new &byte:$self or &short:anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    #method(stacksafe) xor &short:anOperand
//        = ShortNumber new &byte:$self xor &short:anOperand.
//
//    /// Returns the result of bitwise AND operation
//    #method(type:int,stacksafe) and &int:anOperand
//        => system'core_routines'int_and.
//
//    /// Returns the result of bitwise OR operation
//    #method(type:int,stacksafe) or &int:anOperand
//        => system'core_routines'int_or.
//
//    /// Returns the result of bitwise XOR operation
//    #method(type:int,stacksafe) xor &int:anOperand
//        => system'core_routines'int_xor.
//
//    /// Returns the result of bitwise AND operation
//    #method(type:long,stacksafe) and &long:anOperand
//        = LongNumber new &byte:$self and &long:anOperand.
//
//    /// Returns the result of bitwise OR operation
//    #method(type:long,stacksafe) or &long:anOperand
//        = LongNumber new &byte:$self or &long:anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    #method(type:long,stacksafe) xor &long:anOperand
//        = LongNumber new &byte:$self xor &long:anOperand.
//
//    #method shift : anOrder
//        <= shift &int:(anOrder int).
//    
//    /// Bitwise shift operation
//    #method(type:byte) shift &index:anOrder
//        <= shift &int:(anOrder int).
//    
//    #method(type:byte,stacksafe) shift &int:anOrder
//        => system'core_routines'byte_shift.

    /// Returns the literal presentation
    #method literal = literalConvertor convert &byte:$self.

    /// Returns the literal presentation
    #method wide = wideConvertor convert &byte:$self.

//    /// Bitwise inversion
//    #method(type:byte,stacksafe) not
//        => system'core_routines'byte_not.

    /// Clones the value
    #method clone = ByteNumber new &byte:$self.

//    // Reads the object hash code into the output parameter
//    #method(stacksafe) readHashCode &vint:aCode => system'core_routines'int_hashcode.
//    
//    #method(embeddable,type:int,suppress:w3) hash_code
//    [
//        #var(type:int)aCode.
//        $self readHashCode &vint:aCode.
//        
//        ^ aCode.
//    ]
}

// --- ShortNumber ---

/// A signed 16 bit integer
#class(sealed,integerof:2,numeric:short,dispatchable:short) ShortNumber :: IntBaseNumber
{
    /// Returns the minimal value (-32768)
    #constructor min
        = -32768.

    /// Returns the maxial value (32767)
    #constructor max
        = 32767.

    /// Creates the object with specified value
    #constructor(stacksafe) new &byte:value
        = shortConvertor convert &byte:value.
        
    /// Creates the object with specified value
    #constructor(stacksafe,suppress:w3) new &short:value
        = value.

    /// Creates the object with a default value (0)
    #constructor new
        = 0.

    /// Creates the object with specified value
    #constructor new : anObject
        <= new &short:(anObject short).

    /// Returns the byte value        
    #method byte = byteConvertor convert &short:$self.
    
    /// Returns the integer value
    #method int = IntNumber new &short:$self.
    
    /// Returns the long integer value
    #method long = LongNumber new &short:$self.
    
    /// Returns the value as a real number
    #method real = RealNumber new &short:$self.
    
//    #method(stacksafe)read &vint:n
//        => system'core_routines'int_save.
    
    /// Compares the value
    #method(bool,stacksafe) equal &int:anOperand 
        = IntNumber new &short:$self equal &int:anOperand.
        
    /// Compares the value
    #method(bool,stacksafe) less &int:anOperand
        = IntNumber new &short:$self less &int:anOperand.
    
    /// Compares the value
    #method(bool,stacksafe) equal &byte:anOperand 
        = $self equal &short:(ShortNumber new &byte:anOperand).
        
    /// Compares the value
    #method(bool,stacksafe) less &byte:anOperand
        = $self less &short:(ShortNumber new &byte:anOperand).
    
    /// Compares the value
    #method(bool,stacksafe) equal &long:anOperand 
        = LongNumber new &short:$self equal &long:anOperand.
        
    /// Compares the value
    #method(bool,stacksafe) less &long:anOperand
        = LongNumber new &short:$self less &long:anOperand.
    
    /// Compares the value
    #method(bool,stacksafe) equal &real:anOperand 
        = RealNumber new &short:$self equal &real:anOperand.
        
    /// Compares the value
    #method(bool,stacksafe) less &real:anOperand
        = RealNumber new &short:$self less &real:anOperand.

    /// Returns the sum
    #method(stacksafe) add &byte:anOperand
        = $self add &short:(ShortNumber new &byte:anOperand).

    /// Returns the difference
    #method(stacksafe) subtract &byte:anOperand
        = $self subtract &short:(ShortNumber new &byte:anOperand).

    /// Returns the product
    #method(stacksafe) multiply &byte:anOperand
        = $self multiply &short:(ShortNumber new &byte:anOperand).

    /// Returns the result of integer division
    #method(stacksafe) divide &byte:anOperand
        = $self divide &short:(ShortNumber new &byte:anOperand).

    /// Returns the sum
    #method(stacksafe) add &int:anOperand
        = IntNumber new &short:$self add &int:anOperand.

    /// Returns the difference
    #method(stacksafe) subtract &int:anOperand
        = IntNumber new &short:$self subtract &int:anOperand.

    /// Returns the product
    #method(stacksafe) multiply &int:anOperand
        = IntNumber new &short:$self multiply &int:anOperand.
        
    /// Returns the result of integer division
    #method(stacksafe) divide &int:anOperand
        = IntNumber new &short:$self divide &int:anOperand.

    /// Returns the sum
    #method(stacksafe) add &long:anOperand
        = LongNumber new &short:$self add &long:anOperand.

    /// Returns the difference
    #method(stacksafe) subtract &long:anOperand
        = LongNumber new &short:$self subtract &long:anOperand.

    /// Returns the product
    #method(stacksafe) multiply &long:anOperand
        = LongNumber new &short:$self multiply &long:anOperand.
        
    /// Returns the result of integer division
    #method(stacksafe) divide &long:anOperand
        = LongNumber new &short:$self divide &long:anOperand.

    /// Returns the sum
    #method(stacksafe) add &real:anOperand
        = RealNumber new &short:$self add &real:anOperand.

    /// Returns the difference
    #method(stacksafe) subtract &real:anOperand
        = RealNumber new &short:$self subtract &real:anOperand.

    /// Returns the product
    #method(stacksafe) multiply &real:anOperand
        = RealNumber new &short:$self multiply &real:anOperand.
        
    /// Returns the result of integer division
    #method(stacksafe) divide &real:anOperand
        = RealNumber new &short:$self divide &real:anOperand.

//    /// Returns the result of bitwise AND operation
//    #method(type:short,stacksafe) and &short:anOperand
//        => system'core_routines'short_and.
//
//    /// Returns the result of bitwise OR operation
//    #method(type:short,stacksafe) or &short:anOperand
//        => system'core_routines'short_or.
//
//    /// Returns the result of bitwise XOR operation
//    #method(type:short,stacksafe) xor &short:anOperand
//        => system'core_routines'short_xor.
//
//    /// Returns the result of bitwise AND operation
//    #method(stacksafe) and &byte:anOperand
//        => system'core_routines'short_and.
//
//    /// Returns the result of bitwise OR operation
//    #method(stacksafe) or &byte:anOperand
//        => system'core_routines'short_or.
//
//    /// Returns the result of bitwise XOR operation
//    #method(stacksafe) xor &byte:anOperand
//        => system'core_routines'short_xor.
//        
//    /// Returns the result of bitwise AND operation
//    #method(stacksafe) and &int:anOperand
//        = IntNumber new &short:$self and &int:anOperand.        
//        
//    /// Returns the result of bitwise OR operation
//    #method(stacksafe) or &int:anOperand
//        = IntNumber new &short:$self or &int:anOperand.        
//        
//    /// Returns the result of bitwise XOR operation
//    #method(stacksafe) xor &int:anOperand
//        = IntNumber new &short:$self xor &int:anOperand.        
//        
//    /// Returns the result of bitwise AND operation
//    #method(stacksafe) and &long:anOperand
//        = LongNumber new &short:$self and &long:anOperand.        
//        
//    /// Returns the result of bitwise OR operation
//    #method(stacksafe) or &long:anOperand
//        = LongNumber new &short:$self or &long:anOperand.        
//        
//    /// Returns the result of bitwise XOR operation
//    #method(stacksafe) xor &long:anOperand
//        = LongNumber new &short:$self xor &long:anOperand.        
//
//    /// Bitwise shift operation
//    #method(type:short,stacksafe) shift &int:anOrder
//        => system'core_routines'short_shift.
//
//    #method shift:anOrder
//        <= shift &int:(anOrder int).
//
//    #method(type:short) shift &index:anOrder
//        <= shift &int:(anOrder int).

    /// Returns the literal presentation
    #method literal = literalConvertor convert &short:$self.

    /// Returns the literal presentation
    #method wide = wideConvertor convert &short:$self.

//    /// Bitwise inversion
//    #method(type:byte,stacksafe) not
//        => system'core_routines'short_not.

    /// Clones the value
    #method clone = ShortNumber new &short:$self.    
                
//    // Reads the object hash code into the output parameter
//    #method(stacksafe) readHashCode &vint:aCode => system'core_routines'int_hashcode.
//    
//    #method(embeddable,type:int,suppress:w3) hash_code
//    [
//        #var(type:int)aCode.
//        $self readHashCode &vint:aCode.
//        
//        ^ aCode.
//    ]
}

// --- IntNumber ---

/// A signed 32 bit integer
#class(sealed,integerof:4,numeric:int,dispatchable:int) IntNumber :: IntBaseNumber
{
    /// Creates the object with the minimal value
    #constructor min
        = -2147483648.

    /// Creates the object with the maximal value
    #constructor max
        = 2147483647.
    
    /// Creates the object with specified value
    #constructor(stacksafe,suppress:w3) new &int:value
        = value.

    /// Creates the object with specified value
    #constructor(stacksafe) new &short:value
        = intConvertor convert &short:value.

    /// Creates the object with specified value
    #constructor(stacksafe) new &byte:value
        = intConvertor convert &byte:value.

    /// Creates the object with a default value (0)
    #constructor new
        <= new &int:0.

    /// Creates the object with specified value
    #constructor new : anObject
        <= new &int:(anObject int).

    /// Returns the value as a short integer    
    #method(stacksafe) short = shortConvertor convert &int:$self.

    /// Returns the byte value    
    #method byte = byteConvertor convert &int:$self.

    /// Returns the long integer
    #method long = LongNumber new &int:$self.

    /// Returns the real number
    #method real = RealNumber new &int:$self.

//    #method(stacksafe)readShort &vshort:aValue
//    [
//        shortConvertor convert &int:$self &vshort:aValue.
//    ]

    /// Compares the integer value
    #method(bool,stacksafe) equal &short:value 
        = $self equal &int:(IntNumber new &short:value).

    /// Compares the integer value
    #method(bool,stacksafe) equal &byte:value
        = $self equal &int:(IntNumber new &byte:value).
        
    /// Compares the integer value
    #method(bool,stacksafe) equal &long:anOperand 
        = LongNumber new &int:$self equal &long:anOperand.

    /// Compares the integer value
    #method(bool,stacksafe) equal &real:anOperand 
        = RealNumber new &int:$self equal &real:anOperand.

    /// Compares the integer value
    #method(bool,stacksafe) less &short:value
        = $self equal &int:(IntNumber new &short:value).

    /// Compares the integer value
    #method(bool,stacksafe) less &byte:value
        = $self less &int:(IntNumber new &byte:value).
        
    /// Compares the integer value
    #method(bool,stacksafe) less &long:anOperand 
        = LongNumber new &int:$self less &long:anOperand.

    /// Compares the integer value
    #method(bool,stacksafe) less &real:anOperand 
        = RealNumber new &int:$self less &real:anOperand.

    /// Returns the sum of integers
    #method(stacksafe) add &short:value
        = $self add &int:(IntNumber new &short:value).

    /// Returns the sum of integers
    #method(stacksafe) add &byte:value
        = $self add &int:(IntNumber new &byte:value).

    /// Returns the sum of integers
    #method(stacksafe) add &long:anOperand
        = LongNumber new &int:$self add &long:anOperand.

    /// Returns the sum of integers
    #method(stacksafe) add &real:anOperand
        = RealNumber new &int:$self add &real:anOperand.

    /// Returns the difference of integers
    #method(stacksafe) subtract &short:value
        = $self subtract &int:(IntNumber new &short:value).

    /// Returns the difference of integers
    #method(stacksafe) subtract &byte:value
        = $self subtract &int:(IntNumber new &byte:value).

    /// Returns the difference of integers
    #method(stacksafe) subtract &long:anOperand
        = LongNumber new &int:$self subtract &long:anOperand.

    /// Returns the difference of integers
    #method(stacksafe) subtract &real:anOperand
        = RealNumber new &int:$self subtract &real:anOperand.

    /// Returns the product of integers
    #method(stacksafe) multiply &short:value
        = $self multiply &int:(IntNumber new &short:value).        

    /// Returns the product of integers
    #method(stacksafe) multiply &byte:value
        = $self multiply &int:(IntNumber new &byte:value).        

    /// Returns the product of integers
    #method(stacksafe) multiply &long:anOperand
        = LongNumber new &int:$self multiply &long:anOperand.

    /// Returns the product of integers
    #method(stacksafe) multiply &real:anOperand
        = RealNumber new &int:$self multiply &real:anOperand.

    /// Returns the result of integer division
    #method(stacksafe) divide &short:value
        = $self divide &int:(IntNumber new &short:value).        

    /// Returns the result of integer division
    #method(stacksafe) divide &byte:value
        = $self divide &int:(IntNumber new &byte:value).        

    /// Returns the result of integer division
    #method(stacksafe) divide &long:anOperand
        = LongNumber new &int:$self divide &long:anOperand.

    /// Returns the result of integer division
    #method(stacksafe) divide &real:anOperand
        = RealNumber new &int:$self divide &real:anOperand.

//    /// Returns the result of bitwise AND operation
//    #method(type:int,stacksafe) and &int:anOperand
//        => system'core_routines'int_and.
//
//    /// Returns the result of bitwise AND operation
//    #method(type:int,stacksafe) and &short:anOperand
//        => system'core_routines'int_and.
//
//    /// Returns the result of bitwise AND operation
//    #method(type:int,stacksafe) and &byte:anOperand
//        => system'core_routines'int_and.
//
//    /// Returns the result of bitwise AND operation
//    #method(stacksafe) and &long:anOperand
//        = LongNumber new &int:$self and &long:anOperand.
//
//    /// Returns the result of bitwise OR operation
//    #method(type:int,stacksafe) or &int:anOperand
//        => system'core_routines'int_or.
//
//    /// Returns the result of bitwise OR operation
//    #method(type:int,stacksafe) or &short:anOperand
//        => system'core_routines'int_or.
//
//    /// Returns the result of bitwise OR operation
//    #method(type:int,stacksafe) or &byte:anOperand
//        => system'core_routines'int_or.
//
//    /// Returns the result of bitwise OR operation
//    #method(stacksafe) or &long:anOperand
//        = LongNumber new &int:$self or &long:anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    #method(type:int,stacksafe) xor &int:anOperand
//        => system'core_routines'int_xor.
//
//    /// Returns the result of bitwise XOR operation
//    #method(type:int,stacksafe) xor &short:anOperand
//        => system'core_routines'int_xor.
//
//    /// Returns the result of bitwise XOR operation
//    #method(type:int,stacksafe) xor &byte:anOperand
//        => system'core_routines'int_xor.
//
//    /// Returns the result of bitwise XOR operation
//    #method(stacksafe) xor &long:anOperand
//        = LongNumber new &int:$self xor &long:anOperand.
//
//    #method shift:anOrder
//        <= shift &int:(anOrder int).
//    
//    /// Bitwise shift operation
//    #method(type:int) shift &index:anOrder
//        <= shift &int:(anOrder int).
//    
//    #method(type:int,stacksafe) shift &int:anOrder
//        => system'core_routines'int_shift.
//
//    /// Bitwise shift operation
//    #method(stacksafe) shift &int:anOrder &vint:aResult
//        => system'core_routines'int_shift2.

    /// Returns the literal presentation
    #method literal = literalConvertor convert &int:$self.

    /// Returns the literal presentation
    #method wide = wideConvertor convert &int:$self.

//    /// Bitwise inversion
//    #method(type:int,stacksafe) not
//        => system'core_routines'int_not.
//
//    /// Negates the value
//    #method(stacksafe,suppress:w3) negative
//        = 0 - $self.

    /// Clones the value
    #method clone = IntNumber new &int:$self.

//    // Reads the object hash code into the output parameter
//    #method(stacksafe) readHashCode &vint:aCode => system'core_routines'int_hashcode.
//    
//    #method(type:int,embeddable,suppress:w3) hash_code
//    [
//        #var(type:int)aCode.
//        $self readHashCode &vint:aCode.
//        
//        ^ aCode.
//    ]
}

// --- LongNumber ---

/// A signed 64 bit integer value
#class(sealed,integerof:8,numeric:long,dispatchable:long) LongNumber :: IntBaseNumber
{
    /// Returns the minimal value
    #constructor min
        = -9223372036854775808l.

    /// Returns the maximal value
    #constructor max
        = 9223372036854775807l.

    /// Creates the object with a default value (0)
    #constructor new
        <= new &long:0l.

    /// Creates the object with specified value
    #constructor(stacksafe,suppress:w3) new &long:value
        = value.

    /// Creates the object with specified value
    #constructor(stacksafe) new &int:value
        = longConvertor convert &int:value.

    /// Creates the object with specified value
    #constructor(stacksafe) new &short:value 
        = longConvertor convert &short:value.

    /// Creates the object with specified value
    #constructor(stacksafe) new &byte:value 
        = longConvertor convert &byte:value.

    /// Creates the object with specified value
    #constructor new : anObject
        <= new &long:(anObject long).

    /// Returns an integer value
    #method int = intConvertor convert &long:$self.

    /// Returns the value as a short integer    
    #method short = shortConvertor convert &long:$self.
    
    /// Returns the byte value
    #method byte = byteConvertor convert &long:$self.

    /// Returns a real value
    #method real = RealNumber new &long:$self.

    /// Compares the integer value
    #method(bool,stacksafe) equal &int:anOperand 
        = $self equal &long:(LongNumber new &int:anOperand).

    /// Compares the integer value
    #method(bool,stacksafe) equal &short:anOperand 
        = $self equal &long:(LongNumber new &short:anOperand).

    /// Compares the integer value
    #method(bool,stacksafe) equal &byte:anOperand 
        = $self equal &long:(LongNumber new &byte:anOperand).

    /// Compares the integer value
    #method(stacksafe) equal &real:anOperand 
        = RealNumber new &long:$self equal &real:anOperand.

    /// Compares the integer value
    #method(bool,stacksafe) less &int:anOperand 
        = $self less &long:(LongNumber new &int:anOperand).

    /// Compares the integer value
    #method(bool,stacksafe) less &short:anOperand 
        = $self less &long:(LongNumber new &short:anOperand).

    /// Compares the integer value
    #method(bool,stacksafe) less &byte:anOperand 
        = $self less &long:(LongNumber new &byte:anOperand).

    /// Compares the integer value
    #method(stacksafe) less &real:anOperand 
        = RealNumber new &long:$self less &real:anOperand.

    /// Returns the sum of integers
    #method(stacksafe) add &int:anOperand
        = $self add &long:(LongNumber new &int:anOperand).

    /// Returns the sum of integers
    #method(stacksafe) add &short:anOperand
        = $self add &long:(LongNumber new &short:anOperand).

    /// Returns the sum of integers
    #method(stacksafe) add &byte:anOperand
        = $self add &long:(LongNumber new &byte:anOperand).

    /// Returns the sum of integers
    #method(stacksafe) add &real:anOperand
        = RealNumber new &long:$self add &real:anOperand.

    /// Returns the difference of integers
    #method(stacksafe) subtract &int:anOperand
        = $self subtract &long:(LongNumber new &int:anOperand).

    /// Returns the difference of integers
    #method(stacksafe) subtract &short:anOperand
        = $self subtract &long:(LongNumber new &short:anOperand).

    /// Returns the difference of integers
    #method(stacksafe) subtract &byte:anOperand
        = $self subtract &long:(LongNumber new &byte:anOperand).

    /// Returns the difference of integers
    #method(stacksafe) subtract &real:anOperand
        = RealNumber new &long:$self subtract &real:anOperand.

    /// Returns the product of integers
    #method(stacksafe) multiply &real:anOperand
        = RealNumber new &long:$self multiply &real:anOperand.

    /// Returns the product of integers
    #method(stacksafe) multiply &int:anOperand
        = $self multiply &long:(LongNumber new &int:anOperand).

    /// Returns the product of integers
    #method(stacksafe) multiply &byte:anOperand
        = $self multiply &long:(LongNumber new &byte:anOperand).

    /// Returns the product of integers
    #method(stacksafe) multiply &short:anOperand
        = $self multiply &long:(LongNumber new &short:anOperand).

    /// Returns the result of integer division
    #method(stacksafe) divide &int:anOperand
        = $self divide &long:(LongNumber new &int:anOperand).

    /// Returns the result of integer division
    #method(stacksafe) divide &short:anOperand
        = $self divide &long:(LongNumber new &short:anOperand).

    /// Returns the result of integer division
    #method(stacksafe) divide &byte:anOperand
        = $self divide &long:(LongNumber new &byte:anOperand).

    /// Returns the result of integer division
    #method(stacksafe) divide &real:anOperand
        = RealNumber new &long:$self divide &real:anOperand.
        
//    /// Returns the result of bitwise AND operation
//    #method(type:long,stacksafe) and &long:anOperand
//        => system'core_routines'long_and.
//
//    /// Returns the result of bitwise AND operation
//    #method(stacksafe) and &int:anOperand
//        = $self and &long:(LongNumber new &int:anOperand).
//
//    /// Returns the result of bitwise AND operation
//    #method(stacksafe) and &short:anOperand
//        = $self and &long:(LongNumber new &short:anOperand).
//
//    /// Returns the result of bitwise AND operation
//    #method(stacksafe) and &byte:anOperand
//        = $self and &long:(LongNumber new &byte:anOperand).
//
//    /// Returns the result of bitwise OR operation
//    #method(type:long,stacksafe) or &long:anOperand
//        => system'core_routines'long_or.
//
//    /// Returns the result of bitwise OR operation
//    #method(stacksafe) or &int:anOperand
//        = $self or &long:(LongNumber new &int:anOperand).
//
//    /// Returns the result of bitwise OR operation
//    #method(stacksafe) or &short:anOperand
//        = $self or &long:(LongNumber new &short:anOperand).
//
//    /// Returns the result of bitwise OR operation
//    #method(stacksafe) or &byte:anOperand
//        = $self or &long:(LongNumber new &byte:anOperand).        
//
//    /// Returns the result of bitwise XOR operation
//    #method(type:long,stacksafe) xor &long:anOperand
//        => system'core_routines'long_xor.
//
//    /// Returns the result of bitwise XOR operation
//    #method(stacksafe) xor &int:anOperand
//        = $self xor &long:(LongNumber new &int:anOperand).
//
//    /// Returns the result of bitwise XOR operation
//    #method(stacksafe) xor &short:anOperand
//        = $self xor &long:(LongNumber new &short:anOperand).
//
//    /// Returns the result of bitwise XOR operation
//    #method(stacksafe) xor &byte:anOperand
//        = $self xor &long:(LongNumber new &byte:anOperand).
//
//    #method shift:anOrder
//        <= shift &int:(anOrder int).
//    
//    /// Returns the result of bitwise shift operation
//    #method(type:long) shift &index:anOrder
//        <= shift &int:(anOrder int).
//    
//    #method(type:long,stacksafe) shift &int:anOrder
//        => system'core_routines'long_shift.

    /// Returns the literal presentation
    #method literal = literalConvertor convert &long:$self.

    /// Returns the literal presentation
    #method wide = wideConvertor convert &long:$self.

//    /// Negates the value
//    #method(type:long,stacksafe,suppress:w3) negative = 0l - $self.
//
//    /// Bitwise inversion
//    #method(type:long,stacksafe) not
//        => system'core_routines'long_not.

    /// Clones the value
    #method clone = LongNumber new &long:$self.
    
//    // Reads the object hash code into the output parameter
//    #method(stacksafe) readHashCode &vint:aCode => system'core_routines'int_hashcode.
//    
//    #method(type:int,embeddable,suppress:w3) hash_code
//    [
//        #var(type:int)aCode.
//        $self readHashCode &vint:aCode.
//        
//        ^ aCode.
//    ]
}

// --- realNumber ---

/// A 64bit float numeric value
#class(sealed,floatof:8,numeric:real,dispatchable:real) RealNumber :: BaseNumber
{
    /// Returns the minimal value
    #constructor min
        = -1.7976931348623157e+308r.
    
    /// Returns the maximal value
    #constructor max
        = 1.7976931348623157e+308r.

    /// Creates the object with a default value (0)
    #constructor new
        <= new &real:0.0r.

    /// Creates the object with specified value
    #constructor(stacksafe,suppress:w3) new &real:value
        = value.
  
    /// Creates the object with specified value
    #constructor(stacksafe) new &int:anOperand 
        => system'core_routines'real_copyint.

    /// Creates the object with specified value
    #constructor(stacksafe) new &short:anOperand 
        => system'core_routines'real_copyint.

    /// Creates the object with specified value
    #constructor(stacksafe) new &byte:anOperand 
        => system'core_routines'real_copyint.

    /// Creates the object with specified value
    #constructor(stacksafe) new &long:anOperand 
        => system'core_routines'real_copylong.
        
    /// Creates the object with specified value
    #constructor new : anObject
        <= new &real:(anObject real).
        
    /// Compares the numeric values
    #method(bool,stacksafe) equal &int:anOperand
        = $self equal &real:(RealNumber new &int:anOperand).

    /// Compares the numeric values
    #method(bool,stacksafe) equal &byte:anOperand
        = $self equal &real:(RealNumber new &byte:anOperand).

    /// Compares the numeric values
    #method(bool,stacksafe) equal &short:anOperand
        = $self equal &real:(RealNumber new &short:anOperand).

    /// Compares the numeric values
    #method(bool,stacksafe) equal &long:anOperand
        = $self equal &real:(RealNumber new &long:anOperand).

    /// Compares the numeric values
    #method(bool,stacksafe) less &int:anOperand
        = $self less &real:(RealNumber new &int:anOperand).

    /// Compares the numeric values
    #method(bool,stacksafe) less &byte:anOperand
        = $self less &real:(RealNumber new &byte:anOperand).

    /// Compares the numeric values
    #method(bool,stacksafe) less &short:anOperand
        = $self less &real:(RealNumber new &short:anOperand).

    /// Compares the numeric values
    #method(bool,stacksafe) less &long:anOperand
        = $self less &real:(RealNumber new &long:anOperand).

    /// Returns the sum of numbers
    #method(stacksafe) add &int:anOperand
        = $self add &real:(RealNumber new &int:anOperand).

    /// Returns the sum of numbers
    #method(stacksafe) add &byte:anOperand
        = $self add &real:(RealNumber new &byte:anOperand).

    /// Returns the sum of numbers
    #method(stacksafe) add &short:anOperand
        = $self add &real:(RealNumber new &short:anOperand).

    /// Returns the sum of numbers
    #method(stacksafe) add &long:anOperand
        = $self add &real:(RealNumber new &long:anOperand).

    /// Returns the difference of numbers
    #method(stacksafe) subtract &int:anOperand
        = $self subtract &real:(RealNumber new &int:anOperand).

    /// Returns the difference of numbers
    #method(stacksafe) subtract &byte:anOperand
        = $self subtract &real:(RealNumber new &byte:anOperand).

    /// Returns the difference of numbers
    #method(stacksafe) subtract &short:anOperand
        = $self subtract &real:(RealNumber new &short:anOperand).

    /// Returns the difference of numbers
    #method(stacksafe) subtract &long:anOperand
        = $self subtract &real:(RealNumber new &long:anOperand).

    /// Returns the product of numbers
    #method(stacksafe) multiply &int:anOperand
        = $self multiply &real:(RealNumber new &int:anOperand).

    /// Returns the product of numbers
    #method(stacksafe) multiply &byte:anOperand
        = $self multiply &real:(RealNumber new &byte:anOperand).

    /// Returns the product of numbers
    #method(stacksafe) multiply &short:anOperand
        = $self multiply &real:(RealNumber new &short:anOperand).

    /// Returns the product of numbers
    #method(stacksafe) multiply &long:anOperand
        = $self multiply &real:(RealNumber new &long:anOperand).

    /// Returns the result of the division
    #method(stacksafe) divide &int:anOperand
        = $self divide &real:(RealNumber new &int:anOperand).

    /// Returns the result of the division
    #method(stacksafe) divide &byte:anOperand
        = $self divide &real:(RealNumber new &byte:anOperand).
        
    /// Returns the result of the division
    #method(stacksafe) divide &short:anOperand
        = $self divide &real:(RealNumber new &short:anOperand).

    /// Returns the result of the division
    #method(stacksafe) divide &long:anOperand
        = $self divide &real:(RealNumber new &long:anOperand).

//    /// Negates the value
//    #method(type:real,suppress:w3) negative = 0r - $self.

    /// Returns the literal presentation
    #method literal = literalConvertor convert &real:$self.
    
    /// Returns the literal presentation
    #method wide = wideConvertor convert &real:$self.

    /// Clones the value    
    #method clone = RealNumber new &real:$self.
    
//    // Reads the object hash code into the output parameter
//    #method(stacksafe) readHashCode &vint:aCode => system'core_routines'int_hashcode.
//    
//    #method(type:int,embeddable,suppress:w3) hash_code
//    [
//        #var(type:int)aCode.
//        $self readHashCode &vint:aCode.
//        
//        ^ aCode.
//    ]
}
////
////// --- CharValue ---
////
/////// An UTF-32 character symbol
////#class(sealed,integer:4) CharValue :: BaseValue
////{
////    /// Creates the object with a default value (0)
////    #constructor new
////        => system'core_routines'int_init.        
////
////    /// Creates the object with specified value
////    #constructor(stacksafe) new &char:anOperand
////        => system'core_routines'int_copy.
////
////    /// Creates the object with specified value
////    #constructor new : anOperand
////        <= new &char:(anOperand cast:%convert &to:charConvertor char).
////
////    /// Creates the object with specified numeric value
////    #constructor(stacksafe) new &int:anOperand
////        <= new &char:(charConvertor convert &int:anOperand).
////
////    /// Returns the minimal possible character value (0)
////    #constructor min
////        <= %new &char:#0.
////
////    /// Returns the maximal possible character value (010FFFFh)
////    #constructor max
////        <= %new &char:#010FFFFh.
////
////    /// Returns itself
////    #method char = $self.
////
////    #method(stacksafe) readInt &vint:aRetVal
////    [
////        intConvertor convert &char:$self &vint:aRetVal.
////    ]
////
////    /// Returns the numeric value
////    #method(stacksafe,embeddable,suppress:w2) int
////    [
////        #var(type:int)aRetVal.
////        $self readInt &vint:aRetVal.
////        
////        ^ aRetVal.
////    ]
////    
////    #method add : anObject
////        = anObject cast:%eval &to:
////            {
////                eval:aChar &int:n
////                    = aChar add &int:n.
////                    
////                eval:aChar &short:n
////                    = aChar add &int:(n int).
////                    
////                eval:aChar &byte:n
////                    = aChar add &int:(n int).
////                    
////                eval:aChar &literal:aLiteral
////                    = aChar literal add &literal:aLiteral.
////            }
////            &with:$self.
////        
////    #method subtract : anObject
////        = anObject cast:%eval &to:
////            {
////                eval:aChar &int:n
////                    = aChar subtract &int:n.
////                    
////                eval:aChar &short:n
////                    = aChar subtract &int:(n int).
////                    
////                eval:aChar &byte:n
////                    = aChar subtract &int:(n int).
////            }
////            &with:$self.
////        
////    #method add &int:aNumber
////    [
////        #var(type:int)n := $self int.
////        
////        ^ CharValue new &int:(n + aNumber).
////    ]
////        
////    #method subtract &int:aNumber
////    [
////        #var(type:int)n := $self int.
////        
////        ^ CharValue new &int:(n - aNumber).
////    ]
////    
////    /// Compares values
////    #method(stacksafe) equal &char:anOperand 
////        => system'core_routines'int_equal.
////
////    /// Compares values
////    #method(stacksafe) less &char:anOperand 
////        => system'core_routines'int_less.
////
////    /// Compares values
////    #method(stacksafe) equal &int:anOperand 
////        => system'core_routines'int_equal.
////
////    /// Compares values
////    #method(stacksafe) less &int:anOperand 
////        => system'core_routines'int_less.
////
////    /// Clones the value
////    #method clone = CharValue new &char:$self.
////
////    /// Dispatches the verb to the target object with char subject
////    #method cast:aVerb &to:aTarget = aTarget::aVerb eval &char:$self.
////    
////    /// Dispatches the verb to the target object with char subject
////    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &char:$self.
////
////    /// Returns the literal value
////    #method(stacksafe) literal = literalConvertor convert &char:$self.
////
////    /// Returns the wide literal value
////    #method(stacksafe) wide = wideConvertor convert &char:$self.
////    
////    /// Reads the number of bytes to represent the symbol
////    #method(stacksafe) readLength &vint:aLength
////        => system'core_routines'char_getlen.
////        
////    /// Reads the number of bytes to represent the symbol
////    #method(stacksafe) readWideLength &vint:aRetVal
////    [
////        #var(type:int)aLength.
////        $self readLength &vint:aLength.
////        
////        aLength := aLength + 1.
////        aLength := aLength / 2.
////
////        aRetVal << aLength.
////    ]
////        
////    /// Retuns the number of bytes to represent the symbol        
////    #method(stacksafe,type:int,embeddable,suppress:w3) length
////    [
////        #var(type:int)aLength.
////        $self readLength &vint:aLength.
////        
////        ^ aLength.
////    ]
////    
////    /// Retuns the number of short integers to represent the symbol
////    #method(stacksafe,type:int,embeddable,suppress:w3) wideLength
////    [
////        #var(type:int)aLength.
////        $self readWideLength &vint:aLength.
////        
////        ^ aLength.
////    ]
////
////    // Reads the object hash code into the output parameter
////    #method(stacksafe) readHashCode &vint:aCode => system'core_routines'int_hashcode.
////    
////    #method(embeddable,type:int,suppress:w3) hash_code
////    [
////        #var(type:int)aCode.
////        $self readHashCode &vint:aCode.
////        
////        ^ aCode.
////    ]
////}
////
////// --- Char ---
////
/////// An UTF-32 character variable
///////
/////// Extends CharValue class
////#class(sealed,struct:embeddable) Char :: BaseVariable
////{
////    #field(type:char)theValue.
////    
////    /// Creates a variable with a default value (0)
////    #constructor new
////    [
////        theValue := #0.
////    ]
////    
////    /// Creates a variable with a specified value
////    #constructor new : aValue  
////    [
////        aValue cast:%write &to:$self.
////    ]
////    
////    /// Creates a variable with a specified value
////    #constructor(stacksafe) new &char:aValue
////    [
////        theValue := aValue.
////    ]
////    
////    /// Creates a variable with a specified value
////    #constructor new &int:aValue
////    [
////        theValue := charConvertor convert &int:aValue.
////    ]
////    
////    /// Returns itself
////    #method vchar = $self.
////    
////    /// Returns the character value
////    #method char = CharValue new &char:theValue.
////    
////    /// Copies the character value
////    #method(stacksafe) write &char:anOperand
////    [
////        theValue := anOperand.
////    ]
////    
////    /// Copies the character value
////    #method(stacksafe) write &int:value
////    [
////        theValue := charConvertor convert &int:value.
////    ]
////    
////    /// Reads the character value
////    #method(stacksafe) read &vchar:aRetVal
////    [
////        aRetVal << theValue.
////    ]
////    
////    /// Compares values
////    #method(type:bool,stacksafe) equal &char:anOperand 
////        = theValue == anOperand.
////
////    /// Compares values
////    #method(type:bool,stacksafe) less &char:anOperand
////        = theValue < anOperand.
////
////    // Returns the value        
////    #method value = CharValue new &char:theValue.
////
////    /// Returns the literal value
////    #method literal = literalConvertor convert &char:theValue.
////
////    /// Returns the literal value
////    #method wide = wideConvertor convert &char:theValue.
////
////    /// Clones the variable
////    #method clone = Char new &char:theValue.
////    
////    /// Reads the number of bytes to represent the symbol
////    #method(stacksafe) readLength &vint:aLength
////        => system'core_routines'char_getlen.
////        
////    /// Retuns the number of bytes to represent the symbol        
////    #method length
////    [
////        #var(type:int)aLength.
////        $self readLength &vint:aLength.
////        
////        ^ IntNumber new &int:aLength.
////    ]
////    
////    /// Dispatches the verb to the target object with char subject
////    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &char:(CharValue new &char:theValue).
////    
////    /// Dispatches the verb to the target object with char subject
////    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &char:(CharValue new &char:theValue).
////
////    /// Extends the character value
////    #method(suppress:w3) => theValue.    
////}
////
////// === Basic Factory symbols ==
////
////#class BaseExtender
////{
////    #field theObject.
////    
////    /// Returns true if the specified object is equal to the current object; otherwise, false.
////    #method(type:bool) equal : anObject
////        => theObject.
////      
////    /// Dispatches the verb to the target object
////    #method cast : aVerb &to:aTarget => theObject.
////    
////    /// Dispatches the verb to the target object
////    #method cast : aVerb &to:aTarget &with:aParam => theObject.
////    
////    #method => theObject.
////}
////
/////// A generic variable. 
///////
/////// Extends an assigned value
////#class(variable) Variable
////{
////    #field theValue.
////    
////    /// Creates the empty variable
////    #constructor new
////    [
////    ]        
////        
////    /// Creates the variable
////    #constructor new : aValue
////    [
////        theValue := aValue.
////    ]
////
////    /// Returns true if the specified object is equal to the current object; otherwise, false.
////    #method(type:bool) equal : anObject
////        => theValue.
////      
////    /// Assigns the variable
////    #method write : aValue
////    [
////        theValue := aValue.
////    ]
////        
////    /// Returns the value
////    #method value = theValue.        
////
////    /// Appends the value
////    #method append : aValue
////    [
////        ($nil == theValue)
////            ? [ theValue := aValue. ]
////            ! [ theValue := theValue + aValue. ].
////    ]
////    
////    /// Subtracts the value
////    #method reduce : aValue
////    [
////        theValue := theValue - aValue.
////    ]
////
////    /// Multiplies by the value
////    #method multiplyBy : aValue
////    [
////        theValue := theValue * aValue.
////    ]
////    
////    /// Divides into the value
////    #method divideInto : aValue
////    [
////        theValue := theValue / aValue.
////    ]
////    
////    /// Dispatches the verb to the target object
////    #method cast : aVerb &to:aTarget => theValue.
////    
////    /// Dispatches the verb to the target object
////    #method cast : aVerb &to:aTarget &with:aParam => theValue.
////
////    /// Expands the value
////    #method => theValue.
////}
////
////// --- KeyValue ---
////
/////// A key - object pair
///////
/////// Extends the object with a key property
////#class(sealed) KeyValue
////{
////    #field theKey.
////    #field theObject.
////    
////    /// Creates a new objext
////    #constructor new &key:aKey &object:anObject
////    [
////        theKey := aKey.
////        theObject := anObject.
////    ]
////    
////    #method keyvalue = $self.
////    
////    /// Returns the key
////    #method key = theKey.
////    
////    /// Returns the key
////    #method object = theObject.
////
////    #method set &object:anObject
////    [
////        theObject := anObject.
////    ]
////    
////    /// Dispatches the verb to the target object
////    #method cast : aVerb &to:aTarget => theObject.
////    
////    /// Dispatches the verb to the target object
////    #method cast : aVerb &to:aTarget &with:aParam => theObject.
////            
////    /// Redirects the incoming messages to the object
////    #method => theObject.
////}
////
////// --- DTO ---
////
////#class DTO
////{
////    /// Dispatches the verb to the target object
////    #method cast : verb &to:target
////        = target::verb eval &dto:$self.
////    
////    /// Dispatches the verb to the target object
////    #method cast : verb &to:target &with:param
////        = target::verb eval:param &dto:$self.
////}

// === Basic Convertors ===

// --- convertor helpers ---

/// A byte convertor
#symbol byteConvertor =
{
//    /// Converts a character into a byte value and saves the result into an output parameter
//    #method(stacksafe) convert &char:aValue &vbyte:aRetVal
//        => system'core_routines'byte_copychar.
        
    /// Converts a short integer into a byte value and saves the result into an output parameter
    #method(byte,stacksafe) convert &short:aValue &vbyte:aRetVal
        => system'core_routines'__byteconvertor.
        
    /// Converts an integer into a byte value and saves the result into an output parameter
    #method(byte,stacksafe) convert &int:aValue &vbyte:aRetVal
        => system'core_routines'__byteconvertor.

    /// Converts a long integer into a byte value and saves the result into an output parameter
    #method(stacksafe) convert &long:aValue &vbyte:aRetVal
        => system'core_routines'__byteconvertor.
        
//    /// Converts a real number into a byte value and saves the result into an output parameter
//    #method(stacksafe) convert &real:aValue &vbyte:aRetVal
//        => system'core_routines'byte_copyreal.
//        
//    /// Converts a literal value into a byte value and saves the result into an output parameter
//    #method(stacksafe) convert &literal:aValue &vbyte:aRetVal
//        => system'core_routines'byte_copystr.
//        
//    /// Converts a wide literal value into a byte value and saves the result into an output parameter
//    #method(stacksafe) convert &wide:aValue &vbyte:aRetVal
//        => system'core_routines'byte_copywstr.
        
    /// Returns the parameter        
    #method(byte,stacksafe) convert &byte:anInt
        = ByteNumber new &byte:(anInt).
        
//    /// Converts a character into a byte value
//    #method(type:byte,stacksafe) convert &char:aValue
//    [
//        #var(type:byte)aRetVal.
//        $self convert &char:aValue &vbyte:aRetVal.
//        
//        ^ ByteNumber new &byte:aRetVal.
//    ]
        
    /// Converts a short integer into a byte value
    #method(byte,stacksafe) convert &short:aValue
    [
        #var(byte)aRetVal.
        $self convert &short:aValue &vbyte:aRetVal.
        
        ^ ByteNumber new &byte:aRetVal.
    ]
        
    /// Converts an integer into a byte value
    #method(byte,stacksafe) convert &int:aValue
    [
        #var(byte)aRetVal.
        $self convert &int:aValue &vbyte:aRetVal.
        
        ^ ByteNumber new &byte:aRetVal.
    ]

    /// Converts a long integer into a byte value
    #method(byte,stacksafe) convert &long:aValue
    [
        #var(byte)aRetVal.
        $self convert &long:aValue &vbyte:aRetVal.
        
        ^ ByteNumber new &byte:aRetVal.
    ]

//    /// Converts a real number into a byte value
//    #method(type:byte,stacksafe) convert &real:aValue
//    [
//        #var(type:byte)aRetVal.
//        $self convert &real:aValue &vbyte:aRetVal.
//        
//        ^ ByteNumber new &byte:aRetVal.
//    ]
//                
//    /// Converts a literal value into a byte value
//    #method(type:byte,stacksafe) convert &literal:aLiteral
//    [
//        #var(type:byte)aValue.
//        $self convert &literal:aLiteral &vbyte:aValue.
//        
//        ^ ByteNumber new &byte:aValue.
//    ]
//                
//    /// Converts a wide literal value into a byte value
//    #method(type:byte,stacksafe) convert &wide:aLiteral
//    [
//        #var(type:byte)aValue. 
//        $self convert &wide:aLiteral &vbyte:aValue.
//        
//        ^ ByteNumber new &byte:aValue.
//    ]
        
    #method convert : anObject
        = anObject cast:%convert &to:$self.
}.

/// A 16bit integer convertor
#symbol shortConvertor =
{
//    /// Converts a character into a short integer value and saves the result into an output parameter
//    #method(stacksafe) convert &char:aValue &vshort:aRetVal
//        => system'core_routines'word_copychar.
        
    /// Converts a byte value into a short integer value and saves the result into an output parameter
    #method(stacksafe) convert &byte:aValue &vshort:aRetVal
        => system'core_routines'__shortconvertor.
        
    /// Converts an integer into a short integer value and saves the result into an output parameter
    #method(stacksafe) convert &int:aValue &vshort:aRetVal
        => system'core_routines'__shortconvertor.
        
    /// Converts a long integer into a short integer value and saves the result into an output parameter
    #method(stacksafe) convert &long:aValue &vshort:aRetVal
        => system'core_routines'__shortconvertor.
        
//    /// Converts a real number into a short integer value and saves the result into an output parameter
//    #method(stacksafe) convert &real:aValue &vshort:aRetVal
//        => system'core_routines'word_copyreal.
//        
//    /// Converts a literal string into a short integer value and saves the result into an output parameter
//    #method(stacksafe) convert &literal:aValue &vshort:aRetVal
//        => system'core_routines'word_copystr.
//        
//    /// Converts a wide literal string into a short integer value and saves the result into an output parameter
//    #method(stacksafe) convert &wide:aValue &vshort:aRetVal
//        => system'core_routines'word_copywstr.
        
    /// Returns a short integer value        
    #method(short,stacksafe) convert &short:anInt
        = ShortNumber new &short:anInt.
        
    /// Converts a byte value into a short integer value
    #method(short,stacksafe) convert &byte:aValue
    [
        #var(short)aRetVal.
        $self convert &byte:aValue &vshort:aRetVal.
        
        ^ ShortNumber new &short:aRetVal.
    ]

    /// Converts an integer into a short integer value
    #method(short,stacksafe) convert &int:aValue
    [
        #var(short)aRetVal.
        $self convert &int:aValue &vshort:aRetVal.
        
        ^ ShortNumber new &short:aRetVal.
    ]

    /// Converts a long integer into a short integer value
    #method(short,stacksafe) convert &long:aValue
    [
        #var(short)aRetVal.
        $self convert &long:aValue &vshort:aRetVal.
        
        ^ ShortNumber new &short:aRetVal.
    ]
        
//    /// Converts a real number into a short integer value
//    #method(type:short,stacksafe) convert &real:aValue
//    [
//        #var(type:short)aRetVal.
//        $self convert &real:aValue &vshort:aRetVal.
//        
//        ^ ShortNumber new &short:aRetVal.
//    ]
//        
//    /// Converts a character into a short integer value
//    #method(type:short,stacksafe) convert &char:aValue
//    [
//        #var(type:short)aRetVal.
//        $self convert &char:aValue &vshort:aRetVal.
//        
//        ^ ShortNumber new &short:aRetVal.
//    ]
//        
//    /// Converts a literal string into a short integer value
//    #method(type:short,stacksafe) convert &literal:aValue
//    [
//        #var(type:short)aRetVal.
//        $self convert &literal:aValue &vshort:aRetVal.
//        
//        ^ ShortNumber new &short:aRetVal.
//    ]
//        
//    /// Converts a wide literal string into a short integer value
//    #method(type:short,stacksafe) convert &wide:aValue
//    [
//        #var(type:short)aRetVal.
//        $self convert &wide:aValue &vshort:aRetVal.
//        
//        ^ ShortNumber new &short:aRetVal.
//    ]
        
    #method convert : anObject
        = anObject cast:%convert &to:$self.
}.

/////// An Unicode character convertor
////#symbol charConvertor =
////{
////    /// Converts a byte value into a character and saves the result into an output parameter
////    #method(stacksafe) convert &byte:aValue &vchar:aRetVal
////        => system'core_routines'char_savebyte.
////        
////    /// Converts a short integer into a character and saves the result into an output parameter
////    #method(stacksafe) convert &short:aValue &vchar:aRetVal
////        => system'core_routines'char_saveshort.
////        
////    /// Converts an integer into a character and saves the result into an output parameter
////    #method(stacksafe) convert &int:aValue &vchar:aRetVal
////        => system'core_routines'char_saveint.
////        
////    /// Returns a parameter        
////    #method(type:char,stacksafe)convert &char:aChar
////        = CharValue new &char:aChar.
////  
////    #method(type:char,stacksafe) $convert &int:anIndex &literal:aLiteral &vchar:char
////        => system'core_routines'char_copystr.
////  
////    #method(type:char,stacksafe) $convert &int:anIndex &bytearray:aLiteral &vchar:char
////        => system'core_routines'char_copystr.
////  
////    #method(type:char,stacksafe) $convert &int:anIndex &wide:aLiteral &vchar:char
////        => system'core_routines'char_copywstr.
////        
////    #method(type:char,stacksafe) $convert &int:anIndex &shortarray:aLiteral &vchar:char
////        => system'core_routines'char_copywstr.
////
////    #method(stacksafe) $save &char:aChar &int:anIndex &shortarray:aCharArray &vint:aLength
////        => system'core_routines'sa_copychar.
////    
////    #method(stacksafe) $save &char:aChar &int:anIndex &bytearray:aCharArray &vint:aLength
////        => system'core_routines'ba_copychar.
////
////    /// Converts a literal string into a character                
////    #method(type:char,stacksafe) convert &literal:aValue
////    [
////        #var(type:char)aRetVal.
////        $self $convert &int:0 &literal:aValue &vchar:aRetVal.
////        
////        #var(type:int)l1.
////        #var(type:int)l2 := aValue length int.
////        aRetVal readLength &vint:l1.
////        
////        (l1 != l2)
////            ? [ #throw system'InvalidArgumentException new. ].
////        
////        ^ CharValue new &char:aRetVal.
////    ]        
////        
////    /// Converts a wide literal string into a character                
////    #method(type:char,stacksafe) convert &wide:aValue
////    [
////        #var(type:char)aRetVal.
////        $self $convert &int:0 &wide:aValue &vchar:aRetVal.
////
////        #var(type:int)l1.
////        #var(type:int)l2 := aValue length int.
////        aRetVal readWideLength &vint:l1.
////                
////        (l1 != l2)
////            ? [ #throw system'InvalidArgumentException new. ].
////        
////        ^ CharValue new &char:aRetVal.
////    ]        
////        
////    /// Converts a byte value into a character
////    #method(type:char,stacksafe) convert &byte:aValue
////    [
////        #var(type:char)aRetVal.
////        $self convert &byte:aValue &vchar:aRetVal.
////        
////        ^ CharValue new &char:aRetVal.
////    ]        
////        
////    /// Converts a short integer into a character
////    #method(type:char,stacksafe) convert &short:aValue
////    [
////        #var(type:char)aRetVal.
////        $self convert &short:aValue &vchar:aRetVal.
////        
////        ^ CharValue new &char:aRetVal.
////    ]        
////        
////    /// Converts an integer into a character
////    #method(type:char,stacksafe) convert &int:aValue
////    [
////        #var(type:char)aRetVal.
////        $self convert &int:aValue &vchar:aRetVal.
////        
////        ^ CharValue new &char:aRetVal.
////    ]
////        
////    #method convert : anObject
////        = anObject cast:%convert &to:$self.
////}.

/// An integer convertor
#symbol intConvertor =
{
//    /// Converts a literal string into an integer and saves the result into an output parameter
//    #method(stacksafe)convert &literal:aValue &vint:aRetVal
//        => system'core_routines'int_copystr.
//
//    /// Converts a literal string into an integer and saves the result into an output parameter
//    #method(stacksafe)convert &literal:value &int:base &vint:retVal
//        => system'core_routines'int_copystr2.
//
//    /// Converts a wide literal string into an integer and saves the result into an output parameter
//    #method(stacksafe)convert &wide:aValue &vint:aRetVal
//        => system'core_routines'int_copywstr.
//
//    /// Converts a wide literal string into an integer and saves the result into an output parameter
//    #method(stacksafe)convert &wide:value &int:base &vint:retVal
//        => system'core_routines'int_copywstr2.
//
//    /// Converts a character value into an integer and saves the result into an output parameter
//    #method(stacksafe)convert &char:aValue &vint:aRetVal
//        => system'core_routines'int_saveint.

    /// Converts a short integer value into an integer and saves the result into an output parameter
    #method(stacksafe)convert &short:aValue &vint:aRetVal
        => system'core_routines'__intconvertor.

    /// Converts a byte value into an integer and saves the result into an output parameter
    #method(stacksafe)convert &byte:aValue &vint:aRetVal
        => system'core_routines'__intconvertor.

    /// Converts a long integer into an integer and saves the result into an output parameter
    #method(stacksafe)convert &long:aValue &vint:aRetVal
        => system'core_routines'__intconvertor.
        
//    /// Converts a real number into an integer and saves the result into an output parameter
//    #method(stacksafe)convert &real:aReal &vint:aRetVal
//        => system'core_routines'int_copyreal.
//        
//    /// Converts a literal string into an integer
//    #method(type:int,stacksafe) convert &literal:aLiteral
//    [
//        #var(type:int)aRetVal.
//        $self convert &literal:aLiteral &vint:aRetVal.
//        
//        ^ IntNumber new &int:aRetVal.
//    ]
//        
//    /// Converts a literal string into an integer
//    #method(type:int,stacksafe) convert &literal:literal &base:base
//    [
//        #var(type:int)retVal.
//        $self convert &literal:literal &int:(base int) &vint:retVal.
//        
//        ^ IntNumber new &int:retVal.
//    ]
//        
//    /// Converts a wide literal string into an integer
//    #method(type:int,stacksafe) convert &wide:aLiteral
//    [
//        #var(type:int)aRetVal.
//        $self convert &wide:aLiteral &vint:aRetVal.
//        
//        ^ IntNumber new &int:aRetVal.
//    ]
//        
//    /// Converts a wide literal string into an integer
//    #method(type:int,stacksafe) convert &wide:literal &base:base
//    [
//        #var(type:int)retVal.
//        $self convert &wide:literal &int:(base int) &vint:retVal.
//        
//        ^ IntNumber new &int:retVal.
//    ]
//        
//    /// Converts a character value into an integer
//    #method(type:int,stacksafe) convert &char:aValue
//    [
//        #var(type:int)aRetVal.
//        $self convert &char:aValue &vint:aRetVal.
//        
//        ^ IntNumber new &int:aRetVal.
//    ]
        
    /// Converts a byte value into an integer
    #method(int,stacksafe) convert &byte:aValue
    [
        #var(int)aRetVal.
        $self convert &byte:aValue &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]
        
    /// Converts a short integer value into an integer
    #method(int,stacksafe) convert &short:aValue
    [
        #var(int)aRetVal.
        $self convert &short:aValue &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]
        
    /// Converts a long integer value into an integer and saves the result into an output parameter
    #method(int,stacksafe) convert &long:aValue
    [
        #var(int)aRetVal.
        $self convert &long:aValue &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

//    /// Converts a long integer into an integer
//    #method(type:int,stacksafe) convert &real:aReal
//    [
//        #var(type:int)aRetVal.
//        $self convert &real:aReal &vint:aRetVal.
//        
//        ^ IntNumber new &int:aRetVal.
//    ]

    /// Returns a parameter                                    
    #method(int,stacksafe) convert &int:anInt
        = IntNumber new &int:anInt.
        
    #method convert : anObject
        = anObject cast:%convert &to:$self.
}.

/// A long integer convertor
#symbol longConvertor =
{
//    /// Converts a literal string into a long integer and saves the result into an output parameter
//    #method(stacksafe)convert &literal:aValue &vlong:aRetVal
//        => system'core_routines'long_copystr.
//
//    /// Converts a literal string into a long integer and saves the result into an output parameter
//    #method(stacksafe)convert &literal:value &int:base &vlong:retVal
//        => system'core_routines'long_copystr2.
//
//    /// Converts a wide literal string into a long integer and saves the result into an output parameter
//    #method(stacksafe)convert &wide:aValue &vlong:aRetVal
//        => system'core_routines'long_copywstr.
//
//    /// Converts a wide literal string into a long integer and saves the result into an output parameter
//    #method(stacksafe)convert &wide:value &int:base &vlong:retVal
//        => system'core_routines'long_copywstr2.

    #method(stacksafe)convert &int:aValue &vlong:aRetVal
        => system'core_routines'__longconvertor.

    #method(stacksafe)convert &short:aValue &vlong:aRetVal
        => system'core_routines'__longconvertor.

    #method(stacksafe)convert &byte:aValue &vlong:aRetVal
        => system'core_routines'__longconvertor.

//    /// Converts a real number into a long integer and saves the result into an output parameter
//    #method(stacksafe)convert &real:aValue &vlong:aRetVal
//        => system'core_routines'long_copyreal.

    /// Converts an integer value into a long integer
    #method(long,stacksafe)convert &int:aValue
    [
        #var(long)n.
        $self convert &int:aValue &vlong:n.
        
        ^ LongNumber new &long:n.
    ]

    /// Converts an integer value into a long integer
    #method(long,stacksafe)convert &byte:aValue
    [
        #var(long)n.
        $self convert &byte:aValue &vlong:n.
        
        ^ LongNumber new &long:n.
    ]

    /// Converts an integer value into a long integer
    #method(long,stacksafe)convert &short:aValue
    [
        #var(long)n.
        $self convert &short:aValue &vlong:n.
        
        ^ LongNumber new &long:n.
    ]

//    /// Converts a real number into a long integer
//    #method(stacksafe)convert &real:aValue
//    [
//        #var(type:long)aRetVal.
//        $self convert &real:aValue &vlong:aRetVal.
//        
//        ^ LongNumber new &long:aRetVal.
//    ]
//
//    /// Converts a character into a long integer
//    #method(stacksafe)convert &char:aValue
//    [
//        #var(type:int)aRetVal.
//        intConvertor convert &char:aValue &vint:aRetVal.
//        
//        ^ LongNumber new &int:aRetVal.
//    ]
//
//    /// Converts a literal string into a long integer
//    #method(type:long,stacksafe) convert &literal:aLiteral
//    [
//        #var(type:long)aRetVal.
//        $self convert &literal:aLiteral &vlong:aRetVal.
//        
//        ^ LongNumber new &long:aRetVal.
//    ]
//
//    /// Converts a literal string into a long integer
//    #method(type:long,stacksafe) convert &literal:literal &base:base
//    [
//        #var(type:long)retVal.
//        $self convert &literal:literal &int:(base int) &vlong:retVal.
//        
//        ^ LongNumber new &long:retVal.
//    ]
//
//    /// Converts a wide literal string into a long integer
//    #method(type:long,stacksafe) convert &wide:aLiteral
//    [
//        #var(type:long)aRetVal.
//        $self convert &wide:aLiteral &vlong:aRetVal.
//        
//        ^ LongNumber new &long:aRetVal.
//    ]
//
//    /// Converts a wide literal string into a long integer
//    #method(type:long,stacksafe) convert &wide:literal &base:base
//    [
//        #var(type:long)retVal.
//        $self convert &wide:literal &int:(base int) &vlong:retVal.
//        
//        ^ LongNumber new &long:retVal.
//    ]
    
    /// Returns a parameter
    #method(long,stacksafe) convert &long:aLong
        = LongNumber new &long:aLong.
        
    #method convert : anObject
        = anObject cast:%convert &to:$self.
}.

/////// A real number convertor
////#symbol realConvertor =
////{
////    /// Converts an integer into a real number and saves the result into an output parameter
////    #method(stacksafe)convert &int:aValue &vreal:aRetVal
////        => system'core_routines'real_saveint.
////
////    /// Converts a literal string into a real number and saves the result into an output parameter
////    #method(stacksafe)convert &literal:aValue &vreal:aRetVal
////        => system'core_routines'real_copystr.
////        
////    /// Converts a wide literal string into a real number and saves the result into an output parameter
////    #method(stacksafe)convert &wide:aValue &vreal:aRetVal
////        => system'core_routines'real_copywstr.
////        
////    /// Converts a literal string into a real number
////    #method(type:real,stacksafe)convert &literal:aLiteral
////    [
////        #var(type:real)aRetVal.
////        $self convert &literal:aLiteral &vreal:aRetVal.
////        
////        ^ RealNumber new &real:aRetVal.
////    ]
////        
////    /// Converts a wide literal string into a real number
////    #method(type:real,stacksafe)convert &wide:aLiteral
////    [
////        #var(type:real)aRetVal.
////        $self convert &wide:aLiteral &vreal:aRetVal.
////        
////        ^ RealNumber new &real:aRetVal.
////    ]
////
////    /// Returns a parameter
////    #method(type:real,stacksafe)convert &real:aReal
////        = RealNumber new &real:aReal.
////        
////    /// Converts an integer into a real number
////    #method(type:real,stacksafe)convert &int:anInt
////        = RealNumber new &int:anInt.
////        
////    /// Converts a long integer into a real number
////    #method(type:real,stacksafe)convert &long:aLong
////        = RealNumber new &long:aLong.
////        
////    /// Converts a byte into a real number
////    #method(type:real,stacksafe)convert &byte:aValue
////        = RealNumber new &byte:aValue.
////        
////    /// Converts a short integer into a real number
////    #method(type:real,stacksafe)convert &short:aValue
////        = RealNumber new &short:aValue.
////        
////    #method convert : anObject
////        = anObject cast:%convert &to:$self.
////}.

// --- literalConvertor ---

/// A literal convertor
#symbol literalConvertor =
{
//    /// Converts the parameter to the literal value
//    #method(type:literal,stacksafe) convert &char:aValue
//        => system'core_routines's_copychar.
    
    /// Converts the parameter to the literal value
    #method(literal,stacksafe) convert &int:aNumber
        => system'core_routines'__literalconvertor.

    /// Converts the parameter to the literal value
    #method(literal,stacksafe) convert &short:aNumber
        => system'core_routines'__literalconvertor.

    /// Converts the parameter to the literal value
    #method(literal,stacksafe) convert &byte:aNumber
        => system'core_routines'__literalconvertor.

    /// Converts the parameter to the literal value
    #method(literal,stacksafe) convert &long:aNumber
        => system'core_routines'__literalconvertor.

    /// Converts the parameter to the literal value with the provided radix
    #method(literal,stacksafe) convert &int:aNumber &int:aRadix
        => system'core_routines'__literalconvertor.

    /// Converts the parameter to the literal value with the provided radix
    #method(literal,stacksafe) convert &short:aNumber &int:aRadix
        => system'core_routines'__literalconvertor.

    /// Converts the parameter to the literal value with the provided radix
    #method(literal,stacksafe) convert &byte:aNumber &int:aRadix
        => system'core_routines'__literalconvertor.

    /// Converts the parameter to the literal value with the provided radix
    #method(literal,stacksafe) convert &long:aNumber &int:aRadix
        => system'core_routines'__literalconvertor.
//        => system'core_routines's_copylong_radix.

    /// Converts the parameter to the literal value
    #method(literal,stacksafe) convert &real:aNumber
        => system'core_routines'__literalconvertor.

//    /// Converts UTF-16 literal to UTF-8 one
//    #method(type:literal,stacksafe) convert &wide:aLiteral
//        => system'core_routines'str_copywstr.

    /// Returns the literal presentation of the parameter
    #method(literal,stacksafe) convert : anObject
        = anObject literal.

    /// Returns a parameter                
    #method(literal,stacksafe) convert &literal:aLiteral
        = aLiteral.
}.

// --- wideConvertor ---

/// A wide literal convertor
#symbol wideConvertor =
{
//    /// Converts the parameter to the literal value
//    #method(type:wide,stacksafe) convert &char:aValue
//        => system'core_routines'ws_copychar.
    
    /// Converts the parameter to the literal value
    #method(wide,stacksafe) convert &int:aNumber
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value
    #method(wide,stacksafe) convert &short:aNumber
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value
    #method(wide,stacksafe) convert &byte:aNumber
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value
    #method(wide,stacksafe) convert &long:aNumber
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value with the provided radix
    #method(wide,stacksafe) convert &int:aNumber &int:aRadix
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value with the provided radix
    #method(wide,stacksafe) convert &short:aNumber &int:aRadix
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value with the provided radix
    #method(wide,stacksafe) convert &byte:aNumber &int:aRadix
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value with the provided radix
    #method(wide,stacksafe) convert &long:aNumber &int:aRadix
        => system'core_routines'__wideconvertor.

    /// Converts the parameter to the literal value
    #method(wide,stacksafe) convert &real:aNumber
        => system'core_routines'__wideconvertor.

    /// Converts UTF-8 literal to UTF-16e
    #method(wide,stacksafe) convert &literal:aLiteral
        => system'core_routines'__wideconvertor.
        
    /// Returns the literal presentation of the parameter
    #method(wide,stacksafe) convert : anObject
        = anObject wide.
        
    /// Returns a parameter
    #method(wide,stacksafe) convert &wide:aLiteral
        = aLiteral.
}.

////// --- realOpController ---
////
////#symbol realOpController =
////{
////    #method(stacksafe) exp &real:aReal &vreal:aRetVal
////        => system'core_routines'r_exp.
////        
////    #method(stacksafe) abs &real:aReal &vreal:aRetVal
////        => system'core_routines'r_abs.
////        
////    #method(stacksafe) ln &real:aReal &vreal:aRetVal
////        => system'core_routines'r_ln.
////        
////    #method(stacksafe) round &real:aReal &vreal:aRetVal
////        => system'core_routines'r_round.
////        
////    #method(stacksafe) truncate &real:aReal &vreal:aRetVal
////        => system'core_routines'r_truncate.
////        
////    #method(stacksafe) sin &real:aReal &vreal:aRetVal
////        => system'core_routines'r_sin.
////        
////    #method(stacksafe) cos &real:aReal &vreal:aRetVal
////        => system'core_routines'r_cos.
////        
////    #method(stacksafe) arctan &real:aReal &vreal:aRetVal
////        => system'core_routines'r_arctan.
////                
////    #method(stacksafe) sqrt &real:aReal &vreal:aRetVal
////        => system'core_routines'r_sqrt.
////        
////    #method(stacksafe) pi &vreal:aRetVal
////        => system'core_routines'r_pi.
////        
////    #method(type:real,embeddable,suppress:w3,stacksafe) pi
////    [
////        #var(type:real)aRetVal.
////        $self pi &vreal:aRetVal.
////        
////        ^ aRetVal.
////    ]        
////    
////    #method(type:real,suppress:w3,stacksafe) e
////    [
////        #var(type:real)aRetVal.
////        $self exp &real:1.0r &vreal:aRetVal.
////        
////        ^ aRetVal.
////    ]        
////}.
////
////#symbol(type:real)pi_value = realOpController pi.
////
////#symbol(type:real)e_value = realOpController e.
