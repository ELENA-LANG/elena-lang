// base variable mixin
public class BaseVariable
{
    // should be overridden
    value = nil.
}

sealed template<BaseVariable> ref :: template_subj
{
    template_subj theValue.
    
    cast T<template_subj>:val
    [
        theValue := val.
    ]
    
    cast T<template_subj> = theValue.
    
    value = theValue.
    
    set value(template_subj val)
    [
        theValue := val.
    ]
    
//    stacksafe append template_subj:val
//    [
//        $value := $value + val.
//    ]
//    
//    multi append:val
//        <= append template_subj:val.
//    
//    stacksafe reduce template_subj:val
//    [
//        $value := $value - val.
//    ]
//    
//    multi reduce:val
//        <= reduce template_subj:val.
    
    bool equal : anObject
        => theValue.

    T<literal> literal
        => theValue.

    embeddable dispatch => theValue.
}

//sealed template<BaseVariable> ref :: template_subj
//{
//    template_subj $value.
//    
//    stacksafe implicit template_subj:val
//    [
//        $value := val.
//    ]
//    
//    stacksafe set template_subj:val
//    [
//        $value := val.
//    ]
//    
//    value = $value.
//    
//    sealed embeddable template_subj = $value.
//    
//    bool equal : anObject
//        => $value.
//
//    embeddable dispatch => $value.
//}

// === Boolean values ===

// --- true ---

/// A true value symbol
public class true = BoolValue::
{
    /// Executes the action
    __ifoperator if : aTrueAction = aTrueAction().

    /// Executes then-action
    if:aTrueAction else:aFalseAction = aTrueAction().

    /// Always returns nil.
    __ifnotoperator ifnot : aTrueAction = nil.

    iif(object trueVal, object falseVal) = trueVal.
    
    /// Executes logical OR operation
    ///
    /// Always return true
    bool or : anExpr = self.

    /// Executes logical AND operation
    bool and : anExpr = anExpr.

    /// Executes logical XOR operation
    bool xor : aBool
    [
        T<bool>(aBool) ? [ ^ false ].
        
        ^ self
    ]

    /// Determines whether the specified object is equal to the current object boolean value.
    bool equal : anExpr = anExpr.

    /// Determines whether the specified object is equal to the current object boolean value.
    bool equal(bool aBool) = aBool.

    /// Always returns false.
    bool inverted = false.

    /// Returns literal representation.
    T<literal> literal = "true".

    /// Returns itself.
    clone = self.
}.

// --- false ---

/// A false value symbol
public class false = BoolValue::
{
    /// Always returns nil
    __ifnotoperator if : anAction = nil.
    
    /// Executes else-action
    if:aTrueAction else:aFalseAction = aFalseAction().

    /// Executes the action
    __ifoperator ifnot:aFalseAction = aFalseAction().

    iif(object aTrueValue, object aFalseValue) = aFalseValue.

    /// Executes logical OR operation
    bool or : anExpr = anExpr.

    /// Executes logical AND operation
    ///
    /// Always return false
    bool and : anExpr = self.

    /// Executes logical XOR operation
    bool xor : anExpr 
    [
        T<bool>(anExpr) ? [ ^ true ].
        
        ^ self
    ]

    /// Determines whether the specified object is equal to the current object boolean value
    bool equal : anExpr
    [
        T<bool>(anExpr) ? [ ^ self ].
        
        ^ true
    ]

    /// Determines whether the specified object is equal to the current object boolean value
    bool equal(bool anExpr)
    [
        ^ anExpr inverted
    ]

    /// Always returns true
    bool inverted = true.

    /// Returns literal representation
    T<literal> literal = "false".
    
    /// Returns itself
    clone = self.
}.

// === basic data classes ===

// --- BaseValue ---

// A base value
public class BaseValue
{
    bool equal(BaseVariable var)
        <= equal(var value).
    
    bool less(BaseVariable var)
        <= equal(var value).
    
    /// Compares the values
    bool equal : anObject
        = false.

    /// Compares the values
    bool less : anObject
    [
        system'NotSupportedException new; raise.
    ]

    /// Compares the values
    bool greater : anObject = anObject less:self.

    bool notless : anObject = self less:anObject; inverted.

    bool notgreater : anObject = self greater:anObject; inverted.
}

// --- BaseNumber ---

/// A base numeric value
public class BaseNumber :: BaseValue
{
    add(BaseVariable var)
        <= add(var value).

    subtract(BaseVariable var)
        <= subtract(var value).
    
    multiply(BaseVariable var)
        <= multiply(var value).

    divide(BaseVariable var)
        <= divide(var value).
    
    /// Adds the value
    add : anObject
    [
        system'NotSupportedException new; raise.
    ]

    /// Subtracts the value
    subtract : anObject
    [
        system'NotSupportedException new; raise.
    ]

    /// Multiply by the value
    multiply : anObject
    [
        system'NotSupportedException new; raise.
    ]

    /// Divides into the value
    divide : anObject
    [
        system'NotSupportedException new; raise.
    ]
    
    negative
        = 0 - self.
}

// --- IntBaseNumber ---

/// A base integer value
public class IntBaseNumber :: BaseNumber
{
    and(BaseVariable var)
        <= and(var value).
        
    or(BaseVariable var)
        <= or(var value).
        
    xor(BaseVariable var)
        <= xor(var value).
    
    /// Execute bitwise AND operation
    and : anObject
    [
        system'NotSupportedException new; raise.
    ]

    /// Execute bitwise OR operation
    or : anObject
    [
        system'NotSupportedException new; raise.
    ]
    
    /// Execute bitwise XOR operation
    xor : anObject
    [
        system'NotSupportedException new; raise.
    ]
    
    /// Returns true if all the mask bits are set
    bool allMask : anOperand
        = self and:anOperand == anOperand.
        
    /// Returns true if any of the mask bits are set    
    bool anyMask : anOperand
        = 0 != self and:anOperand.
}

// --- ByteNumber ---

/// An unsigned 8 bit integer
public sealed struct ByteNumber :: IntBaseNumber
{
    T<__int,1> theValue.

    cast T<int>:n
    [
        theValue := n && 255.
    ]

    /// Creates the object with a default value (0)
    constructor new
        = 0.

    /// Returns the minimal value (0)
    constructor min
        = 0.

    /// Returns the maxial value (255)
    constructor max
        = 255.
    
    /// Creates the object with a specified value
    constructor new(byte value)
        = value.

    /// Creates the object with a specified value
    constructor new : val
        <= new(T<byte>(val)).

    /// Returns the value as a short integer
    cast T<short> = ShortNumber new(self).

    /// Returns the value as an integer
    cast T<int> = IntNumber new(self).

    /// Returns the value as an integer
    cast T<uint> = UIntNumber new(self).

    /// Returns the value as a long integer
    cast T<long> = LongNumber new(self).

    /// Returns the value as a real number
    cast T<real> = RealNumber new(self).

    bool equal(BaseVariable v)
        <= equal(v value).

    bool equal(byte anOperand)
        = theValue == anOperand.

    bool equal(short anOperand)
        = ShortNumber new(self); equal(anOperand).
        
    /// Compares the value
    bool equal(int anOperand)
        = IntNumber new(self); less(anOperand).
        
    /// Compares the value
    bool equal(real anOperand)
        = RealNumber new(self); equal(anOperand).
        
    bool equal : o
    [
        var n := T<byte>(o) \ back:nil.
        (nil == n)
            ? [ ^ false ]
            ! [ ^ self equal(n) ]
    ]

    bool less : n
        <= less(T<byte>(n)).

    bool less(BaseVariable v)
        <= less(v value).

    bool less(byte anOperand)
        = theValue < anOperand.

    bool less(short anOperand)
        = ShortNumber new(self); less(anOperand).

    /// Compares the value
    bool less(int anOperand)
        = IntNumber new(self); less(anOperand).
        
    /// Compares the value
    bool equal(long anOperand)
        = LongNumber new(self); equal(anOperand).
        
    /// Compares the value
    bool less(long anOperand)
        = LongNumber new(self); less(anOperand).

    /// Compares the value
    bool less(real anOperand)
        = RealNumber new(self); less(anOperand).

    add:n
        <= add(T<byte>(n)).

    subtract:n
        <= subtract(T<byte>(n)).

    multiply:n
        <= multiply(T<byte>(n)).

    divide:n
        <= divide(T<byte>n).

    byte add(BaseVariable v)
        <= add(v value).

    byte subtract(BaseVariable v)
        <= subtract(v value).

    byte multiply(BaseVariable v)
        <= multiply(v value).

    byte divide(BaseVariable v)
        <= divide(v value).

    /// Returns the sum
    byte add(byte anOperand)
        = theValue + anOperand.

    /// Returns the sum
    byte subtract(byte anOperand)
        = theValue - anOperand.

    /// Returns the product
    byte multiply(byte anOperand)
        = theValue * anOperand.

    /// Returns the result of integer division
    byte divide(byte anOperand)
        = theValue / anOperand.

    /// Returns the sum
    short add(short anOperand)
        = ShortNumber new(self); add(anOperand).

    /// Returns the difference
    short subtract(short anOperand)
        = ShortNumber new(self); subtract(anOperand).

    /// Returns the product
    short multiply(short anOperand)
        = ShortNumber new(self); multiply(anOperand).

    /// Returns the result of integer division
    short divide(short anOperand)
        = ShortNumber new(self); divide(anOperand).

    /// Returns the sum
    int add(int anOperand)
        = IntNumber new(self); add(anOperand).

    /// Returns the difference
    int subtract(int anOperand)
        = IntNumber new(self); subtract(anOperand).

    /// Returns the product
    int multiply(int anOperand)
        = IntNumber new(self); multiply(anOperand).

    /// Returns the result of integer division
    int divide(int anOperand)
        = IntNumber new(self); divide(anOperand).

    /// Returns the sum
    long add(long anOperand)
        = LongNumber new(self); add(anOperand).

    /// Returns the difference
    long subtract(long anOperand)
        = LongNumber new(self); subtract(anOperand).

    /// Returns the product
    long multiply(long anOperand)
        = LongNumber new(self); multiply(anOperand).

    /// Returns the result of integer division
    long divide(long anOperand)
        = LongNumber new(self); divide(anOperand).

    /// Returns the sum
    real add(real anOperand)
        = RealNumber new(self); add(anOperand).

    /// Returns the difference
    real subtract(real anOperand)
        = RealNumber new(self); subtract(anOperand).

    /// Returns the product
    real multiply(real anOperand)
        = RealNumber new(self); multiply(anOperand).

    /// Returns the result of integer division
    real divide(real anOperand)
        = RealNumber new(self); divide(anOperand).

    and:n
        <= and(T<byte>(n)).
        
    or:n
        <= or(T<byte>(n)).
        
    xor:n
        <= xor(T<byte>(n)).
        
    byte and(BaseVariable v)
        <= and(v value).
        
    byte or(BaseVariable v)
        <= or(v value).
        
    byte xor(BaseVariable v)
        <= xor(v value).
        
    /// Returns the result of bitwise AND operation
    byte and(byte anOperand)
        = theValue && anOperand.

    /// Returns the result of bitwise OR operation
    byte or(byte anOperand)
        = theValue || anOperand.

    /// Returns the result of bitwise XOR operation
    byte xor(byte anOperand)
        = theValue ^^ anOperand.

    /// Returns the result of bitwise AND operation
    short and(short anOperand)
        = ShortNumber new(self); and(anOperand).

    /// Returns the result of bitwise OR operation
    short or(short anOperand)
        = ShortNumber new(self); or(anOperand).

    /// Returns the result of bitwise XOR operation
    short xor(short anOperand)
        = ShortNumber new(self); xor(anOperand).

    /// Returns the result of bitwise AND operation
    int and(int anOperand)
        = IntNumber new(self); and(anOperand).

    /// Returns the result of bitwise OR operation
    int or(int anOperand)
        = IntNumber new(self); or(anOperand).

    /// Returns the result of bitwise XOR operation
    int xor(int anOperand)
        = IntNumber new(self); xor(anOperand).

    /// Returns the result of bitwise AND operation
    long and(long anOperand)
        = LongNumber new(self); and(anOperand).

    /// Returns the result of bitwise OR operation
    long or(long anOperand)
        = LongNumber new(self); or(anOperand).

    /// Returns the result of bitwise XOR operation
    long xor(long anOperand)
        = LongNumber new(self); xor(anOperand).

    byte shiftLeft(int val)
    [
        ^ theValue << val
    ]

    byte shiftRight(int val)
    [
        ^ theValue >> val
    ]
    
    /// Bitwise inversion
    byte inverted
        => system'core_routines'__byte.
        
    /// Returns the literal presentation
    T<literal> literal = literalConvertor convert(self).
        
    /// Clones the value
    clone = ByteNumber new(self).

    // Reads the object hash code into the output parameter
    readHashCodeTo(ref<int> retVal)
        => system'core_routines'__int.
    
    embeddable int hash_code
    [
        int aCode := 0.
        self readHashCodeTo(&aCode).
        
        ^ aCode
    ]
}

// --- ShortNumber ---

/// A signed 16 bit integer
public sealed struct ShortNumber :: IntBaseNumber
{
    T<__int,2> theValue.

    cast T<int>:n
    [
        (n < 0)
            ? [ 
                theValue := n && 0FFFFh. 
                
                theValue := theValue || 0FFFF0000h
            ]
            ! [ theValue := n && 0FFFFh ]
    ]
    
    /// Returns the minimal value (-32768)
    constructor min
        = -32768.

    /// Returns the maxial value (32767)
    constructor max
        = 32767.

    /// Creates the object with specified value
    constructor new(byte value)
        = shortConvertor convert(value).
        
    /// Creates the object with specified value
    constructor new(short value)
        = value.

    /// Creates the object with a default value (0)
    constructor new
        = 0.

    /// Creates the object with specified value
    constructor new : anObject
        <= new(T<short>(anObject)).

    /// Returns the byte value        
    cast T<byte> = byteConvertor convert(self).
    
    /// Returns the integer value
    cast T<int> = IntNumber new(self).
    
    /// Returns the integer value
    cast T<uint> = UIntNumber new(self).
    
    /// Returns the long integer value
    cast T<long> = LongNumber new(self).
    
    /// Returns the value as a real number
    cast T<real> = RealNumber new(self).

    bool equal:o
    [
        var n := T<short>(o) \ back:nil.
        (nil == n)
            ? [ ^ false ]
            ! [ ^ self equal(n) ]
    ]

    bool less:n
        <= less(n).

    bool equal(BaseVariable v)
        <= equal(v value).
                
    bool less(BaseVariable v)
        <= equal(v value).

    /// Compares the value
    bool equal(short anOperand)
        = theValue == anOperand.
        
    /// Compares the value
    bool less(short anOperand)
        = theValue < anOperand.
        
    /// Compares the value
    bool equal(int anOperand)
        = IntNumber new(self); equal(anOperand).
        
    /// Compares the value
    bool less(int anOperand)
        = IntNumber new(self); less(anOperand).
    
    /// Compares the value
    bool equal(byte anOperand)
        = self equal(ShortNumber new(anOperand)).
        
    /// Compares the value
    bool less (byte anOperand)
        = self less(ShortNumber new(anOperand)).
    
    /// Compares the value
    bool equal(long anOperand)
        = LongNumber new(self); equal(anOperand).
        
    /// Compares the value
    bool less(long anOperand)
        = LongNumber new(self); less(anOperand).
    
    /// Compares the value
    bool equal(real anOperand )
        = RealNumber new(self); equal(anOperand).
        
    /// Compares the value
    bool less(real anOperand)
        = RealNumber new(self); less(anOperand).

    add:n
        <= add(T<short>(n)).

    subtract:n
        <= subtract(T<short>(n)).

    multiply:n
        <= multiply(T<short>(n)).

    divide:n
        <= divide(T<short>(n)).

    short add(BaseVariable v)
        <= add(v value).

    short subtract(BaseVariable v)
        <= subtract(v value).

    short multiply(BaseVariable v)
        <= multiply(v value).

    short divide(BaseVariable v)
        <= divide(v value).

    /// Returns the sum
    short add(short anOperand)
        = theValue + anOperand.

    /// Returns the sum
    short subtract(short anOperand)
        = theValue - anOperand.

    /// Returns the product
    short multiply(short anOperand)
        = theValue * anOperand.

    /// Returns the result of integer division
    short divide(short anOperand)
        = theValue / anOperand.

    /// Returns the sum
    add(byte anOperand)
        = self add(ShortNumber new(anOperand)).

    /// Returns the difference
    subtract(byte anOperand)
        = self subtract(ShortNumber new(anOperand)).

    /// Returns the product
    multiply(byte anOperand)
        = self multiply(ShortNumber new(anOperand)).

    /// Returns the result of integer division
    divide(byte anOperand)
        = self divide(ShortNumber new(anOperand)).

    /// Returns the sum
    add(int anOperand)
        = IntNumber new(self); add(anOperand).

    /// Returns the difference
    subtract(int anOperand)
        = IntNumber new(self); subtract(anOperand).

    /// Returns the product
    multiply(int anOperand)
        = IntNumber new(self); multiply(anOperand).
        
    /// Returns the result of integer division
    divide(int anOperand)
        = IntNumber new(self); divide(anOperand).

    /// Returns the sum
    add(long anOperand)
        = LongNumber new(self); add(anOperand).

    /// Returns the difference
    subtract(long anOperand)
        = LongNumber new(self); subtract(anOperand).

    /// Returns the product
    multiply(long anOperand)
        = LongNumber new(self); multiply(anOperand).
        
    /// Returns the result of integer division
    divide(long anOperand)
        = LongNumber new (self); divide(anOperand).

    /// Returns the sum
    add(real anOperand)
        = RealNumber new(self); add(anOperand).

    /// Returns the difference
    subtract(real anOperand)
        = RealNumber new(self); subtract(anOperand).

    /// Returns the product
    multipl(real anOperand)
        = RealNumber new(self); multiply(anOperand).
        
    /// Returns the result of integer division
    divide(real anOperand)
        = RealNumber new(self); divide(anOperand).

    and:n
        <= and(T<short>(n)).
        
    or:n
        <= divide(T<short>(n)).
        
    xor:n
        <= xor(T<short>(n)).
        
    short and(BaseVariable v)
        <= and(v value).
        
    short or(BaseVariable v)
        <= divide(v value).
        
    short xor(BaseVariable v)
        <= xor(v value).
        
    /// Returns the result of bitwise AND operation
    short and(short anOperand)
        = theValue && anOperand.

    /// Returns the result of bitwise OR operation
    short or(short anOperand)
        = theValue || anOperand.

    /// Returns the result of bitwise XOR operation
    short xor(short anOperand)
        = theValue ^^ anOperand.
        
    /// Returns the result of bitwise AND operation
    and(byte anOperand)
        = self and(ShortNumber new(anOperand)). 

    /// Returns the result of bitwise OR operation
    or(byte anOperand)
        = self or(ShortNumber new(anOperand)). 

    /// Returns the result of bitwise XOR operation
    xor(byte anOperand)
        = self xor(ShortNumber new(anOperand)). 
        
    /// Returns the result of bitwise AND operation
    and(int anOperand)
        = IntNumber new(self); and(anOperand).
        
    /// Returns the result of bitwise OR operation
    or(int anOperand)
        = IntNumber new(self); or(anOperand).
        
    /// Returns the result of bitwise XOR operation
    xor(int anOperand)
        = IntNumber new(self); xor(anOperand).
        
    /// Returns the result of bitwise AND operation
    and(long anOperand)
        = LongNumber new(self); and(anOperand).
        
    /// Returns the result of bitwise OR operation
    or(long anOperand)
        = LongNumber new(self); or(anOperand).
        
    /// Returns the result of bitwise XOR operation
    xor(long anOperand)
        = LongNumber new(self); xor(anOperand).

    short shiftLeft(int val)
    [
        ^ theValue << val
    ]

    short shiftRight(int val)
    [
        ^ theValue >> val
    ]
    
    /// Bitwise inversion
    short inverted
        => system'core_routines'__short.  

    /// Returns the literal presentation
    T<literal> literal = literalConvertor convert(self).

//    /// Returns the literal presentation
//    wide = system'wideConvertor convert short:$self.

    /// Clones the value
    clone = ShortNumber new(self).

    // Reads the object hash code into the output parameter
    readHashCodeTo(ref<int> aCode)
        => system'core_routines'__int.
    
    embeddable int hash_code
    [
        int aCode := 0.
        self readHashCodeTo(&aCode).
        
        ^ aCode.
    ]        
}

// --- IntNumber ---

/// A signed 32 bit integer
public sealed struct IntNumber :: IntBaseNumber
{
    type<__int, 4> theValue.

    cast T<byte>:n
    [
        theValue := n.
    ]
    
    cast T<short>:n
    [
        theValue := n.
    ]
    
    /// Creates the object with the minimal value
    constructor min
        = -2147483648.

    /// Creates the object with the maximal value
    constructor max
        = 2147483647.
    
    /// Creates the object with specified value
    constructor new(int value)
        = value.

    /// Creates the object with specified value
    constructor new(short value)
        = intConvertor convert(value).

    /// Creates the object with specified value
    constructor new(byte value)
        = intConvertor convert(value).

    /// Creates the object with a default value (0)
    constructor new
        <= new(0).

    /// Creates the object with specified value
    constructor new : anObject
        <= new(T<int>(anObject)).

    /// Returns the value as a short integer    
    cast T<short> = shortConvertor convert(self).

    /// Returns the byte value    
    cast T<byte> = byteConvertor convert(self).

    /// Returns the long integer
    cast T<long> = LongNumber new(self).

    /// Returns the real number
    cast T<real> = RealNumber new(self).

    /// Returns the real number
    cast T<uint> = UIntNumber new(self).
    
    bool equal:o
    [
        var n := o int \ back:nil.
        (nil == n)
            ? [ ^ false ]
            ! [ ^ self equal(n) ]
    ]

    bool less:n
        <= less(T<int>(n)).
        
    bool equal(BaseVariable v)
        <= equal(v value).
        
    bool less(BaseVariable v)
        <= less(v value).
        
    /// Compares the value
    bool equal(int anOperand)
        = theValue == anOperand.
        
    /// Compares the value
    bool less(int anOperand)
        = theValue < anOperand.
        
    add:n
        <= add(T<int>(n)).

    subtract:n
        <= subtract(T<int>(n)).

    multiply:n
        <= multiply(T<int>(n)).

    divide:n
        <= divide(T<int>(n)).
        
    int add(BaseVariable v)
        <= add(v value).

    int subtract(BaseVariable v)
        <= subtract(v value).

    int multiply(BaseVariable v)
        <= multiply(v value).

    int divide(BaseVariable v)
        <= divide(v value).
        
    /// Returns the sum
    int add(int anOperand)
        = theValue + anOperand.

    /// Returns the sum
    int subtract(int anOperand)
        = theValue - anOperand.

    /// Returns the product
    int multiply(int anOperand)
        = theValue * anOperand.

    /// Returns the result of integer division
    int divide(int anOperand)
        = theValue / anOperand.

    and:n
        <= and(T<int>(n)).
        
    or:n
        <= divide(T<int>(n)).
        
    xor:n
        <= xor(T<int>(n)).
        
    int and(BaseVariable v)
        <= and(v value).
        
    int or(BaseVariable v)
        <= or(v value).
        
    int xor(BaseVariable v)
        <= xor(v value).
        
    /// Returns the result of bitwise AND operation
    int and(int anOperand)
        = theValue && anOperand.

    /// Returns the result of bitwise OR operation
    int or(int anOperand)
        = theValue || anOperand.

    /// Returns the result of bitwise XOR operation
    int xor(int anOperand)
        = theValue ^^ anOperand.    

    /// Compares the integer value
    bool equal(short value)
        = self equal(IntNumber new(value)).

    /// Compares the integer value
    bool equal(byte value)
        = self equal(IntNumber new(value)).
        
    /// Compares the integer value
    bool equal(long anOperand)
        = LongNumber new(self); equal(anOperand).

    /// Compares the integer value
    bool equal(real anOperand)
        = RealNumber new(self); equal(anOperand).

    /// Compares the integer value
    bool less(short value)
        = self equal(IntNumber new(value)).

    /// Compares the integer value
    bool less(byte value)
        = self less(IntNumber new(value)).
        
    /// Compares the integer value
    bool less(long anOperand)
        = LongNumber new(self); less(anOperand).

    /// Compares the integer value
    bool less(real anOperand)
        = RealNumber new(self); less(anOperand).

    /// Returns the sum of integers
    add(short value)
        = self add(IntNumber new (value)).

    /// Returns the sum of integers
    add(byte value)
        = self add(IntNumber new(value)).

    /// Returns the sum of integers
    long add(long anOperand)
        = LongNumber new(self); add(anOperand).

    /// Returns the sum of integers
    real add(real anOperand)
        = RealNumber new(self); add(anOperand).

    /// Returns the difference of integers
    subtract(short value)
        = self subtract(IntNumber new(value)).

    /// Returns the difference of integers
    subtract(byte value)
        = self subtract(IntNumber new(value)).

    /// Returns the difference of integers
    long subtract(long anOperand)
        = LongNumber new(self); subtract(anOperand).

    /// Returns the difference of integers
    real subtract(real anOperand)
        = RealNumber new(self); subtract(anOperand).

    /// Returns the product of integers
    multiply(short value)
        = self multiply(IntNumber new(value)).        

    /// Returns the product of integers
    multiply(byte value)
        = self multiply(IntNumber new(value)).

    /// Returns the product of integers
    long multiply(long anOperand)
        = LongNumber new(self); multiply(anOperand).

    /// Returns the product of integers
    real multiply(real anOperand)
        = RealNumber new(self); multiply(anOperand).

    /// Returns the result of integer division
    divide(short value)
        = self divide(IntNumber new(value)).

    /// Returns the result of integer division
    divide(byte value)
        = self divide(IntNumber new(value)).        

    /// Returns the result of integer division
    long divide(long anOperand)
        = LongNumber new(self); divide(anOperand).

    /// Returns the result of integer division
    real divide(real anOperand)
        = RealNumber new(self); divide(anOperand).

    /// Returns the result of bitwise AND operation
    and(short anOperand)
        = self and(IntNumber new(anOperand)).

    /// Returns the result of bitwise AND operation
    and(byte anOperand)
        = self and(IntNumber new(anOperand)).

    /// Returns the result of bitwise AND operation
    and(long anOperand)
        = LongNumber new(self); and(anOperand).

    /// Returns the result of bitwise OR operation
    or(short anOperand)
        = self or(IntNumber new(anOperand)).

    /// Returns the result of bitwise OR operation
    or(byte anOperand)
        = self or(IntNumber new(anOperand)).

    /// Returns the result of bitwise OR operation
    or(long anOperand)
        = LongNumber new(self); or(anOperand).

    /// Returns the result of bitwise XOR operation
    xor(short anOperand)
        = self xor(IntNumber new(anOperand)).

    /// Returns the result of bitwise XOR operation
    xor(byte anOperand)
        = self xor(IntNumber new(anOperand)).

    /// Returns the result of bitwise XOR operation
    xor(long anOperand)
        = LongNumber new(self); xor(anOperand).

    /// Returns the literal presentation
    T<literal> literal = literalConvertor convert(self).

//    /// Returns the literal presentation
//    wide = system'wideConvertor convert int:$self.

    /// Clones the value
    clone = IntNumber new(self).

    // Reads the object hash code into the output parameter
    readHashCodeTo(ref<int> aCode)
        => system'core_routines'__int.
    
    embeddable int hash_code
    [
        int aCode := 0.
        self readHashCodeTo(&aCode).
        
        ^ aCode.
    ]        

    int shiftLeft(int val)
    [
        ^ theValue << val
    ]

    int shiftRight(int val)
    [
        ^ theValue >> val
    ]
    
    /// Bitwise inversion
    int inverted
        => system'core_routines'__int.  
        
    /// Negates the value
    negative
        = 0 - theValue.
}

// --- UIntNumber ---

/// A unsigned 32 bit integer

public sealed struct UIntNumber :: IntBaseNumber
{
    T<__int,4> theValue.

    cast T<byte>:n
    [
        theValue := n.
    ]
    
    cast T<short>:n
    [
        theValue := n.
    ]
    
    cast T<int>:n
    [
        theValue := n.
    ]
    
//    implicit u literal:s
//    [
//        theValue := intConvertor convertUnsigned literal:s.
//    ]
    
    /// Creates the object with the minimal value
    constructor min
        = 0.

    /// Creates the object with the maximal value
    constructor max
        = 0FFFFFFFFh.
        
    constructor new(int n)
    [
        theValue := n.
    ]        
        
    constructor new(byte n)
    [
        theValue := n.
    ]        
        
    constructor new(short n)
    [
        theValue := n.
    ]        
        
    constructor new(uint n)
    [
        theValue := n.
    ]        
        
    constructor new
    [
        theValue := 0.
    ]        

    /// Returns the literal presentation
    T<literal> literal = literalConvertor convert(self).

//    /// Returns the literal presentation
//    wide = system'wideConvertor convert uint:$self.
        
    /// Returns the value as a short integer    
    cast T<short> = shortConvertor convert(self).

    /// Returns the byte value    
    cast T<byte> = byteConvertor convert(self).

    cast T<int> = self.

    /// Returns the long integer
    cast T<long> = LongNumber new(self).

    /// Returns the real number
    cast T<real> = RealNumber new(self).

    bool equal:o
    [
        var n := T<int>(o) \ back:nil.
        (nil == n)
            ? [ ^ false ]
            ! [ ^ self equal(n) ]
    ]

    bool less:n
        <= less(T<int>(n)).
        
    bool equal(BaseVariable v)
        <= equal(v value).
        
    bool less(BaseVariable v)
        <= less(v value).
        
    /// Compares the value
    bool equal(int anOperand)
        = theValue == anOperand.
        
    /// Compares the value
    bool less(int anOperand)
        = theValue < anOperand.
        
    add:n
        <= add(T<int>(n)).

    subtract:n
        <= subtract(T<int>(n)).

    multiply:n
        <= multiply(T<int>(n)).

    divide:n
        <= divide(T<int>(n)).
        
    int add(BaseVariable v)
        <= add(v value).

    int subtract(BaseVariable v)
        <= subtract(v value).

    int multiply(BaseVariable v)
        <= multiply(v value).

    int divide(BaseVariable v)
        <= divide(v value).
        
    /// Returns the sum
    int add(int anOperand)
        = theValue + anOperand.

    /// Returns the sum
    int subtract(int anOperand)
        = theValue - anOperand.

    /// Returns the product
    int multiply(int anOperand)
        = theValue * anOperand.

    /// Returns the result of integer division
    int divide(int anOperand)
        = theValue / anOperand.

    and:n
        <= and(T<int>(n)).
        
    or:n
        <= divide(T<int>(n)).
        
    xor:n
        <= xor(T<int>(n)).
        
    int and(BaseVariable v)
        <= and(v value).
        
    int or(BaseVariable v)
        <= or(v value).
        
    int xor(BaseVariable v)
        <= xor(v value).
        
    /// Returns the result of bitwise AND operation
    int and(int anOperand)
        = theValue && anOperand.

    /// Returns the result of bitwise OR operation
    int or(int anOperand)
        = theValue || anOperand.

    /// Returns the result of bitwise XOR operation
    int xor(int anOperand)
        = theValue ^^ anOperand.

    /// Compares the value
    bool equal(uint anOperand)
        = theValue == anOperand.
        
    /// Compares the value
    bool less(uint anOperand)
        = theValue < anOperand.
        
    /// Returns the sum
    uint add(uint anOperand)
        = theValue + anOperand.

    /// Returns the sum
    uint subtract(uint anOperand)
        = theValue - anOperand.

    /// Returns the product
    uint multiply(uint anOperand)
        = theValue * anOperand.

    /// Returns the result of integer division
    uint divide(uint anOperand)
        = theValue / anOperand.

    /// Returns the result of bitwise AND operation
    uint and(uint anOperand)
        = theValue && anOperand.

    /// Returns the result of bitwise OR operation
    uint or(uint anOperand)
        = theValue || anOperand.

    /// Returns the result of bitwise XOR operation
    uint xor(uint anOperand)
        = theValue ^^ anOperand.

    /// Compares the integer value
    bool equal(short value)
        = self equal(T<int>(value)).

    /// Compares the integer value
    bool equal(byte value)
        = self equal(T<int>(value)).
        
    /// Compares the integer value
    bool equal(long anOperand )
        = LongNumber new(self); equal(anOperand).

    /// Compares the integer value
    bool equal(real anOperand)
        = RealNumber new(self); equal(anOperand).

    /// Compares the integer value
    bool less(short value)
        = self equal(T<uint>(value)).

    /// Compares the integer value
    bool less(byte value)
        = self less(T<uint>(value)).
        
    /// Compares the integer value
    bool less(long anOperand)
        = LongNumber new(self); less(anOperand).

    /// Compares the integer value
    bool less(real anOperand)
        = RealNumber new(self); less(anOperand).

    /// Returns the sum of integers
    add(short value)
        = self add(T<uint>(value)).

    /// Returns the sum of integers
    add(byte value)
        = self add(T<uint>(value)).

    /// Returns the sum of integers
    add(long anOperand)
        = LongNumber new(self); add(anOperand).

    /// Returns the sum of integers
    add(real anOperand)
        = RealNumber new(self); add(anOperand).

    /// Returns the difference of integers
    subtract(short value)
        = self subtract(T<uint>(value)).

    /// Returns the difference of integers
    subtract(byte value)
        = self subtract(T<uint>(value)).

    /// Returns the difference of integers
    subtract(long anOperand)
        = LongNumber new(self); subtract(anOperand).

    /// Returns the difference of integers
    subtract(real anOperand)
        = RealNumber new(self); subtract(anOperand).

    /// Returns the product of integers
    multiply(short value)
        = self multiply(T<uint>(value)).

    /// Returns the product of integers
    multiply(byte value)
        = self multiply(T<uint>(value)).

    /// Returns the product of integers
    multiply(long anOperand)
        = LongNumber new(self); multiply(anOperand).

    /// Returns the product of integers
    multiply(real anOperand)
        = RealNumber new(self); multiply(anOperand).

    /// Returns the result of integer division
    divide(short value)
        = self divide(T<uint>(value)).

    /// Returns the result of integer division
    divide(byte value)
        = self divide(T<uint>(value)).

    /// Returns the result of integer division
    divide(long anOperand)
        = LongNumber new(self); divide(anOperand).

    /// Returns the result of integer division
    divide(real anOperand)
        = RealNumber new(self); divide(anOperand).

    /// Returns the result of bitwise AND operation
    and(short value)
        = self and(T<uint>(value)).

    /// Returns the result of bitwise AND operation
    and(byte value)
        = self and(T<uint>(value)).

    /// Returns the result of bitwise AND operation
    and(long value)
        = LongNumber new(self); and(value).

    /// Returns the result of bitwise OR operation
    or(short value)
        = self or(T<uint>(value)).

    /// Returns the result of bitwise OR operation
    or(byte value)
        = self or(T<uint>(value)).

    /// Returns the result of bitwise OR operation
    or(long value)
        = LongNumber new(self); or(value).

    /// Returns the result of bitwise XOR operation
    xor(short value)
        = self xor(T<uint>(value)).

    /// Returns the result of bitwise XOR operation
    xor(byte value)
        = self xor(T<uint>(value)).

    /// Returns the result of bitwise XOR operation
    xor(long anOperand)
        = LongNumber new(self); xor(anOperand).

    /// Clones the value
    clone = UIntNumber new(self).

    // Reads the object hash code into the output parameter
    readHashCodeTo(ref<int> aCode)
        => system'core_routines'__int.
    
    embeddable int hash_code
    [
        int aCode := 0.
        self readHashCodeTo(&aCode).
        
        ^ aCode.
    ]        

    uint shiftLeft(int val)
    [
        ^ theValue << val
    ]

    uint shiftRight(int val)
    [
        ^ theValue >> val
    ]
    
    /// Bitwise inversion
    uint inverted
        => system'core_routines'__int.  
}

// --- LongNumber ---

/// A signed 64 bit integer value
public sealed struct LongNumber :: IntBaseNumber
{
    T<__int,8> theValue.
    
    /// Returns the minimal value
    constructor min
        = -9223372036854775808l.

    /// Returns the maximal value
    constructor max
        = 9223372036854775807l.

    /// Creates the object with a default value (0)
    constructor new
        <= new:0l.

    /// Creates the object with specified value
    constructor new(long value)
        = value.

    /// Creates the object with specified value
    constructor new(int value)
        = longConvertor convert(value).

    /// Creates the object with specified value
    constructor new(uint value)
        = longConvertor convert(value).

    /// Creates the object with specified value
    constructor new(short value)
        = longConvertor convert(value).

    /// Creates the object with specified value
    constructor new(byte value)
        = longConvertor convert(value).

    /// Creates the object with specified value
    constructor new : anObject
        <= new(T<long>(anObject)).

    /// Returns an integer value
    cast T<int> = intConvertor convert(self).

    /// Returns an integer value
    cast T<uint> = uintConvertor convert(self).

    /// Returns the value as a short integer    
    cast T<short> = shortConvertor convert(self).
    
    /// Returns the byte value
    cast T<byte> = byteConvertor convert(self).

    /// Returns a real value
    cast T<real> = RealNumber new(self).
    
    bool equal:o
    [
        var n := o long \ back:nil.
        (nil == n)
            ? [ ^ false ]
            ! [ ^ self equal(n) ]
    ]

    bool less:n
        <= less(T<long>(n)).
        
    bool equal(BaseVariable v)
        <= equal(v value).
        
    bool less(BaseVariable v)
        <= less(v value).
        
    /// Compares the value
    bool equal(long anOperand)
        = theValue == anOperand.
        
    /// Compares the value
    bool less(long anOperand)
        = theValue < anOperand.
        
    add:n
        <= add(T<long>(n)).

    subtract:n
        <= subtract(T<long>(n)).

    multiply:n
        <= multiply(T<long>(n)).

    divide:n
        <= divide(T<long>(n)).
        
    long add(BaseVariable v)
        <= add(v value).

    long subtract(BaseVariable v)
        <= subtract(v value).

    long multiply(BaseVariable v)
        <= multiply(v value).

    long divide(BaseVariable v)
        <= divide(v value).
        
    /// Returns the sum
    long add(long anOperand)
        = theValue + anOperand.

    /// Returns the sum
    long subtract(long anOperand)
        = theValue - anOperand.

    /// Returns the product
    long multiply(long anOperand)
        = theValue * anOperand.

    /// Returns the result of integer division
    long divide(long anOperand)
        = theValue / anOperand.

    and:n
        <= and(T<long>(n)).
        
    or:n
        <= divide(T<long>(n)).
        
    xor:n
        <= xor(T<long>(n)).
        
    long and(BaseVariable v)
        <= and(v value).
        
    long or(BaseVariable v)
        <= divide(v value).
        
    long xor(BaseVariable v)
        <= xor(v value).
        
    /// Returns the result of bitwise AND operation
    long and(long anOperand)
        = theValue && anOperand.

    /// Returns the result of bitwise OR operation
    long or(long anOperand)
        = theValue || anOperand.

    /// Returns the result of bitwise XOR operation
    long xor(long anOperand)
        = theValue ^^ anOperand.    

    /// Compares the integer value
    bool equal(int anOperand) 
        = self equal(LongNumber new(anOperand)).

    /// Compares the integer value
    bool equal(short anOperand)
        = self equal(LongNumber new(anOperand)).

    /// Compares the integer value
    bool equal(byte anOperand)
        = self equal(LongNumber new(anOperand)).

    /// Compares the integer value
    bool equal(real anOperand)
        = RealNumber new(self); equal(anOperand).

    /// Compares the integer value
    bool less(int anOperand)
        = self less(LongNumber new(anOperand)).

    /// Compares the integer value
    bool less(short anOperand)
        = self less(LongNumber new(anOperand)).

    /// Compares the integer value
    bool less(byte anOperand)
        = self less(LongNumber new(anOperand)).

    /// Compares the integer value
    bool less(real anOperand)
        = RealNumber new(self); less(anOperand).

    /// Returns the sum of integers
    add(int anOperand)
        = self add(LongNumber new(anOperand)).

    /// Returns the sum of integers
    add(short anOperand)
        = self add(LongNumber new(anOperand)).

    /// Returns the sum of integers
    add(byte anOperand)
        = self add(LongNumber new(anOperand)).

    /// Returns the sum of integers
    add(real anOperand)
        = RealNumber new(self); add(anOperand).

    /// Returns the difference of integers
    subtract(int anOperand)
        = self subtract(LongNumber new(anOperand)).

    /// Returns the difference of integers
    subtract(short anOperand)
        = self subtract(LongNumber new(anOperand)).

    /// Returns the difference of integers
    subtract(byte anOperand)
        = self subtract(LongNumber new(anOperand)).

    /// Returns the difference of integers
    subtract(real anOperand)
        = RealNumber new(self); subtract(anOperand).

    /// Returns the product of integers
    multiply(real anOperand)
        = RealNumber new(self); multiply(anOperand).

    /// Returns the product of integers
    multiply(int anOperand)
        = self multiply(LongNumber new(anOperand)).

    /// Returns the product of integers
    multiply(byte anOperand)
        = self multiply(LongNumber new(anOperand)).

    /// Returns the product of integers
    multiply(short anOperand)
        = self multiply(LongNumber new(anOperand)).

    /// Returns the result of integer division
    divide(int anOperand)
        = self divide(LongNumber new(anOperand)).

    /// Returns the result of integer division
    divide(short anOperand)
        = self divide(LongNumber new(anOperand)).

    /// Returns the result of integer division
    divide(byte anOperand)
        = self divide(LongNumber new(anOperand)).

    /// Returns the result of integer division
    divide(real anOperand)
        = RealNumber new(self); divide(anOperand).
        
    /// Returns the result of bitwise AND operation
    and(int anOperand)
        = self and(LongNumber new(anOperand)).

    /// Returns the result of bitwise AND operation
    and(short anOperand)
        = self and(LongNumber new(anOperand)).

    /// Returns the result of bitwise AND operation
    and(byte anOperand)
        = self and(LongNumber new(anOperand)).

    /// Returns the result of bitwise OR operation
    or(int anOperand)
        = self or(LongNumber new(anOperand)).

    /// Returns the result of bitwise OR operation
    or(short anOperand)
        = self or(LongNumber new(anOperand)).

    /// Returns the result of bitwise OR operation
    or(byte anOperand)
        = self or(LongNumber new(anOperand)).  

    /// Returns the result of bitwise XOR operation
    xor(int anOperand)
        = self xor(LongNumber new(anOperand)).

    /// Returns the result of bitwise XOR operation
    xor(short anOperand)
        = self xor(LongNumber new(anOperand)).

    /// Returns the result of bitwise XOR operation
    xor(byte anOperand)
        = self xor(LongNumber new(anOperand)).

    /// Returns the literal presentation
    T<literal> literal = literalConvertor convert(self).

//    /// Returns the literal presentation
//    wide = system'wideConvertor convert long:$self.

    /// Clones the value
    clone = LongNumber new(self).
    
    // Reads the object hash code into the output parameter
    readHashCodeTo(ref<int> aCode)
        => system'core_routines'__int.
    
    embeddable int hash_code
    [
        int aCode := 0.
        self readHashCodeTo(&aCode).
        
        ^ aCode.
    ]

    long shiftLeft(int val)
    [
        ^ theValue << val
    ]

    long shiftRight(int val)
    [
        ^ theValue >> val
    ]
    
    /// Bitwise inversion
    long inverted
        => system'core_routines'__long.  
        
    /// Negates the value
    negative
        = 0l - theValue.
}

// --- realNumber ---

/// A 64bit float numeric value
public sealed struct RealNumber :: BaseNumber
{
    T<__real,8> theValue.

    cast T<int>:n
    [
        realConvertor convert(n,&self).
    ]
            
    /// Returns the minimal value
    constructor min
        = -1.7976931348623157e+308r.
    
    /// Returns the maximal value
    constructor max
        = 1.7976931348623157e+308r.
        
    /// Returns the negative infinity
    embeddable constructor negativeInfinity 
        = -1.0r / 0.0r. 
        
    /// Returns the positive infinity 
    embeddable constructor positiveInfinity 
        = 1.0r / 0.0r.
        
    /// returns the NaN
    embeddable constructor NaN
        = 0.0r / 0.0r.

    /// Creates the object with a default value (0)
    constructor new
        <= new(0.0r).

    /// Creates the object with specified value
    constructor new(real value)
        = value.
  
    /// Creates the object with specified value
    constructor new(int value)
        = realConvertor convert(value).
  
    /// Creates the object with specified value
    constructor new(uint value)
        = realConvertor convert(value).

    /// Creates the object with specified value
    constructor new(short value)
        = realConvertor convert(value).

    /// Creates the object with specified value
    constructor new(byte value)
        = realConvertor convert(value).

    /// Creates the object with specified value
    constructor new(long value)
        = realConvertor convert(value).
        
    /// Creates the object with specified value
    constructor new : anObject
        <= new(T<real>(anObject)).
        
    bool equal:o
    [
        var n := T<real>(o) \ back:nil.
        (nil == n)
            ? [ ^ false ]
            ! [ ^ self equal(n) ]
    ]

    bool less:n
        <= less(T<real>(n)).
        
    bool equal(BaseVariable v)
        <= equal(v value).
        
    bool less(BaseVariable v)
        <= less(v value).
        
    /// Compares the value
    bool equal(real anOperand)
        = theValue == anOperand.
        
    /// Compares the value
    bool less(real anOperand)
        = theValue < anOperand.
        
    add:n
        <= add(T<real>(n)).

    subtract:n
        <= subtract(T<real>(n)).

    multiply:n
        <= multiply(T<real>(n)).

    divide:n
        <= divide(T<real>(n)).
        
    real add(BaseVariable v)
        <= add(v value).

    real subtract(BaseVariable v)
        <= subtract(v value).

    real multiply(BaseVariable v)
        <= multiply(v value).

    real divide(BaseVariable v)
        <= divide(v value).
        
    /// Returns the sum
    real add(real anOperand)
        = theValue + anOperand.

    /// Returns the sum
    real subtract(real anOperand)
        = theValue - anOperand.

    /// Returns the product
    real multiply(real anOperand)
        = theValue * anOperand.

    /// Returns the result of integer division
    real divide(real anOperand)
        = theValue / anOperand.
        
    /// Compares the numeric values
    bool equal(int anOperand)
        = self equal(RealNumber new(anOperand)).

    /// Compares the numeric values
    bool equal(byte anOperand)
        = self equal(RealNumber new(anOperand)).

    /// Compares the numeric values
    bool equal(short anOperand)
        = self equal(RealNumber new(anOperand)).

    /// Compares the numeric values
    bool equal(long anOperand)
        = self equal(RealNumber new(anOperand)).

    /// Compares the numeric values
    bool less(int anOperand)
        = self less(RealNumber new(anOperand)).

    /// Compares the numeric values
    bool less(byte anOperand)
        = self less(RealNumber new(anOperand)).

    /// Compares the numeric values
    bool less(short anOperand)
        = self less(RealNumber new(anOperand)).

    /// Compares the numeric values
    bool less(long anOperand)
        = self less(RealNumber new(anOperand)).

    /// Returns the sum of numbers
    real add(int anOperand)
        = self add(RealNumber new(anOperand)).

    /// Returns the sum of numbers
    real add(byte anOperand)
        = self add(RealNumber new(anOperand)).

    /// Returns the sum of numbers
    real add(short anOperand)
        = self add(RealNumber new(anOperand)).

    /// Returns the sum of numbers
    real add(long anOperand)
        = self add(RealNumber new(anOperand)).

    /// Returns the difference of numbers
    real subtract(int anOperand)
        = self subtract(RealNumber new(anOperand)).

    /// Returns the difference of numbers
    real subtract(byte anOperand)
        = self subtract(RealNumber new(anOperand)).

    /// Returns the difference of numbers
    real subtract(short anOperand)
        = self subtract(RealNumber new(anOperand)).

    /// Returns the difference of numbers
    real subtract(long anOperand)
        = self subtract(RealNumber new(anOperand)).

    /// Returns the product of numbers
    real multiply(int anOperand)
    [
        real n := anOperand.
        
        ^ self multiply(n).
    ]

    /// Returns the product of numbers
    real multiply(byte anOperand)
        = self multiply(RealNumber new(anOperand)).

    /// Returns the product of numbers
    real multiply(short anOperand)
        = self multiply(RealNumber new(anOperand)).

    /// Returns the product of numbers
    real multiply(long anOperand)
        = self multiply(RealNumber new(anOperand)).

    /// Returns the result of the division
    real divide(int anOperand)
    [
        real n := anOperand.
        
        ^ self divide(n)
    ]

    /// Returns the result of the division
    real divide(byte anOperand)
        = self divide(RealNumber new(anOperand)).
        
    /// Returns the result of the division
    real divide(short anOperand)
        = self divide(RealNumber new(anOperand)).

    /// Returns the result of the division
    real divide(long anOperand)
        = self divide(RealNumber new(anOperand)).

    /// Returns the literal presentation
    T<literal> literal = literalConvertor convert(self).

//    /// Returns the literal presentation
//    wide = system'wideConvertor convert real:$self.

    /// Clones the value    
    clone = RealNumber new(self).
    
    // Reads the object hash code into the output parameter
    readHashCodeTo(ref<int> aCode)
        => system'core_routines'__int.
    
    embeddable int hash_code
    [
        int aCode := 0.
        self readHashCodeTo(&aCode).
        
        ^ aCode
    ]
        
    /// Negates the value
    negative
        = 0.0r - theValue.
}

// --- CharValue ---

/// An UTF-32 character symbol
public sealed const struct CharValue :: BaseValue
{
    int theValue.

    /// Creates the object with a default value (0)
    constructor new
        <= new($0).

    /// Creates the object with specified value
    constructor new(char value)
        = value.

    /// Creates the object with specified numeric value
    constructor new(int anOperand)
        <= new(charConvertor convert(anOperand)).

    /// Returns the minimal possible character value (0)
    constructor min
        = $0.

    /// Returns the maximal possible character value (010FFFFh)
    constructor max
        = $010FFFFh.

    bool equal:o
    [
        var n := T<char>(o) \ back:nil.
        (nil == n)
            ? [ ^ false ]
            ! [ ^ self equal(n) ]
    ]

    bool less:n
        <= less(T<char>(n)).
        
    bool equal(BaseVariable v)
        <= equal(v value).
        
    bool less(BaseVariable v)
        <= less(v value).
        
    bool equal(int val)
        = theValue == val.

//    bool equal(literal val)
//        = val == T<literal>(self).

//    bool equal(wide val)
//        = val == T<wide>(self).
//
//    bool less(literal val)
//        = val < (T<literal>(self)).
//
//    bool less(wide val)
//        = val < (T<wide>(self)).

    /// Clones the value
    clone = CharValue new(self).

    /// Returns the literal value
    T<literal> literal = literalConvertor convert(self).

    /// Returns the literal value
    cast T<literal> = literalConvertor convert(self).
    
    /// Returns the wide literal value
    cast T<wide> = wideConvertor convert(self).
    
//    /// Reads the number of bytes to represent the symbol
//    stacksafe readLengthTo vint:aLength
//        => system'core_routines'__char.
//
//    /// Reads the number of bytes to represent the symbol
//    stacksafe readWideLengthTo vint:aRetVal
//    [
//        int aLength := 0.
//        $self readLengthTo vint:aLength.
//        
//        aLength := aLength + 1.
//        aLength := aLength / 2.
//
//        aRetVal int := aLength
//    ]
//        
//    /// Retuns the number of bytes to represent the symbol        
//    stacksafe embeddable int length
//    [
//        int aLength := 0.
//        $self readLengthTo vint:aLength.
//        
//        ^ aLength
//    ]
//    
//    /// Retuns the number of short integers to represent the symbol
//    stacksafe embeddable int wideLength
//    [
//        int aLength := 0.
//        $self readWideLengthTo vint:aLength.
//        
//        ^ aLength.
//    ]
//
//    /// Compares values
//    stacksafe bool equal char:anOperand 
//        = theValue == anOperand.
//
//    /// Compares values
//    stacksafe bool less char:anOperand 
//        = theValue < anOperand.
//        
//    // Reads the object hash code into the output parameter
//    stacksafe readHashCodeTo vint:aCode 
//        => system'core_routines'__int.
//    
//    embeddable int hash_code
//    [
//        int aCode := 0.
//        $self readHashCodeTo vint:aCode.
//        
//        ^ aCode.
//    ]
}

//// --- nil constant ---
//
///// A nil value
//singleton nil
//{
//    /// Returns true if the specified object is nil.
//    bool equal : anObject 
//    [
//        ($nil == anObject)
//            ? [ ^ true. ].
//        
//        ($super equal:anObject)
//            ? [ ^ true. ].
//        
//        ^ false.
//    ]
//    
//    /// Returns nil
//    ///
//    /// Any typecasting message will be handled and nil value returned
//    generic = $nil.
//}
//
//// === Basic Factory symbols ==
//
//class BaseExtender
//{
//    object theObject.
//    
//    /// Returns true if the specified object is equal to the current object; otherwise, false.
//    bool equal : anObject
//        => theObject.
//      
//    dispatch => theObject.
//}
//
///// A generic variable. 
/////
///// Extends an assigned value
//sealed class Variable :: BaseVariable, var<object>
//{
//    /// Creates the empty variable
//    constructor new
//    [
//    ]        
//        
//    /// Creates the variable
//    constructor new : value
//    [
//        $value := value.
//    ]
//    
////    set : value
////    [
////        $value := value.
////    ]
//    
//    append : val
//    [
//        ($nil == $value)
//            ? [ $value := val ]
//            ! [ $value := $value + val ]
//    ]
//    
//    reduce : val
//    [
//        ($nil == $value)
//            ? [ system'InvalidOperationException new; raise ]
//            ! [ $value := $value - val ]
//    ]
//    
//    multiplyBy : val
//    [
//        ($nil == $value)
//            ? [ system'InvalidOperationException new; raise ]
//            ! [ $value := $value * val ]
//    ]
//    
//    /// Divides into the value
//    divideInto : val
//    [
//        ($nil == $value)
//            ? [ system'InvalidOperationException new; raise ]
//            ! [ $value := $value / val ]
//    ]    
//}
//
//// --- KeyValue ---
//
///// A key - object pair
/////
///// Extends the object with a key property
//sealed class KeyValue
//{
//    object theKey.
//    object theObject.
//    
//    /// Creates a new objext
//    constructor new key:aKey value:anObject
//    [
//        theKey := aKey.
//        theObject := anObject.
//    ]
//    
//    /// Returns the key
//    key = theKey.
//    
//    /// Returns the key
//    value = theObject.
//
//    set value:anObject
//    [
//        theObject := anObject.
//    ]
//           
//    bool equal : o
//        => theObject.
//                                                                                      
//    /// Redirects the incoming messages to the object
//    dispatch => theObject.
//}

// === Basic Convertors ===

// --- convertor helpers ---

/// A byte convertor
public class byteConvertor =
{
//    /// Converts a character into a byte value and saves the result into an output parameter
//    stacksafe convert char:aValue vbyte:aRetVal
//        => system'core_routines'__byteconvertor.
//        
//    /// Converts a short integer into a byte value and saves the result into an output parameter
//    stacksafe byte convert short:aValue vbyte:aRetVal
//        => system'core_routines'__byteconvertor.
//        
//    /// Converts an integer into a byte value and saves the result into an output parameter
//    stacksafe byte convert int:aValue vbyte:aRetVal
//        => system'core_routines'__byteconvertor.
//
//    /// Converts an integer into a byte value and saves the result into an output parameter
//    stacksafe byte convert uint:aValue vbyte:aRetVal
//        => system'core_routines'__byteconvertor.
//
//    /// Converts a long integer into a byte value and saves the result into an output parameter
//    stacksafe convert long:aValue vbyte:aRetVal
//        => system'core_routines'__byteconvertor.
//        
//    /// Converts a real number into a byte value and saves the result into an output parameter
//    stacksafe convert real:aValue vbyte:aRetVal
//        => system'core_routines'__byteconvertor.
//        
//    /// Converts a literal value into a byte value and saves the result into an output parameter
//    stacksafe convert literal:aValue vbyte:aRetVal
//        => system'core_routines'__byteconvertor.
//
//    /// Converts a wide literal value into a byte value and saves the result into an output parameter
//    stacksafe convert wide:aValue vbyte:aRetVal
//        => system'core_routines'__byteconvertor.
//        
//    byte convert(BaseVariable v)
//        <= convert(v value).
//                        
//    /// Returns the parameter        
//    stacksafe byte convert byte:anInt
//        = ByteNumber new byte:anInt.
//        
//    /// Converts a character into a byte value
//    stacksafe byte convert char:aValue
//    [
//        byte aRetVal := 0.
//        $self convert char:aValue vbyte:aRetVal.
//        
//        ^ ByteNumber new byte:aRetVal
//    ]
//
//    /// Converts a short integer into a byte value
//    stacksafe byte convert short:aValue
//    [
//        byte aRetVal := 0.
//        $self convert short:aValue vbyte:aRetVal.
//        
//        ^ ByteNumber new byte:aRetVal
//    ]
//        
//    /// Converts an integer into a byte value
//    stacksafe byte convert int:aValue
//    [
//        byte aRetVal := 0.
//        $self convert int:aValue vbyte:aRetVal.
//        
//        ^ ByteNumber new byte:aRetVal
//    ]
//        
//    /// Converts an integer into a byte value
//    stacksafe byte convert uint:aValue
//    [
//        byte aRetVal := 0.
//        $self convert uint:aValue vbyte:aRetVal.
//        
//        ^ ByteNumber new byte:aRetVal
//    ]
//
//    /// Converts a long integer into a byte value
//    stacksafe byte convert long:aValue
//    [
//        byte aRetVal := 0.
//        $self convert long:aValue vbyte:aRetVal.
//        
//        ^ ByteNumber new byte:aRetVal
//    ]
//
//    /// Converts a real number into a byte value
//    stacksafe byte convert real:aValue
//    [
//        byte aRetVal := 0.
//        $self convert real:aValue vbyte:aRetVal.
//        
//        ^ ByteNumber new byte:aRetVal
//    ]
//
//    /// Converts a literal value into a byte value
//    stacksafe byte convert literal:aLiteral
//    [
//        byte aValue := 0.
//        $self convert literal:aLiteral vbyte:aValue.
//        
//        ^ ByteNumber new byte:aValue
//    ]
//                
//    /// Converts a wide literal value into a byte value
//    stacksafe byte convert wide:aLiteral
//    [
//        byte aValue := 0. 
//        $self convert wide:aLiteral vbyte:aValue.
//        
//        ^ ByteNumber new byte:aValue
//    ]

    byte convert : anObject
        = T<byte>(anObject).
}.

/// A 16bit integer convertor
public class shortConvertor =
{
//    /// Converts a character into a short integer value and saves the result into an output parameter
//    stacksafe convert char:aValue vshort:aRetVal
//        => system'core_routines'__shortconvertor.
//        
//    /// Converts a byte value into a short integer value and saves the result into an output parameter
//    stacksafe convert byte:aValue vshort:aRetVal
//        => system'core_routines'__shortconvertor.
//        
//    /// Converts an integer into a short integer value and saves the result into an output parameter
//    stacksafe convert int:aValue vshort:aRetVal
//        => system'core_routines'__shortconvertor.
//        
//    /// Converts an integer into a short integer value and saves the result into an output parameter
//    stacksafe convert uint:aValue vshort:aRetVal
//        => system'core_routines'__shortconvertor.
//        
//    /// Converts a long integer into a short integer value and saves the result into an output parameter
//    stacksafe convert long:aValue vshort:aRetVal
//        => system'core_routines'__shortconvertor.
//        
//    /// Converts a real number into a short integer value and saves the result into an output parameter
//    stacksafe convert real:aValue vshort:aRetVal
//        => system'core_routines'__shortconvertor.
//
//    /// Converts a literal string into a short integer value and saves the result into an output parameter
//    stacksafe convert literal:aValue vshort:aRetVal
//        => system'core_routines'__shortconvertor.
//        
//    /// Converts a wide literal string into a short integer value and saves the result into an output parameter
//    stacksafe convert wide:aValue vshort:aRetVal
//        => system'core_routines'__shortconvertor.
//        
//    short convert(BaseVariable v)
//        <= convert(v value).
//                        
//    /// Returns a short integer value        
//    stacksafe short convert short:anInt
//        = ShortNumber new short:anInt.
//        
//    /// Converts a byte value into a short integer value
//    stacksafe short convert byte:aValue
//    [
//        short aRetVal := 0.
//        $self convert byte:aValue vshort:aRetVal.
//        
//        ^ ShortNumber new short:aRetVal
//    ]
//
//    /// Converts an integer into a short integer value
//    stacksafe short convert int:aValue
//    [
//        short aRetVal := 0.
//        $self convert int:aValue vshort:aRetVal.
//        
//        ^ ShortNumber new short:aRetVal
//    ]
//
//    /// Converts an integer into a short integer value
//    stacksafe short convert uint:aValue
//    [
//        short aRetVal := 0.
//        $self convert uint:aValue vshort:aRetVal.
//        
//        ^ ShortNumber new short:aRetVal
//    ]
//
//    /// Converts a long integer into a short integer value
//    stacksafe short convert long:aValue
//    [
//        short aRetVal := 0.
//        $self convert long:aValue vshort:aRetVal.
//        
//        ^ ShortNumber new short:aRetVal
//    ]
//        
//    /// Converts a real number into a short integer value
//    stacksafe short convert real:aValue
//    [
//        short aRetVal := 0.
//        $self convert real:aValue vshort:aRetVal.
//        
//        ^ ShortNumber new short:aRetVal
//    ]
//        
//    /// Converts a character into a short integer value
//    stacksafe short convert char:aValue
//    [
//        short aRetVal := 0.
//        $self convert char:aValue vshort:aRetVal.
//        
//        ^ ShortNumber new short:aRetVal
//    ]
//        
//    /// Converts a literal string into a short integer value
//    stacksafe short convert literal:aValue
//    [
//        short aRetVal := 0.
//        $self convert literal:aValue vshort:aRetVal.
//        
//        ^ ShortNumber new short:aRetVal
//    ]
//        
//    /// Converts a wide literal string into a short integer value
//    stacksafe short convert wide:aValue
//    [
//        short aRetVal := 0.
//        $self convert wide:aValue vshort:aRetVal.
//        
//        ^ ShortNumber new short:aRetVal
//    ]
        
    short convert : anObject
        = T<short>(anObject).
}.

/// An Unicode character convertor
public class charConvertor =
{
//    /// Converts a byte value into a character and saves the result into an output parameter
//    stacksafe convert byte:aValue vchar:aRetVal
//        => system'core_routines'__charconvertor.
//
//    /// Converts a short integer into a character and saves the result into an output parameter
//    stacksafe convert short:aValue vchar:aRetVal
//        => system'core_routines'__charconvertor.
//
//    /// Converts an integer into a character and saves the result into an output parameter
//    stacksafe convert int:aValue vchar:aRetVal
//        => system'core_routines'__charconvertor.
//        
//    char convert(BaseVariable v)
//        <= convert(v value).
//        
//    /// Returns a parameter        
//    stacksafe char convert char:aChar
//        = CharValue new char:aChar.
//  
//    stacksafe $convert int:anIndex literal:aLiteral vchar:char
//        => system'core_routines'__charconvertor.
//        
//    stacksafe $convert int:anIndex bytearray:aLiteral vchar:char
//        => system'core_routines'__charconvertor.
//  
//    stacksafe $convert int:anIndex wide:aLiteral vchar:char
//        => system'core_routines'__charconvertor.
//
//    stacksafe $convert int:anIndex shortarray:aLiteral vchar:char
//        => system'core_routines'__charconvertor.
//
//    stacksafe $save char:aChar int:anIndex shortarray:aCharArray vint:aLength
//        => system'core_routines'__charconvertor.
//    
//    stacksafe $save char:aChar int:anIndex bytearray:aCharArray vint:aLength
//        => system'core_routines'__charconvertor.
//
//    /// Converts a literal string into a character
//    stacksafe char convert literal:aValue
//    [
//        char aRetVal := $0.
//        $self $convert int:0 literal:aValue vchar:aRetVal.
//        
//        int l1 := 0.
//        int l2 := aValue length.
//        aRetVal readLengthTo vint:l1.
//        
//        (l1 != l2)
//            ? [ system'InvalidArgumentException new; raise ].
//        
//        ^ CharValue new char:aRetVal
//    ]        
//        
//    /// Converts a wide literal string into a character                
//    stacksafe char convert wide:aValue
//    [
//        char aRetVal := $0.
//        $self $convert int:0 wide:aValue vchar:aRetVal.
//
//        int l1 := 0.
//        int l2 := aValue length.
//        aRetVal readWideLengthTo vint:l1.
//                
//        (l1 != l2)
//            ? [ system'InvalidArgumentException new; raise ].
//        
//        ^ CharValue new char:aRetVal
//    ]        
//        
//    /// Converts a byte value into a character
//    stacksafe char convert byte:aValue
//    [
//        char aRetVal := $0.
//        $self convert byte:aValue vchar:aRetVal.
//        
//        ^ CharValue new char:aRetVal
//    ]        
//        
//    /// Converts a short integer into a character
//    stacksafe char convert short:aValue
//    [
//        char aRetVal := $0.
//        $self convert short:aValue vchar:aRetVal.
//        
//        ^ CharValue new char:aRetVal
//    ]        
//        
//    /// Converts an integer into a character
//    stacksafe char convert int:aValue
//    [
//        char aRetVal := $0.
//        $self convert int:aValue vchar:aRetVal.
//        
//        ^ CharValue new char:aRetVal
//    ]
        
    char convert : anObject
        = T<char>(anObject).
}.

/// An integer convertor
public class intConvertor =
{
//    /// Converts a literal string into an integer and saves the result into an output parameter
//    stacksafe convert literal:aValue vint:aRetVal
//        => system'core_routines'__intconvertor.
//
//    /// Converts a literal string into an integer and saves the result into an output parameter
//    stacksafe convertUnsigned literal:aValue vint:aRetVal
//        => system'core_routines'__intconvertor.
//
//    /// Converts a literal string into an integer and saves the result into an output parameter
//    stacksafe convert literal:value int:base vint:retVal
//        => system'core_routines'__intconvertor.
//
//    /// Converts a wide literal string into an integer and saves the result into an output parameter
//    stacksafe convert wide:aValue vint:aRetVal
//        => system'core_routines'__intconvertor.
//
//    /// Converts a wide literal string into an integer and saves the result into an output parameter
//    stacksafe convert wide:value int:base vint:retVal
//        => system'core_routines'__intconvertor.
//
//    /// Converts a character value into an integer and saves the result into an output parameter
//    stacksafe convert char:aValue vint:aRetVal
//        => system'core_routines'__intconvertor.
//
//    /// Converts a short integer value into an integer and saves the result into an output parameter
//    stacksafe convert short:aValue vint:aRetVal
//        => system'core_routines'__intconvertor.
//
//    /// Converts a byte value into an integer and saves the result into an output parameter
//    stacksafe convert byte:aValue vint:aRetVal
//        => system'core_routines'__intconvertor.
//
//    /// Converts a long integer into an integer and saves the result into an output parameter
//    stacksafe convert long:aValue vint:aRetVal
//        => system'core_routines'__intconvertor.
//        
//    /// Converts a real number into an integer and saves the result into an output parameter
//    stacksafe convert real:aReal vint:aRetVal
//        => system'core_routines'__intconvertor.
//
//    int convert(BaseVariable v)
//        <= convert(v value).
//        
//    /// Converts a literal string into an integer
//    stacksafe int convert literal:aLiteral
//    [
//        int aRetVal := 0.
//        $self convert literal:aLiteral vint:aRetVal.
//        
//        ^ IntNumber new int:aRetVal
//    ]
//        
//    /// Converts a literal string into an integer
//    stacksafe int convertUnsigned literal:aLiteral
//    [
//        int aRetVal := 0.
//        $self convertUnsigned literal:aLiteral vint:aRetVal.
//        
//        ^ IntNumber new int:aRetVal
//    ]
//        
//    /// Converts a literal string into an integer
//    stacksafe int convert literal:literal int:base
//    [
//        int retVal := 0.
//        $self convert literal:literal int:base vint:retVal.
//        
//        ^ IntNumber new int:retVal
//    ]
//        
//    /// Converts a wide literal string into an integer
//    stacksafe int convert wide:aLiteral
//    [
//        int aRetVal := 0.
//        $self convert wide:aLiteral vint:aRetVal.
//        
//        ^ IntNumber new int:aRetVal
//    ]
//
//    /// Converts a wide literal string into an integer
//    stacksafe int convert wide:literal int:base
//    [
//        int retVal := 0.
//        $self convert wide:literal int:base vint:retVal.
//        
//        ^ IntNumber new int:retVal
//    ]
//        
//    /// Converts a character value into an integer
//    stacksafe int convert char:aValue
//    [
//        int aRetVal := 0.
//        $self convert char:aValue vint:aRetVal.
//        
//        ^ IntNumber new int:aRetVal
//    ]
//        
//    /// Converts a byte value into an integer
//    stacksafe int convert byte:aValue
//    [
//        int aRetVal := 0.
//        $self convert byte:aValue vint:aRetVal.
//        
//        ^ IntNumber new int:aRetVal
//    ]
//        
//    /// Converts a short integer value into an integer
//    stacksafe int convert short:aValue
//    [
//        int aRetVal := 0.
//        $self convert short:aValue vint:aRetVal.
//        
//        ^ IntNumber new int:aRetVal
//    ]
//        
//    /// Converts a long integer value into an integer and saves the result into an output parameter
//    stacksafe int convert long:aValue
//    [
//        int aRetVal := 0.
//        $self convert long:aValue vint:aRetVal.
//        
//        ^ IntNumber new int:aRetVal
//    ]
//
//    /// Converts a long integer into an integer
//    stacksafe int convert real:aReal
//    [
//        int aRetVal := 0.
//        $self convert real:aReal vint:aRetVal.
//        
//        ^ IntNumber new int:aRetVal
//    ]
//
//    /// Returns a parameter                                    
//    stacksafe int convert int:anInt
//        = IntNumber new int:anInt.
//
//    stacksafe int convert uint:anInt
//        = IntNumber new int:anInt.        
                        
    int convert : anObject
        = T<int>(anObject).
}.

/// A unsigned integer convertor
public class uintConvertor =
{
    uint convert : object
        = T<uint>(object).
    
//    uint convert int:val
//        = UIntNumber new int:val.
//        
//    /// Converts a long integer into an integer and saves the result into an output parameter
//    stacksafe convert long:aValue vint:aRetVal
//        => system'core_routines'__uintconvertor.
//        
//    /// Converts a long integer value into an integer and saves the result into an output parameter
//    stacksafe uint convert long:aValue
//    [
//        int aRetVal := 0.
//        $self convert long:aValue vint:aRetVal.
//        
//        ^ UIntNumber new int:aRetVal
//    ]        
}.

/// A long integer convertor
public class longConvertor =
{
//    /// Converts a literal string into a long integer and saves the result into an output parameter
//    stacksafe convert literal:aValue vlong:aRetVal
//        => system'core_routines'__longconvertor.
//
//    /// Converts a literal string into a long integer and saves the result into an output parameter
//    stacksafe convert literal:value int:base vlong:retVal
//        => system'core_routines'__longconvertor.
//
//    /// Converts a wide literal string into a long integer and saves the result into an output parameter
//    stacksafe convert wide:aValue vlong:aRetVal
//        => system'core_routines'__longconvertor.
//
//    /// Converts a wide literal string into a long integer and saves the result into an output parameter
//    stacksafe convert wide:value int:base vlong:retVal
//        => system'core_routines'__longconvertor.
//
//    stacksafe convert int:aValue vlong:aRetVal
//        => system'core_routines'__longconvertor.
//        
//    stacksafe convert uint:aValue vlong:aRetVal
//        => system'core_routines'__longconvertor.
//        
//    stacksafe convert short:aValue vlong:aRetVal
//        => system'core_routines'__longconvertor.
//
//    stacksafe convert byte:aValue vlong:aRetVal
//        => system'core_routines'__longconvertor.
//
//    /// Converts a real number into a long integer and saves the result into an output parameter
//    stacksafe convert real:aValue vlong:aRetVal
//        => system'core_routines'__longconvertor.
//
//    long convert(BaseVariable v)
//        <= convert(v value).
//        
//    /// Converts an integer value into a long integer
//    stacksafe long convert int:aValue
//    [
//        long n := 0l.
//        $self convert int:aValue vlong:n.
//        
//        ^ LongNumber new long:n
//    ]
//
//    /// Converts an integer value into a long integer
//    stacksafe long convert uint:aValue
//    [
//        long n := 0l.
//        $self convert uint:aValue vlong:n.
//        
//        ^ LongNumber new long:n
//    ]
//
//    /// Converts an integer value into a long integer
//    stacksafe long convert byte:aValue
//    [
//        long n := 0l.
//        $self convert byte:aValue vlong:n.
//        
//        ^ LongNumber new long:n
//    ]
//
//    /// Converts an integer value into a long integer
//    stacksafe long convert short:aValue
//    [
//        long n := 0l.
//        $self convert short:aValue vlong:n.
//        
//        ^ LongNumber new long:n
//    ]
//
//    /// Converts a real number into a long integer
//    stacksafe long convert real:aValue
//    [
//        long aRetVal := 0l.
//        $self convert real:aValue vlong:aRetVal.
//        
//        ^ LongNumber new long:aRetVal
//    ]
//
//    /// Converts a character into a long integer
//    stacksafe long convert char:aValue
//    [
//        int aRetVal := 0.
//        intConvertor convert char:aValue vint:aRetVal.
//        
//        ^ LongNumber new int:aRetVal
//    ]
//
//    /// Converts a literal string into a long integer
//    stacksafe long convert literal:aLiteral
//    [
//        long aRetVal := 0l.
//        $self convert literal:aLiteral vlong:aRetVal.
//        
//        ^ LongNumber new long:aRetVal
//    ]
//
////    /// Converts a literal string into a long integer
////    stacksafe long convert literal:literal base:base
////    [
////        long retVal := 0l.
////        $self convert literal:literal int:base vlong:retVal.
////        
////        ^ LongNumber new long:retVal
////    ]
//
//    /// Converts a wide literal string into a long integer
//    stacksafe long convert wide:aLiteral
//    [
//        long aRetVal := 0l.
//        $self convert wide:aLiteral vlong:aRetVal.
//        
//        ^ LongNumber new long:aRetVal
//    ]
//
////    /// Converts a wide literal string into a long integer
////    stacksafe long convert wide:literal base:base
////    [
////        long retVal := 0l.
////        $self convert wide:literal int:base vlong:retVal.
////        
////        ^ LongNumber new long:retVal
////    ]
//    
//    /// Returns a parameter
//    stacksafe long convert long:aLong
//        = LongNumber new long:aLong.
        
    long convert : anObject
        = T<long>(anObject).
}.

/// A real number convertor
class realConvertor =
{
//    /// Converts an integer into a real number and saves the result into an output parameter
//    stacksafe convert int:aValue vreal:aRetVal
//        => system'core_routines'__realconvertor.
//
//    /// Converts an integer into a real number and saves the result into an output parameter
//    stacksafe convert uint:aValue vreal:aRetVal
//    [
//        (aValue < 0)?
//        [
//            // If the number is bigger than 2^31
//            // than we are unlucky
//            int odd := aValue && 1.            
//            int halfed := aValue >> 1.
//            
//            real r := 0.
//            $self convert int:halfed vreal:r.
//            r := r * 2.0r.
//            r := r + odd.
//            
//            aRetVal real := r.
//        ]
//        ! [ $self convert int:aValue vreal:aRetVal. ]
//    ]    
//    
//    //    => system'core_routines'__realconvertor.
//
//    /// Converts an integer into a real number and saves the result into an output parameter
//    stacksafe convert long:aValue vreal:aRetVal
//        => system'core_routines'__realconvertor.
//
//    /// Converts an integer into a real number and saves the result into an output parameter
//    stacksafe convert byte:aValue vreal:aRetVal
//        => system'core_routines'__realconvertor.
//
//    /// Converts an integer into a real number and saves the result into an output parameter
//    stacksafe convert short:aValue vreal:aRetVal
//        => system'core_routines'__realconvertor.
//
//    /// Converts a literal string into a real number and saves the result into an output parameter
//    stacksafe convert literal:aValue vreal:aRetVal
//        => system'core_routines'__realconvertor.
//
//    /// Converts a wide literal string into a real number and saves the result into an output parameter
//    stacksafe convert wide:aValue vreal:aRetVal
//        => system'core_routines'__realconvertor.
//        
//    real convert(BaseVariable v)
//        <= convert(v value).
//        
//    /// Converts a literal string into a real number
//    stacksafe real convert literal:aLiteral
//    [
//        real aRetVal := 0.0r.
//        $self convert literal:aLiteral vreal:aRetVal.
//        
//        ^ RealNumber new real:aRetVal
//    ]
//        
//    /// Converts a wide literal string into a real number
//    stacksafe real convert wide:aLiteral
//    [
//        real aRetVal := 0.0r.
//        $self convert wide:aLiteral vreal:aRetVal.
//        
//        ^ RealNumber new real:aRetVal
//    ]
//
//    /// Returns a parameter
//    stacksafe real convert real:aReal
//        = RealNumber new real:aReal.
//        
//    /// Converts an integer into a real number
//    stacksafe real convert int:anInt
//    [
//        real aRetVal := 0.0r.
//        $self convert int:anInt vreal:aRetVal.
//        
//        ^ RealNumber new real:aRetVal
//    ]
//        
//    /// Converts an integer into a real number
//    stacksafe real convert uint:anInt
//    [
//        real aRetVal := 0.0r.
//        $self convert uint:anInt vreal:aRetVal.
//        
//        ^ RealNumber new real:aRetVal
//    ]
//
//    /// Converts a long integer into a real number
//    stacksafe real convert long:aLong
//    [
//        real aRetVal := 0.0r.
//        $self convert long:aLong vreal:aRetVal.
//        
//        ^ RealNumber new real:aRetVal
//    ]
//
//    /// Converts a byte into a real number
//    stacksafe real convert byte:aValue
//    [
//        real aRetVal := 0.0r.
//        $self convert byte:aValue vreal:aRetVal.
//        
//        ^ RealNumber new real:aRetVal
//    ]
//
//    /// Converts a short integer into a real number
//    stacksafe real convert short:aValue
//    [
//        real aRetVal := 0.0r.
//        $self convert short:aValue vreal:aRetVal.
//        
//        ^ RealNumber new real:aRetVal
//    ]
        
    real convert : o
        = T<real>(o).
}.

//// --- realOpController ---
//
//class realOpController =
//{
//    stacksafe exp real:aReal vreal:aRetVal
//        => system'core_routines'__real.
//        
//    stacksafe abs real:aReal vreal:aRetVal
//        => system'core_routines'__real.
//        
//    stacksafe ln real:aReal vreal:aRetVal
//        => system'core_routines'__real.
//        
//    stacksafe round real:aReal vreal:aRetVal
//        => system'core_routines'__real.
//        
//    stacksafe truncate real:aReal vreal:aRetVal
//        => system'core_routines'__real.
//        
//    stacksafe sin real:aReal vreal:aRetVal
//        => system'core_routines'__real.
//        
//    stacksafe cos real:aReal vreal:aRetVal
//        => system'core_routines'__real.
//        
//    stacksafe arctan real:aReal vreal:aRetVal
//        => system'core_routines'__real.
//                
//    stacksafe sqrt real:aReal vreal:aRetVal
//        => system'core_routines'__real.
//        
//    stacksafe pi vreal:aRetVal
//        => system'core_routines'__real.
//        
//    stacksafe embeddable real pi
//    [
//        real aRetVal := 0.0r.
//        $self pi vreal:aRetVal.
//        
//        ^ aRetVal
//    ]        
//    
//    stacksafe embeddable real e
//    [
//        real aRetVal := 0.0r.
//        $self exp real:1.0r vreal:aRetVal.
//        
//        ^ aRetVal
//    ]        
//}.
//
//real pi_value = realOpController pi.
//
//real e_value = realOpController e.
//
//// --- obsolete variables ---
//
//class Integer =
//{
//    vint new
//        = 0.
//        
//    vint new int:n
//        = n.
//        
//    vint new:n
//        = n int.
//}.
//
//class LongInteger =
//{
//    vlong new
//        = 0l.
//        
//    vlong new long:n
//        = n.
//        
//    vlong new:n
//        = n long.
//}.
//
//class Real =
//{
//    vreal new
//        = 0.0r.
//        
//    vreal new : n
//        = n real.
//        
//    vreal new int:n
//        = n real.
//        
//    vreal new real:n
//        = n.
//}.