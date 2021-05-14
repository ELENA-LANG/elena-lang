[info:"variable base class"]
public abstract class BaseVariable
{
    [info:"returns the variable value;<br/>should be overridden"]
    get abstract Value();
    
    [info:"set the variable value;<br/>should be overridden"]
    set abstract Value(o);
}

[info:"a variable template"]
public sealed ref<T> : BaseVariable
{
    embeddable T value;
    
    [info:"initializes a variable"]
    constructor(T val)
    {
        value := val
    }
    
    [info:"returns the variable value"]
    T cast() = value;
    
    [info:"returns the variable value"]
    get T Value()
    {
         ^ value
    }
    
    [info:"set the variable value"]
    set Value(T val)
    {
        value := val
    }

    [info:"Returns true if o is equal to the variable value; otherwise, false"]
    bool equal(o)
        => value;

    [info:"Returns the value string representation"]
    string toPrintable()
        => value;
        
    dispatch() => value;
}

// === basic data classes ===

// --- BaseValue ---

[info:"a base value"]
public abstract class BaseValue
{
    [info:"Returns true if a value of var is equal to the object value; otherwise, false"]
    bool equal(BaseVariable var)
        <= equal(*var);
    
    [info:"Returns true if the object value is less than a value of var; otherwise, false"]
    bool less(BaseVariable var)
        <= less(*var);
    
    [info:"Returns true if the object value is less than o; otherwise, false;<br/>should be overridden"]
    abstract bool less(o);

    [info:"Returns true if the object value is greater than o; otherwise, false;<br/>by default sends less[2] to o with the object as an argument"]
    bool greater(o) = o.less(self);

    [info:"Returns true if the object value is not less than o; otherwise, false;<br/>by default inverts the result of less[2] operation"]
    bool notless(o) = !self.less(o);

    [info:"Returns true if the object value is not greater than o; otherwise, false;<br/>by default inverts the result of greater[2] operation"]
    bool notgreater(o) = !self.greater(o);
}

// --- BaseNumber ---

[info:"a base numeric value"]
public abstract class BaseNumber : BaseValue
{
    add(BaseVariable var)
        <= add(*var);

    subtract(BaseVariable var)
        <= subtract(*var);
    
    multiply(BaseVariable var)
        <= multiply(*var);

    divide(BaseVariable var)
        <= divide(*var);
    
    [info:"adds the value;<br/>should be overridden"]
    abstract add(o);

    [info:"subtracts the value;<br/>should be overridden"]
    abstract subtract(o);

    [info:"multiplies by the value;<br/>should be overridden"]
    abstract multiply(o);

    [info:"divides by the value;<br/>should be overridden"]
    abstract divide(o);
    
    [info:"returns the negative value of a number"]
    Negative
        = 0 - self;
}

// --- IntBaseNumber ---

[info:"a base integer value"]
public abstract class IntBaseNumber : BaseNumber
{
    band(BaseVariable var)
        <= band(*var);
        
    bor(BaseVariable var)
        <= bor(*var);
        
    bxor(BaseVariable var)
        <= bxor(*var);
    
    [info:"executes bitwise AND operation;<br/>should be overridden"]
    abstract band(o);

    [info:"executes bitwise OR operation;<br/>should be overridden"]
    abstract bor(o);
    
    [info:"executes bitwise XOR operation;<br/>should be overridden"]
    abstract bxor(o);
    
    [info:"returns true if all the mask bits are set"]
    bool allMask(operand)
        = self.band:operand == operand;
        
    [info:"returns true if any of the mask bits are set"]
    bool anyMask(operand)
        = 0 != self.band:operand;
        
    get abstract BInverted();
}

// --- ByteNumber ---

[info:"an unsigned 8 bit integer"]
public sealed const struct ByteNumber : IntBaseNumber
{
    embeddable __int theValue[1];

    [info:"returns the default value"]
    constructor()
        = 0;

    [info:"returns the minimal value (0)"]
    static byte MinValue
        = 0;

    [info:"returns the maxial value (255)"]
    static byte MaxValue
        = 255;
    
    [info:"creates the object with a specified value"]
    constructor(byte value)
        = value;

    [info:"creates the object with a specified value"]
    constructor(int n)
        = n & 255;

    [info:"returns the value as a short integer"]
    short cast() = new ShortNumber(self);

    [info:"returns the value as an integer"]
    int cast() = new IntNumber(self);

    [info:"returns the value as an integer"]
    uint cast() = new UIntNumber(self);

    [info:"returns the value as a long integer"]
    long cast() = new LongNumber(self);

    [info:"returns the value as a real number"]
    real cast() = new RealNumber(self);

//    bool equal(BaseVariable v)
//        <= equal(*v);

    [info:"compares the value"]
    bool equal(byte n)
        = theValue == n;

    [info:"compares the value"]
    bool equal(short n)
        = new ShortNumber(self).equal(n);
        
    [info:"compares the value"]
    bool equal(int n)
        = new IntNumber(self).less(n);
        
    [info:"compares the value"]
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

//    bool less(BaseVariable v)
//        <= less(v.Value);

    bool less(byte n)
        = theValue < n;

    bool less(short n)
        = new ShortNumber(self).less(n);

    [info:"compares the value"]
    bool less(int n)
        = new IntNumber(self).less(n);
        
    [info:"compares the value"]
    bool equal(long n)
        = new LongNumber(self).equal(n);
        
    [info:"compares the value"]
    bool less(long n)
        = new LongNumber(self).less(n);

    [info:"compares the value"]
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

//    byte add(BaseVariable v)
//        <= add(v.Value);
//
//    byte subtract(BaseVariable v)
//        <= subtract(v.Value);
//
//    byte multiply(BaseVariable v)
//        <= multiply(v.Value);
//
//    byte divide(BaseVariable v)
//        <= divide(v.Value);

    [info:"returns the sum"]
    byte add(byte n)
        = theValue + n;

    [info:"returns the difference"]
    byte subtract(byte n)
        = theValue - n;

    [info:"returns the product"]
    byte multiply(byte n)
        = theValue * n;

    [info:"returns the result of integer division"]
    byte divide(byte n)
        = theValue / n;

    [info:"returns the sum"]
    short add(short n)
        = new ShortNumber(self).add(n);

    [info:"returns the difference"]
    short subtract(short n)
        = new ShortNumber(self).subtract(n);

    [info:"returns the product"]
    short multiply(short n)
        = new ShortNumber(self).multiply(n);

    [info:"returns the result of integer division"]
    short divide(short n)
        = new ShortNumber(self).divide(n);

    [info:"returns the sum"]
    int add(int n)
        = new IntNumber(self).add(n);

    [info:"returns the difference"]
    int subtract(int n)
        = new IntNumber(self).subtract(n);

    [info:"returns the product"]
    int multiply(int n)
        = new IntNumber(self).multiply(n);

    [info:"returns the result of integer division"]
    int divide(int n)
        = new IntNumber(self).divide(n);

    [info:"returns the sum"]
    long add(long n)
        = new LongNumber(self).add(n);

    [info:"returns the difference"]
    long subtract(long n)
        = new LongNumber(self).subtract(n);

    [info:"returns the product"]
    long multiply(long n)
        = new LongNumber(self).multiply(n);

    [info:"returns the result of integer division"]
    long divide(long n)
        = new LongNumber(self).divide(n);

    [info:"returns the sum"]
    real add(real n)
        = new RealNumber(self).add(n);

    [info:"returns the difference"]
    real subtract(real n)
        = new RealNumber(self).subtract(n);

    [info:"returns the product"]
    real multiply(real n)
        = new RealNumber(self).multiply(n);

    [info:"returns the result of integer division"]
    real divide(real n)
        = new RealNumber(self).divide(n);

    band(n)
        <= band(cast byte(n));
        
    bor(n)
        <= bor(cast byte(n));
        
    bxor(n)
        <= bxor(cast byte(n));
        
//    byte and(BaseVariable v)
//        <= and(v.Value);
//        
//    byte or(BaseVariable v)
//        <= or(v.Value);
//        
//    byte xor(BaseVariable v)
//        <= xor(v.Value);
        
    [info:"returns the result of bitwise AND operation"]
    byte band(byte n)
        = theValue & n;

    [info:"returns the result of bitwise OR operation"]
    byte bor(byte n)
        = theValue | n;

    [info:"returns the result of bitwise XOR operation"]
    byte bxor(byte n)
        = theValue ^ n;

    [info:"returns the result of bitwise AND operation"]
    short band(short n)
        = new ShortNumber(self).band(n);

    [info:"returns the result of bitwise OR operation"]
    short bor(short n)
        = new ShortNumber(self).bor(n);

    [info:"returns the result of bitwise XOR operation"]
    short bxor(short n)
        = new ShortNumber(self).bxor(n);

    [info:"returns the result of bitwise AND operation"]
    int band(int n)
        = new IntNumber(self).band(n);

    [info:"returns the result of bitwise OR operation"]
    int bor(int n)
        = new IntNumber(self).bor(n);

    [info:"returns the result of bitwise XOR operation"]
    int bxor(int n)
        = new IntNumber(self).bxor(n);

    [info:"returns the result of bitwise AND operation"]
    long band(long n)
        = new LongNumber(self).band(n);

    [info:"returns the result of bitwise OR operation"]
    long bor(long n)
        = new LongNumber(self).bor(n);

    [info:"returns the result of bitwise XOR operation"]
    long bxor(long n)
        = new LongNumber(self).bxor(n);

    [info:"shifts an integer value to the left by a specified number of bits"]
    byte shiftLeft(int val)
    {
        ^ theValue $shl val;
    }

    [info:"shifts an integer value to the right by a specified number of bits"]
    byte shiftRight(int val)
    {
        ^ theValue $shr val;
    }
    
    [info:"bitwise inversion"]
    get byte BInverted()
        = ~theValue;

    [info:"returns the literal presentation"]
    string toPrintable() = stringConvertor.convert(self);
        
    [info:"clones the value"]
    byte clone() = new ByteNumber(self);
}

// --- ShortNumber ---

[info:"a signed 16 bit integer"]
public sealed const struct ShortNumber : IntBaseNumber
{
    embeddable __int theValue[2];

    [info:"returns the minimal value (-32768)"]
    static short MinValue
        = -32768;

    [info:"returns the maxial value (32767)"]
    static short MaxValue
        = 32767;

    constructor(int n)
    {
        (n < 0)
            ? {
                theValue := n & 0FFFFh;
                
                theValue := theValue | 0FFFF0000h
            }
            : { theValue := n & 0FFFFh }
    }
    
    [info:"creates the object with specified value"]
    constructor(byte value)
        = shortConvertor.convert(value);
        
    [info:"creates the object with specified value"]
    constructor(short value)
        = value;

    [info:"creates the object with a default value (0)"]
    constructor()
        = 0;

    [info:"returns the byte value"]
    byte cast() = byteConvertor.convert(self);
    
    [info:"returns the integer value"]
    int cast() = new IntNumber(self);
    
    [info:"returns the integer value"]
    uint cast() = new UIntNumber(self);
    
    [info:"returns the long integer value"]
    long cast() = new LongNumber(self);
    
    [info:"returns the value as a real number"]
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

//    bool equal(BaseVariable v)
//        <= equal(v.Value);
//                
//    bool less(BaseVariable v)
//        <= equal(v.Value);

    [info:"compares the value"]
    bool equal(short n)
        = theValue == n;
        
    [info:"compares the value"]
    bool less(short n)
        = theValue < n;
        
    [info:"compares the value"]
    bool equal(int n)
        = new IntNumber(self).equal(n);
        
    [info:"compares the value"]
    bool less(int n)
        = new IntNumber(self).less(n);
    
    [info:"compares the value"]
    bool equal(byte n)
        = self.equal(new ShortNumber(n));
        
    [info:"compares the value"]
    bool less (byte n)
        = self.less(new ShortNumber(n));
    
    [info:"compares the value"]
    bool equal(long n)
        = new LongNumber(self).equal(n);
        
    [info:"compares the value"]
    bool less(long n)
        = new LongNumber(self).less(n);
    
    [info:"compares the value"]
    bool equal(real n)
        = new RealNumber(self).equal(n);
        
    [info:"compares the value"]
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

//    short add(BaseVariable v)
//        <= add(v.Value);
//
//    short subtract(BaseVariable v)
//        <= subtract(v.Value);
//
//    short multiply(BaseVariable v)
//        <= multiply(v.Value);
//
//    short divide(BaseVariable v)
//        <= divide(v.Value);

    [info:"returns the sum"]
    short add(short n)
        = theValue + n;

    [info:"returns the sum"]
    short subtract(short n)
        = theValue - n;

    [info:"returns the product"]
    short multiply(short n)
        = theValue * n;

    [info:"returns the result of integer division"]
    short divide(short n)
        = theValue / n;

    [info:"returns the sum"]
    add(byte n)
        = self.add(new ShortNumber(n));

    [info:"returns the difference"]
    subtract(byte n)
        = self.subtract(new ShortNumber(n));

    [info:"returns the product"]
    multiply(byte n)
        = self.multiply(new ShortNumber(n));

    [info:"returns the result of integer division"]
    divide(byte n)
        = self.divide(new ShortNumber(n));

    [info:"returns the sum"]
    add(int n)
        = new IntNumber(self).add(n);

    [info:"returns the difference"]
    subtract(int n)
        = new IntNumber(self).subtract(n);

    [info:"returns the product"]
    multiply(int n)
        = new IntNumber(self).multiply(n);
        
    [info:"returns the result of integer division"]
    divide(int n)
        = new IntNumber(self).divide(n);

    [info:"returns the sum"]
    add(long n)
        = new LongNumber(self).add(n);

    [info:"returns the difference"]
    subtract(long n)
        = new LongNumber(self).subtract(n);

    [info:"returns the product"]
    multiply(long n)
        = new LongNumber(self).multiply(n);
        
    [info:"returns the result of integer division"]
    divide(long n)
        = new LongNumber(self).divide(n);

    [info:"returns the sum"]
    add(real n)
        = new RealNumber(self).add(n);

    [info:"returns the difference"]
    subtract(real n)
        = new RealNumber(self).subtract(n);

    [info:"returns the product"]
    multipl(real n)
        = new RealNumber(self).multiply(n);
        
    [info:"Returns the result of integer division"]
    divide(real n)
        = new RealNumber(self).divide(n);

    band(n)
        <= band(cast short(n));
        
    bor(n)
        <= bor(cast short(n));
        
    bxor(n)
        <= bxor(cast short(n));
        
//    short and(BaseVariable v)
//        <= and(v.Value);
//        
//    short or(BaseVariable v)
//        <= or(v.Value);
//        
//    short xor(BaseVariable v)
//        <= xor(v.Value);
        
    [info:"Returns the result of bitwise AND operation"]
    short band(short n)
        = theValue & n;

    [info:"Returns the result of bitwise OR operation"]
    short bor(short n)
        = theValue | n;

    [info:"Returns the result of bitwise XOR operation"]
    short bxor(short n)
        = theValue ^ n;
        
    [info:"Returns the result of bitwise AND operation"]
    band(byte n)
        = self.band(new ShortNumber(n));

    [info:"Returns the result of bitwise OR operation"]
    bor(byte n)
        = self.bor(new ShortNumber(n));

    [info:"Returns the result of bitwise XOR operation"]
    bxor(byte n)
        = self.bxor(new ShortNumber(n));
        
    [info:"Returns the result of bitwise AND operation"]
    band(int n)
        = new IntNumber(self).band(n);
        
    [info:"Returns the result of bitwise OR operation"]
    bor(int n)
        = new IntNumber(self).bor(n);
        
    [info:"Returns the result of bitwise XOR operation"]
    bxor(int n)
        = new IntNumber(self).bxor(n);
        
    [info:"Returns the result of bitwise AND operation"]
    band(long n)
        = new LongNumber(self).band(n);
        
    [info:"Returns the result of bitwise OR operation"]
    bor(long n)
        = new LongNumber(self).bor(n);
        
    [info:"Returns the result of bitwise XOR operation"]
    bxor(long n)
        = new LongNumber(self).bxor(n);

    short shiftLeft(int val)
    {
        ^ theValue $shl val
    }

    short shiftRight(int val)
    {
        ^ theValue $shr val
    }
    
    [info:"Bitwise inversion"]
    get short BInverted()
        = ~theValue;
    
    get short Negative()
        = 0 - theValue;

    [info:"Returns the literal presentation"]
    string toPrintable() = stringConvertor.convert(self);

    [info:"Clones the value"]
    short clone() = new ShortNumber(self);
}

// --- IntNumber ---

[info:"A signed 32 bit integer"]
public sealed const struct IntNumber : IntBaseNumber
{
    embeddable __int theValue[4];
    
    [info:"Creates the object with the minimal value"]
    static int MinValue
        = -2147483648;

    [info:"Creates the object with the maximal value"]
    static int MaxValue
        = 2147483647;

    constructor(byte n)
    {
        theValue := n
    }
    
    constructor(short n)
    {
        theValue := n
    }
    
    constructor()
        = 0;
    
    [info:"Creates the object with specified value"]
    constructor(int value)
        = value;

    [info:"Creates the object with specified value"]
    constructor(uint value)
        = value;

    [info:"Returns the value as a short integer"]
    short cast() = shortConvertor.convert(self);

    [info:"Returns the byte value"]
    byte cast() = byteConvertor.convert(self);

    [info:"Returns the long integer"]
    long cast() = new LongNumber(self);

    [info:"Returns the real number"]
    real cast() = new RealNumber(self);

    [info:"Returns the real number"]
    uint cast() = new UIntNumber(self);
    
    bool equal(o)
    {
        var n := cast int(o) ?? nil;
        (nil == n)
            ? { ^ false } : { ^ self.equal(n) }
    }

    bool less(n)
        <= less(cast int(n));
        
//    bool equal(BaseVariable v)
//        <= equal(v.Value);
//        
//    bool less(BaseVariable v)
//        <= less(v.Value);
        
    [info:"Compares the value"]
    bool equal(int n)
        = theValue == n;
        
    [info:"Compares the value"]
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
//        <= add(v.Value);
//
//    int subtract(BaseVariable v)
//        <= subtract(v.Value);
//
//    int multiply(BaseVariable v)
//        <= multiply(v.Value);
//
//    int divide(BaseVariable v)
//        <= divide(v.Value);
        
    [info:"Returns the sum"]
    int add(int n)
        = theValue + n;

    [info:"Returns the sum"]
    int subtract(int n)
        = theValue - n;

    [info:"Returns the product"]
    int multiply(int n)
        = theValue * n;

    [info:"Returns the result of integer division"]
    int divide(int n)
        = theValue / n;

    band(n)
        <= band(cast int(n));
        
    bor(n)
        <= bor(cast int(n));
        
    bxor(n)
        <= bxor(cast int(n));
        
    int band(BaseVariable v)
        <= band(v.Value);
        
    int bor(BaseVariable v)
        <= bor(v.Value);
        
    int bxor(BaseVariable v)
        <= bxor(v.Value);
        
    [info:"Returns the result of bitwise AND operation"]
    int band(int n)
        = theValue & n;

    [info:"Returns the result of bitwise OR operation"]
    int bor(int n)
        = theValue | n;

    [info:"Returns the result of bitwise XOR operation"]
    int bxor(int n)
        = theValue ^ n;

    [info:"Compares the integer value"]
    bool equal(short value)
        = self.equal(new IntNumber(value));

    [info:"Compares the integer value"]
    bool equal(byte value)
        = self.equal(new IntNumber(value));
        
    [info:"Compares the integer value"]
    bool equal(long n)
        = new LongNumber(self).equal(n);

    [info:"Compares the integer value"]
    bool equal(real n)
        = new RealNumber(self).equal(n);

    [info:"Compares the integer value"]
    bool less(short value)
        = self.equal(new IntNumber(value));

    [info:"Compares the integer value"]
    bool less(byte value)
        = self.less(new IntNumber(value));
        
    [info:"Compares the integer value"]
    bool less(long n)
        = new LongNumber(self).less(n);

    [info:"Compares the integer value"]
    bool less(real n)
        = new RealNumber(self).less(n);

    [info:"Returns the sum of integers"]
    add(short value)
        = self.add(new IntNumber(value));
        
    [info:"Returns the sum of integers"]
    add(byte value)
        = self.add(new IntNumber(value));

    [info:"Returns the sum of integers"]
    long add(long n)
        = new LongNumber(self).add(n);

    [info:"Returns the sum of integers"]
    real add(real n)
        = new RealNumber(self).add(n);

    [info:"Returns the difference of integers"]
    subtract(short value)
        = self.subtract(new IntNumber(value));

    [info:"Returns the difference of integers"]
    subtract(byte value)
        = self.subtract(new IntNumber(value));

    [info:"Returns the difference of integers"]
    long subtract(long n)
        = new LongNumber(self).subtract(n);

    [info:"Returns the difference of integers"]
    real subtract(real n)
        = new RealNumber(self).subtract(n);

    [info:"Returns the product of integers"]
    multiply(short value)
        = self.multiply(new IntNumber(value));

    [info:"Returns the product of integers"]
    multiply(byte value)
        = self.multiply(new IntNumber(value));

    [info:"Returns the product of integers"]
    long multiply(long n)
        = new LongNumber(self).multiply(n);

    [info:"Returns the product of integers"]
    real multiply(real n)
        = new RealNumber(self).multiply(n);

    [info:"Returns the result of integer division"]
    divide(short value)
        = self.divide(new IntNumber(value));

    [info:"Returns the result of integer division"]
    divide(byte value)
        = self.divide(new IntNumber(value));        

    [info:"Returns the result of integer division"]
    long divide(long n)
        = new LongNumber(self).divide(n);

    [info:"Returns the result of integer division"]
    real divide(real n)
        = new RealNumber(self).divide(n);

    [info:"Returns the result of bitwise AND operation"]
    band(short n)
        = self.band(new IntNumber(n));

    [info:"Returns the result of bitwise AND operation"]
    band(byte n)
        = self.band(new IntNumber(n));

    [info:"Returns the result of bitwise AND operation"]
    band(long n)
        = new LongNumber(self).band(n);

    [info:"Returns the result of bitwise OR operation"]
    bor(short n)
        = self.bor(new IntNumber(n));

    [info:"Returns the result of bitwise OR operation"]
    bor(byte n)
        = self.bor(new IntNumber(n));

    [info:"Returns the result of bitwise OR operation"]
    bor(long n)
        = new LongNumber(self).bor(n);

    [info:"Returns the result of bitwise XOR operation"]
    bxor(short n)
        = self.bxor(new IntNumber(n));

    [info:"Returns the result of bitwise XOR operation"]
    bxor(byte n)
        = self.bxor(new IntNumber(n));

    [info:"Returns the result of bitwise XOR operation"]
    bxor(long n)
        = new LongNumber(self).bxor(n);

    [info:"Returns the literal presentation"]
    string toPrintable()
        = stringConvertor.convert(self);

    [info:"Clones the value"]
    int clone() = new IntNumber(self);

//    int shiftLeft(BaseVariable v)
//        <= shiftLeft(v.Value);

    int shiftLeft(int val)
    {
        ^ theValue $shl val
    }

//    int shiftRight(BaseVariable v)
//        <= shiftRight(v.Value);

    int shiftRight(int val)
    {
        ^ theValue $shr val
    }
    
    [info:"Bitwise inversion"]
    get int BInverted()
        = ~theValue;
        
    get int Negative()
        = -theValue;
        
    [info:"Returns true if all the mask bits are set"]
    bool allMask(int operand)
    {
        int v := operand & theValue;
        
        ^ v == operand
    }
        
    [info:"Returns true if any of the mask bits are set"]
    bool anyMask(int operand)
    {
        int v := operand & theValue;
        
        ^ v != 0
    }        
}

// --- UIntNumber ---

[info:"A unsigned 32 bit integer"]
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
        theValue := uintConvertor.convert(s)
    }
    
    cast H(string s)
    {
        theValue := uintConvertor.convert(s, 16)
    }
    
    constructor(uint n)
    {
        theValue := n
    }        
        
    constructor()
        = 0;

    [info:"Creates the object with the minimal value"]
    static int MinValue
        = 0;

    [info:"Creates the object with the maximal value"]
    static int MaxValue
        = 0FFFFFFFFh;
        
    [info:"Returns the literal presentation"]
    string toPrintable() = stringConvertor.convert(self);

    [info:"Returns the value as a short integer"]
    short cast() = shortConvertor.convert(self);

    [info:"Returns the byte value"]
    byte cast() = byteConvertor.convert(self);

    int cast() = self;

    [info:"Returns the long integer"]
    long cast() = new LongNumber(self);

    [info:"Returns the real number"]
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
        
//    bool equal(BaseVariable v)
//        <= equal(v.Value);
//        
//    bool less(BaseVariable v)
//        <= less(v.Value);
        
    [info:"Compares the value"]
    bool equal(int n)
        = theValue == n;
        
    [info:"Compares the value"]
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
        
    [info:"Returns the sum"]
    int add(int n)
        = theValue + n;

    [info:"Returns the sum"]
    int subtract(int n)
        = theValue - n;

    [info:"Returns the product"]
    int multiply(int n)
        = theValue * n;

    [info:"Returns the result of integer division"]
    int divide(int n)
        = theValue / n;

    band(n)
        <= band(cast int(n));
        
    bor(n)
        <= bor(cast int(n));
        
    bxor(n)
        <= bxor(cast int(n));
        
//    int and(BaseVariable v)
//        <= and(v.Value);
//        
//    int or(BaseVariable v)
//        <= or(v.Value);
//        
//    int xor(BaseVariable v)
//        <= xor(v.Value);
        
    [info:"Returns the result of bitwise AND operation"]
    int band(int n)
        = theValue & n;

    [info:"Returns the result of bitwise OR operation"]
    int bor(int n)
        = theValue | n;

    [info:"Returns the result of bitwise XOR operation"]
    int bxor(int n)
        = theValue ^ n;

    [info:"Compares the value"]
    bool equal(uint n)
        = theValue == n;
        
    [info:"Compares the value"]
    bool less(uint n)
        = theValue < n;
        
    [info:"Returns the sum"]
    uint add(uint n)
        = theValue + n;

    [info:"Returns the sum"]
    uint subtract(uint n)
        = theValue - n;

    [info:"Returns the product"]
    uint multiply(uint n)
        = theValue * n;

    [info:"Returns the result of integer division"]
    uint divide(uint n)
        = theValue / n;

    [info:"Returns the result of bitwise AND operation"]
    uint band(uint n)
        = theValue & n;

    [info:"Returns the result of bitwise OR operation"]
    uint bor(uint n)
        = theValue | n;

    [info:"Returns the result of bitwise XOR operation"]
    uint bxor(uint n)
        = theValue ^ n;

    [info:"Compares the integer value"]
    bool equal(short value)
        = self.equal(cast int(value));

    [info:"Compares the integer value"]
    bool equal(byte value)
        = self.equal(cast int(value));
        
    [info:"Compares the integer value"]
    bool equal(long n )
        = new LongNumber(self).equal(n);

    [info:"Compares the integer value"]
    bool equal(real n)
        = new RealNumber(self).equal(n);

    [info:"Compares the integer value"]
    bool less(short value)
        = self.equal(cast uint(value));

    [info:"Compares the integer value"]
    bool less(byte value)
        = self.less(cast uint(value));
        
    [info:"Compares the integer value"]
    bool less(long n)
        = new LongNumber(self).less(n);

    [info:"Compares the integer value"]
    bool less(real n)
        = new RealNumber(self).less(n);

    [info:"Returns the sum of integers"]
    add(short value)
        = self.add(cast uint(value));

    [info:"Returns the sum of integers"]
    add(byte value)
        = self.add(cast uint(value));

    [info:"Returns the sum of integers"]
    add(long n)
        = new LongNumber(self).add(n);

    [info:"Returns the sum of integers"]
    add(real n)
        = new RealNumber(self).add(n);

    [info:"Returns the difference of integers"]
    subtract(short value)
        = self.subtract(cast uint(value));

    [info:"Returns the difference of integers"]
    subtract(byte value)
        = self.subtract(cast uint(value));

    [info:"Returns the difference of integers"]
    subtract(long n)
        = new LongNumber(self).subtract(n);

    [info:"Returns the difference of integers"]
    subtract(real n)
        = new RealNumber(self).subtract(n);

    [info:"Returns the product of integers"]
    multiply(short value)
        = self.multiply(cast uint(value));

    [info:"Returns the product of integers"]
    multiply(byte value)
        = self.multiply(cast uint(value));

    [info:"Returns the product of integers"]
    multiply(long n)
        = new LongNumber(self).multiply(n);

    [info:"Returns the product of integers"]
    multiply(real n)
        = new RealNumber(self).multiply(n);

    [info:"Returns the result of integer division"]
    divide(short value)
        = self.divide(cast uint(value));

    [info:"Returns the result of integer division"]
    divide(byte value)
        = self.divide(cast uint(value));

    [info:"Returns the result of integer division"]
    divide(long n)
        = new LongNumber(self).divide(n);

    [info:"Returns the result of integer division"]
    divide(real n)
        = new RealNumber(self).divide(n);

    [info:"Returns the result of bitwise AND operation"]
    band(short value)
        = self.band(cast uint(value));

    [info:"Returns the result of bitwise AND operation"]
    band(byte value)
        = self.band(cast uint(value));

    [info:"Returns the result of bitwise AND operation"]
    band(long value)
        = new LongNumber(self).band(value);

    [info:"Returns the result of bitwise OR operation"]
    bor(short value)
        = self.bor(cast uint(value));

    [info:"Returns the result of bitwise OR operation"]
    bor(byte value)
        = self.bor(cast uint(value));

    [info:"Returns the result of bitwise OR operation"]
    bor(long value)
        = new LongNumber(self).bor(value);

    [info:"Returns the result of bitwise XOR operation"]
    bxor(short value)
        = self.bxor(cast uint(value));

    [info:"Returns the result of bitwise XOR operation"]
    bxor(byte value)
        = self.bxor(cast uint(value));

    [info:"Returns the result of bitwise XOR operation"]
    bxor(long n)
        = new LongNumber(self).bxor(n);

    [info:"Clones the value"]
    uint clone = new UIntNumber(self);

    uint shiftLeft(int val)
    {
        ^ theValue $shl val
    }

    uint shiftRight(int val)
    {
        ^ theValue $shr val
    }
    
    [info:"Bitwise inversion"]
    get uint BInverted()
        = ~theValue;
}

// --- LongNumber ---

[info:"A signed 64 bit integer value"]
public sealed const struct LongNumber : IntBaseNumber
{
    embeddable __int theValue[8];

    [info:"Returns the minimal value"]
    static long MinValue
        = -9223372036854775808l;

    [info:"Returns the maximal value"]
    static long MaxValue
        = 9223372036854775807l;

    [info:"Creates the object with a default value (0)"]
    constructor()
        = 0l;

    [info:"Creates the object with specified value"]
    constructor(long value)
        = value;

    [info:"Creates the object with specified value"]
    constructor(int value)
        = longConvertor.convert(value);

    [info:"Creates the object with specified value"]
    constructor(uint value)
        = longConvertor.convert(value);

    [info:"Creates the object with specified value"]
    constructor(short value)
        = longConvertor.convert(value);

    [info:"Creates the object with specified value"]
    constructor(byte value)
        = longConvertor.convert(value);

    [info:"Returns an integer value"]
    int cast() = intConvertor.convert(self);

    [info:"Returns an integer value"]
    uint cast() = uintConvertor.convert(self);

    [info:"Returns the value as a short integer"]
    short cast() = shortConvertor.convert(self);
    
    [info:"Returns the byte value"]
    byte cast() = byteConvertor.convert(self);

    [info:"Returns a real value"]
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
        
//    bool equal(BaseVariable v)
//        <= equal(v.Value);
//        
//    bool less(BaseVariable v)
//        <= less(v.Value);
        
    [info:"Compares the value"]
    bool equal(long n)
        = theValue == n;
        
    [info:"Compares the value"]
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
        
//    long add(BaseVariable v)
//        <= add(v.Value);
//
//    long subtract(BaseVariable v)
//        <= subtract(v.Value);
//
//    long multiply(BaseVariable v)
//        <= multiply(v.Value);
//
//    long divide(BaseVariable v)
//        <= divide(v.Value);
        
    [info:"Returns the sum"]
    long add(long n)
        = theValue + n;

    [info:"Returns the sum"]
    long subtract(long n)
        = theValue - n;

    [info:"Returns the product"]
    long multiply(long n)
        = theValue * n;

    [info:"Returns the result of integer division"]
    long divide(long n)
        = theValue / n;

    band(n)
        <= band(cast long(n));
        
    bor(n)
        <= bor(cast long(n));
        
    bxor(n)
        <= bxor(cast long(n));
        
//    long and(BaseVariable v)
//        <= and(v.Value);
//        
//    long or(BaseVariable v)
//        <= bor(v.Value);
//        
//    long xor(BaseVariable v)
//        <= xor(v.Value);
        
    [info:"Returns the result of bitwise AND operation"]
    long band(long n)
        = theValue & n;

    [info:"Returns the result of bitwise OR operation"]
    long bor(long n)
        = theValue | n;

    [info:"Returns the result of bitwise XOR operation"]
    long bxor(long n)
        = theValue ^ n;

    [info:"Compares the integer value"]
    bool equal(int n) 
        = self.equal(new LongNumber(n));

    [info:"Compares the integer value"]
    bool equal(short n)
        = self.equal(new LongNumber(n));

    [info:"Compares the integer value"]
    bool equal(byte n)
        = self.equal(new LongNumber(n));

    [info:"Compares the integer value"]
    bool equal(real n)
        = new RealNumber(self).equal(n);

    [info:"Compares the integer value"]
    bool less(int n)
        = self.less(new LongNumber(n));

    [info:"Compares the integer value"]
    bool less(short n)
        = self.less(new LongNumber(n));

    [info:"Compares the integer value"]
    bool less(byte n)
        = self.less(new LongNumber(n));

    [info:"Compares the integer value"]
    bool less(real n)
        = new RealNumber(self).less(n);

    [info:"Returns the sum of integers"]
    add(int n)
        = self.add(new LongNumber(n));

    [info:"Returns the sum of integers"]
    add(short n)
        = self.add(new LongNumber(n));

    [info:"Returns the sum of integers"]
    add(byte n)
        = self.add(new LongNumber(n));

    [info:"Returns the sum of integers"]
    add(real n)
        = new RealNumber(self).add(n);

    [info:"Returns the difference of integers"]
    subtract(int n)
        = self.subtract(new LongNumber(n));

    [info:"Returns the difference of integers"]
    subtract(short n)
        = self.subtract(new LongNumber(n));

    [info:"Returns the difference of integers"]
    subtract(byte n)
        = self.subtract(new LongNumber(n));

    [info:"Returns the difference of integers"]
    subtract(real n)
        = new RealNumber(self).subtract(n);

    [info:"Returns the product of integers"]
    multiply(real n)
        = new RealNumber(self).multiply(n);

    [info:"Returns the product of integers"]
    multiply(int n)
        = self.multiply(new LongNumber(n));

    [info:"Returns the product of integers"]
    multiply(byte n)
        = self.multiply(new LongNumber(n));

    [info:"Returns the product of integers"]
    multiply(short n)
        = self.multiply(new LongNumber(n));

    [info:"Returns the result of integer division"]
    divide(int n)
        = self.divide(new LongNumber(n));

    [info:"Returns the result of integer division"]
    divide(short n)
        = self.divide(new LongNumber(n));

    [info:"Returns the result of integer division"]
    divide(byte n)
        = self.divide(new LongNumber(n));

    [info:"Returns the result of integer division"]
    divide(real n)
        = new RealNumber(self).divide(n);
        
    [info:"Returns the result of bitwise AND operation"]
    band(int n)
        = self.band(new LongNumber(n));

    [info:"Returns the result of bitwise AND operation"]
    band(short n)
        = self.band(new LongNumber(n));

    [info:"Returns the result of bitwise AND operation"]
    band(byte n)
        = self.band(new LongNumber(n));

    [info:"Returns the result of bitwise OR operation"]
    bor(int n)
        = self.bor(new LongNumber(n));

    [info:"Returns the result of bitwise OR operation"]
    bor(short n)
        = self.bor(new LongNumber(n));

    [info:"Returns the result of bitwise OR operation"]
    bor(byte n)
        = self.bor(new LongNumber(n));  

    [info:"Returns the result of bitwise XOR operation"]
    bxor(int n)
        = self.bxor(new LongNumber(n));

    [info:"Returns the result of bitwise XOR operation"]
    bxor(short n)
        = self.bxor(new LongNumber(n));

    [info:"Returns the result of bitwise XOR operation"]
    bxor(byte n)
        = self.bxor(new LongNumber(n));

    [info:"Returns the literal presentation"]
    string toPrintable() = stringConvertor.convert(self);

    [info:"Clones the value"]
    long clone() = new LongNumber(self);
    
    long shiftLeft(int val)
    {
        ^ theValue $shl val
    }

    long shiftRight(int val)
    {
        ^ theValue $shr val
    }
    
    [info:"Bitwise inversion"]
    get long BInverted()
    {
        //!!TODO : implement op
        ^ ~theValue;
        
//        intern system'core_routines'__long.readInvertedTo(theValue, ref long retVal);
//        
//        ^ retVal
    }

    [info:"Negates the value"]
    get long Negative()
    {
        long r := 0l;
        r -= self;
        
        ^ r  
    }
}

// --- realNumber ---

[info:"A 64bit float numeric value"]
public sealed const struct RealNumber : BaseNumber
{
    embeddable __float theValue[8];

    [info:"Returns the minimal value"]
    static real MinValue
        = -1.7976931348623157e+308r;
    
    [info:"Returns the maximal value"]
    static real MaxValue
        = 1.7976931348623157e+308r;
        
    [info:"Returns the negative infinity"]
    static real NegativeInfinity 
        = -1.0r / 0.0r;
        
    [info:"Returns the positive infinity"]
    static real PositiveInfinity 
        = 1.0r / 0.0r;
        
    [info:"returns the NaN"]
    static real NaN
        = 0.0r / 0.0r;

    static real Pi
        = Pi_value;

    constructor(int n)
        = realConvertor.convert(n);
            
    constructor(byte n)
        = realConvertor.convert(n);
            
    constructor(short n)
        = realConvertor.convert(n);
            
    [info:"Creates the object with a default value (0)"]
    constructor()
        = 0.0r;

    [info:"Creates the object with specified value"]
    constructor(real value)
        = value;
  
    [info:"Creates the object with specified value"]
    constructor(uint value)
        = realConvertor.convert(value);

    [info:"Creates the object with specified value"]
    constructor(long value)
        = realConvertor.convert(value);
        
    bool equal(o)
    {
        var n := cast real(o) ?? nil;
        (nil == n)
            ? { ^ false }
            : { ^ self.equal(n) }
    }

    bool less(n)
        <= less(cast real(n));
        
//    bool equal(BaseVariable v)
//        <= equal(v.Value);
//        
//    bool less(BaseVariable v)
//        <= less(v.Value);
        
    [info:"Compares the value"]
    bool equal(real n)
        = theValue == n;
        
    [info:"Compares the value"]
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
//        <= add(v.Value);
//
//    real subtract(BaseVariable v)
//        <= subtract(v.Value);
//
//    real multiply(BaseVariable v)
//        <= multiply(v.Value);
//
//    real divide(BaseVariable v)
//        <= divide(v.Value);
        
    [info:"Returns the sum"]
    real add(real n)
        = theValue + n;

    [info:"Returns the sum"]
    real subtract(real n)
        = theValue - n;

    [info:"Returns the product"]
    real multiply(real n)
        = theValue * n;

    [info:"Returns the result of integer division"]
    real divide(real n)
        = theValue / n;
        
    [info:"Compares the numeric values"]
    bool equal(int n)
        = self.equal(new RealNumber(n));

    [info:"Compares the numeric values"]
    bool equal(byte n)
        = self.equal(new RealNumber(n));

    [info:"Compares the numeric values"]
    bool equal(short n)
        = self.equal(new RealNumber(n));

    [info:"Compares the numeric values"]
    bool equal(long n)
        = self.equal(new RealNumber(n));

    [info:"Compares the numeric values"]
    bool less(int n)
        = self.less(new RealNumber(n));

    [info:"Compares the numeric values"]
    bool less(byte n)
        = self.less(new RealNumber(n));

    [info:"Compares the numeric values"]
    bool less(short n)
        = self.less(new RealNumber(n));

    [info:"Compares the numeric values"]
    bool less(long n)
        = self.less(new RealNumber(n));

    [info:"Returns the sum of numbers"]
    real add(int n)
    {
        real r := n;
        
        ^ self.add(r)
    }
    
    [info:"Returns the sum of numbers"]
    real add(byte n)
        = self.add(new RealNumber(n));

    [info:"Returns the sum of numbers"]
    real add(short n)
        = self.add(new RealNumber(n));

    [info:"Returns the sum of numbers"]
    real add(long n)
        = self.add(new RealNumber(n));

    [info:"Returns the difference of numbers"]
    real subtract(int n)
        = self.subtract(new RealNumber(n));

    [info:"Returns the difference of numbers"]
    real subtract(byte n)
        = self.subtract(new RealNumber(n));

    [info:"Returns the difference of numbers"]
    real subtract(short n)
        = self.subtract(new RealNumber(n));

    [info:"Returns the difference of numbers"]
    real subtract(long n)
        = self.subtract(new RealNumber(n));

    [info:"Returns the product of numbers"]
    real multiply(int n)
    {
        real r := n;
        
        ^ self.multiply(r)
    }

    [info:"Returns the product of numbers"]
    real multiply(byte n)
        = self.multiply(new RealNumber(n));

    [info:"Returns the product of numbers"]
    real multiply(short n)
        = self.multiply(new RealNumber(n));

    [info:"Returns the product of numbers"]
    real multiply(long n)
        = self.multiply(new RealNumber(n));

    [info:"Returns the result of the division"]
    real divide(int n)
    {
        real r := n;
        
        ^ self.divide(r)
    }

    [info:"Returns the result of the division"]
    real divide(byte n)
        = self.divide(new RealNumber(n));
        
    [info:"Returns the result of the division"]
    real divide(short n)
        = self.divide(new RealNumber(n));

    [info:"Returns the result of the division"]
    real divide(long n)
        = self.divide(new RealNumber(n));

    [info:"Returns the literal presentation"]
    string toPrintable() = stringConvertor.convert(self);

    [info:"Clones the value"]
    real clone = new RealNumber(self);
    
    [info:"Negates the value"]
    Negative
        = 0.0r - theValue;
}

// --- CharValue ---

[info:"An UTF-32 character symbol"]
public sealed const struct CharValue : BaseValue
{
    embeddable __raw theValue[4];

    [info:"Returns the minimal possible character value (0)"]
    static char MinValue
        = $0;

    [info:"Returns the maximal possible character value (010FFFFh)"]
    static char MaxValue
        = $010FFFFh;

    [info:"Creates the object with a default value (0)"]
    constructor()
        = /*$0*/0;

    [info:"Creates the object with specified value"]
    constructor(char value)
        = value;

    [info:"Creates the object with specified numeric value"]
    constructor load(int n)
        = charConvertor.convert(n);

    get internal Value()
    {
        int val := intConvertor.convert(self);
        
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
        
//    bool equal(BaseVariable v)
//        <= equal(v.Value);
//        
//    bool less(BaseVariable v)
//        <= less(v.Value);
        
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

    [info:"Clones the value"]
    char clone() = new CharValue(self);

    [info:"Returns the literal value"]
    string toPrintable() = stringConvertor.convert(self);

    [info:"Returns the literal value"]
    string cast() = stringConvertor.convert(self);
    
    [info:"Returns the wide literal value"]
    wide cast() = wideConvertor.convert(self);

    [info:"Retuns the number of bytes to represent the symbol in UTF-8"]
    get int Length()
    {
        intern system'core_routines'__char.readLengthTo(self, ref int retVal);
        
        ^ retVal
    }
    
    [info:"Reads the number of bytes to represent the symbol in UTF-16"]
    get int WideLength()
    {
        intern system'core_routines'__char.readWideLengthTo(self, ref int retVal);

        ^ retVal
    }
}

// --- nil constant ---

[info:"A nil value"]
public singleton nilValue
{
    [info:"Returns true if the specified object is nil."]
    bool equal(object)
    {
        (nil == object)
            ? { ^ true };
        
        (super.equal:object)
            ? { ^ true };

        ^ false
    }
    
    [info:"Returns nil<br/>Any typecasting message will be handled and nil value returned"]
    cast generic() = nil;
}

// === Basic Factory symbols ==

public class BaseExtender
{
    object theObject;
    
    [info:"Returns true if the specified object is equal to the current object; otherwise, false."]
    bool equal(object)
        => theObject;

    string toPrintable()
        => theObject;
                  
    dispatch() => theObject;
}

[info:"A generic variable.<br/>Extends an assigned value"]
public sealed class Variable : BaseVariable, ref<object>
{
    [info:"Creates the empty variable"]
    constructor()
    {
    }        
        
    [info:"Creates the variable"]
    constructor(value)
    {
        this value := value
    }
    
    append(val)
    {
        (nil == value)
            ? { value := val }
            : { value := value + val }
    }
    
    reduce(val)
    {
        (nil == value)
            ? { system'InvalidOperationException.raise() }
            : { value := value - val }
    }

    multiplyBy(val)
    {
        (nil == value)
            ? { system'InvalidOperationException.raise() }
            : { value := value * val }
    }

    divideBy(val)
    {
        (nil == value)
            ? { system'InvalidOperationException.raise() }
            : { value := value / val }
    }
}

// --- KeyValue ---

[info:"A key - object pair<br/>Extends the object with a key property"]
public sealed class KeyValue
{
    object theKey;
    object theObject;
    
    [info:"Creates a new object"]
    constructor new(key, object)
    {
        theKey := key;
        theObject := object
    }
    
    [info:"Returns the key"]
    Key = theKey;
    
    [info:"Returns the key"]
    Value = theObject;

    set Value(object)
    {
        theObject := object
    }
           
    bool equal(o)
        => theObject;
                                                                                      
    string toPrintable()
        => theObject;
                                                                                      
    [info:"Redirects the incoming messages to the object"]
    dispatch() => theObject;
}

// --- DTO --

public class DTO
{
    const MessageName[] __dto_properties;
    
    sealed static MessageName[] __dto_properties()
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
        <= convert(v.Value);
                        
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
        intern system'core_routines'__byteconvertor.convert(s, 10, ref byte retVal);
        
        ^ retVal
    }
                
    /// Converts a wide literal value into a byte value
    byte convert(wide s)
    {
        intern system'core_routines'__byteconvertor.convert(s, 10, ref byte retVal);
        
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
        intern system'core_routines'__shortconvertor.convert(s, 10, ref short retVal);
        
        ^ retVal
    }
        
    /// Converts a wide literal string into a short integer value
    short convert(wide s)
    {
        intern system'core_routines'__shortconvertor.convert(s, 10, ref short retVal);
        
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
        intern system'core_routines'__intconvertor.convert(s, 10, ref int retVal);
        
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
        intern system'core_routines'__intconvertor.convert(s, 10, ref int retVal);
        
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
        intern system'core_routines'__intconvertor.convertUnsigned(s, 10, ref int retVal);
        
        ^ retVal
    }    
    
    /// Converts a literal string into an unsgined integer
    uint convert(string s, int radix)
    {
        intern system'core_routines'__intconvertor.convertUnsigned(s, radix, ref int retVal);
        
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
        intern system'core_routines'__longconvertor.convert(s, 10, ref long retVal);
        
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
        intern system'core_routines'__longconvertor.convert(s, 10, ref long retVal);
        
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
        intern system'core_routines'__realconvertor.convert(s, 19, ref real retVal);
        
        ^ retVal
    }
        
    /// Converts a wide literal string into a real number
    real convert(wide s)
    {
        intern system'core_routines'__realconvertor.convert(s, 19, ref real retVal);
        
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
            int odd := n & 1;
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
        };
        
        ^ retVal
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
    
    real e()
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
        int v := *self;
        v := v + n;
        
        self.Value := v
    }
    
    reduce(int n)
    {
        int v := *self;
        v := v - n;
        
        self.Value := v
    }
}

public extension realRefOp : ref<real>
{
    append(real n)
    {
        real v := *self;
        v := v + n;
        
        self.Value := v
    }
    
    reduce(real n)
    {
        real v := *self;
        v := v - n;
        
        self.Value := v
    }
}
