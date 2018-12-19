//// base variable mixin
//public abstract class BaseVariable
//{
//    // should be overridden
//    abstract value []
//}

public sealed ref<T>// : BaseVariable
{
    embeddable T theValue;
    
    constructor(T val)
    {
        theValue := val
    }
    
    T cast() = theValue;
    
    T Value = theValue;
    
    set Value(T val)
    {
        theValue := val
    }
    
    readValueTo(ref T retVal)
    {
        retVal := theValue
    }

//    bool equal : anObject
//        => theValue.
//
//    T<literal> literal
//        => theValue.
}

//public extension<ref<real>> realRefOp
//{
//    append(real n)
//    [
//        real v := self.
//        v := v + n.
//        
//        self value := v.
//    ]
//    
//    reduce(real n)
//    [
//        real v := self.
//        v := v - n.
//        
//        self value := v.
//    ]
//}

// === Boolean values ===

// --- true ---

/// A true value symbol
public sealed singleton true : BoolValue
{
    /// Executes the action
    if(trueAction) = trueAction();

    /// Executes then-action
    if(trueAction,falseAction) = trueAction();

//    /// Always returns nil.
//    __ifnotoperator ifnot : aTrueAction = nil.
//
//    iif(object trueVal, object falseVal) = trueVal.
//    
//    /// Executes logical OR operation
//    ///
//    /// Always return true
//    bool or : anExpr = self.
//
//    /// Executes logical AND operation
//    bool and : anExpr = anExpr.
//
//    /// Executes logical XOR operation
//    bool xor : aBool
//    [
//        T<bool>(aBool) ? [ ^ false ].
//        
//        ^ self
//    ]

    /// Determines whether the specified object is equal to the current object boolean value.
    bool equal(expr) = expr;

    /// Determines whether the specified object is equal to the current object boolean value.
    bool equal(bool b) = b;

    /// Always returns false.
    bool Inverted = false;

//    /// Returns literal representation.
//    T<literal> literal = "true".
//
//    /// Returns itself.
//    clone = self.
}

// --- false ---

/// A false value symbol
public sealed singleton false : BoolValue
{
    /// Always returns nil
    if(trueAction) = nil;
    
    /// Executes else-action
    if(trueAction,falseAction) = falseAction();

//    /// Executes the action
//    __ifoperator ifnot:aFalseAction = aFalseAction().
//
//    iif(object aTrueValue, object aFalseValue) = aFalseValue.
//
//    /// Executes logical OR operation
//    bool or : anExpr = anExpr.
//
//    /// Executes logical AND operation
//    ///
//    /// Always return false
//    bool and : anExpr = self.
//
//    /// Executes logical XOR operation
//    bool xor : anExpr 
//    [
//        T<bool>(anExpr) ? [ ^ true ].
//        
//        ^ self
//    ]

    /// Determines whether the specified object is equal to the current object boolean value
    bool equal(expr)
    {
        cast bool(expr) ? { ^ self } : { ^ true }
    }

    /// Determines whether the specified object is equal to the current object boolean value
    bool equal(bool f)
    {
        ^ f.Inverted
    }

    /// Always returns true
    bool Inverted = true;

//    /// Returns literal representation
//    T<literal> literal = "false".
//    
//    /// Returns itself
//    clone = self.
}

// === basic data classes ===

// --- BaseValue ---

// A base value
public class BaseValue
{
//    bool equal(BaseVariable var)
//        <= equal(var value).
//    
//    bool less(BaseVariable var)
//        <= equal(var value).
//    
//    /// Compares the values
//    bool equal : anObject
//        = false.
//
//    /// Compares the values
//    bool less : anObject
//    [
//        system'NotSupportedException new; raise.
//    ]
//
//    /// Compares the values
//    bool greater : anObject = anObject less:self.
//
//    bool notless : anObject = self less:anObject; inverted.
//
//    bool notgreater : anObject = self greater:anObject; inverted.
}

// --- BaseNumber ---

/// A base numeric value
public class BaseNumber : BaseValue
{
//    add(BaseVariable var)
//        <= add(var value).
//
//    subtract(BaseVariable var)
//        <= subtract(var value).
//    
//    multiply(BaseVariable var)
//        <= multiply(var value).
//
//    divide(BaseVariable var)
//        <= divide(var value).
//    
//    /// Adds the value
//    add : anObject
//    [
//        system'NotSupportedException new; raise.
//    ]
//
//    /// Subtracts the value
//    subtract : anObject
//    [
//        system'NotSupportedException new; raise.
//    ]
//
//    /// Multiply by the value
//    multiply : anObject
//    [
//        system'NotSupportedException new; raise.
//    ]
//
//    /// Divides into the value
//    divide : anObject
//    [
//        system'NotSupportedException new; raise.
//    ]
//    
//    negative
//        = 0 - self.
}

// --- IntBaseNumber ---

/// A base integer value
public class IntBaseNumber : BaseNumber
{
//    and(BaseVariable var)
//        <= and(var value).
//        
//    or(BaseVariable var)
//        <= or(var value).
//        
//    xor(BaseVariable var)
//        <= xor(var value).
//    
//    /// Execute bitwise AND operation
//    and : anObject
//    [
//        system'NotSupportedException new; raise.
//    ]
//
//    /// Execute bitwise OR operation
//    or : anObject
//    [
//        system'NotSupportedException new; raise.
//    ]
//    
//    /// Execute bitwise XOR operation
//    xor : anObject
//    [
//        system'NotSupportedException new; raise.
//    ]
//    
//    /// Returns true if all the mask bits are set
//    bool allMask : anOperand
//        = self and:anOperand == anOperand.
//        
//    /// Returns true if any of the mask bits are set    
//    bool anyMask : anOperand
//        = 0 != self and:anOperand.
}

// --- ByteNumber ---

/// An unsigned 8 bit integer
public sealed const struct ByteNumber : IntBaseNumber
{
    embeddable __int theValue[1];

//    constructor(int n)
//    [
//        theValue := n && 255.
//    ]
//
//    /// Creates the object with a default value (0)
//    constructor new
//        = 0.
//
//    /// return the default value
//    constructor
//        = 0.
//
//    /// Returns the minimal value (0)
//    constructor min
//        = 0.
//
//    /// Returns the maxial value (255)
//    constructor max
//        = 255.
//    
//    /// Creates the object with a specified value
//    constructor new(byte value)
//        = value.
//
//    /// Creates the object with a specified value
//    constructor new : val
//        <= new(T<byte>(val)).
//
//    /// Returns the value as a short integer
//    cast T<short> = ShortNumber new(self).
//
//    /// Returns the value as an integer
//    cast T<int> = IntNumber new(self).
//
//    /// Returns the value as an integer
//    cast T<uint> = UIntNumber new(self).
//
//    /// Returns the value as a long integer
//    cast T<long> = LongNumber new(self).
//
//    /// Returns the value as a real number
//    cast T<real> = RealNumber new(self).
//
//    bool equal(BaseVariable v)
//        <= equal(v value).
//
//    bool equal(byte anOperand)
//        = theValue == anOperand.
//
//    bool equal(short anOperand)
//        = ShortNumber new(self); equal(anOperand).
//        
//    /// Compares the value
//    bool equal(int anOperand)
//        = IntNumber new(self); less(anOperand).
//        
//    /// Compares the value
//    bool equal(real anOperand)
//        = RealNumber new(self); equal(anOperand).
//        
//    bool equal : o
//    [
//        var n := T<byte>(o) ?? nil.
//        (nil == n)
//            ? [ ^ false ]
//            ! [ ^ self equal(n) ]
//    ]
//
//    bool less : n
//        <= less(T<byte>(n)).
//
//    bool less(BaseVariable v)
//        <= less(v value).
//
//    bool less(byte anOperand)
//        = theValue < anOperand.
//
//    bool less(short anOperand)
//        = ShortNumber new(self); less(anOperand).
//
//    /// Compares the value
//    bool less(int anOperand)
//        = IntNumber new(self); less(anOperand).
//        
//    /// Compares the value
//    bool equal(long anOperand)
//        = LongNumber new(self); equal(anOperand).
//        
//    /// Compares the value
//    bool less(long anOperand)
//        = LongNumber new(self); less(anOperand).
//
//    /// Compares the value
//    bool less(real anOperand)
//        = RealNumber new(self); less(anOperand).
//
//    add:n
//        <= add(T<byte>(n)).
//
//    subtract:n
//        <= subtract(T<byte>(n)).
//
//    multiply:n
//        <= multiply(T<byte>(n)).
//
//    divide:n
//        <= divide(T<byte>(n)).
//
//    byte add(BaseVariable v)
//        <= add(v value).
//
//    byte subtract(BaseVariable v)
//        <= subtract(v value).
//
//    byte multiply(BaseVariable v)
//        <= multiply(v value).
//
//    byte divide(BaseVariable v)
//        <= divide(v value).
//
//    /// Returns the sum
//    byte add(byte anOperand)
//        = theValue + anOperand.
//
//    /// Returns the sum
//    byte subtract(byte anOperand)
//        = theValue - anOperand.
//
//    /// Returns the product
//    byte multiply(byte anOperand)
//        = theValue * anOperand.
//
//    /// Returns the result of integer division
//    byte divide(byte anOperand)
//        = theValue / anOperand.
//
//    /// Returns the sum
//    short add(short anOperand)
//        = ShortNumber new(self); add(anOperand).
//
//    /// Returns the difference
//    short subtract(short anOperand)
//        = ShortNumber new(self); subtract(anOperand).
//
//    /// Returns the product
//    short multiply(short anOperand)
//        = ShortNumber new(self); multiply(anOperand).
//
//    /// Returns the result of integer division
//    short divide(short anOperand)
//        = ShortNumber new(self); divide(anOperand).
//
//    /// Returns the sum
//    int add(int anOperand)
//        = IntNumber new(self); add(anOperand).
//
//    /// Returns the difference
//    int subtract(int anOperand)
//        = IntNumber new(self); subtract(anOperand).
//
//    /// Returns the product
//    int multiply(int anOperand)
//        = IntNumber new(self); multiply(anOperand).
//
//    /// Returns the result of integer division
//    int divide(int anOperand)
//        = IntNumber new(self); divide(anOperand).
//
//    /// Returns the sum
//    long add(long anOperand)
//        = LongNumber new(self); add(anOperand).
//
//    /// Returns the difference
//    long subtract(long anOperand)
//        = LongNumber new(self); subtract(anOperand).
//
//    /// Returns the product
//    long multiply(long anOperand)
//        = LongNumber new(self); multiply(anOperand).
//
//    /// Returns the result of integer division
//    long divide(long anOperand)
//        = LongNumber new(self); divide(anOperand).
//
//    /// Returns the sum
//    real add(real anOperand)
//        = RealNumber new(self); add(anOperand).
//
//    /// Returns the difference
//    real subtract(real anOperand)
//        = RealNumber new(self); subtract(anOperand).
//
//    /// Returns the product
//    real multiply(real anOperand)
//        = RealNumber new(self); multiply(anOperand).
//
//    /// Returns the result of integer division
//    real divide(real anOperand)
//        = RealNumber new(self); divide(anOperand).
//
//    and:n
//        <= and(T<byte>(n)).
//        
//    or:n
//        <= or(T<byte>(n)).
//        
//    xor:n
//        <= xor(T<byte>(n)).
//        
//    byte and(BaseVariable v)
//        <= and(v value).
//        
//    byte or(BaseVariable v)
//        <= or(v value).
//        
//    byte xor(BaseVariable v)
//        <= xor(v value).
//        
//    /// Returns the result of bitwise AND operation
//    byte and(byte anOperand)
//        = theValue && anOperand.
//
//    /// Returns the result of bitwise OR operation
//    byte or(byte anOperand)
//        = theValue || anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    byte xor(byte anOperand)
//        = theValue ^^ anOperand.
//
//    /// Returns the result of bitwise AND operation
//    short and(short anOperand)
//        = ShortNumber new(self); and(anOperand).
//
//    /// Returns the result of bitwise OR operation
//    short or(short anOperand)
//        = ShortNumber new(self); or(anOperand).
//
//    /// Returns the result of bitwise XOR operation
//    short xor(short anOperand)
//        = ShortNumber new(self); xor(anOperand).
//
//    /// Returns the result of bitwise AND operation
//    int and(int anOperand)
//        = IntNumber new(self); and(anOperand).
//
//    /// Returns the result of bitwise OR operation
//    int or(int anOperand)
//        = IntNumber new(self); or(anOperand).
//
//    /// Returns the result of bitwise XOR operation
//    int xor(int anOperand)
//        = IntNumber new(self); xor(anOperand).
//
//    /// Returns the result of bitwise AND operation
//    long and(long anOperand)
//        = LongNumber new(self); and(anOperand).
//
//    /// Returns the result of bitwise OR operation
//    long or(long anOperand)
//        = LongNumber new(self); or(anOperand).
//
//    /// Returns the result of bitwise XOR operation
//    long xor(long anOperand)
//        = LongNumber new(self); xor(anOperand).
//
//    byte shiftLeft(int val)
//    [
//        ^ theValue << val
//    ]
//
//    byte shiftRight(int val)
//    [
//        ^ theValue >> val
//    ]
//    
//    /// Bitwise inversion
//    byte inverted
//        => system'core_routines'__byte.
//        
//    /// Returns the literal presentation
//    T<literal> literal = literalConvertor convert(self).
//        
//    /// Clones the value
//    clone = ByteNumber new(self).
//
//    // Reads the object hash code into the output parameter
//    readHashCodeTo(ref<int> retVal)
//        => system'core_routines'__int.
//    
//    embeddable int hash_code
//    [
//        int aCode := 0.
//        self readHashCodeTo(&aCode).
//        
//        ^ aCode
//    ]
}

// --- ShortNumber ---

/// A signed 16 bit integer
public sealed const struct ShortNumber : IntBaseNumber
{
    embeddable __int theValue[2];

//    constructor(int n)
//    [
//        (n < 0)
//            ? [ 
//                theValue := n && 0FFFFh. 
//                
//                theValue := theValue || 0FFFF0000h
//            ]
//            ! [ theValue := n && 0FFFFh ]
//    ]
//    
//    /// returns the default value
//    constructor
//        = 0.
//    
//    /// Returns the minimal value (-32768)
//    constructor min
//        = -32768.
//
//    /// Returns the maxial value (32767)
//    constructor max
//        = 32767.
//
//    /// Creates the object with specified value
//    constructor new(byte value)
//        = shortConvertor convert(value).
//        
//    /// Creates the object with specified value
//    constructor new(short value)
//        = value.
//
//    /// Creates the object with a default value (0)
//    constructor new
//        = 0.
//
//    /// Creates the object with specified value
//    constructor new : anObject
//        <= new(T<short>(anObject)).
//
//    /// Returns the byte value        
//    cast T<byte> = byteConvertor convert(self).
//    
//    /// Returns the integer value
//    cast T<int> = IntNumber new(self).
//    
//    /// Returns the integer value
//    cast T<uint> = UIntNumber new(self).
//    
//    /// Returns the long integer value
//    cast T<long> = LongNumber new(self).
//    
//    /// Returns the value as a real number
//    cast T<real> = RealNumber new(self).
//
//    bool equal:o
//    [
//        var n := T<short>(o) ?? nil.
//        (nil == n)
//            ? [ ^ false ]
//            ! [ ^ self equal(n) ]
//    ]
//
//    bool less:n
//        <= less(n).
//
//    bool equal(BaseVariable v)
//        <= equal(v value).
//                
//    bool less(BaseVariable v)
//        <= equal(v value).
//
//    /// Compares the value
//    bool equal(short anOperand)
//        = theValue == anOperand.
//        
//    /// Compares the value
//    bool less(short anOperand)
//        = theValue < anOperand.
//        
//    /// Compares the value
//    bool equal(int anOperand)
//        = IntNumber new(self); equal(anOperand).
//        
//    /// Compares the value
//    bool less(int anOperand)
//        = IntNumber new(self); less(anOperand).
//    
//    /// Compares the value
//    bool equal(byte anOperand)
//        = self equal(ShortNumber new(anOperand)).
//        
//    /// Compares the value
//    bool less (byte anOperand)
//        = self less(ShortNumber new(anOperand)).
//    
//    /// Compares the value
//    bool equal(long anOperand)
//        = LongNumber new(self); equal(anOperand).
//        
//    /// Compares the value
//    bool less(long anOperand)
//        = LongNumber new(self); less(anOperand).
//    
//    /// Compares the value
//    bool equal(real anOperand )
//        = RealNumber new(self); equal(anOperand).
//        
//    /// Compares the value
//    bool less(real anOperand)
//        = RealNumber new(self); less(anOperand).
//
//    add:n
//        <= add(T<short>(n)).
//
//    subtract:n
//        <= subtract(T<short>(n)).
//
//    multiply:n
//        <= multiply(T<short>(n)).
//
//    divide:n
//        <= divide(T<short>(n)).
//
//    short add(BaseVariable v)
//        <= add(v value).
//
//    short subtract(BaseVariable v)
//        <= subtract(v value).
//
//    short multiply(BaseVariable v)
//        <= multiply(v value).
//
//    short divide(BaseVariable v)
//        <= divide(v value).
//
//    /// Returns the sum
//    short add(short anOperand)
//        = theValue + anOperand.
//
//    /// Returns the sum
//    short subtract(short anOperand)
//        = theValue - anOperand.
//
//    /// Returns the product
//    short multiply(short anOperand)
//        = theValue * anOperand.
//
//    /// Returns the result of integer division
//    short divide(short anOperand)
//        = theValue / anOperand.
//
//    /// Returns the sum
//    add(byte anOperand)
//        = self add(ShortNumber new(anOperand)).
//
//    /// Returns the difference
//    subtract(byte anOperand)
//        = self subtract(ShortNumber new(anOperand)).
//
//    /// Returns the product
//    multiply(byte anOperand)
//        = self multiply(ShortNumber new(anOperand)).
//
//    /// Returns the result of integer division
//    divide(byte anOperand)
//        = self divide(ShortNumber new(anOperand)).
//
//    /// Returns the sum
//    add(int anOperand)
//        = IntNumber new(self); add(anOperand).
//
//    /// Returns the difference
//    subtract(int anOperand)
//        = IntNumber new(self); subtract(anOperand).
//
//    /// Returns the product
//    multiply(int anOperand)
//        = IntNumber new(self); multiply(anOperand).
//        
//    /// Returns the result of integer division
//    divide(int anOperand)
//        = IntNumber new(self); divide(anOperand).
//
//    /// Returns the sum
//    add(long anOperand)
//        = LongNumber new(self); add(anOperand).
//
//    /// Returns the difference
//    subtract(long anOperand)
//        = LongNumber new(self); subtract(anOperand).
//
//    /// Returns the product
//    multiply(long anOperand)
//        = LongNumber new(self); multiply(anOperand).
//        
//    /// Returns the result of integer division
//    divide(long anOperand)
//        = LongNumber new (self); divide(anOperand).
//
//    /// Returns the sum
//    add(real anOperand)
//        = RealNumber new(self); add(anOperand).
//
//    /// Returns the difference
//    subtract(real anOperand)
//        = RealNumber new(self); subtract(anOperand).
//
//    /// Returns the product
//    multipl(real anOperand)
//        = RealNumber new(self); multiply(anOperand).
//        
//    /// Returns the result of integer division
//    divide(real anOperand)
//        = RealNumber new(self); divide(anOperand).
//
//    and:n
//        <= and(T<short>(n)).
//        
//    or:n
//        <= divide(T<short>(n)).
//        
//    xor:n
//        <= xor(T<short>(n)).
//        
//    short and(BaseVariable v)
//        <= and(v value).
//        
//    short or(BaseVariable v)
//        <= divide(v value).
//        
//    short xor(BaseVariable v)
//        <= xor(v value).
//        
//    /// Returns the result of bitwise AND operation
//    short and(short anOperand)
//        = theValue && anOperand.
//
//    /// Returns the result of bitwise OR operation
//    short or(short anOperand)
//        = theValue || anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    short xor(short anOperand)
//        = theValue ^^ anOperand.
//        
//    /// Returns the result of bitwise AND operation
//    and(byte anOperand)
//        = self and(ShortNumber new(anOperand)). 
//
//    /// Returns the result of bitwise OR operation
//    or(byte anOperand)
//        = self or(ShortNumber new(anOperand)). 
//
//    /// Returns the result of bitwise XOR operation
//    xor(byte anOperand)
//        = self xor(ShortNumber new(anOperand)). 
//        
//    /// Returns the result of bitwise AND operation
//    and(int anOperand)
//        = IntNumber new(self); and(anOperand).
//        
//    /// Returns the result of bitwise OR operation
//    or(int anOperand)
//        = IntNumber new(self); or(anOperand).
//        
//    /// Returns the result of bitwise XOR operation
//    xor(int anOperand)
//        = IntNumber new(self); xor(anOperand).
//        
//    /// Returns the result of bitwise AND operation
//    and(long anOperand)
//        = LongNumber new(self); and(anOperand).
//        
//    /// Returns the result of bitwise OR operation
//    or(long anOperand)
//        = LongNumber new(self); or(anOperand).
//        
//    /// Returns the result of bitwise XOR operation
//    xor(long anOperand)
//        = LongNumber new(self); xor(anOperand).
//
//    short shiftLeft(int val)
//    [
//        ^ theValue << val
//    ]
//
//    short shiftRight(int val)
//    [
//        ^ theValue >> val
//    ]
//    
//    /// Bitwise inversion
//    short inverted
//        => system'core_routines'__short.  
//
//    /// Returns the literal presentation
//    T<literal> literal = literalConvertor convert(self).
//
////    /// Returns the literal presentation
////    wide = system'wideConvertor convert short:$self.
//
//    /// Clones the value
//    clone = ShortNumber new(self).
//
//    // Reads the object hash code into the output parameter
//    readHashCodeTo(ref<int> aCode)
//        => system'core_routines'__int.
//    
//    embeddable int hash_code
//    [
//        int aCode := 0.
//        self readHashCodeTo(&aCode).
//        
//        ^ aCode.
//    ]        
}

// --- IntNumber ---

/// A signed 32 bit integer
public sealed const struct IntNumber : IntBaseNumber
{
    embeddable __int theValue[4];
    
//    constructor(byte n)
//    [
//        theValue := n.
//    ]
//    
//    constructor(short n)
//    [
//        theValue := n.
//    ]

    /// return the default value
    static int DefaultValue
        = 0;
    
    /// Creates the object with the minimal value
    static int MinValue
        = -2147483648;

    /// Creates the object with the maximal value
    static int MaxValue
        = 2147483647;

    constructor()
        = 0;
    
//    /// Creates the object with specified value
//    constructor new(int value)
//        = value.
//
//    /// Creates the object with specified value
//    constructor new(short value)
//        = intConvertor convert(value).
//
//    /// Creates the object with specified value
//    constructor new(byte value)
//        = intConvertor convert(value).
//
//    /// Creates the object with a default value (0)
//    constructor new
//        <= new(0).
//
//    /// Creates the object with specified value
//    constructor new : anObject
//        <= new(T<int>(anObject)).
//
//    /// Returns the value as a short integer    
//    cast T<short> = shortConvertor convert(self).
//
//    /// Returns the byte value    
//    cast T<byte> = byteConvertor convert(self).
//
//    /// Returns the long integer
//    cast T<long> = LongNumber new(self).
//
//    /// Returns the real number
//    cast T<real> = RealNumber new(self).
//
//    /// Returns the real number
//    cast T<uint> = UIntNumber new(self).
//    
//    bool equal:o
//    [
//        var n := T<int>(o) ?? nil.
//        (nil == n)
//            ? [ ^ false ]
//            ! [ ^ self equal(n) ]
//    ]
//
//    bool less:n
//        <= less(T<int>(n)).
//        
//    bool equal(BaseVariable v)
//        <= equal(v value).
//        
//    bool less(BaseVariable v)
//        <= less(v value).
//        
//    /// Compares the value
//    bool equal(int anOperand)
//        = theValue == anOperand.
//        
//    /// Compares the value
//    bool less(int anOperand)
//        = theValue < anOperand.
//        
//    add:n
//        <= add(T<int>(n)).
//
//    subtract:n
//        <= subtract(T<int>(n)).
//
//    multiply:n
//        <= multiply(T<int>(n)).
//
//    divide:n
//        <= divide(T<int>(n)).
//        
//    int add(BaseVariable v)
//        <= add(v value).
//
//    int subtract(BaseVariable v)
//        <= subtract(v value).
//
//    int multiply(BaseVariable v)
//        <= multiply(v value).
//
//    int divide(BaseVariable v)
//        <= divide(v value).
//        
//    /// Returns the sum
//    int add(int anOperand)
//        = theValue + anOperand.
//
//    /// Returns the sum
//    int subtract(int anOperand)
//        = theValue - anOperand.
//
//    /// Returns the product
//    int multiply(int anOperand)
//        = theValue * anOperand.
//
//    /// Returns the result of integer division
//    int divide(int anOperand)
//        = theValue / anOperand.
//
//    and:n
//        <= and(T<int>(n)).
//        
//    or:n
//        <= divide(T<int>(n)).
//        
//    xor:n
//        <= xor(T<int>(n)).
//        
//    int and(BaseVariable v)
//        <= and(v value).
//        
//    int or(BaseVariable v)
//        <= or(v value).
//        
//    int xor(BaseVariable v)
//        <= xor(v value).
//        
//    /// Returns the result of bitwise AND operation
//    int and(int anOperand)
//        = theValue && anOperand.
//
//    /// Returns the result of bitwise OR operation
//    int or(int anOperand)
//        = theValue || anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    int xor(int anOperand)
//        = theValue ^^ anOperand.    
//
//    /// Compares the integer value
//    bool equal(short value)
//        = self equal(IntNumber new(value)).
//
//    /// Compares the integer value
//    bool equal(byte value)
//        = self equal(IntNumber new(value)).
//        
//    /// Compares the integer value
//    bool equal(long anOperand)
//        = LongNumber new(self); equal(anOperand).
//
//    /// Compares the integer value
//    bool equal(real anOperand)
//        = RealNumber new(self); equal(anOperand).
//
//    /// Compares the integer value
//    bool less(short value)
//        = self equal(IntNumber new(value)).
//
//    /// Compares the integer value
//    bool less(byte value)
//        = self less(IntNumber new(value)).
//        
//    /// Compares the integer value
//    bool less(long anOperand)
//        = LongNumber new(self); less(anOperand).
//
//    /// Compares the integer value
//    bool less(real anOperand)
//        = RealNumber new(self); less(anOperand).
//
//    /// Returns the sum of integers
//    add(short value)
//        = self add(IntNumber new (value)).
//
//    /// Returns the sum of integers
//    add(byte value)
//        = self add(IntNumber new(value)).
//
//    /// Returns the sum of integers
//    long add(long anOperand)
//        = LongNumber new(self); add(anOperand).
//
//    /// Returns the sum of integers
//    real add(real anOperand)
//        = RealNumber new(self); add(anOperand).
//
//    /// Returns the difference of integers
//    subtract(short value)
//        = self subtract(IntNumber new(value)).
//
//    /// Returns the difference of integers
//    subtract(byte value)
//        = self subtract(IntNumber new(value)).
//
//    /// Returns the difference of integers
//    long subtract(long anOperand)
//        = LongNumber new(self); subtract(anOperand).
//
//    /// Returns the difference of integers
//    real subtract(real anOperand)
//        = RealNumber new(self); subtract(anOperand).
//
//    /// Returns the product of integers
//    multiply(short value)
//        = self multiply(IntNumber new(value)).        
//
//    /// Returns the product of integers
//    multiply(byte value)
//        = self multiply(IntNumber new(value)).
//
//    /// Returns the product of integers
//    long multiply(long anOperand)
//        = LongNumber new(self); multiply(anOperand).
//
//    /// Returns the product of integers
//    real multiply(real anOperand)
//        = RealNumber new(self); multiply(anOperand).
//
//    /// Returns the result of integer division
//    divide(short value)
//        = self divide(IntNumber new(value)).
//
//    /// Returns the result of integer division
//    divide(byte value)
//        = self divide(IntNumber new(value)).        
//
//    /// Returns the result of integer division
//    long divide(long anOperand)
//        = LongNumber new(self); divide(anOperand).
//
//    /// Returns the result of integer division
//    real divide(real anOperand)
//        = RealNumber new(self); divide(anOperand).
//
//    /// Returns the result of bitwise AND operation
//    and(short anOperand)
//        = self and(IntNumber new(anOperand)).
//
//    /// Returns the result of bitwise AND operation
//    and(byte anOperand)
//        = self and(IntNumber new(anOperand)).
//
//    /// Returns the result of bitwise AND operation
//    and(long anOperand)
//        = LongNumber new(self); and(anOperand).
//
//    /// Returns the result of bitwise OR operation
//    or(short anOperand)
//        = self or(IntNumber new(anOperand)).
//
//    /// Returns the result of bitwise OR operation
//    or(byte anOperand)
//        = self or(IntNumber new(anOperand)).
//
//    /// Returns the result of bitwise OR operation
//    or(long anOperand)
//        = LongNumber new(self); or(anOperand).
//
//    /// Returns the result of bitwise XOR operation
//    xor(short anOperand)
//        = self xor(IntNumber new(anOperand)).
//
//    /// Returns the result of bitwise XOR operation
//    xor(byte anOperand)
//        = self xor(IntNumber new(anOperand)).
//
//    /// Returns the result of bitwise XOR operation
//    xor(long anOperand)
//        = LongNumber new(self); xor(anOperand).
//
//    /// Returns the literal presentation
//    T<literal> literal = literalConvertor convert(self).
//
////    /// Returns the literal presentation
////    wide = system'wideConvertor convert int:$self.
//
//    /// Clones the value
//    clone = IntNumber new(self).
//
//    // Reads the object hash code into the output parameter
//    readHashCodeTo(ref<int> aCode)
//        => system'core_routines'__int.
//    
//    embeddable int hash_code
//    [
//        int aCode := 0.
//        self readHashCodeTo(&aCode).
//        
//        ^ aCode.
//    ]        
//
//    int shiftLeft(BaseVariable v)
//        <= shiftLeft(v value).
//
//    int shiftLeft(int val)
//    [
//        ^ theValue << val
//    ]
//
//    int shiftRight(BaseVariable v)
//        <= shiftRight(v value).
//
//    int shiftRight(int val)
//    [
//        ^ theValue >> val
//    ]
//    
//    /// Bitwise inversion
//    readInvertedTo(ref<int> retVal)
//        => system'core_routines'__int.
//    
//    embeddable int inverted
//    [
//        int val := 0.
//        self readInvertedTo(&val).
//        
//        ^ val
//    ]
//        
//    /// Negates the value
//    negative
//        = 0 - theValue.
}

//// --- UIntNumber ---
//
///// A unsigned 32 bit integer
//
//public sealed struct UIntNumber :: IntBaseNumber
//{
//    T<__int,4> theValue.
//
//    constructor(byte n)
//    [
//        theValue := n.
//    ]
//    
//    constructor(short n)
//    [
//        theValue := n.
//    ]
//    
//    constructor(int n)
//    [
//        theValue := n.
//    ]
//    
//    cast u(literal s)
//    [
//        theValue := intConvertor convertUnsigned(s).
//    ]
//    
//    /// return the default value
//    constructor
//        = 0.
//    
//    /// Creates the object with the minimal value
//    constructor min
//        = 0.
//
//    /// Creates the object with the maximal value
//    constructor max
//        = 0FFFFFFFFh.
//        
//    constructor new(int n)
//    [
//        theValue := n.
//    ]        
//        
//    constructor new(byte n)
//    [
//        theValue := n.
//    ]        
//        
//    constructor new(short n)
//    [
//        theValue := n.
//    ]        
//        
//    constructor new(uint n)
//    [
//        theValue := n.
//    ]        
//        
//    constructor new
//    [
//        theValue := 0.
//    ]        
//
//    /// Returns the literal presentation
//    T<literal> literal = literalConvertor convert(self).
//
////    /// Returns the literal presentation
////    wide = system'wideConvertor convert uint:$self.
//        
//    /// Returns the value as a short integer    
//    cast T<short> = shortConvertor convert(self).
//
//    /// Returns the byte value    
//    cast T<byte> = byteConvertor convert(self).
//
//    cast T<int> = self.
//
//    /// Returns the long integer
//    cast T<long> = LongNumber new(self).
//
//    /// Returns the real number
//    cast T<real> = RealNumber new(self).
//
//    bool equal:o
//    [
//        var n := T<int>(o) ?? nil.
//        (nil == n)
//            ? [ ^ false ]
//            ! [ ^ self equal(n) ]
//    ]
//
//    bool less:n
//        <= less(T<int>(n)).
//        
//    bool equal(BaseVariable v)
//        <= equal(v value).
//        
//    bool less(BaseVariable v)
//        <= less(v value).
//        
//    /// Compares the value
//    bool equal(int anOperand)
//        = theValue == anOperand.
//        
//    /// Compares the value
//    bool less(int anOperand)
//        = theValue < anOperand.
//        
//    add:n
//        <= add(T<int>(n)).
//
//    subtract:n
//        <= subtract(T<int>(n)).
//
//    multiply:n
//        <= multiply(T<int>(n)).
//
//    divide:n
//        <= divide(T<int>(n)).
//        
//    int add(BaseVariable v)
//        <= add(v value).
//
//    int subtract(BaseVariable v)
//        <= subtract(v value).
//
//    int multiply(BaseVariable v)
//        <= multiply(v value).
//
//    int divide(BaseVariable v)
//        <= divide(v value).
//        
//    /// Returns the sum
//    int add(int anOperand)
//        = theValue + anOperand.
//
//    /// Returns the sum
//    int subtract(int anOperand)
//        = theValue - anOperand.
//
//    /// Returns the product
//    int multiply(int anOperand)
//        = theValue * anOperand.
//
//    /// Returns the result of integer division
//    int divide(int anOperand)
//        = theValue / anOperand.
//
//    and:n
//        <= and(T<int>(n)).
//        
//    or:n
//        <= divide(T<int>(n)).
//        
//    xor:n
//        <= xor(T<int>(n)).
//        
//    int and(BaseVariable v)
//        <= and(v value).
//        
//    int or(BaseVariable v)
//        <= or(v value).
//        
//    int xor(BaseVariable v)
//        <= xor(v value).
//        
//    /// Returns the result of bitwise AND operation
//    int and(int anOperand)
//        = theValue && anOperand.
//
//    /// Returns the result of bitwise OR operation
//    int or(int anOperand)
//        = theValue || anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    int xor(int anOperand)
//        = theValue ^^ anOperand.
//
//    /// Compares the value
//    bool equal(uint anOperand)
//        = theValue == anOperand.
//        
//    /// Compares the value
//    bool less(uint anOperand)
//        = theValue < anOperand.
//        
//    /// Returns the sum
//    uint add(uint anOperand)
//        = theValue + anOperand.
//
//    /// Returns the sum
//    uint subtract(uint anOperand)
//        = theValue - anOperand.
//
//    /// Returns the product
//    uint multiply(uint anOperand)
//        = theValue * anOperand.
//
//    /// Returns the result of integer division
//    uint divide(uint anOperand)
//        = theValue / anOperand.
//
//    /// Returns the result of bitwise AND operation
//    uint and(uint anOperand)
//        = theValue && anOperand.
//
//    /// Returns the result of bitwise OR operation
//    uint or(uint anOperand)
//        = theValue || anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    uint xor(uint anOperand)
//        = theValue ^^ anOperand.
//
//    /// Compares the integer value
//    bool equal(short value)
//        = self equal(T<int>(value)).
//
//    /// Compares the integer value
//    bool equal(byte value)
//        = self equal(T<int>(value)).
//        
//    /// Compares the integer value
//    bool equal(long anOperand )
//        = LongNumber new(self); equal(anOperand).
//
//    /// Compares the integer value
//    bool equal(real anOperand)
//        = RealNumber new(self); equal(anOperand).
//
//    /// Compares the integer value
//    bool less(short value)
//        = self equal(T<uint>(value)).
//
//    /// Compares the integer value
//    bool less(byte value)
//        = self less(T<uint>(value)).
//        
//    /// Compares the integer value
//    bool less(long anOperand)
//        = LongNumber new(self); less(anOperand).
//
//    /// Compares the integer value
//    bool less(real anOperand)
//        = RealNumber new(self); less(anOperand).
//
//    /// Returns the sum of integers
//    add(short value)
//        = self add(T<uint>(value)).
//
//    /// Returns the sum of integers
//    add(byte value)
//        = self add(T<uint>(value)).
//
//    /// Returns the sum of integers
//    add(long anOperand)
//        = LongNumber new(self); add(anOperand).
//
//    /// Returns the sum of integers
//    add(real anOperand)
//        = RealNumber new(self); add(anOperand).
//
//    /// Returns the difference of integers
//    subtract(short value)
//        = self subtract(T<uint>(value)).
//
//    /// Returns the difference of integers
//    subtract(byte value)
//        = self subtract(T<uint>(value)).
//
//    /// Returns the difference of integers
//    subtract(long anOperand)
//        = LongNumber new(self); subtract(anOperand).
//
//    /// Returns the difference of integers
//    subtract(real anOperand)
//        = RealNumber new(self); subtract(anOperand).
//
//    /// Returns the product of integers
//    multiply(short value)
//        = self multiply(T<uint>(value)).
//
//    /// Returns the product of integers
//    multiply(byte value)
//        = self multiply(T<uint>(value)).
//
//    /// Returns the product of integers
//    multiply(long anOperand)
//        = LongNumber new(self); multiply(anOperand).
//
//    /// Returns the product of integers
//    multiply(real anOperand)
//        = RealNumber new(self); multiply(anOperand).
//
//    /// Returns the result of integer division
//    divide(short value)
//        = self divide(T<uint>(value)).
//
//    /// Returns the result of integer division
//    divide(byte value)
//        = self divide(T<uint>(value)).
//
//    /// Returns the result of integer division
//    divide(long anOperand)
//        = LongNumber new(self); divide(anOperand).
//
//    /// Returns the result of integer division
//    divide(real anOperand)
//        = RealNumber new(self); divide(anOperand).
//
//    /// Returns the result of bitwise AND operation
//    and(short value)
//        = self and(T<uint>(value)).
//
//    /// Returns the result of bitwise AND operation
//    and(byte value)
//        = self and(T<uint>(value)).
//
//    /// Returns the result of bitwise AND operation
//    and(long value)
//        = LongNumber new(self); and(value).
//
//    /// Returns the result of bitwise OR operation
//    or(short value)
//        = self or(T<uint>(value)).
//
//    /// Returns the result of bitwise OR operation
//    or(byte value)
//        = self or(T<uint>(value)).
//
//    /// Returns the result of bitwise OR operation
//    or(long value)
//        = LongNumber new(self); or(value).
//
//    /// Returns the result of bitwise XOR operation
//    xor(short value)
//        = self xor(T<uint>(value)).
//
//    /// Returns the result of bitwise XOR operation
//    xor(byte value)
//        = self xor(T<uint>(value)).
//
//    /// Returns the result of bitwise XOR operation
//    xor(long anOperand)
//        = LongNumber new(self); xor(anOperand).
//
//    /// Clones the value
//    clone = UIntNumber new(self).
//
//    // Reads the object hash code into the output parameter
//    readHashCodeTo(ref<int> aCode)
//        => system'core_routines'__int.
//    
//    embeddable int hash_code
//    [
//        int aCode := 0.
//        self readHashCodeTo(&aCode).
//        
//        ^ aCode.
//    ]        
//
//    uint shiftLeft(int val)
//    [
//        ^ theValue << val
//    ]
//
//    uint shiftRight(int val)
//    [
//        ^ theValue >> val
//    ]
//    
//    /// Bitwise inversion
//    uint inverted
//        => system'core_routines'__int.  
//}
//
//// --- LongNumber ---
//
///// A signed 64 bit integer value
//public sealed struct LongNumber :: IntBaseNumber
//{
//    T<__int,8> theValue.
//    
//    /// return the default value
//    constructor
//        = 0l.
//    
//    /// Returns the minimal value
//    constructor min
//        = -9223372036854775808l.
//
//    /// Returns the maximal value
//    constructor max
//        = 9223372036854775807l.
//
//    /// Creates the object with a default value (0)
//    constructor new
//        <= new:0l.
//
//    /// Creates the object with specified value
//    constructor new(long value)
//        = value.
//
//    /// Creates the object with specified value
//    constructor new(int value)
//        = longConvertor convert(value).
//
//    /// Creates the object with specified value
//    constructor new(uint value)
//        = longConvertor convert(value).
//
//    /// Creates the object with specified value
//    constructor new(short value)
//        = longConvertor convert(value).
//
//    /// Creates the object with specified value
//    constructor new(byte value)
//        = longConvertor convert(value).
//
//    /// Creates the object with specified value
//    constructor new : anObject
//        <= new(T<long>(anObject)).
//
//    /// Returns an integer value
//    cast T<int> = intConvertor convert(self).
//
//    /// Returns an integer value
//    cast T<uint> = uintConvertor convert(self).
//
//    /// Returns the value as a short integer    
//    cast T<short> = shortConvertor convert(self).
//    
//    /// Returns the byte value
//    cast T<byte> = byteConvertor convert(self).
//
//    /// Returns a real value
//    cast T<real> = RealNumber new(self).
//    
//    bool equal:o
//    [
//        var n := T<long>(o) ?? nil.
//        (nil == n)
//            ? [ ^ false ]
//            ! [ ^ self equal(n) ]
//    ]
//
//    bool less:n
//        <= less(T<long>(n)).
//        
//    bool equal(BaseVariable v)
//        <= equal(v value).
//        
//    bool less(BaseVariable v)
//        <= less(v value).
//        
//    /// Compares the value
//    bool equal(long anOperand)
//        = theValue == anOperand.
//        
//    /// Compares the value
//    bool less(long anOperand)
//        = theValue < anOperand.
//        
//    add:n
//        <= add(T<long>(n)).
//
//    subtract:n
//        <= subtract(T<long>(n)).
//
//    multiply:n
//        <= multiply(T<long>(n)).
//
//    divide:n
//        <= divide(T<long>(n)).
//        
//    long add(BaseVariable v)
//        <= add(v value).
//
//    long subtract(BaseVariable v)
//        <= subtract(v value).
//
//    long multiply(BaseVariable v)
//        <= multiply(v value).
//
//    long divide(BaseVariable v)
//        <= divide(v value).
//        
//    /// Returns the sum
//    long add(long anOperand)
//        = theValue + anOperand.
//
//    /// Returns the sum
//    long subtract(long anOperand)
//        = theValue - anOperand.
//
//    /// Returns the product
//    long multiply(long anOperand)
//        = theValue * anOperand.
//
//    /// Returns the result of integer division
//    long divide(long anOperand)
//        = theValue / anOperand.
//
//    and:n
//        <= and(T<long>(n)).
//        
//    or:n
//        <= divide(T<long>(n)).
//        
//    xor:n
//        <= xor(T<long>(n)).
//        
//    long and(BaseVariable v)
//        <= and(v value).
//        
//    long or(BaseVariable v)
//        <= divide(v value).
//        
//    long xor(BaseVariable v)
//        <= xor(v value).
//        
//    /// Returns the result of bitwise AND operation
//    long and(long anOperand)
//        = theValue && anOperand.
//
//    /// Returns the result of bitwise OR operation
//    long or(long anOperand)
//        = theValue || anOperand.
//
//    /// Returns the result of bitwise XOR operation
//    long xor(long anOperand)
//        = theValue ^^ anOperand.    
//
//    /// Compares the integer value
//    bool equal(int anOperand) 
//        = self equal(LongNumber new(anOperand)).
//
//    /// Compares the integer value
//    bool equal(short anOperand)
//        = self equal(LongNumber new(anOperand)).
//
//    /// Compares the integer value
//    bool equal(byte anOperand)
//        = self equal(LongNumber new(anOperand)).
//
//    /// Compares the integer value
//    bool equal(real anOperand)
//        = RealNumber new(self); equal(anOperand).
//
//    /// Compares the integer value
//    bool less(int anOperand)
//        = self less(LongNumber new(anOperand)).
//
//    /// Compares the integer value
//    bool less(short anOperand)
//        = self less(LongNumber new(anOperand)).
//
//    /// Compares the integer value
//    bool less(byte anOperand)
//        = self less(LongNumber new(anOperand)).
//
//    /// Compares the integer value
//    bool less(real anOperand)
//        = RealNumber new(self); less(anOperand).
//
//    /// Returns the sum of integers
//    add(int anOperand)
//        = self add(LongNumber new(anOperand)).
//
//    /// Returns the sum of integers
//    add(short anOperand)
//        = self add(LongNumber new(anOperand)).
//
//    /// Returns the sum of integers
//    add(byte anOperand)
//        = self add(LongNumber new(anOperand)).
//
//    /// Returns the sum of integers
//    add(real anOperand)
//        = RealNumber new(self); add(anOperand).
//
//    /// Returns the difference of integers
//    subtract(int anOperand)
//        = self subtract(LongNumber new(anOperand)).
//
//    /// Returns the difference of integers
//    subtract(short anOperand)
//        = self subtract(LongNumber new(anOperand)).
//
//    /// Returns the difference of integers
//    subtract(byte anOperand)
//        = self subtract(LongNumber new(anOperand)).
//
//    /// Returns the difference of integers
//    subtract(real anOperand)
//        = RealNumber new(self); subtract(anOperand).
//
//    /// Returns the product of integers
//    multiply(real anOperand)
//        = RealNumber new(self); multiply(anOperand).
//
//    /// Returns the product of integers
//    multiply(int anOperand)
//        = self multiply(LongNumber new(anOperand)).
//
//    /// Returns the product of integers
//    multiply(byte anOperand)
//        = self multiply(LongNumber new(anOperand)).
//
//    /// Returns the product of integers
//    multiply(short anOperand)
//        = self multiply(LongNumber new(anOperand)).
//
//    /// Returns the result of integer division
//    divide(int anOperand)
//        = self divide(LongNumber new(anOperand)).
//
//    /// Returns the result of integer division
//    divide(short anOperand)
//        = self divide(LongNumber new(anOperand)).
//
//    /// Returns the result of integer division
//    divide(byte anOperand)
//        = self divide(LongNumber new(anOperand)).
//
//    /// Returns the result of integer division
//    divide(real anOperand)
//        = RealNumber new(self); divide(anOperand).
//        
//    /// Returns the result of bitwise AND operation
//    and(int anOperand)
//        = self and(LongNumber new(anOperand)).
//
//    /// Returns the result of bitwise AND operation
//    and(short anOperand)
//        = self and(LongNumber new(anOperand)).
//
//    /// Returns the result of bitwise AND operation
//    and(byte anOperand)
//        = self and(LongNumber new(anOperand)).
//
//    /// Returns the result of bitwise OR operation
//    or(int anOperand)
//        = self or(LongNumber new(anOperand)).
//
//    /// Returns the result of bitwise OR operation
//    or(short anOperand)
//        = self or(LongNumber new(anOperand)).
//
//    /// Returns the result of bitwise OR operation
//    or(byte anOperand)
//        = self or(LongNumber new(anOperand)).  
//
//    /// Returns the result of bitwise XOR operation
//    xor(int anOperand)
//        = self xor(LongNumber new(anOperand)).
//
//    /// Returns the result of bitwise XOR operation
//    xor(short anOperand)
//        = self xor(LongNumber new(anOperand)).
//
//    /// Returns the result of bitwise XOR operation
//    xor(byte anOperand)
//        = self xor(LongNumber new(anOperand)).
//
//    /// Returns the literal presentation
//    T<literal> literal = literalConvertor convert(self).
//
////    /// Returns the literal presentation
////    wide = system'wideConvertor convert long:$self.
//
//    /// Clones the value
//    clone = LongNumber new(self).
//    
//    // Reads the object hash code into the output parameter
//    readHashCodeTo(ref<int> aCode)
//        => system'core_routines'__int.
//    
//    embeddable int hash_code
//    [
//        int aCode := 0.
//        self readHashCodeTo(&aCode).
//        
//        ^ aCode.
//    ]
//
//    long shiftLeft(int val)
//    [
//        ^ theValue << val
//    ]
//
//    long shiftRight(int val)
//    [
//        ^ theValue >> val
//    ]
//    
//    /// Bitwise inversion
//    long inverted
//        => system'core_routines'__long.  
//        
//    /// Negates the value
//    negative
//        = 0l - theValue.
//}
//
//// --- realNumber ---
//
///// A 64bit float numeric value
//public sealed struct RealNumber :: BaseNumber
//{
//    T<__real,8> theValue.
//
//    constructor(int n)
//    [
//        realConvertor convert(n,&self).
//    ]
//            
//    /// return the default value
//    constructor
//        = 0.0r.
//            
//    /// Returns the minimal value
//    constructor min
//        = -1.7976931348623157e+308r.
//    
//    /// Returns the maximal value
//    constructor max
//        = 1.7976931348623157e+308r.
//        
//    /// Returns the negative infinity
//    embeddable constructor negativeInfinity 
//        = -1.0r / 0.0r. 
//        
//    /// Returns the positive infinity 
//    embeddable constructor positiveInfinity 
//        = 1.0r / 0.0r.
//        
//    /// returns the NaN
//    embeddable constructor NaN
//        = 0.0r / 0.0r.
//
//    /// Creates the object with a default value (0)
//    constructor new
//        <= new(0.0r).
//
//    /// Creates the object with specified value
//    constructor new(real value)
//        = value.
//  
//    /// Creates the object with specified value
//    constructor new(int value)
//        = realConvertor convert(value).
//  
//    /// Creates the object with specified value
//    constructor new(uint value)
//        = realConvertor convert(value).
//
//    /// Creates the object with specified value
//    constructor new(short value)
//        = realConvertor convert(value).
//
//    /// Creates the object with specified value
//    constructor new(byte value)
//        = realConvertor convert(value).
//
//    /// Creates the object with specified value
//    constructor new(long value)
//        = realConvertor convert(value).
//        
//    /// Creates the object with specified value
//    constructor new : anObject
//        <= new(T<real>(anObject)).
//        
//    bool equal:o
//    [
//        var n := T<real>(o) ?? nil.
//        (nil == n)
//            ? [ ^ false ]
//            ! [ ^ self equal(n) ]
//    ]
//
//    bool less:n
//        <= less(T<real>(n)).
//        
//    bool equal(BaseVariable v)
//        <= equal(v value).
//        
//    bool less(BaseVariable v)
//        <= less(v value).
//        
//    /// Compares the value
//    bool equal(real anOperand)
//        = theValue == anOperand.
//        
//    /// Compares the value
//    bool less(real anOperand)
//        = theValue < anOperand.
//        
//    add:n
//        <= add(T<real>(n)).
//
//    subtract:n
//        <= subtract(T<real>(n)).
//
//    multiply:n
//        <= multiply(T<real>(n)).
//
//    divide:n
//        <= divide(T<real>(n)).
//        
//    real add(BaseVariable v)
//        <= add(v value).
//
//    real subtract(BaseVariable v)
//        <= subtract(v value).
//
//    real multiply(BaseVariable v)
//        <= multiply(v value).
//
//    real divide(BaseVariable v)
//        <= divide(v value).
//        
//    /// Returns the sum
//    real add(real anOperand)
//        = theValue + anOperand.
//
//    /// Returns the sum
//    real subtract(real anOperand)
//        = theValue - anOperand.
//
//    /// Returns the product
//    real multiply(real anOperand)
//        = theValue * anOperand.
//
//    /// Returns the result of integer division
//    real divide(real anOperand)
//        = theValue / anOperand.
//        
//    /// Compares the numeric values
//    bool equal(int anOperand)
//        = self equal(RealNumber new(anOperand)).
//
//    /// Compares the numeric values
//    bool equal(byte anOperand)
//        = self equal(RealNumber new(anOperand)).
//
//    /// Compares the numeric values
//    bool equal(short anOperand)
//        = self equal(RealNumber new(anOperand)).
//
//    /// Compares the numeric values
//    bool equal(long anOperand)
//        = self equal(RealNumber new(anOperand)).
//
//    /// Compares the numeric values
//    bool less(int anOperand)
//        = self less(RealNumber new(anOperand)).
//
//    /// Compares the numeric values
//    bool less(byte anOperand)
//        = self less(RealNumber new(anOperand)).
//
//    /// Compares the numeric values
//    bool less(short anOperand)
//        = self less(RealNumber new(anOperand)).
//
//    /// Compares the numeric values
//    bool less(long anOperand)
//        = self less(RealNumber new(anOperand)).
//
//    /// Returns the sum of numbers
//    real add(int anOperand)
//        = self add(RealNumber new(anOperand)).
//
//    /// Returns the sum of numbers
//    real add(byte anOperand)
//        = self add(RealNumber new(anOperand)).
//
//    /// Returns the sum of numbers
//    real add(short anOperand)
//        = self add(RealNumber new(anOperand)).
//
//    /// Returns the sum of numbers
//    real add(long anOperand)
//        = self add(RealNumber new(anOperand)).
//
//    /// Returns the difference of numbers
//    real subtract(int anOperand)
//        = self subtract(RealNumber new(anOperand)).
//
//    /// Returns the difference of numbers
//    real subtract(byte anOperand)
//        = self subtract(RealNumber new(anOperand)).
//
//    /// Returns the difference of numbers
//    real subtract(short anOperand)
//        = self subtract(RealNumber new(anOperand)).
//
//    /// Returns the difference of numbers
//    real subtract(long anOperand)
//        = self subtract(RealNumber new(anOperand)).
//
//    /// Returns the product of numbers
//    real multiply(int anOperand)
//    [
//        real n := anOperand.
//        
//        ^ self multiply(n).
//    ]
//
//    /// Returns the product of numbers
//    real multiply(byte anOperand)
//        = self multiply(RealNumber new(anOperand)).
//
//    /// Returns the product of numbers
//    real multiply(short anOperand)
//        = self multiply(RealNumber new(anOperand)).
//
//    /// Returns the product of numbers
//    real multiply(long anOperand)
//        = self multiply(RealNumber new(anOperand)).
//
//    /// Returns the result of the division
//    real divide(int anOperand)
//    [
//        real n := anOperand.
//        
//        ^ self divide(n)
//    ]
//
//    /// Returns the result of the division
//    real divide(byte anOperand)
//        = self divide(RealNumber new(anOperand)).
//        
//    /// Returns the result of the division
//    real divide(short anOperand)
//        = self divide(RealNumber new(anOperand)).
//
//    /// Returns the result of the division
//    real divide(long anOperand)
//        = self divide(RealNumber new(anOperand)).
//
//    /// Returns the literal presentation
//    T<literal> literal = literalConvertor convert(self).
//
////    /// Returns the literal presentation
////    wide = system'wideConvertor convert real:$self.
//
//    /// Clones the value    
//    clone = RealNumber new(self).
//    
//    // Reads the object hash code into the output parameter
//    readHashCodeTo(ref<int> aCode)
//        => system'core_routines'__int.
//    
//    embeddable int hash_code
//    [
//        int aCode := 0.
//        self readHashCodeTo(&aCode).
//        
//        ^ aCode
//    ]
//        
//    /// Negates the value
//    negative
//        = 0.0r - theValue.
//}
//
//// --- CharValue ---
//
///// An UTF-32 character symbol
//public sealed const struct CharValue :: BaseValue
//{
//    __dword theValue.
//
//    /// Creates the object with a default value (0)
//    constructor new
//        <= new($0).
//
//    /// Creates the object with specified value
//    constructor new(char value)
//        = value.
//
//    /// Creates the object with specified numeric value
//    constructor new(int anOperand)
//        <= new(charConvertor convert(anOperand)).
//
//    /// return the default value
//    constructor
//        = $0.
//
//    /// Returns the minimal possible character value (0)
//    constructor min
//        = $0.
//
//    /// Returns the maximal possible character value (010FFFFh)
//    constructor max
//        = $010FFFFh.
//
//    internal readValue(ref<int> retVal)
//    [
//        int val := theValue.
//        
//        retVal value := val.
//    ]
//
//    bool equal:o
//    [
//        var n := T<char>(o) ?? nil.
//        (nil == n)
//            ? [ ^ false ]
//            ! [ ^ self equal(n) ]
//    ]
//
//    bool less:n
//        <= less(T<char>(n)).
//        
//    bool equal(BaseVariable v)
//        <= equal(v value).
//        
//    bool less(BaseVariable v)
//        <= less(v value).
//        
//    bool equal(int val)
//        = theValue == val.
//
//    bool equal(literal val)
//        = val == T<literal>(self).
//
//    bool equal(wide val)
//        = val == T<wide>(self).
//
//    bool equal(char val)
//        = val == theValue.
//
//    bool less(char val)
//    [
//        int c := theValue.
//        
//        int v := 0.
//        val readValue(&v).        
//        
//        ^ c < v.
//    ]
//    
//    bool less(literal val)
//        = val < T<literal>(self).
//
//    bool less(wide val)
//        = val < T<wide>(self).
//
//    /// Clones the value
//    clone = CharValue new(self).
//
//    /// Returns the literal value
//    T<literal> literal = literalConvertor convert(self).
//
//    /// Returns the literal value
//    cast T<literal> = literalConvertor convert(self).
//    
//    /// Returns the wide literal value
//    cast T<wide> = wideConvertor convert(self).
//    
//    /// Reads the number of bytes to represent the symbol
//    readLengthTo(ref<int> aLength)
//        => system'core_routines'__char.
//
//    /// Reads the number of bytes to represent the symbol
//    readWideLengthTo(ref<int> aRetVal)
//    [
//        int aLength := 0.
//        self readLengthTo(&aLength).
//        
//        aLength := aLength + 1.
//        aLength := aLength / 2.
//
//        aRetVal value := aLength
//    ]
//        
//    /// Retuns the number of bytes to represent the symbol        
//    embeddable int length
//    [
//        int aLength := 0.
//        self readLengthTo(&aLength).
//        
//        ^ aLength
//    ]
//    
//    /// Retuns the number of short integers to represent the symbol
//    embeddable int wideLength
//    [
//        int aLength := 0.
//        self readWideLengthTo(&aLength).
//        
//        ^ aLength.
//    ]
//
//    // Reads the object hash code into the output parameter
//    readHashCodeTo(ref<int> aCode)
//        => system'core_routines'__int.
//    
//    embeddable int hash_code
//    [
//        int aCode := 0.
//        self readHashCodeTo(&aCode).
//        
//        ^ aCode.
//    ]
//}
//
//// --- nil constant ---
//
///// A nil value
//public singleton nilValue
//{
//    /// Returns true if the specified object is nil.
//    bool equal : anObject 
//    [
//        (nil == anObject)
//            ? [ ^ true. ].
//        
//        (super equal:anObject)
//            ? [ ^ true. ].
//
//        ^ false.
//    ]
//    
//    /// Returns nil
//    ///
//    /// Any typecasting message will be handled and nil value returned
//    generic = nil.
//}
//
//// === Basic Factory symbols ==
//
//public class BaseExtender
//{
//    object theObject.
//    
//    /// Returns true if the specified object is equal to the current object; otherwise, false.
//    bool equal : anObject
//        => theObject.
//
//    T<literal> literal
//        => theObject.
//                  
//    dispatch => theObject.
//}
//
///// A generic variable. 
/////
///// Extends an assigned value
//sealed public class Variable :: BaseVariable, ref<object>
//{
//    /// Creates the empty variable
//    constructor new
//    [
//    ]        
//        
//    /// Creates the variable
//    constructor new : value
//    [
//        theValue := value.
//    ]
//    
////    set : value
////    [
////        $value := value.
////    ]
//    
//    append : val
//    [
//        (nil == theValue)
//            ? [ theValue := val ]
//            ! [ theValue := theValue + val ]
//    ]
//    
//    reduce : val
//    [
//        (nil == theValue)
//            ? [ system'InvalidOperationException new; raise ]
//            ! [ theValue := theValue - val ]
//    ]
//    
////    multiplyBy : val
////    [
////        ($nil == $value)
////            ? [ system'InvalidOperationException new; raise ]
////            ! [ $value := $value * val ]
////    ]
////    
////    /// Divides into the value
////    divideInto : val
////    [
////        ($nil == $value)
////            ? [ system'InvalidOperationException new; raise ]
////            ! [ $value := $value / val ]
////    ]    
//}
//
//// --- KeyValue ---
//
///// A key - object pair
/////
///// Extends the object with a key property
//public sealed class KeyValue
//{
//    object theKey.
//    object theObject.
//    
//    /// Creates a new objext
//    constructor new(object aKey, object anObject)
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
//    T<literal> literal
//        => theObject.                                                                           
//                                                                                      
//    /// Redirects the incoming messages to the object
//    dispatch => theObject.
//}
//
//// === Basic Convertors ===
//
//// --- convertor helpers ---
//
///// A byte convertor
//public class byteConvertor =
//{
//    /// Converts a character into a byte value and saves the result into an output parameter
//    convert(char aValue, ref<byte> aRetVal)
//        => system'core_routines'__byteconvertor.
//        
//    /// Converts a short integer into a byte value and saves the result into an output parameter
//    byte convert(short aValue, ref<byte> aRetVal)
//        => system'core_routines'__byteconvertor.
//        
//    /// Converts an integer into a byte value and saves the result into an output parameter
//    byte convert(int aValue, ref<byte> aRetVal)
//        => system'core_routines'__byteconvertor.
//
//    /// Converts an integer into a byte value and saves the result into an output parameter
//    byte convert(uint aValue, ref<byte> aRetVal)
//        => system'core_routines'__byteconvertor.
//
//    /// Converts a long integer into a byte value and saves the result into an output parameter
//    convert(long aValue, ref<byte> aRetVal)
//        => system'core_routines'__byteconvertor.
//        
//    /// Converts a real number into a byte value and saves the result into an output parameter
//    convert(real aValue, ref<byte> aRetVal)
//        => system'core_routines'__byteconvertor.
//        
//    /// Converts a literal value into a byte value and saves the result into an output parameter
//    convert(literal aValue, ref<byte> aRetVal)
//        => system'core_routines'__byteconvertor.
//
//    /// Converts a wide literal value into a byte value and saves the result into an output parameter
//    convert(wide aValue, ref<byte> aRetVal)
//        => system'core_routines'__byteconvertor.
//        
//    byte convert(BaseVariable v)
//        <= convert(v value).
//                        
//    /// Returns the parameter        
//    byte convert(byte anInt)
//        = ByteNumber new(anInt).
//        
//    /// Converts a character into a byte value
//    byte convert(char aValue)
//    [
//        byte aRetVal := 0.
//        self convert(aValue, &aRetVal).
//        
//        ^ ByteNumber new(aRetVal)
//    ]
//
//    /// Converts a short integer into a byte value
//    byte convert(short aValue)
//    [
//        byte aRetVal := 0.
//        self convert(aValue, &aRetVal).
//        
//        ^ ByteNumber new(aRetVal)
//    ]
//        
//    /// Converts an integer into a byte value
//    byte convert(int aValue)
//    [
//        byte aRetVal := 0.
//        self convert(aValue, &aRetVal).
//        
//        ^ ByteNumber new(aRetVal)
//    ]
//        
//    /// Converts an integer into a byte value
//    byte convert(uint aValue)
//    [
//        byte aRetVal := 0.
//        self convert(aValue, &aRetVal).
//        
//        ^ ByteNumber new(aRetVal)
//    ]
//
//    /// Converts a long integer into a byte value
//    byte convert(long aValue)
//    [
//        byte aRetVal := 0.
//        self convert(aValue, &aRetVal).
//        
//        ^ ByteNumber new(aRetVal)
//    ]
//
//    /// Converts a real number into a byte value
//    byte convert(real aValue)
//    [
//        byte aRetVal := 0.
//        self convert(aValue, &aRetVal).
//        
//        ^ ByteNumber new(aRetVal)
//    ]
//
//    /// Converts a literal value into a byte value
//    byte convert(literal aLiteral)
//    [
//        byte aValue := 0.
//        self convert(aLiteral, &aValue).
//        
//        ^ ByteNumber new(aValue)
//    ]
//                
//    /// Converts a wide literal value into a byte value
//    byte convert(wide aLiteral)
//    [
//        byte aValue := 0. 
//        self convert(aLiteral, &aValue).
//        
//        ^ ByteNumber new(aValue)
//    ]
//
//    byte convert : anObject
//        = T<byte>(anObject).
//}.
//
///// A 16bit integer convertor
//public class shortConvertor =
//{
//    /// Converts a character into a short integer value and saves the result into an output parameter
//    convert(char aValue, ref<short> aRetVal)
//        => system'core_routines'__shortconvertor.
//        
//    /// Converts a byte value into a short integer value and saves the result into an output parameter
//    convert(byte aValue, ref<short> aRetVal)
//        => system'core_routines'__shortconvertor.
//        
//    /// Converts an integer into a short integer value and saves the result into an output parameter
//    convert(int aValue, ref<short> aRetVal)
//        => system'core_routines'__shortconvertor.
//        
//    /// Converts an integer into a short integer value and saves the result into an output parameter
//    convert(uint aValue, ref<short> aRetVal)
//        => system'core_routines'__shortconvertor.
//        
//    /// Converts a long integer into a short integer value and saves the result into an output parameter
//    convert(long aValue, ref<short> aRetVal)
//        => system'core_routines'__shortconvertor.
//        
//    /// Converts a real number into a short integer value and saves the result into an output parameter
//    convert(real aValue, ref<short> aRetVal)
//        => system'core_routines'__shortconvertor.
//
//    /// Converts a literal string into a short integer value and saves the result into an output parameter
//    convert(literal aValue, ref<short> aRetVal)
//        => system'core_routines'__shortconvertor.
//        
//    /// Converts a wide literal string into a short integer value and saves the result into an output parameter
//    convert(wide aValue, ref<short> aRetVal)
//        => system'core_routines'__shortconvertor.
//        
//    short convert(BaseVariable v)
//        <= convert(v value).
//                        
//    /// Returns a short integer value        
//    short convert(short anInt)
//        = ShortNumber new(anInt).
//        
//    /// Converts a byte value into a short integer value
//    short convert(byte aValue)
//    [
//        short aRetVal := 0.
//        self convert(aValue, &aRetVal).
//        
//        ^ ShortNumber new(aRetVal)
//    ]
//
//    /// Converts an integer into a short integer value
//    short convert(int aValue)
//    [
//        short aRetVal := 0.
//        self convert(aValue, &aRetVal).
//        
//        ^ ShortNumber new(aRetVal)
//    ]
//
//    /// Converts an integer into a short integer value
//    short convert(uint aValue)
//    [
//        short aRetVal := 0.
//        self convert(aValue, &aRetVal).
//        
//        ^ ShortNumber new(aRetVal)
//    ]
//
//    /// Converts a long integer into a short integer value
//    short convert(long aValue)
//    [
//        short aRetVal := 0.
//        self convert(aValue, &aRetVal).
//        
//        ^ ShortNumber new(aRetVal)
//    ]
//        
//    /// Converts a real number into a short integer value
//    short convert(real aValue)
//    [
//        short aRetVal := 0.
//        self convert(aValue, &aRetVal).
//        
//        ^ ShortNumber new(aRetVal)
//    ]
//        
//    /// Converts a character into a short integer value
//    short convert(char aValue)
//    [
//        short aRetVal := 0.
//        self convert(aValue, &aRetVal).
//        
//        ^ ShortNumber new(aRetVal)
//    ]
//        
//    /// Converts a literal string into a short integer value
//    short convert(literal aValue)
//    [
//        short aRetVal := 0.
//        self convert(aValue, &aRetVal).
//        
//        ^ ShortNumber new(aRetVal)
//    ]
//        
//    /// Converts a wide literal string into a short integer value
//    short convert(wide aValue)
//    [
//        short aRetVal := 0.
//        self convert(aValue, &aRetVal).
//        
//        ^ ShortNumber new(aRetVal)
//    ]
//        
//    short convert : anObject
//        = T<short>(anObject).
//}.
//
///// An Unicode character convertor
//public class charConvertor =
//{
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
//        
//    /// Returns a parameter        
//    char convert(char aChar)
//        = CharValue new(aChar).
//  
//    internal convert(int anIndex, literal aLiteral, ref<char> char)
//        => system'core_routines'__charconvertor.
//        
//    internal convert(int anIndex, ByteArray aLiteral, ref<char> char)
//        => system'core_routines'__charconvertor.
//  
//    internal convert(int anIndex, wide aLiteral, ref<char> char)
//        => system'core_routines'__charconvertor.
//
//    internal convert(int anIndex, ShortArray aLiteral, ref<char> char)
//        => system'core_routines'__charconvertor.
//
//    internal save(char aChar, int anIndex, ShortArray aCharArray, ref<int> aLength)
//        => system'core_routines'__charconvertor.
//    
//    internal save(char aChar, int anIndex, ByteArray aCharArray, ref<int> aLength)
//        => system'core_routines'__charconvertor.
//
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
//        
//    char convert : anObject
//        = T<char>(anObject).
//}.
//
///// An integer convertor
//public class intConvertor =
//{
//    /// Converts a literal string into an integer and saves the result into an output parameter
//    convert(literal aValue, ref<int> aRetVal)
//        => system'core_routines'__intconvertor.
//
//    /// Converts a literal string into an integer and saves the result into an output parameter
//    convertUnsigned(literal aValue, ref<int> aRetVal)
//        => system'core_routines'__intconvertor.
//
//    /// Converts a literal string into an integer and saves the result into an output parameter
//    convert(literal value, int base, ref<int> retVal)
//        => system'core_routines'__intconvertor.
//
//    /// Converts a wide literal string into an integer and saves the result into an output parameter
//    convert(wide aValue, ref<int> aRetVal)
//        => system'core_routines'__intconvertor.
//
//    /// Converts a wide literal string into an integer and saves the result into an output parameter
//    convert(wide value, int base, ref<int> retVal)
//        => system'core_routines'__intconvertor.
//
//    /// Converts a character value into an integer and saves the result into an output parameter
//    convert(char aValue, ref<int> aRetVal)
//        => system'core_routines'__intconvertor.
//
//    /// Converts a short integer value into an integer and saves the result into an output parameter
//    convert(short aValue, ref<int> aRetVal)
//        => system'core_routines'__intconvertor.
//
//    /// Converts a byte value into an integer and saves the result into an output parameter
//    convert(byte aValue, ref<int> aRetVal)
//        => system'core_routines'__intconvertor.
//
//    /// Converts a long integer into an integer and saves the result into an output parameter
//    convert(long aValue, ref<int> aRetVal)
//        => system'core_routines'__intconvertor.
//        
//    /// Converts a real number into an integer and saves the result into an output parameter
//    convert(real aReal, ref<int> aRetVal)
//        => system'core_routines'__intconvertor.
//
//    int convert(BaseVariable v)
//        <= convert(v value).
//        
//    /// Converts a literal string into an integer
//    int convert(literal aLiteral)
//    [
//        int aRetVal := 0.
//        self convert(aLiteral, &aRetVal).
//        
//        ^ IntNumber new(aRetVal)
//    ]
//        
//    /// Converts a literal string into an integer
//    int convertUnsigned(literal aLiteral)
//    [
//        int aRetVal := 0.
//        self convertUnsigned(aLiteral, &aRetVal).
//        
//        ^ IntNumber new(aRetVal)
//    ]
//        
//    /// Converts a literal string into an integer
//    int convert(literal literal, int base)
//    [
//        int retVal := 0.
//        self convert(literal, base, &retVal).
//        
//        ^ IntNumber new(retVal)
//    ]
//        
//    /// Converts a wide literal string into an integer
//    int convert(wide aLiteral)
//    [
//        int aRetVal := 0.
//        self convert(aLiteral, &aRetVal).
//        
//        ^ IntNumber new(aRetVal)
//    ]
//
//    /// Converts a wide literal string into an integer
//    int convert(wide literal, int base)
//    [
//        int retVal := 0.
//        self convert(literal, base, &retVal).
//        
//        ^ IntNumber new(retVal)
//    ]
//        
//    /// Converts a character value into an integer
//    int convert(char aValue)
//    [
//        int aRetVal := 0.
//        self convert(aValue, &aRetVal).
//        
//        ^ IntNumber new(aRetVal)
//    ]
//        
//    /// Converts a byte value into an integer
//    int convert(byte aValue)
//    [
//        int aRetVal := 0.
//        self convert(aValue, &aRetVal).
//        
//        ^ IntNumber new(aRetVal)
//    ]
//        
//    /// Converts a short integer value into an integer
//    int convert(short aValue)
//    [
//        int aRetVal := 0.
//        self convert(aValue, &aRetVal).
//        
//        ^ IntNumber new(aRetVal)
//    ]
//        
//    /// Converts a long integer value into an integer and saves the result into an output parameter
//    int convert(long aValue)
//    [
//        int aRetVal := 0.
//        self convert(aValue, &aRetVal).
//        
//        ^ IntNumber new(aRetVal)
//    ]
//
//    /// Converts a long integer into an integer
//    int convert(real aReal)
//    [
//        int aRetVal := 0.
//        self convert(aReal, &aRetVal).
//        
//        ^ IntNumber new(aRetVal)
//    ]
//
//    /// Returns a parameter                                    
//    int convert(int anInt)
//        = IntNumber new(anInt).
//
//    int convert(uint anInt)
//        = IntNumber new(anInt).
//                        
//    int convert : anObject
//        = T<int>(anObject).
//}.
//
///// A unsigned integer convertor
//public class uintConvertor =
//{
//    uint convert : object
//        = T<uint>(object).
//    
//    uint convert(int val)
//        = UIntNumber new(val).
//        
//    /// Converts a long integer into an integer and saves the result into an output parameter
//    convert(long aValue, ref<int> aRetVal)
//        => system'core_routines'__uintconvertor.
//        
//    /// Converts a long integer value into an integer and saves the result into an output parameter
//    uint convert(long aValue)
//    [
//        int aRetVal := 0.
//        self convert(aValue, &aRetVal).
//        
//        ^ UIntNumber new(aRetVal)
//    ]        
//}.
//
///// A long integer convertor
//public class longConvertor =
//{
//    /// Converts a literal string into a long integer and saves the result into an output parameter
//    convert(literal aValue, ref<long> aRetVal)
//        => system'core_routines'__longconvertor.
//
//    /// Converts a literal string into a long integer and saves the result into an output parameter
//    convert(literal value, int base, ref<long> retVal)
//        => system'core_routines'__longconvertor.
//
//    /// Converts a wide literal string into a long integer and saves the result into an output parameter
//    convert(wide aValue, ref<long> aRetVal)
//        => system'core_routines'__longconvertor.
//
//    /// Converts a wide literal string into a long integer and saves the result into an output parameter
//    convert(wide value, int base, ref<long> retVal)
//        => system'core_routines'__longconvertor.
//
//    convert(int aValue, ref<long> aRetVal)
//        => system'core_routines'__longconvertor.
//        
//    convert(uint aValue, ref<long> aRetVal)
//        => system'core_routines'__longconvertor.
//        
//    convert(short aValue, ref<long> aRetVal)
//        => system'core_routines'__longconvertor.
//
//    convert(byte aValue, ref<long> aRetVal)
//        => system'core_routines'__longconvertor.
//
//    /// Converts a real number into a long integer and saves the result into an output parameter
//    convert(real aValue, ref<long> aRetVal)
//        => system'core_routines'__longconvertor.
//
//    long convert(BaseVariable v)
//        <= convert(v value).
//        
//    /// Converts an integer value into a long integer
//    long convert(int aValue)
//    [
//        long n := 0l.
//        self convert(aValue, &n).
//        
//        ^ LongNumber new(n)
//    ]
//
//    /// Converts an integer value into a long integer
//    long convert(uint aValue)
//    [
//        long n := 0l.
//        self convert(aValue, &n).
//        
//        ^ LongNumber new(n)
//    ]
//
//    /// Converts an integer value into a long integer
//    long convert(byte aValue)
//    [
//        long n := 0l.
//        self convert(aValue, &n).
//        
//        ^ LongNumber new(n)
//    ]
//
//    /// Converts an integer value into a long integer
//    long convert(short aValue)
//    [
//        long n := 0l.
//        self convert(aValue, &n).
//        
//        ^ LongNumber new(n)
//    ]
//
//    /// Converts a real number into a long integer
//    long convert(real aValue)
//    [
//        long aRetVal := 0l.
//        self convert(aValue, &aRetVal).
//        
//        ^ LongNumber new(aRetVal)
//    ]
//
//    /// Converts a character into a long integer
//    long convert(char aValue)
//    [
//        int aRetVal := 0.
//        intConvertor convert(aValue, &aRetVal).
//        
//        ^ LongNumber new(aRetVal)
//    ]
//
//    /// Converts a literal string into a long integer
//    long convert(literal aLiteral)
//    [
//        long aRetVal := 0l.
//        self convert(aLiteral, &aRetVal).
//        
//        ^ LongNumber new(aRetVal)
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
//    long convert(wide aLiteral)
//    [
//        long aRetVal := 0l.
//        self convert(aLiteral, &aRetVal).
//        
//        ^ LongNumber new(aRetVal)
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
//    long convert(long aLong)
//        = LongNumber new(aLong).
//        
//    long convert : anObject
//        = T<long>(anObject).
//}.
//
///// A real number convertor
//public class realConvertor =
//{
//    /// Converts an integer into a real number and saves the result into an output parameter
//    convert(int aValue, ref<real> aRetVal)
//        => system'core_routines'__realconvertor.
//
//    /// Converts an integer into a real number and saves the result into an output parameter
//    convert(uint aValue, ref<real> aRetVal)
//    [
//        (aValue < 0)?
//        [
//            // If the number is bigger than 2^31
//            // than we are unlucky
//            int odd := aValue && 1.            
//            int halfed := aValue >> 1.
//            
//            real r := 0.
//            self convert(halfed, &r).
//            r := r * 2.0r.
//            r := r + odd.
//            
//            aRetVal value := r.
//        ]
//        ! [ self convert(aValue, &aRetVal). ]
//    ]        
//    //    => system'core_routines'__realconvertor.
//
//    /// Converts an integer into a real number and saves the result into an output parameter
//    convert(long aValue, ref<real> aRetVal)
//        => system'core_routines'__realconvertor.
//
//    /// Converts an integer into a real number and saves the result into an output parameter
//    convert(byte aValue, ref<real> aRetVal)
//        => system'core_routines'__realconvertor.
//
//    /// Converts an integer into a real number and saves the result into an output parameter
//    convert(short aValue, ref<real> aRetVal)
//        => system'core_routines'__realconvertor.
//
//    /// Converts a literal string into a real number and saves the result into an output parameter
//    convert(literal aValue, ref<real> aRetVal)
//        => system'core_routines'__realconvertor.
//
//    /// Converts a wide literal string into a real number and saves the result into an output parameter
//    convert(wide aValue, ref<real> aRetVal)
//        => system'core_routines'__realconvertor.
//        
//    real convert(BaseVariable v)
//        <= convert(v value).
//        
//    /// Converts a literal string into a real number
//    real convert(literal aLiteral)
//    [
//        real aRetVal := 0.0r.
//        self convert(aLiteral, &aRetVal).
//        
//        ^ RealNumber new(aRetVal)
//    ]
//        
//    /// Converts a wide literal string into a real number
//    real convert(wide aLiteral)
//    [
//        real aRetVal := 0.0r.
//        self convert(aLiteral, &aRetVal).
//        
//        ^ RealNumber new(aRetVal)
//    ]
//
//    /// Returns a parameter
//    real convert(real aReal)
//        = RealNumber new(aReal).
//        
//    /// Converts an integer into a real number
//    real convert(int anInt)
//    [
//        real aRetVal := 0.0r.
//        self convert(anInt, &aRetVal).
//        
//        ^ RealNumber new(aRetVal)
//    ]
//        
//    /// Converts an integer into a real number
//    real convert(uint anInt)
//    [
//        real aRetVal := 0.0r.
//        self convert(anInt, &aRetVal).
//        
//        ^ RealNumber new(aRetVal)
//    ]
//
//    /// Converts a long integer into a real number
//    real convert(long aLong)
//    [
//        real aRetVal := 0.0r.
//        self convert(aLong, &aRetVal).
//        
//        ^ RealNumber new(aRetVal)
//    ]
//
//    /// Converts a byte into a real number
//    real convert(byte aValue)
//    [
//        real aRetVal := 0.0r.
//        self convert(aValue, &aRetVal).
//        
//        ^ RealNumber new(aRetVal)
//    ]
//
//    /// Converts a short integer into a real number
//    real convert(short aValue)
//    [
//        real aRetVal := 0.0r.
//        self convert(aValue, &aRetVal).
//        
//        ^ RealNumber new(aRetVal)
//    ]
//        
//    real convert : o
//        = T<real>(o).
//}.
//
//// --- realOpController ---
//
//public class realOpController =
//{
//    exp(real aReal, ref<real> aRetVal)
//        => system'core_routines'__real.
//        
//    abs(real aReal, ref<real> aRetVal)
//        => system'core_routines'__real.
//        
//    ln(real aReal, ref<real> aRetVal)
//        => system'core_routines'__real.
//        
//    round(real aReal, ref<real> aRetVal)
//        => system'core_routines'__real.
//        
//    truncate(real aReal, ref<real> aRetVal)
//        => system'core_routines'__real.
//        
//    sin(real aReal, ref<real> aRetVal)
//        => system'core_routines'__real.
//        
//    cos(real aReal, ref<real> aRetVal)
//        => system'core_routines'__real.
//        
//    arctan(real aReal, ref<real> aRetVal)
//        => system'core_routines'__real.
//                
//    sqrt(real aReal, ref<real> aRetVal)
//        => system'core_routines'__real.
//        
//    pi(ref<real> aRetVal)
//        => system'core_routines'__real.
//        
//    embeddable real pi
//    [
//        real aRetVal := 0.0r.
//        self pi(&aRetVal).
//        
//        ^ aRetVal
//    ]        
//    
//    embeddable real e
//    [
//        real aRetVal := 0.0r.
//        self exp(1.0r, &aRetVal).
//        
//        ^ aRetVal
//    ]        
//}.
//
//real public pi_value = realOpController pi.
//
//real public e_value = realOpController e.
//
//// --- variables ---
//
//public sealed class Integer :: BaseVariable, ref<int>
//{
//    constructor new
//    [
//        theValue := 0.
//    ]
//    
//    constructor new(int n) 
//    [
//        theValue := n.
//    ]
//    
//    constructor new : n
//        <= new(T<int>(n)).
//        
//    append : n
//        <= append(T<int>(n)).
//
//    reduce : n
//        <= reduce(T<int>(n)).
//        
//    append(int n)
//    [
//        int v := theValue.
//        v := v + n.
//        
//        theValue := v.
//    ]
//    
//    reduce(int n)
//    [
//        int v := theValue.
//        v := v - n.
//        
//        theValue := v.
//    ]
//}
//
//public sealed class LongInteger :: BaseVariable, ref<long>
//{
//    constructor new
//    [
//        theValue := 0l.
//    ]
//    
//    constructor new(long n) 
//    [
//        theValue := n.
//    ]
//    
//    constructor new : n
//        <= new(T<long>(n)).
//        
//    append : n
//        <= append(T<long>(n)).
//
//    reduce : n
//        <= reduce(T<long>(n)).
//                
//    append(long n)
//    [
//        long v := theValue.
//        v := v + n.
//        
//        theValue := v.
//    ]
//    
//    reduce(long n)
//    [
//        long v := theValue.
//        v := v - n.
//        
//        theValue := v.
//    ]
//}
//
//public sealed class Real :: BaseVariable, ref<real>
//{
//    constructor new
//    [
//        theValue := 0.0r.
//    ]
//    
//    constructor new(real n) 
//    [
//        theValue := n.
//    ]
//    
//    constructor new : n
//        <= new(T<real>(n)).
//        
//    append(real n)
//    [
//        real v := theValue.
//        v := v + n.
//        
//        theValue := v.
//    ]
//    
//    append : n
//        <= append(T<real>(n)).
//    
//    reduce(real n)
//    [
//        real v := theValue.
//        v := v - n.
//        
//        theValue := v.
//    ]
//    
//    reduce : n
//        <= reduce(T<real>(n)).
//}

// === basic type extensions ===

public extension intRefOp : ref<int>
{
    append(int n)
    {
        int v := self.Value;
        v := v + n;
        
        self.Value := v
    }
    
    reduce(int n)
    {
        int v := self.Value;
        v := v - n;
        
        self.Value := v
    }
}