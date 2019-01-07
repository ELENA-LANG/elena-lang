// base variable mixin
public abstract class BaseVariable
{
    // should be overridden
    abstract get Value() {}
    
    abstract set Value(o) {}
}

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

    bool equal(o)
        => theValue;

    string Printable
//        => theValue;
        = theValue.Printable;
        
    dispatch() => theValue;
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

    /// Always returns nil.
    ifnot(trueAction) = nil;

    iif(object trueVal, object falseVal) = trueVal;
    
    /// Executes logical OR operation
    ///
    /// Always return true
    bool or(expr) = self;

    /// Executes logical AND operation
    bool and(expr) = expr;

    /// Executes logical XOR operation
    bool xor(expr)
    {
        cast bool(expr) ? { ^ false };
        
        ^ self
    }

    /// Determines whether the specified object is equal to the current object boolean value.
    bool equal(expr) = expr;

    /// Determines whether the specified object is equal to the current object boolean value.
    bool equal(bool b) = b;

    /// Always returns false.
    bool Inverted = false;

    /// Returns literal representation.
    string Printable = "true";

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

    /// Executes the action
    ifnot(action) = action();

    iif(object trueVal, object falseVal) = falseVal;

    /// Executes logical OR operation
    bool or(expr) = expr;

    /// Executes logical AND operation
    ///
    /// Always return false
    bool and(expr) = self;

    /// Executes logical XOR operation
    bool xor(expr)
    {
        cast bool(expr) ? { ^ true };
        
        ^ self
    }

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

    /// Returns literal representation
    string Printable = "false";
    
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

    /// Compares the values
    bool greater(o) = o.less(self);

    bool notless(o) = self.less(o).Inverted;

    bool notgreater(o) = self.greater(o).Inverted;
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

    constructor(int n)
    {
        theValue := n && 255
    }

    /// Creates the object with a default value (0)
    static byte Default
        = 0;

    /// return the default value
    constructor()
        = 0;

    /// Returns the minimal value (0)
    static byte MinValue
        = 0;

    /// Returns the maxial value (255)
    static byte MaxValue
        = 255;
    
    /// Creates the object with a specified value
    constructor(byte value)
        = value;

//    /// Creates the object with a specified value
//    constructor new : val
//        <= new(T<byte>(val)).

    /// Returns the value as a short integer
    short cast() = new ShortNumber(self);

    /// Returns the value as an integer
    int cast() = new IntNumber(self);

    /// Returns the value as an integer
    uint cast() = new UIntNumber(self);

    /// Returns the value as a long integer
    long cast() = new LongNumber(self);

    /// Returns the value as a real number
    real cast() = new RealNumber(self);

//    bool equal(BaseVariable v)
//        <= equal(v value).

    bool equal(byte n)
        = theValue == n;

    bool equal(short n)
        = new ShortNumber(self).equal(n);
        
    /// Compares the value
    bool equal(int n)
        = new IntNumber(self).less(n);
        
    /// Compares the value
    bool equal(real n)
        = new RealNumber(self).equal(n);
        
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

    bool less(byte n)
        = theValue < n;

    bool less(short n)
        = new ShortNumber(self).less(n);

    /// Compares the value
    bool less(int n)
        = new IntNumber(self).less(n);
        
    /// Compares the value
    bool equal(long n)
        = new LongNumber(self).equal(n);
        
    /// Compares the value
    bool less(long n)
        = new LongNumber(self).less(n);

    /// Compares the value
    bool less(real n)
        = new RealNumber(self).less(n);

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

    /// Returns the sum
    byte add(byte n)
        = theValue + n;

    /// Returns the sum
    byte subtract(byte n)
        = theValue - n;

    /// Returns the product
    byte multiply(byte n)
        = theValue * n;

    /// Returns the result of integer division
    byte divide(byte n)
        = theValue / n;

    /// Returns the sum
    short add(short n)
        = new ShortNumber(self).add(n);

    /// Returns the difference
    short subtract(short n)
        = new ShortNumber(self).subtract(n);

    /// Returns the product
    short multiply(short n)
        = new ShortNumber(self).multiply(n);

    /// Returns the result of integer division
    short divide(short n)
        = new ShortNumber(self).divide(n);

    /// Returns the sum
    int add(int n)
        = new IntNumber(self).add(n);

    /// Returns the difference
    int subtract(int n)
        = new IntNumber(self).subtract(n);

    /// Returns the product
    int multiply(int n)
        = new IntNumber(self).multiply(n);

    /// Returns the result of integer division
    int divide(int n)
        = new IntNumber(self).divide(n);

    /// Returns the sum
    long add(long n)
        = new LongNumber(self).add(n);

    /// Returns the difference
    long subtract(long n)
        = new LongNumber(self).subtract(n);

    /// Returns the product
    long multiply(long n)
        = new LongNumber(self).multiply(n);

    /// Returns the result of integer division
    long divide(long n)
        = new LongNumber(self).divide(n);

    /// Returns the sum
    real add(real n)
        = new RealNumber(self).add(n);

    /// Returns the difference
    real subtract(real n)
        = new RealNumber(self).subtract(n);

    /// Returns the product
    real multiply(real n)
        = new RealNumber(self).multiply(n);

    /// Returns the result of integer division
    real divide(real n)
        = new RealNumber(self).divide(n);

//    and:n
//        <= and(T<byte>(n)).
//        
//    or:n
//        <= or(T<byte>(n)).
//        
//    xor:n
//        <= xor(T<byte>(n)).
        
//    byte and(BaseVariable v)
//        <= and(v value).
//        
//    byte or(BaseVariable v)
//        <= or(v value).
//        
//    byte xor(BaseVariable v)
//        <= xor(v value).
        
    /// Returns the result of bitwise AND operation
    byte and(byte n)
        = theValue && n;

    /// Returns the result of bitwise OR operation
    byte or(byte n)
        = theValue || n;

    /// Returns the result of bitwise XOR operation
    byte xor(byte n)
        = theValue ^^ n;

    /// Returns the result of bitwise AND operation
    short and(short n)
        = new ShortNumber(self).and(n);

    /// Returns the result of bitwise OR operation
    short or(short n)
        = new ShortNumber(self).or(n);

    /// Returns the result of bitwise XOR operation
    short xor(short n)
        = new ShortNumber(self).xor(n);

    /// Returns the result of bitwise AND operation
    int and(int n)
        = new IntNumber(self).and(n);

    /// Returns the result of bitwise OR operation
    int or(int n)
        = new IntNumber(self).or(n);

    /// Returns the result of bitwise XOR operation
    int xor(int n)
        = new IntNumber(self).xor(n);

    /// Returns the result of bitwise AND operation
    long and(long n)
        = new LongNumber(self).and(n);

    /// Returns the result of bitwise OR operation
    long or(long n)
        = new LongNumber(self).or(n);

    /// Returns the result of bitwise XOR operation
    long xor(long n)
        = new LongNumber(self).xor(n);

//    byte shiftLeft(int val)
//    {
//        ^ theValue $shl val;
//    }
//
//    byte shiftRight(int val)
//    {
//        ^ theValue $shr val;
//    }
    
//    /// Bitwise inversion
//    byte inverted
//        => system'core_routines'__byte.
        
    /// Returns the literal presentation
    string Printable = stringConvertor.convert(self);
        
    /// Clones the value
    byte clone() = new ByteNumber(self);
}

// --- ShortNumber ---

/// A signed 16 bit integer
public sealed const struct ShortNumber : IntBaseNumber
{
    embeddable __int theValue[2];

    constructor(int n)
    {
        (n < 0)
            ? {
                theValue := n && 0FFFFh;
                
                theValue := theValue || 0FFFF0000h
            }
            : { theValue := n && 0FFFFh }
    }
    
    /// returns the default value
    static short Default
        = 0;
    
    /// Returns the minimal value (-32768)
    static short MinValue
        = -32768;

    /// Returns the maxial value (32767)
    static short MaxValue
        = 32767;

    /// Creates the object with specified value
    constructor(byte value)
        = shortConvertor.convert(value);
        
    /// Creates the object with specified value
    constructor(short value)
        = value;

    /// Creates the object with a default value (0)
    constructor()
        = 0;

//    /// Creates the object with specified value
//    constructor new : anObject
//        <= new(T<short>(anObject)).

    /// Returns the byte value        
    byte cast() = byteConvertor.convert(self);
    
    /// Returns the integer value
    int cast() = new IntNumber(self);
    
    /// Returns the integer value
    uint cast() = new UIntNumber(self);
    
    /// Returns the long integer value
    long cast() = new LongNumber(self);
    
    /// Returns the value as a real number
    real cast() = new RealNumber(self);

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

    /// Compares the value
    bool equal(short n)
        = theValue == n;
        
    /// Compares the value
    bool less(short n)
        = theValue < n;
        
    /// Compares the value
    bool equal(int n)
        = new IntNumber(self).equal(n);
        
    /// Compares the value
    bool less(int n)
        = new IntNumber(self).less(n);
    
    /// Compares the value
    bool equal(byte n)
        = self.equal(new ShortNumber(n));
        
    /// Compares the value
    bool less (byte n)
        = self.less(new ShortNumber(n));
    
    /// Compares the value
    bool equal(long n)
        = new LongNumber(self).equal(n);
        
    /// Compares the value
    bool less(long n)
        = new LongNumber(self).less(n);
    
    /// Compares the value
    bool equal(real n)
        = new RealNumber(self).equal(n);
        
    /// Compares the value
    bool less(real n)
        = new RealNumber(self).less(n);

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

    /// Returns the sum
    short add(short n)
        = theValue + n;

    /// Returns the sum
    short subtract(short n)
        = theValue - n;

    /// Returns the product
    short multiply(short n)
        = theValue * n;

    /// Returns the result of integer division
    short divide(short n)
        = theValue / n;

    /// Returns the sum
    add(byte n)
        = self.add(new ShortNumber(n));

    /// Returns the difference
    subtract(byte n)
        = self.subtract(new ShortNumber(n));

    /// Returns the product
    multiply(byte n)
        = self.multiply(new ShortNumber(n));

    /// Returns the result of integer division
    divide(byte n)
        = self.divide(new ShortNumber(n));

    /// Returns the sum
    add(int n)
        = new IntNumber(self).add(n);

    /// Returns the difference
    subtract(int n)
        = new IntNumber(self).subtract(n);

    /// Returns the product
    multiply(int n)
        = new IntNumber(self).multiply(n);
        
    /// Returns the result of integer division
    divide(int n)
        = new IntNumber(self).divide(n);

    /// Returns the sum
    add(long n)
        = new LongNumber(self).add(n);

    /// Returns the difference
    subtract(long n)
        = new LongNumber(self).subtract(n);

    /// Returns the product
    multiply(long n)
        = new LongNumber(self).multiply(n);
        
    /// Returns the result of integer division
    divide(long n)
        = new LongNumber(self).divide(n);

    /// Returns the sum
    add(real n)
        = new RealNumber(self).add(n);

    /// Returns the difference
    subtract(real n)
        = new RealNumber(self).subtract(n);

    /// Returns the product
    multipl(real n)
        = new RealNumber(self).multiply(n);
        
    /// Returns the result of integer division
    divide(real n)
        = new RealNumber(self).divide(n);

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
        
    /// Returns the result of bitwise AND operation
    short and(short n)
        = theValue && n;

    /// Returns the result of bitwise OR operation
    short or(short n)
        = theValue || n;

    /// Returns the result of bitwise XOR operation
    short xor(short n)
        = theValue ^^ n;
        
    /// Returns the result of bitwise AND operation
    and(byte n)
        = self.and(new ShortNumber(n));

    /// Returns the result of bitwise OR operation
    or(byte n)
        = self.or(new ShortNumber(n));

    /// Returns the result of bitwise XOR operation
    xor(byte n)
        = self.xor(new ShortNumber(n));
        
    /// Returns the result of bitwise AND operation
    and(int n)
        = new IntNumber(self).and(n);
        
    /// Returns the result of bitwise OR operation
    or(int n)
        = new IntNumber(self).or(n);
        
    /// Returns the result of bitwise XOR operation
    xor(int n)
        = new IntNumber(self).xor(n);
        
    /// Returns the result of bitwise AND operation
    and(long n)
        = new LongNumber(self).and(n);
        
    /// Returns the result of bitwise OR operation
    or(long n)
        = new LongNumber(self).or(n);
        
    /// Returns the result of bitwise XOR operation
    xor(long n)
        = new LongNumber(self).xor(n);

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

    /// Returns the literal presentation
    string Printable = stringConvertor.convert(self);

//    /// Returns the literal presentation
//    wide = system'wideConvertor convert short:$self.

    /// Clones the value
    short clone = new ShortNumber(self);
}

// --- IntNumber ---

/// A signed 32 bit integer
public sealed const struct IntNumber : IntBaseNumber
{
    embeddable __int theValue[4];
    
    constructor(byte n)
    {
        theValue := n
    }
    
    constructor(short n)
    {
        theValue := n
    }

    /// return the default value
    static int Default
        = 0;
    
    /// Creates the object with the minimal value
    static int MinValue
        = -2147483648;

    /// Creates the object with the maximal value
    static int MaxValue
        = 2147483647;

    constructor()
        = 0;
    
    /// Creates the object with specified value
    constructor(int value)
        = value;

    /// Creates the object with specified value
    constructor(uint value)
        = value;

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

    /// Returns the value as a short integer    
    short cast() = shortConvertor.convert(self);

    /// Returns the byte value    
    byte cast() = byteConvertor.convert(self);

    /// Returns the long integer
    long cast() = new LongNumber(self);

    /// Returns the real number
    real cast() = new RealNumber(self);

    /// Returns the real number
    uint cast() = new UIntNumber(self);
    
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
        
    /// Compares the value
    bool equal(int n)
        = theValue == n;
        
    /// Compares the value
    bool less(int n)
        = theValue < n;
        
    add(n)
        <= add(cast int(n));

    subtract(n)
        <= subtract(cast int(n));

    multiply(n)
        <= multiply(cast int(n));

    divide(n)
        <= divide(cast int(n));
        
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
        
    /// Returns the sum
    int add(int n)
        = theValue + n;

    /// Returns the sum
    int subtract(int n)
        = theValue - n;

    /// Returns the product
    int multiply(int n)
        = theValue * n;

    /// Returns the result of integer division
    int divide(int n)
        = theValue / n;

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
    /// Returns the result of bitwise AND operation
    int and(int n)
        = theValue && n;

    /// Returns the result of bitwise OR operation
    int or(int n)
        = theValue || n;

    /// Returns the result of bitwise XOR operation
    int xor(int n)
        = theValue ^^ n;

    /// Compares the integer value
    bool equal(short value)
        = self.equal(new IntNumber(value));

    /// Compares the integer value
    bool equal(byte value)
        = self.equal(new IntNumber(value));
        
    /// Compares the integer value
    bool equal(long n)
        = new LongNumber(self).equal(n);

    /// Compares the integer value
    bool equal(real n)
        = new RealNumber(self).equal(n);

    /// Compares the integer value
    bool less(short value)
        = self.equal(new IntNumber(value));

    /// Compares the integer value
    bool less(byte value)
        = self.less(new IntNumber(value));
        
    /// Compares the integer value
    bool less(long n)
        = new LongNumber(self).less(n);

    /// Compares the integer value
    bool less(real n)
        = new RealNumber(self).less(n);

    /// Returns the sum of integers
    add(short value)
        = self.add(new IntNumber(value));
        
    /// Returns the sum of integers
    add(byte value)
        = self.add(new IntNumber(value));

    /// Returns the sum of integers
    long add(long n)
        = new LongNumber(self).add(n);

    /// Returns the sum of integers
    real add(real n)
        = new RealNumber(self).add(n);

    /// Returns the difference of integers
    subtract(short value)
        = self.subtract(new IntNumber(value));

    /// Returns the difference of integers
    subtract(byte value)
        = self.subtract(new IntNumber(value));

    /// Returns the difference of integers
    long subtract(long n)
        = new LongNumber(self).subtract(n);

    /// Returns the difference of integers
    real subtract(real n)
        = new RealNumber(self).subtract(n);

    /// Returns the product of integers
    multiply(short value)
        = self.multiply(new IntNumber(value));

    /// Returns the product of integers
    multiply(byte value)
        = self.multiply(new IntNumber(value));

    /// Returns the product of integers
    long multiply(long n)
        = new LongNumber(self).multiply(n);

    /// Returns the product of integers
    real multiply(real n)
        = new RealNumber(self).multiply(n);

    /// Returns the result of integer division
    divide(short value)
        = self.divide(new IntNumber(value));

    /// Returns the result of integer division
    divide(byte value)
        = self.divide(new IntNumber(value));        

    /// Returns the result of integer division
    long divide(long n)
        = new LongNumber(self).divide(n);

    /// Returns the result of integer division
    real divide(real n)
        = new RealNumber(self).divide(n);

    /// Returns the result of bitwise AND operation
    and(short n)
        = self.and(new IntNumber(n));

    /// Returns the result of bitwise AND operation
    and(byte n)
        = self.and(new IntNumber(n));

    /// Returns the result of bitwise AND operation
    and(long n)
        = new LongNumber(self).and(n);

    /// Returns the result of bitwise OR operation
    or(short n)
        = self.or(new IntNumber(n));

    /// Returns the result of bitwise OR operation
    or(byte n)
        = self.or(new IntNumber(n));

    /// Returns the result of bitwise OR operation
    or(long n)
        = new LongNumber(self).or(n);

    /// Returns the result of bitwise XOR operation
    xor(short n)
        = self.xor(new IntNumber(n));

    /// Returns the result of bitwise XOR operation
    xor(byte n)
        = self.xor(new IntNumber(n));

    /// Returns the result of bitwise XOR operation
    xor(long n)
        = new LongNumber(self).xor(n);

    /// Returns the literal presentation
    string Printable = stringConvertor.convert(self);

////    /// Returns the literal presentation
////    wide = system'wideConvertor convert int:$self.

    /// Clones the value
    int clone = new IntNumber(self);

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

// --- UIntNumber ---

/// A unsigned 32 bit integer

public sealed const struct UIntNumber : IntBaseNumber
{
    embeddable __int theValue[4];

    constructor(byte n)
    {
        theValue := n
    }
    
    constructor(short n)
    {
        theValue := n
    }
    
    constructor(int n)
    {
        theValue := n
    }
    
//    cast u(literal s)
//    [
//        theValue := intConvertor convertUnsigned(s).
//    ]
    
    /// return the default value
    static int Default
        = 0;
    
    /// Creates the object with the minimal value
    static int MinValue
        = 0;

    /// Creates the object with the maximal value
    static int MaxValue
        = 0FFFFFFFFh;
        
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
        
    constructor(uint n)
    {
        theValue := n
    }        
        
    constructor()
        = 0;

    /// Returns the literal presentation
    string Printable = stringConvertor.convert(self);

//    /// Returns the literal presentation
//    wide = system'wideConvertor convert uint:$self.
        
    /// Returns the value as a short integer    
    short cast() = shortConvertor.convert(self);

    /// Returns the byte value    
    byte cast() = byteConvertor.convert(self);

    int cast() = self;

    /// Returns the long integer
    long cast() = new LongNumber(self);

    /// Returns the real number
    real cast() = new RealNumber(self);

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
        
    /// Compares the value
    bool equal(int n)
        = theValue == n;
        
    /// Compares the value
    bool less(int n)
        = theValue < n;
        
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
        
    /// Returns the sum
    int add(int n)
        = theValue + n;

    /// Returns the sum
    int subtract(int n)
        = theValue - n;

    /// Returns the product
    int multiply(int n)
        = theValue * n;

    /// Returns the result of integer division
    int divide(int n)
        = theValue / n;

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
        
    /// Returns the result of bitwise AND operation
    int and(int n)
        = theValue && n;

    /// Returns the result of bitwise OR operation
    int or(int n)
        = theValue || n;

    /// Returns the result of bitwise XOR operation
    int xor(int n)
        = theValue ^^ n;

    /// Compares the value
    bool equal(uint n)
        = theValue == n;
        
    /// Compares the value
    bool less(uint n)
        = theValue < n;
        
    /// Returns the sum
    uint add(uint n)
        = theValue + n;

    /// Returns the sum
    uint subtract(uint n)
        = theValue - n;

    /// Returns the product
    uint multiply(uint n)
        = theValue * n;

    /// Returns the result of integer division
    uint divide(uint n)
        = theValue / n;

    /// Returns the result of bitwise AND operation
    uint and(uint n)
        = theValue && n;

    /// Returns the result of bitwise OR operation
    uint or(uint n)
        = theValue || n;

    /// Returns the result of bitwise XOR operation
    uint xor(uint n)
        = theValue ^^ n;

    /// Compares the integer value
    bool equal(short value)
        = self.equal(cast int(value));

    /// Compares the integer value
    bool equal(byte value)
        = self.equal(cast int(value));
        
    /// Compares the integer value
    bool equal(long n )
        = new LongNumber(self).equal(n);

    /// Compares the integer value
    bool equal(real n)
        = new RealNumber(self).equal(n);

    /// Compares the integer value
    bool less(short value)
        = self.equal(cast uint(value));

    /// Compares the integer value
    bool less(byte value)
        = self.less(cast uint(value));
        
    /// Compares the integer value
    bool less(long n)
        = new LongNumber(self).less(n);

    /// Compares the integer value
    bool less(real n)
        = new RealNumber(self).less(n);

    /// Returns the sum of integers
    add(short value)
        = self.add(cast uint(value));

    /// Returns the sum of integers
    add(byte value)
        = self.add(cast uint(value));

    /// Returns the sum of integers
    add(long n)
        = new LongNumber(self).add(n);

    /// Returns the sum of integers
    add(real n)
        = new RealNumber(self).add(n);

    /// Returns the difference of integers
    subtract(short value)
        = self.subtract(cast uint(value));

    /// Returns the difference of integers
    subtract(byte value)
        = self.subtract(cast uint(value));

    /// Returns the difference of integers
    subtract(long n)
        = new LongNumber(self).subtract(n);

    /// Returns the difference of integers
    subtract(real n)
        = new RealNumber(self).subtract(n);

    /// Returns the product of integers
    multiply(short value)
        = self.multiply(cast uint(value));

    /// Returns the product of integers
    multiply(byte value)
        = self.multiply(cast uint(value));

    /// Returns the product of integers
    multiply(long n)
        = new LongNumber(self).multiply(n);

    /// Returns the product of integers
    multiply(real n)
        = new RealNumber(self).multiply(n);

    /// Returns the result of integer division
    divide(short value)
        = self.divide(cast uint(value));

    /// Returns the result of integer division
    divide(byte value)
        = self.divide(cast uint(value));

    /// Returns the result of integer division
    divide(long n)
        = new LongNumber(self).divide(n);

    /// Returns the result of integer division
    divide(real n)
        = new RealNumber(self).divide(n);

    /// Returns the result of bitwise AND operation
    and(short value)
        = self.and(cast uint(value));

    /// Returns the result of bitwise AND operation
    and(byte value)
        = self.and(cast uint(value));

    /// Returns the result of bitwise AND operation
    and(long value)
        = new LongNumber(self).and(value);

    /// Returns the result of bitwise OR operation
    or(short value)
        = self.or(cast uint(value));

    /// Returns the result of bitwise OR operation
    or(byte value)
        = self.or(cast uint(value));

    /// Returns the result of bitwise OR operation
    or(long value)
        = new LongNumber(self).or(value);

    /// Returns the result of bitwise XOR operation
    xor(short value)
        = self.xor(cast uint(value));

    /// Returns the result of bitwise XOR operation
    xor(byte value)
        = self.xor(cast uint(value));

    /// Returns the result of bitwise XOR operation
    xor(long n)
        = new LongNumber(self).xor(n);

    /// Clones the value
    uint clone = new UIntNumber(self);

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
}

// --- LongNumber ---

/// A signed 64 bit integer value
public sealed const struct LongNumber : IntBaseNumber
{
    embeddable __int theValue[8];
//    
    /// return the default value
    static long Default
        = 0l;
    
    /// Returns the minimal value
    static long MinValue
        = -9223372036854775808l;

    /// Returns the maximal value
    static long MaxValue
        = 9223372036854775807l;

    /// Creates the object with a default value (0)
    constructor()
        = 0l;

    /// Creates the object with specified value
    constructor(long value)
        = value;

    /// Creates the object with specified value
    constructor(int value)
        = longConvertor.convert(value);

    /// Creates the object with specified value
    constructor(uint value)
        = longConvertor.convert(value);

    /// Creates the object with specified value
    constructor(short value)
        = longConvertor.convert(value);

    /// Creates the object with specified value
    constructor(byte value)
        = longConvertor.convert(value);

//    /// Creates the object with specified value
//    constructor new : anObject
//        <= new(T<long>(anObject)).

    /// Returns an integer value
    int cast() = intConvertor.convert(self);

    /// Returns an integer value
    uint cast() = uintConvertor.convert(self);

    /// Returns the value as a short integer    
    short cast() = shortConvertor.convert(self);
    
    /// Returns the byte value
    byte cast() = byteConvertor.convert(self);

    /// Returns a real value
    real cast() = new RealNumber(self);
    
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
        
    /// Compares the value
    bool equal(long n)
        = theValue == n;
        
    /// Compares the value
    bool less(long n)
        = theValue < n;
        
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
        
    /// Returns the sum
    long add(long n)
        = theValue + n;

    /// Returns the sum
    long subtract(long n)
        = theValue - n;

    /// Returns the product
    long multiply(long n)
        = theValue * n;

    /// Returns the result of integer division
    long divide(long n)
        = theValue / n;

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
        
    /// Returns the result of bitwise AND operation
    long and(long n)
        = theValue && n;

    /// Returns the result of bitwise OR operation
    long or(long n)
        = theValue || n;

    /// Returns the result of bitwise XOR operation
    long xor(long n)
        = theValue ^^ n;

    /// Compares the integer value
    bool equal(int n) 
        = self.equal(new LongNumber(n));

    /// Compares the integer value
    bool equal(short n)
        = self.equal(new LongNumber(n));

    /// Compares the integer value
    bool equal(byte n)
        = self.equal(new LongNumber(n));

    /// Compares the integer value
    bool equal(real n)
        = new RealNumber(self).equal(n);

    /// Compares the integer value
    bool less(int n)
        = self.less(new LongNumber(n));

    /// Compares the integer value
    bool less(short n)
        = self.less(new LongNumber(n));

    /// Compares the integer value
    bool less(byte n)
        = self.less(new LongNumber(n));

    /// Compares the integer value
    bool less(real n)
        = new RealNumber(self).less(n);

    /// Returns the sum of integers
    add(int n)
        = self.add(new LongNumber(n));

    /// Returns the sum of integers
    add(short n)
        = self.add(new LongNumber(n));

    /// Returns the sum of integers
    add(byte n)
        = self.add(new LongNumber(n));

    /// Returns the sum of integers
    add(real n)
        = new RealNumber(self).add(n);

    /// Returns the difference of integers
    subtract(int n)
        = self.subtract(new LongNumber(n));

    /// Returns the difference of integers
    subtract(short n)
        = self.subtract(new LongNumber(n));

    /// Returns the difference of integers
    subtract(byte n)
        = self.subtract(new LongNumber(n));

    /// Returns the difference of integers
    subtract(real n)
        = new RealNumber(self).subtract(n);

    /// Returns the product of integers
    multiply(real n)
        = new RealNumber(self).multiply(n);

    /// Returns the product of integers
    multiply(int n)
        = self.multiply(new LongNumber(n));

    /// Returns the product of integers
    multiply(byte n)
        = self.multiply(new LongNumber(n));

    /// Returns the product of integers
    multiply(short n)
        = self.multiply(new LongNumber(n));

    /// Returns the result of integer division
    divide(int n)
        = self.divide(new LongNumber(n));

    /// Returns the result of integer division
    divide(short n)
        = self.divide(new LongNumber(n));

    /// Returns the result of integer division
    divide(byte n)
        = self.divide(new LongNumber(n));

    /// Returns the result of integer division
    divide(real n)
        = new RealNumber(self).divide(n);
        
    /// Returns the result of bitwise AND operation
    and(int n)
        = self.and(new LongNumber(n));

    /// Returns the result of bitwise AND operation
    and(short n)
        = self.and(new LongNumber(n));

    /// Returns the result of bitwise AND operation
    and(byte n)
        = self.and(new LongNumber(n));

    /// Returns the result of bitwise OR operation
    or(int n)
        = self.or(new LongNumber(n));

    /// Returns the result of bitwise OR operation
    or(short n)
        = self.or(new LongNumber(n));

    /// Returns the result of bitwise OR operation
    or(byte n)
        = self.or(new LongNumber(n));  

    /// Returns the result of bitwise XOR operation
    xor(int n)
        = self.xor(new LongNumber(n));

    /// Returns the result of bitwise XOR operation
    xor(short n)
        = self.xor(new LongNumber(n));

    /// Returns the result of bitwise XOR operation
    xor(byte n)
        = self.xor(new LongNumber(n));

    /// Returns the literal presentation
    string Printable = stringConvertor.convert(self);

//    /// Returns the literal presentation
//    wide = system'wideConvertor convert long:$self.

    /// Clones the value
    long clone() = new LongNumber(self);
    
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
}

// --- realNumber ---

/// A 64bit float numeric value
public sealed const struct RealNumber : BaseNumber
{
    embeddable __float theValue[8];

    constructor(int n)
    {
        realConvertor.convert(n,ref self)
    }
            
    constructor(byte n)
    {
        realConvertor.convert(n,ref self)
    }
            
    constructor(short n)
    {
        realConvertor.convert(n,ref self)
    }
            
    /// return the default value
    static real Default
        = 0.0r;
            
    /// Returns the minimal value
    static real MinValue
        = -1.7976931348623157e+308r;
    
    /// Returns the maximal value
    static real MaxValue
        = 1.7976931348623157e+308r;
        
    /// Returns the negative infinity
    static real NegativeInfinity 
        = -1.0r / 0.0r;
        
    /// Returns the positive infinity 
    static real PositiveInfinity 
        = 1.0r / 0.0r;
        
    /// returns the NaN
    static real NaN
        = 0.0r / 0.0r;

    /// Creates the object with a default value (0)
    constructor()
        = 0.0r;

    /// Creates the object with specified value
    constructor(real value)
        = value;
  
    /// Creates the object with specified value
    constructor(uint value)
        = realConvertor.convert(value);

    /// Creates the object with specified value
    constructor(long value)
        = realConvertor.convert(value);
        
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
        
    /// Compares the value
    bool equal(real n)
        = theValue == n;
        
    /// Compares the value
    bool less(real n)
        = theValue < n;
        
    add(n)
        <= add(cast real(n));

    subtract(n)
        <= subtract(cast real(n));

    multiply(n)
        <= multiply(cast real(n));

    divide(n)
        <= divide(cast real(n));
        
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
        
    /// Returns the sum
    real add(real n)
        = theValue + n;

    /// Returns the sum
    real subtract(real n)
        = theValue - n;

    /// Returns the product
    real multiply(real n)
        = theValue * n;

    /// Returns the result of integer division
    real divide(real n)
        = theValue / n;
        
    /// Compares the numeric values
    bool equal(int n)
        = self.equal(new RealNumber(n));

    /// Compares the numeric values
    bool equal(byte n)
        = self.equal(new RealNumber(n));

    /// Compares the numeric values
    bool equal(short n)
        = self.equal(new RealNumber(n));

    /// Compares the numeric values
    bool equal(long n)
        = self.equal(new RealNumber(n));

    /// Compares the numeric values
    bool less(int n)
        = self.less(new RealNumber(n));

    /// Compares the numeric values
    bool less(byte n)
        = self.less(new RealNumber(n));

    /// Compares the numeric values
    bool less(short n)
        = self.less(new RealNumber(n));

    /// Compares the numeric values
    bool less(long n)
        = self.less(new RealNumber(n));

    /// Returns the sum of numbers
    real add(int n)
    {
        real r := n;
        
        ^ self.add(r)
    }
    
    /// Returns the sum of numbers
    real add(byte n)
        = self.add(new RealNumber(n));

    /// Returns the sum of numbers
    real add(short n)
        = self.add(new RealNumber(n));

    /// Returns the sum of numbers
    real add(long n)
        = self.add(new RealNumber(n));

    /// Returns the difference of numbers
    real subtract(int n)
        = self.subtract(new RealNumber(n));

    /// Returns the difference of numbers
    real subtract(byte n)
        = self.subtract(new RealNumber(n));

    /// Returns the difference of numbers
    real subtract(short n)
        = self.subtract(new RealNumber(n));

    /// Returns the difference of numbers
    real subtract(long n)
        = self.subtract(new RealNumber(n));

    /// Returns the product of numbers
    real multiply(int n)
    {
        real r := n;
        
        ^ self.multiply(r)
    }

    /// Returns the product of numbers
    real multiply(byte n)
        = self.multiply(new RealNumber(n));

    /// Returns the product of numbers
    real multiply(short n)
        = self.multiply(new RealNumber(n));

    /// Returns the product of numbers
    real multiply(long n)
        = self.multiply(new RealNumber(n));

    /// Returns the result of the division
    real divide(int n)
    {
        real r := n;
        
        ^ self.divide(r)
    }

    /// Returns the result of the division
    real divide(byte n)
        = self.divide(new RealNumber(n));
        
    /// Returns the result of the division
    real divide(short n)
        = self.divide(new RealNumber(n));

    /// Returns the result of the division
    real divide(long n)
        = self.divide(new RealNumber(n));

    /// Returns the literal presentation
    string Printable = stringConvertor.convert(self);

//    /// Returns the literal presentation
//    wide = system'wideConvertor convert real:$self.

    /// Clones the value    
    real clone = new RealNumber(self);
    
//    /// Negates the value
//    negative
//        = 0.0r - theValue.
}

// --- CharValue ---

/// An UTF-32 character symbol
public sealed const struct CharValue : BaseValue
{
    embeddable __raw theValue[4];

    static char Default
        = $0;

    /// Creates the object with a default value (0)
    constructor()
        <= ($0);

    /// Creates the object with specified value
    constructor(char value)
        = value;

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

    bool equal(char val)
        = val == theValue;

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

    /// Returns the literal value
    string Printable = stringConvertor.convert(self);

//    /// Returns the literal value
//    cast T<literal> = literalConvertor convert(self).
//    
//    /// Returns the wide literal value
//    cast T<wide> = wideConvertor convert(self).
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

    /// Reads the number of bytes to represent the symbol
    readLengthTo(ref int retVal)
        => system'core_routines'__char;

    /// Retuns the number of bytes to represent the symbol        
    /*embeddable */get int Length()
    {
        int retVal := 0;
        
        self.readLengthTo(ref retVal);
        
        ^ retVal
    }

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
}

// --- nil constant ---

/// A nil value
public singleton nilValue
{
    /// Returns true if the specified object is nil.
    bool equal(object)
    {
        (nil == object)
            ? { ^ true };
        
        (super.equal:object)
            ? { ^ true };

        ^ false
    }
    
    /// Returns nil
    ///
    /// Any typecasting message will be handled and nil value returned
    generic() = nil;
}

// === Basic Factory symbols ==

public class BaseExtender
{
    object theObject;
    
    /// Returns true if the specified object is equal to the current object; otherwise, false.
    bool equal(object)
        => theObject;

    get string Printable()
        => theObject;
                  
    dispatch() => theObject;
}

/// A generic variable. 
///
/// Extends an assigned value
sealed public class Variable : BaseVariable, ref<object>
{
    /// Creates the empty variable
    constructor()
    {
    }        
        
    /// Creates the variable
    constructor(value)
    {
        theValue := value
    }
    
//    set : value
//    [
//        $value := value.
//    ]
    
    append(val)
    {
        (nil == theValue)
            ? { theValue := val }
            : { theValue := theValue + val }
    }
    
    reduce(val)
    {
        (nil == theValue)
            ? { system'InvalidOperationException.raise() }
            : { theValue := theValue - val }
    }
    
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
}

// --- KeyValue ---

/// A key - object pair
///
/// Extends the object with a key property
public sealed class KeyValue
{
    object theKey;
    object theObject;
    
    /// Creates a new objext
    constructor new(key, object)
    {
        theKey := key;
        theObject := object
    }
    
    /// Returns the key
    Key = theKey;
    
    /// Returns the key
    Value = theObject;

    set Value(object)
    {
        theObject := object
    }
           
    bool equal(o)
        => theObject;
                                                                                      
    get string Printable()
        => theObject;
                                                                                      
    /// Redirects the incoming messages to the object
    dispatch() => theObject;
}

// === Basic Convertors ===

// --- convertor helpers ---

/// A byte convertor
public singleton byteConvertor
{
//    /// Converts a character into a byte value and saves the result into an output parameter
//    convert(char aValue, ref<byte> aRetVal)
//        => system'core_routines'__byteconvertor.
        
    /// Converts a short integer into a byte value and saves the result into an output parameter
    byte convert(short n, ref byte retVal)
        => system'core_routines'__byteconvertor;
        
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
                        
    /// Returns the parameter        
    byte convert(byte n)
        = new ByteNumber(n);
        
//    /// Converts a character into a byte value
//    byte convert(char aValue)
//    [
//        byte aRetVal := 0.
//        self convert(aValue, &aRetVal).
//        
//        ^ ByteNumber new(aRetVal)
//    ]

    /// Converts a short integer into a byte value
    byte convert(short n)
    {
        byte retVal := 0;
        self.convert(n, ref retVal);
        
        ^ new ByteNumber(retVal)
    }
        
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

    byte convert(o)
        = cast byte(o);
}

/// A 16bit integer convertor
public singleton shortConvertor
{
//    /// Converts a character into a short integer value and saves the result into an output parameter
//    convert(char aValue, ref<short> aRetVal)
//        => system'core_routines'__shortconvertor.
        
    /// Converts a byte value into a short integer value and saves the result into an output parameter
    convert(byte n, ref short retVal)
        => system'core_routines'__shortconvertor;
        
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
                        
    /// Returns a short integer value        
    short convert(short n)
        = new ShortNumber(n);
        
    /// Converts a byte value into a short integer value
    short convert(byte n)
    {
        short retVal := 0;
        self.convert(n, ref retVal);
        
        ^ new ShortNumber(retVal)
    }

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
        
    short convert(o)
        = cast short(o);
}

/// An integer convertor
public singleton intConvertor
{
    /// Converts a literal string into an integer and saves the result into an output parameter
    convert(string s, ref int retVal)
        => system'core_routines'__intconvertor;

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
        
    /// Converts a literal string into an integer
    int convert(string s)
    {
        int retVal := 0;
        self.convert(s, ref retVal);
        
        ^ new IntNumber(retVal)
    }
        
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

    /// Returns a parameter                                    
    int convert(int n)
        = new IntNumber(n);

    int convert(uint n)
        = new IntNumber(n);
                        
    int convert(o)
        = cast int(o);
}

/// A unsigned integer convertor
public singleton uintConvertor
{
//    uint convert : object
//        = T<uint>(object).
//    
//    uint convert(int val)
//        = UIntNumber new(val).
        
    /// Converts a long integer into an integer and saves the result into an output parameter
    convert(long n, ref int retVal)
        => system'core_routines'__uintconvertor;
        
    /// Converts a long integer value into an integer and saves the result into an output parameter
    uint convert(long n)
    {
        int retVal := 0;
        self.convert(n, ref retVal);
        
        ^ new UIntNumber(retVal)
    }        
}

/// A long integer convertor
public singleton longConvertor
{
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

    convert(int n, ref long retVal)
        => system'core_routines'__longconvertor;
        
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
        
    /// Converts an integer value into a long integer
    long convert(int n)
    {
        long retVal := 0l;
        self.convert(n, ref retVal);
        
        ^ new LongNumber(retVal)
    }

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
    
    /// Returns a parameter
    long convert(long n)
        = new LongNumber(n);
        
    long convert(n)
        = cast long(n);
}

/// A real number convertor
public singleton realConvertor
{
    /// Converts an integer into a real number and saves the result into an output parameter
    convert(int v, ref real retVal)
        => system'core_routines'__realconvertor;

    /// Converts an integer into a real number and saves the result into an output parameter
    convert(uint n, ref real retVal)
    {
        (n < 0)?
        {
            // If the number is bigger than 2^31
            // than we are unlucky
            int odd := n && 1;
            int halfed := n $shl 1;
            
            real r := 0;
            self.convert(halfed, ref r);
            r := r * 2.0r;
            r := r + odd;
            
            retVal := r
        }
        : { self.convert(n, ref retVal) }
    }        
    //    => system'core_routines'__realconvertor.

    /// Converts an integer into a real number and saves the result into an output parameter
    convert(long n, ref real retVal)
        => system'core_routines'__realconvertor;

    /// Converts an integer into a real number and saves the result into an output parameter
    convert(byte n, ref real retVal)
        => system'core_routines'__realconvertor;

    /// Converts an integer into a real number and saves the result into an output parameter
    convert(short n, ref real retVal)
        => system'core_routines'__realconvertor;

    /// Converts a literal string into a real number and saves the result into an output parameter
    convert(string s, ref real retVal)
        => system'core_routines'__realconvertor;

//    /// Converts a wide literal string into a real number and saves the result into an output parameter
//    convert(wide aValue, ref<real> aRetVal)
//        => system'core_routines'__realconvertor.
//        
//    real convert(BaseVariable v)
//        <= convert(v value).
        
    /// Converts a literal string into a real number
    real convert(string s)
    {
        real retVal := 0.0r;
        self.convert(s, ref retVal);
        
        ^ new RealNumber(retVal)
    }
        
//    /// Converts a wide literal string into a real number
//    real convert(wide aLiteral)
//    [
//        real aRetVal := 0.0r.
//        self convert(aLiteral, &aRetVal).
//        
//        ^ RealNumber new(aRetVal)
//    ]

    /// Returns a parameter
    real convert(real r)
        = new RealNumber(r);
        
    /// Converts an integer into a real number
    real convert(int n)
    {
        real retVal := 0.0r;
        self.convert(n, ref retVal);
        
        ^ new RealNumber(retVal)
    }
        
    /// Converts an integer into a real number
    real convert(uint n)
    {
        real retVal := 0.0r;
        self.convert(n, ref retVal);
        
        ^ new RealNumber(retVal)
    }

    /// Converts a long integer into a real number
    real convert(long n)
    {
        real retVal := 0.0r;
        self.convert(n, ref retVal);
        
        ^ new RealNumber(retVal)
    }

    /// Converts a byte into a real number
    real convert(byte n)
    {
        real retVal := 0.0r;
        self.convert(n, ref retVal);
        
        ^ new RealNumber(retVal)
    }

    /// Converts a short integer into a real number
    real convert(short n)
    {
        real retVal := 0.0r;
        self.convert(n, ref retVal);
        
        ^ new RealNumber(retVal)
    }
        
    real convert(o)
        = cast real(o);
}

// --- realOpController ---

public singleton realOpController
{
    exp(real n, ref real retVal)
        => system'core_routines'__real;
        
//    abs(real aReal, ref<real> aRetVal)
//        => system'core_routines'__real.
        
    ln(real n, ref real retVal)
        => system'core_routines'__real;
        
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
}

//real public pi_value = realOpController pi.
//
//real public e_value = realOpController e.
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