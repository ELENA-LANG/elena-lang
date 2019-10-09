// base variable mixin
public abstract class BaseVariable
{
    // should be overridden
    abstract get Value() {}
    
    abstract set Value(o) {}
}

public sealed ref<T> : BaseVariable
{
    embeddable T theValue;
    
    constructor(T val)
    {
        theValue := val
    }
    
    T cast() = theValue;
    
    get T Value()
    {
         ^ theValue
    }
    
    set Value(T val)
    {
        theValue := val
    }

    bool equal(o)
        => theValue;

    get string Printable()
        => theValue;
        
    dispatch() => theValue;
}


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

    /// Returns itself.
    bool clone() = self;
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
    
    /// Returns itself
    bool clone() = self;
}

// === basic data classes ===

// --- BaseValue ---

/// A base value
public abstract class BaseValue
{
    bool equal(BaseVariable var)
        <= equal(var.Value);
    
    bool less(BaseVariable var)
        <= equal(var.Value);
    
    /// Compares the values
    abstract bool less(o) {}

    /// Compares the values
    bool greater(o) = o.less(self);

    bool notless(o) = self.less(o).Inverted;

    bool notgreater(o) = self.greater(o).Inverted;
}

// --- BaseNumber ---

/// A base numeric value
public abstract class BaseNumber : BaseValue
{
    add(BaseVariable var)
        <= add(var.Value);

    subtract(BaseVariable var)
        <= subtract(var.Value);
    
    multiply(BaseVariable var)
        <= multiply(var.Value);

    divide(BaseVariable var)
        <= divide(var.Value);
    
    /// Adds the value
    abstract add(o) {}

    /// Subtracts the value
    abstract subtract(o) {}

    /// Multiply by the value
    abstract multiply(o) {}

    /// Divides into the value
    abstract divide(o) {}
    
    Negative
        = 0 - self;
}

// --- IntBaseNumber ---

/// A base integer value
public abstract class IntBaseNumber : BaseNumber
{
    and(BaseVariable var)
        <= and(var.Value);
        
    or(BaseVariable var)
        <= or(var.Value);
        
    xor(BaseVariable var)
        <= xor(var.Value);
    
    /// Execute bitwise AND operation
    abstract and(o) {}

    /// Execute bitwise OR operation
    abstract or(o) {}
    
    /// Execute bitwise XOR operation
    abstract xor(o) {}
    
    /// Returns true if all the mask bits are set
    bool allMask(operand)
        = self.and:operand == operand;
        
    /// Returns true if any of the mask bits are set    
    bool anyMask(operand)
        = 0 != self.and:operand;
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

    /// Creates the object with a specified value
    constructor new(val)
        <= (cast byte(val));

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

    bool equal(BaseVariable v)
        <= equal(v.Value);

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
        
    bool equal(o)
    {
        var n := cast byte(o) ?? nil;
        (nil == n)
            ? { ^ false }
            : { ^ self.equal(n) }
    }

    bool less(n)
        <= less(cast byte(n));

    bool less(BaseVariable v)
        <= less(v.Value);

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

    add(n)
        <= add(cast byte(n));

    subtract(n)
        <= subtract(cast byte(n));

    multiply(n)
        <= multiply(cast byte(n));

    divide(n)
        <= divide(cast byte(n));

    byte add(BaseVariable v)
        <= add(v.Value);

    byte subtract(BaseVariable v)
        <= subtract(v.Value);

    byte multiply(BaseVariable v)
        <= multiply(v.Value);

    byte divide(BaseVariable v)
        <= divide(v.Value);

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

    and(n)
        <= and(cast byte(n));
        
    or(n)
        <= or(cast byte(n));
        
    xor(n)
        <= xor(cast byte(n));
        
    byte and(BaseVariable v)
        <= and(v.Value);
        
    byte or(BaseVariable v)
        <= or(v.Value);
        
    byte xor(BaseVariable v)
        <= xor(v.Value);
        
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

    byte shiftLeft(int val)
    {
        ^ theValue $shl val;
    }

    byte shiftRight(int val)
    {
        ^ theValue $shr val;
    }
    
    /// Bitwise inversion
    get byte Inverted()
    {
        intern system'core_routines'__byte.readInvertedTo(self, ref byte retVal);
        
        ^ retVal
    }
        
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

    /// Creates the object with specified value
    constructor new(o)
        <= (cast short(o));

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

    bool equal(o)
    {
        var n := cast short(o) ?? nil;
        (nil == n)
            ? { ^ false }
            : { ^ self.equal(n) }
    }

    bool less(n)
        <= less(cast short(n));

    bool equal(BaseVariable v)
        <= equal(v.Value);
                
    bool less(BaseVariable v)
        <= equal(v.Value);

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

    add(n)
        <= add(cast short(n));

    subtract(n)
        <= subtract(cast short(n));

    multiply(n)
        <= multiply(cast short(n));

    divide(n)
        <= divide(cast short(n));

    short add(BaseVariable v)
        <= add(v.Value);

    short subtract(BaseVariable v)
        <= subtract(v.Value);

    short multiply(BaseVariable v)
        <= multiply(v.Value);

    short divide(BaseVariable v)
        <= divide(v.Value);

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

    and(n)
        <= and(cast short(n));
        
    or(n)
        <= divide(cast short(n));
        
    xor(n)
        <= xor(cast short(n));
        
    short and(BaseVariable v)
        <= and(v.Value);
        
    short or(BaseVariable v)
        <= divide(v.Value);
        
    short xor(BaseVariable v)
        <= xor(v.Value);
        
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

    short shiftLeft(int val)
    {
        ^ theValue $shl val
    }

    short shiftRight(int val)
    {
        ^ theValue $shr val
    }
    
    /// Bitwise inversion
    get short Inverted()
    {
        intern system'core_routines'__short.readInvertedTo(self, ref short retVal);
        
        ^ retVal
    }

    /// Returns the literal presentation
    string Printable = stringConvertor.convert(self);

    /// Clones the value
    short clone() = new ShortNumber(self);
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

    /// Creates the object with specified value
    constructor new(o)
        <= (cast int(o));

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
    
    bool equal(o)
    {
        var n := cast int(o) ?? nil;
        (nil == n)
            ? { ^ false } : { ^ self.equal(n) }
    }

    bool less(n)
        <= less(cast int(n));
        
    bool equal(BaseVariable v)
        <= equal(v.Value);
        
    bool less(BaseVariable v)
        <= less(v.Value);
        
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
        
    int add(BaseVariable v)
        <= add(v.Value);

    int subtract(BaseVariable v)
        <= subtract(v.Value);

    int multiply(BaseVariable v)
        <= multiply(v.Value);

    int divide(BaseVariable v)
        <= divide(v.Value);
        
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

    and(n)
        <= and(cast int(n));
        
    or(n)
        <= divide(cast int(n));
        
    xor(n)
        <= xor(cast int(n));
        
    int and(BaseVariable v)
        <= and(v.Value);
        
    int or(BaseVariable v)
        <= or(v.Value);
        
    int xor(BaseVariable v)
        <= xor(v.Value);
        
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

    /// Clones the value
    int clone() = new IntNumber(self);

    int shiftLeft(BaseVariable v)
        <= shiftLeft(v.Value);

    int shiftLeft(int val)
    {
        ^ theValue $shl val
    }

    int shiftRight(BaseVariable v)
        <= shiftRight(v.Value);

    int shiftRight(int val)
    {
        ^ theValue $shr val
    }
    
    /// Bitwise inversion
    get int Inverted()
    {
        intern system'core_routines'__int.readInvertedTo(theValue, ref int retVal);
        
        ^ retVal
    }
        
    /// Negates the value
    int Negative
        = 0 - theValue;
        
    /// Returns true if all the mask bits are set
    bool allMask(int operand)
    {
        int v := operand && theValue;
        
        ^ v == operand
    }
        
    /// Returns true if any of the mask bits are set    
    bool anyMask(int operand)
    {
        int v := operand && theValue;
        
        ^ v != 0
    }        
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
    
    cast u(string s)
    {
        theValue := intConvertor.convertUnsigned(s)
    }
    
    /// return the default value
    static int Default
        = 0;
    
    /// Creates the object with the minimal value
    static int MinValue
        = 0;

    /// Creates the object with the maximal value
    static int MaxValue
        = 0FFFFFFFFh;
        
    constructor new(o)
        <= (cast int(o));
        
    constructor(uint n)
    {
        theValue := n
    }        
        
    constructor()
        = 0;

    /// Returns the literal presentation
    string Printable = stringConvertor.convert(self);

    /// Returns the value as a short integer    
    short cast() = shortConvertor.convert(self);

    /// Returns the byte value    
    byte cast() = byteConvertor.convert(self);

    int cast() = self;

    /// Returns the long integer
    long cast() = new LongNumber(self);

    /// Returns the real number
    real cast() = new RealNumber(self);

    bool equal(o)
    {
        var n := cast int(o) ?? nil;
        (nil == n)
            ? { ^ false }
            : { ^ self.equal(n) }
    }

    bool less(n)
        <= less(cast int(n));
        
    bool equal(BaseVariable v)
        <= equal(v.Value);
        
    bool less(BaseVariable v)
        <= less(v.Value);
        
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
        
    int add(BaseVariable v)
        <= add(v.Value);

    int subtract(BaseVariable v)
        <= subtract(v.Value);

    int multiply(BaseVariable v)
        <= multiply(v.Value);

    int divide(BaseVariable v)
        <= divide(v.Value);
        
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

    and(n)
        <= and(cast int(n));
        
    or(n)
        <= divide(cast int(n));
        
    xor(n)
        <= xor(cast int(n));
        
    int and(BaseVariable v)
        <= and(v.Value);
        
    int or(BaseVariable v)
        <= or(v.Value);
        
    int xor(BaseVariable v)
        <= xor(v.Value);
        
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

    uint shiftLeft(int val)
    {
        ^ theValue $shl val
    }

    uint shiftRight(int val)
    {
        ^ theValue $shr val
    }
    
    /// Bitwise inversion
    get uint Inverted()
    {
        intern system'core_routines'__int.readInvertedTo(theValue, ref int retVal);
        
        ^ retVal
    }
}

// --- LongNumber ---

/// A signed 64 bit integer value
public sealed const struct LongNumber : IntBaseNumber
{
    embeddable __int theValue[8];

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

    /// Creates the object with specified value
    constructor new(o)
        <= (cast long(o));

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
    
    bool equal(o)
    {
        var n := cast long(o) ?? nil;
        (nil == n)
            ? { ^ false }
            : { ^ self.equal(n) }
    }

    bool less(n)
        <= less(cast long(n));
        
    bool equal(BaseVariable v)
        <= equal(v.Value);
        
    bool less(BaseVariable v)
        <= less(v.Value);
        
    /// Compares the value
    bool equal(long n)
        = theValue == n;
        
    /// Compares the value
    bool less(long n)
        = theValue < n;
        
    add(n)
        <= add(cast long(n));

    subtract(n)
        <= subtract(cast long(n));

    multiply(n)
        <= multiply(cast long(n));

    divide(n)
        <= divide(cast long(n));
        
    long add(BaseVariable v)
        <= add(v.Value);

    long subtract(BaseVariable v)
        <= subtract(v.Value);

    long multiply(BaseVariable v)
        <= multiply(v.Value);

    long divide(BaseVariable v)
        <= divide(v.Value);
        
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

    and(n)
        <= and(cast long(n));
        
    or(n)
        <= divide(cast long(n));
        
    xor(n)
        <= xor(cast long(n));
        
    long and(BaseVariable v)
        <= and(v.Value);
        
    long or(BaseVariable v)
        <= divide(v.Value);
        
    long xor(BaseVariable v)
        <= xor(v.Value);
        
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

    /// Clones the value
    long clone() = new LongNumber(self);
    
    long shiftLeft(int val)
    {
        ^ theValue $shl val
    }

    long shiftRight(int val)
    {
        ^ theValue $shr val
    }
    
    /// Bitwise inversion
    get long Inverted()
    {
        intern system'core_routines'__long.readInvertedTo(theValue, ref long retVal);
        
        ^ retVal
    }
        
    /// Negates the value
    Negative
        = 0l - theValue;
}

// --- realNumber ---

/// A 64bit float numeric value
public sealed const struct RealNumber : BaseNumber
{
    embeddable __float theValue[8];

    constructor(int n)
        = realConvertor.convert(n);
            
    constructor(byte n)
        = realConvertor.convert(n);
            
    constructor(short n)
        = realConvertor.convert(n);
            
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

    static real Pi
        = Pi_value;

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
        
    /// Creates the object with specified value
    constructor new(o)
        <= (cast real(o));
        
    bool equal(o)
    {
        var n := cast real(o) ?? nil;
        (nil == n)
            ? { ^ false }
            : { ^ self.equal(n) }
    }

    bool less(n)
        <= less(cast real(n));
        
    bool equal(BaseVariable v)
        <= equal(v.Value);
        
    bool less(BaseVariable v)
        <= less(v.Value);
        
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
        
    real add(BaseVariable v)
        <= add(v.Value);

    real subtract(BaseVariable v)
        <= subtract(v.Value);

    real multiply(BaseVariable v)
        <= multiply(v.Value);

    real divide(BaseVariable v)
        <= divide(v.Value);
        
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

    /// Clones the value    
    real clone = new RealNumber(self);
    
    /// Negates the value
    Negative
        = 0.0r - theValue;
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

    /// Creates the object with specified numeric value
    constructor load(int n)
        = charConvertor.convert(n);

    /// Returns the minimal possible character value (0)
    static char MinValue
        = $0;

    /// Returns the maximal possible character value (010FFFFh)
    static char MaxValue
        = $010FFFFh;

    internal get Value()
    {
        intConvertor.convert(self, ref int val);
        
        ^ val
    }

    bool equal(o)
    {
        var n := cast char(o) ?? nil;
        (nil == n)
            ? { ^ false }
            : { ^ self.equal(n) }
    }

    bool less(n)
        <= less(cast char(n));
        
    bool equal(BaseVariable v)
        <= equal(v.Value);
        
    bool less(BaseVariable v)
        <= less(v.Value);
        
    bool equal(int val)
        = theValue == val;

    bool equal(string val)
    {
        int chLen := self.Length;        
        int ln := val.Length;
        (ln == chLen)?
        {
            char ch := val[0];
            
            ^ self == ch;
        };
        
        ^ false
    }

    bool equal(wide val)
    {
        int chLen := self.WideLength;        
        int ln := val.Length;
        (ln == chLen)?
        {
            char ch := val[0];
            
            ^ self == ch;
        };
        
        ^ false
    }

    bool equal(char val)
        = theValue == val;

    bool less(char val)
    {
        ^ theValue < val;
    }
    
    bool less(string val)
        = val < (cast string(self));

    bool less(wide val)
        = val < (cast wide(self));

    /// Clones the value
    char clone() = new CharValue(self);

    /// Returns the literal value
    string Printable = stringConvertor.convert(self);

    /// Returns the literal value
    string cast() = stringConvertor.convert(self);
    
    /// Returns the wide literal value
    wide cast() = wideConvertor.convert(self);

    /// Retuns the number of bytes to represent the symbol        
    get int Length()
    {
        intern system'core_routines'__char.readLengthTo(self, ref int retVal);
        
        ^ retVal
    }
    
    /// Reads the number of bytes to represent the symbol
    get int WideLength()
    {
        int length := self.Length;
        
        length := length + 1;
        length := length / 2;

        ^ length
    }
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
public sealed class Variable : BaseVariable, ref<object>
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

// --- DTO --

public class DTO
{
    attribute MessageName[] __dto_properties;
    
    MessageName[] __dto_properties()
    {
        ^ __dto_properties
    }
}

// === Basic Convertors ===

// --- convertor helpers ---

/// A byte convertor
public singleton byteConvertor
{
    byte convert(BaseVariable v)
        <= convert(v.value);
                        
    /// Returns the parameter        
    byte convert(byte n)
        = n;
        
    /// Converts a character into a byte value
    byte convert(char val)
    {
        intern system'core_routines'__byteconvertor.convert(val, ref byte retVal);
        
        ^ retVal
    }

    /// Converts a short integer into a byte value
    byte convert(short n)
    {
        intern system'core_routines'__byteconvertor.convert(n, ref byte retVal);
        
        ^ retVal
    }
        
    /// Converts an integer into a byte value
    byte convert(int val)
    {
        intern system'core_routines'__byteconvertor.convert(val, ref byte retVal);
        
        ^ retVal
    }
        
    /// Converts an integer into a byte value
    byte convert(uint val)
    {
        intern system'core_routines'__byteconvertor.convert(val, ref byte retVal);
        
        ^ retVal
    }

    /// Converts a long integer into a byte value
    byte convert(long val)
    {
        intern system'core_routines'__byteconvertor.convert(val, ref byte retVal);
        
        ^ retVal
    }

    /// Converts a real number into a byte value
    byte convert(real val)
    {
        intern system'core_routines'__byteconvertor.convert(val, ref byte retVal);
        
        ^ retVal
    }

    /// Converts a literal value into a byte value
    byte convert(string s)
    {
        intern system'core_routines'__byteconvertor.convert(s, ref byte retVal);
        
        ^ retVal
    }
                
    /// Converts a wide literal value into a byte value
    byte convert(wide s)
    {
        intern system'core_routines'__byteconvertor.convert(s, ref byte retVal);
        
        ^ retVal
    }

    byte convert(o)
        = cast byte(o);
}

/// A 16bit integer convertor
public singleton shortConvertor
{
    short convert(BaseVariable v)
        <= convert(v.Value);
                        
    /// Returns a short integer value        
    short convert(short n)
        = n;
        
    /// Converts a byte value into a short integer value
    short convert(byte n)
    {
        intern system'core_routines'__shortconvertor.convert(n, ref short retVal);
        
        ^ retVal
    }

    /// Converts an integer into a short integer value
    short convert(int n)
    {
        intern system'core_routines'__shortconvertor.convert(n, ref short retVal);
        
        ^ retVal
    }

    /// Converts an integer into a short integer value
    short convert(uint n)
    {
        intern system'core_routines'__shortconvertor.convert(n, ref short retVal);
        
        ^ retVal
    }

    /// Converts a long integer into a short integer value
    short convert(long n)
    {
        intern system'core_routines'__shortconvertor.convert(n, ref short retVal);
        
        ^ retVal
    }
        
    /// Converts a real number into a short integer value
    short convert(real n)
    {
        intern system'core_routines'__shortconvertor.convert(n, ref short retVal);
        
        ^ retVal
    }
        
    /// Converts a character into a short integer value
    short convert(char ch)
    {
        intern system'core_routines'__shortconvertor.convert(ch, ref short retVal);
        
        ^ retVal
    }
        
    /// Converts a literal string into a short integer value
    short convert(string s)
    {
        intern system'core_routines'__shortconvertor.convert(s, ref short retVal);
        
        ^ retVal
    }
        
    /// Converts a wide literal string into a short integer value
    short convert(wide s)
    {
        intern system'core_routines'__shortconvertor.convert(s, ref short retVal);
        
        ^ retVal
    }
        
    short convert(o)
        = cast short(o);
}

/// An integer convertor
public singleton intConvertor
{
    int convert(BaseVariable v)
        <= convert(v.Value);
        
    /// Converts a literal string into an integer
    int convert(string s)
    {
        intern system'core_routines'__intconvertor.convert(s, ref int retVal);
        
        ^ retVal
    }
        
    /// Converts a literal string into an integer
    int convertUnsigned(string s)
    {
        intern system'core_routines'__intconvertor.convert(s, ref int retVal);
        
        ^ retVal
    }
        
    /// Converts a literal string into an integer
    int convert(string s, int base)
    {
        intern system'core_routines'__intconvertor.convert(s, base, ref int retVal);
        
        ^ retVal
    }
        
    /// Converts a wide literal string into an integer
    int convert(wide s)
    {
        intern system'core_routines'__intconvertor.convert(s, ref int retVal);
        
        ^ retVal
    }

    /// Converts a wide literal string into an integer
    int convert(wide s, int base)
    {
        intern system'core_routines'__intconvertor.convert(s, base, ref int retVal);
        
        ^ retVal
    }
        
    /// Converts a character value into an integer
    int convert(char ch)
    {
        intern system'core_routines'__intconvertor.convert(ch, ref int retVal);
        
        ^ retVal
    }
        
    /// Converts a byte value into an integer
    int convert(byte val)
    {
        intern system'core_routines'__intconvertor.convert(val, ref int retVal);
        
        ^ retVal
    }
        
    /// Converts a short integer value into an integer
    int convert(short val)
    {
        intern system'core_routines'__intconvertor.convert(val, ref int retVal);
        
        ^ retVal
    }
        
    /// Converts a long integer value into an integer and saves the result into an output parameter
    int convert(long val)
    {
        intern system'core_routines'__intconvertor.convert(val, ref int retVal);
        
        ^ retVal
    }

    /// Converts a long integer into an integer
    int convert(real n)
    {
        intern system'core_routines'__intconvertor.convert(n, ref int retVal);
        
        ^ retVal
    }

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
    uint convert(object)
        = cast uint(object);
    
    uint convert(int val)
        = new UIntNumber(val);
        
    /// Converts a long integer value into an integer and saves the result into an output parameter
    uint convert(long n)
    {
        intern system'core_routines'__uintconvertor.convert(n, ref int retVal);
        
        ^ retVal
    }        
    
    /// Converts a literal string into an unsgined integer
    uint convert(string s)
    {
        intern system'core_routines'__intconvertor.convert(s, ref int retVal);
        
        ^ retVal
    }    
}

/// A long integer convertor
public singleton longConvertor
{
    long convert(BaseVariable v)
        <= convert(v.Value);
        
    /// Converts an integer value into a long integer
    long convert(int n)
    {
        intern system'core_routines'__longconvertor.convert(n, ref long retVal);
        
        ^ retVal
    }

    /// Converts an integer value into a long integer
    long convert(uint val)
    {
        intern system'core_routines'__longconvertor.convert(val, ref long retVal);
        
        ^ retVal
    }

    /// Converts an integer value into a long integer
    long convert(byte val)
    {
        intern system'core_routines'__longconvertor.convert(val, ref long retVal);
        
        ^ retVal
    }

    /// Converts an integer value into a long integer
    long convert(short val)
    {
        intern system'core_routines'__longconvertor.convert(val, ref long retVal);
        
        ^ retVal
    }

    /// Converts a real number into a long integer
    long convert(real val)
    {
        intern system'core_routines'__longconvertor.convert(val, ref long retVal);
        
        ^ retVal
    }

    /// Converts a character into a long integer
    long convert(char ch)
    {
        intern system'core_routines'__intconvertor.convert(ch, ref int retVal);
        
        ^ retVal
    }

    /// Converts a literal string into a long integer
    long convert(string s)
    {
        intern system'core_routines'__longconvertor.convert(s, ref long retVal);
        
        ^ retVal
    }

    /// Converts a literal string into a long integer
    long convert(string s, int base)
    {
        intern system'core_routines'__longconvertor.convert(s, base, ref long retVal);
        
        ^ retVal
    }

    /// Converts a wide literal string into a long integer
    long convert(wide s)
    {
        intern system'core_routines'__longconvertor.convert(s, ref long retVal);
        
        ^ retVal
    }

    /// Converts a wide literal string into a long integer
    long convert(wide s, int base)
    {
        intern system'core_routines'__longconvertor.convert(s, base, ref long retVal);
        
        ^ retVal
    }
    
    /// Returns a parameter
    long convert(long n)
        = new LongNumber(n);
        
    long convert(n)
        = cast long(n);
}

/// A real number convertor
public singleton realConvertor
{
    real convert(BaseVariable v)
        <= convert(v.Value);
        
    /// Converts a literal string into a real number
    real convert(string s)
    {
        intern system'core_routines'__realconvertor.convert(s, ref real retVal);
        
        ^ retVal
    }
        
    /// Converts a wide literal string into a real number
    real convert(wide s)
    {
        intern system'core_routines'__realconvertor.convert(s, ref real retVal);
        
        ^ retVal
    }

    /// Returns a parameter
    real convert(real r)
        = new RealNumber(r);
        
    /// Converts an integer into a real number
    real convert(int n)
    {
        intern system'core_routines'__realconvertor.convert(n, ref real retVal);
        
        ^ retVal
    }
        
    /// Converts an integer into a real number
    real convert(uint n)
    {
        real retVal;
        (n < 0)?
        {
            // If the number is bigger than 2^31
            // than we are unlucky
            int odd := n && 1;
            int halfed := n $shl 1;
            
            real r := 0;
            intern system'core_routines'__realconvertor.convert(halfed, ref r);
            r := r * 2.0r;
            r := r + odd;
            
            retVal := r
        }
        : { 
            int number := n;
            intern system'core_routines'__realconvertor.convert(number, ref retVal);
        
            ^ retVal
        }
    }

    /// Converts a long integer into a real number
    real convert(long n)
    {
        intern system'core_routines'__realconvertor.convert(n, ref real retVal);
        
        ^ retVal
    }

    /// Converts a byte into a real number
    real convert(byte n)
    {
        intern system'core_routines'__realconvertor.convert(n, ref real retVal);
        
        ^ retVal
    }

    /// Converts a short integer into a real number
    real convert(short n)
    {
        intern system'core_routines'__realconvertor.convert(n, ref real retVal);
        
        ^ retVal
    }
        
    real convert(o)
        = cast real(o);
}

// --- realOpController ---

public singleton realOpController
{
    real exp(real n)
    {
        intern system'core_routines'__real.exp(n, ref real retVal);
        
        ^ retVal
    }
        
    real abs(real r)
    {
        intern system'core_routines'__real.abs(r, ref real retVal);
        
        ^ retVal
    }
        
    real ln(real n)
    {
        intern system'core_routines'__real.ln(n, ref real retVal);
        
        ^ retVal
    }
        
    real round(real r)
    {
        intern system'core_routines'__real.round(r, ref real retVal);
        
        ^ retVal
    }
        
    real truncate(real r)
    {
        intern system'core_routines'__real.truncate(r, ref real retVal);
        
        ^ retVal
    }
        
    real sin(real r)
    {
        intern system'core_routines'__real.sin(r, ref real retVal);
        
        ^ retVal
    }
        
    real cos(real r)
    {
        intern system'core_routines'__real.cos(r, ref real retVal);
        
        ^ retVal
    }
        
    real arctan(real r)
    {
        intern system'core_routines'__real.arctan(r, ref real retVal);
        
        ^ retVal
    }
                
    real sqrt(real r)
    {
        intern system'core_routines'__real.sqrt(r, ref real retVal);
        
        ^ retVal
    }
        
    real pi()
    {
        intern system'core_routines'__real.pi(ref real retVal);
        
        ^ retVal
    }
    
    embeddable real e()
    {
        real retVal := self.exp(1.0r);
        
        ^ retVal
    }        
}

public real Pi_value = realOpController.pi();

public real E_value = realOpController.e();

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

public extension realRefOp : ref<real>
{
    append(real n)
    {
        real v := self.Value;
        v := v + n;
        
        self.Value := v
    }
    
    reduce(real n)
    {
        real v := self.Value;
        v := v - n;
        
        self.Value := v
    }
}

// template test cases
T1;

template_test()
{
    ref<T1> r;
}
