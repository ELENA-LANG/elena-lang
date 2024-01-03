
// --- BaseValue ---
public abstract class BaseValue
   : info("a base value")
{
   bool equal(BaseVariable var)
            : info("Returns true if a value of var is equal to the object value; otherwise, false")
      <= equal(*var);

   bool less(BaseVariable var)
            : info("Returns true if the object value is less than a value of var; otherwise, false")
      <= less(*var);

   abstract bool less(v)
            : info("Returns true if the object value is less than o; otherwise, false;<br/>should be overridden");

   bool greater(v)
            : info("Returns true if the object value is greater than o; otherwise, false;<br/>by default sends less[2] to o with the object as an argument")
      = v.less(self);

   bool notless(v) 
            : info("Returns true if the object value is not less than o; otherwise, false;<br/>by default inverts the result of less[2] operation")
      = !self.less(v);

   bool notgreater(v) 
            : info("Returns true if the object value is not greater than o; otherwise, false;<br/>by default inverts the result of greater[2] operation")
      = !self.greater(v);
}

// --- BoolValue ---
public interface BoolValue
   : info("a common boolean value")
{
   private constructor() {}

   static bool MinValue
                  : info("Returns the default value")
      = BoolValue.false;

   abstract if(trueAction)
                  : info("Executes trueAction if the value is true;<br/>has to be implemented");

   abstract if(trueAction,falseAction)
                  : info("Executes trueAction if the value is true or falseAction otherwise;<br/>has to be implemented");

   abstract ifnot(trueAction)
                  : info("Executes an action if the value is false;<br/>has to be implemented");

   abstract iif(object trueVal, object falseVal)
                  : info("Returns trueVal if the value is true or falseVal otherwise;<br/>has to be implemented");

   abstract bool equal(bool f)
                  : info("Determines whether the specified object is equal to the current object boolean value;<br/>has to be implemented");

   abstract bool notequal(bool f);

   abstract bool or(expr)
                  : info("Executes logical OR operation<;br/>has to be implemented");

   abstract bool and(expr)
                  : info("Executes logical AND operation;<br/>has to be implemented");

   abstract bool xor(expr)
                  : info("Executes logical XOR operation;<br/>has to be implemented");

   get abstract bool Inverted()
                  : info("Inverts the value;<br/>has to be implemented");

   abstract bool clone()
                  : info("clones the value;<br/>has to be implemented");

   const static bool true : info("Returns true value")
      = BoolValue
      {
         if(trueAction)
                  : info("Executes trueAction.")
            = trueAction();

         if(trueAction,elseAction)
                  : info("Executes trueAction.")
            = trueAction();

         ifnot(trueAction)
                  : info("Always returns nil.")
            = nil;

         iif(object trueVal, object falseVal) 
                  : info("Always returns trueVal.")
            = trueVal;

         bool equal(bool b)
                  : info("Determines whether the specified object is equal to the current object boolean value.")
            = b;

         bool notequal(bool b)
            = !b;

         bool or(expr)
                  : info("Executes logical OR operation;<br/>Always return true")
            = this self;

         bool and(expr)
                  : info("executes logical AND operation.")
            = expr;

         bool xor(expr)
                  : info("Executes logical XOR operation.")
         {
            cast bool(expr) ? { ^ BoolValue.false };
         }

         bool Inverted 
                  : info("Always returns false.")
            = BoolValue.false;

         string toPrintable()
                  : info("Returns literal representation.")
            = "true";

         bool clone()
            = this self;
      };

   const static bool false
                  : info("Returns false value.")
      = BoolValue
      {
         bool Inverted
                  : info("Always returns true")
            = BoolValue.true;

         if(trueAction)
                  : info("Always returns nil")
            = nil;

         if(trueAction,elseAction)
                  : info("Executes else-action")
            = elseAction();

         ifnot(trueAction)
                  : info("Executes the action")
            = trueAction();

         iif(object trueVal, object falseVal)
                  : info("Always returns falseVal.")
            = falseVal;

         bool equal(bool b)
                  : info("Determines whether the specified object is equal to the current object boolean value")
            = !b;

         bool notequal(bool b)
            = b; 

         bool or(expr)
                  : info("Executes logical OR operation")
            = expr;

         bool and(expr)
                  : info("Executes logical AND operation;<br/>Always return false")
            = this self;

         bool xor(expr)
                  : info("Executes logical XOR operation")
         {
            cast bool(expr) ? { ^ BoolValue.true };
            
            ^ this self
         }

         string toPrintable()
                  : info("Returns literal representation")
            = "false";

         bool clone()
            = this self;
   };
}

public const true = BoolValue.true;
public const false = BoolValue.false;

// --- BaseNumber ---

public abstract class BaseNumber : BaseValue,
   info("a base numeric value")
{
   add(BaseVariable var)
      <= add(*var);

   subtract(BaseVariable var)
      <= subtract(*var);

   multiply(BaseVariable var)
      <= multiply(*var);

   divide(BaseVariable var)
      <= divide(*var);

   abstract add(o)
      : info("Adds the value;<br/>should be overridden");

   abstract subtract(o)
      : info("Subtracts the value;<br/>should be overridden");

   abstract multiply(o)
      : info("Multiplies by the value;<br/>should be overridden");

   abstract divide(o)
      : info("Divides by the value;<br/>should be overridden");

   Negative
         : info("Returns the negative value of a number")
      = 0 - self;
}

// --- IntBaseNumber ---

public abstract class IntBaseNumber : BaseNumber,
   info("a base integer value")
{
   band(BaseVariable var)
      <= band(*var);

   bor(BaseVariable var)
      <= bor(*var);

   bxor(BaseVariable var)
      <= bxor(*var);

   abstract band(o)
      : info("Executes bitwise AND operation;<br/>should be overridden");

   abstract bor(o)
      : info("Executes bitwise OR operation;<br/>should be overridden");

   abstract bxor(o)
      : info("Executes bitwise XOR operation;<br/>should be overridden");

   bool allMask(operand)
         : info("Returns true if all the mask bits are set")
      = self.band(operand) == operand;

   bool anyMask(operand)
         : info("Returns true if any of the mask bits are set")
      = 0 != self.band(operand);

   get abstract BInverted();
}

// --- ByteNumber ---

public const struct ByteNumber : IntBaseNumber,
   info("an unsigned 8 bit integer")
{
   embeddable __uint _value[1];

   static byte MinValue
            : info("returns the minimal value (0)")
      = 0;

   static byte MaxValue
            : info("Returns the maxial value (255)")
      = 255;

   static byte Default
      = 0;

   constructor()
            : info("Returns the default value")
      = 0;

   constructor(byte b)
            : info("Creates the object with a specified value")
      = b;

   constructor(int n)
            : info("Creates the object with a specified value")
      = byteConvertor.convert(n);

   add(o)
     <= add(cast byte(o));

   subtract(o)
     <= subtract(cast byte(o));

   multiply(o)
     <= multiply(cast byte(o));

   divide(o)
     <= divide(cast byte(o));

   byte add(byte n)
            : info("Returns the sum")
      = _value + n;   

   byte subtract(byte n)
            : info("Returns the difference")
      = _value - n;   

   byte multiply(byte n)
            : info("Returns the product")
      = _value * n;   

   byte divide(byte n)
            : info("Returns the result of integer division")
      = _value / n;   

   short add(short n)
            : info("Returns the sum")
      = new ShortNumber(_value).add(n);   

   short subtract(short n)
            : info("Returns the difference")
      = new ShortNumber(_value).subtract(n);   

   short multiply(short n)
            : info("Returns the product")
      = new ShortNumber(_value).multiply(n);   

   short divide(short n)
            : info("Returns the result of integer division")
      = new ShortNumber(_value).divide(n);   

   int add(int n)
            : info("Returns the sum")
      = new IntNumber(_value).add(n);   

   int subtract(int n)
            : info("Returns the difference")
      = new IntNumber(_value).subtract(n);   

   int multiply(int n)
            : info("Returns the product")
      = new IntNumber(_value).multiply(n);   

   int divide(int n)
            : info("Returns the result of integer division")
      = new IntNumber(_value).divide(n);   

   int add(long n)
            : info("Returns the sum")
      = new LongNumber(_value).add(n);   

   int subtract(long n)
            : info("Returns the difference")
      = new LongNumber(_value).subtract(n);   

   int multiply(long n)
            : info("Returns the product")
      = new LongNumber(_value).multiply(n);   

   int divide(long n)
            : info("Returns the result of integer division")
      = new LongNumber(_value).divide(n);   

   real add(real n)
            : info("Returns the sum")
      = new RealNumber(_value).add(n);   

   real subtract(real n)
            : info("Returns the difference")
      = new RealNumber(_value).subtract(n);   

   real multiply(real n)
            : info("Returns the product")
      = new RealNumber(_value).multiply(n);   

   real divide(real n)
            : info("Returns the result of integer division")
      = new RealNumber(_value).divide(n);   

   bool equal(o)
   {
      if (o; is byte b)
      {  
         ^ _value == b
      };

      ^ false
   }

   bool less(o)
      <= less(cast byte(o));

   bool equal(byte b)
            : info("Compares the value")
      = _value == b;

   bool less(byte b)
      = _value < b;

   bool equal(short s)
            : info("Compares the value")
      = new ShortNumber(_value).equal(s);

   bool less(short s)
      = new ShortNumber(_value).less(s);

   bool equal(int n)
            : info("Compares the value")
      = new IntNumber(_value).equal(n);

   bool less(int n)
            : info("Compares the value")
      = new IntNumber(_value).less(n);

   bool equal(long n)
            : info("Compares the value")
      = new LongNumber(self).equal(n);

   bool less(long n)
            : info("Compares the value")
      = new LongNumber(self).less(n);

   bool equal(real n)
            : info("Compares the value")
      = new RealNumber(_value).equal(n);

   bool less(real n)
            : info("Compares the value")
      = new RealNumber(_value).less(n);

   band(n)
      <= band(cast byte(n));

   bor(n)
      <= bor(cast byte(n));

   bxor(n)
      <= bxor(cast byte(n));

   byte band(byte n)
            : info("Returns the result of bitwise AND operation")
      = _value & n;

   byte bor(byte n)
            : info("Returns the result of bitwise OR operation")
      = _value | n;

   byte bxor(byte n)
            : info("Returns the result of bitwise XOR operation")
      = _value ^ n;

   short band(short n)
            : info("Returns the result of bitwise AND operation")
      = new ShortNumber(n).band(n);

   short bor(short n)
            : info("Returns the result of bitwise OR operation")
      = new ShortNumber(n).bor(n);

   short bxor(short n)
            : info("Returns the result of bitwise XOR operation")
      = new ShortNumber(n).bxor(n);

   int band(int n)
            : info("Returns the result of bitwise AND operation")
      = new IntNumber(n).band(n);

   int bor(int n)
            : info("Returns the result of bitwise OR operation")
      = new IntNumber(n).bor(n);

   int bxor(int n)
            : info("Returns the result of bitwise XOR operation")
      = new IntNumber(n).bxor(n);

   long band(long n)
            : info("Returns the result of bitwise AND operation")
      = new LongNumber(n).band(n);

   long bor(long n)
            : info("Returns the result of bitwise OR operation")
      = new LongNumber(n).bor(n);

   long bxor(long n)
            : info("Returns the result of bitwise XOR operation")
      = new LongNumber(n).bxor(n);

   short cast()
            : info("Returns the value as a short integer")
      = shortConvertor.convert(self);

   int cast()
            : info("returns the value as an integer")
      = intConvertor.convert(self);

   uint cast()
            : info("returns the value as an unsigned integer")
      = uintConvertor.convert(self);

   long cast()
            : info("Returns the value as a long integer")
      = longConvertor.convert(self);

   real cast()
            : info("Returns the value as a real number")
      = realConvertor.convert(self);

   byte shiftLeft(int val)
            : info("shifts an integer value to the left by a specified number of bits")
      = _value $shl val;

   byte shiftRight(int val)
            : info("shifts an integer value to the right by a specified number of bits")
      = _value $shr val;

   get byte BInverted()
            : info("bitwise inversion")
      = ~_value;

   string toPrintable()
            : info("returns the literal presentation")
      = stringConvertor.convert(self, 10);

   byte clone()
            : info("clones the value")
      = new ByteNumber(self);
}

// --- SByteNumber ---

public const struct SByteNumber : IntBaseNumber,
   info("a signed 8 bit integer")
{
   embeddable __int _value[1];

   static sbyte MinValue
            : info("returns the minimal value (-127)")
      = -128;

   static sbyte MaxValue
            : info("Returns the maxial value (128)")
      = 127;

   static sbyte Default
      = 0;

   constructor()
            : info("Returns the default value")
      = 0;

   constructor(sbyte b)
            : info("Creates the object with a specified value")
      = b;

   constructor(int n)
            : info("Creates the object with a specified value")
      = sbyteConvertor.convert(n);

   add(o)
     <= add(cast sbyte(o));

   subtract(o)
     <= subtract(cast sbyte(o));

   multiply(o)
     <= multiply(cast sbyte(o));

   divide(o)
     <= divide(cast sbyte(o));

   sbyte add(sbyte n)
            : info("Returns the sum")
      = _value + n;   

   sbyte subtract(sbyte n)
            : info("Returns the difference")
      = _value - n;   

   sbyte multiply(sbyte n)
            : info("Returns the product")
      = _value * n;   

   sbyte divide(sbyte n)
            : info("Returns the result of integer division")
      = _value / n;   

   short add(short n)
            : info("Returns the sum")
      = new ShortNumber(self).add(n);   

   short subtract(short n)
            : info("Returns the difference")
      = new ShortNumber(self).subtract(n);   

   short multiply(short n)
            : info("Returns the product")
      = new ShortNumber(self).multiply(n);   

   short divide(short n)
            : info("Returns the result of integer division")
      = new ShortNumber(self).divide(n);   

   int add(int n)
            : info("Returns the sum")
      = new IntNumber(self).add(n);   

   int subtract(int n)
            : info("Returns the difference")
      = new IntNumber(self).subtract(n);   

   int multiply(int n)
            : info("Returns the product")
      = new IntNumber(self).multiply(n);   

   int divide(int n)
            : info("Returns the result of integer division")
      = new IntNumber(self).divide(n);   

   int add(long n)
            : info("Returns the sum")
      = new LongNumber(self).add(n);   

   int subtract(long n)
            : info("Returns the difference")
      = new LongNumber(self).subtract(n);   

   int multiply(long n)
            : info("Returns the product")
      = new LongNumber(self).multiply(n);   

   int divide(long n)
            : info("Returns the result of integer division")
      = new LongNumber(self).divide(n);   

   real add(real n)
            : info("Returns the sum")
      = new RealNumber(self).add(n);   

   real subtract(real n)
            : info("Returns the difference")
      = new RealNumber(self).subtract(n);   

   real multiply(real n)
            : info("Returns the product")
      = new RealNumber(self).multiply(n);   

   real divide(real n)
            : info("Returns the result of integer division")
      = new RealNumber(self).divide(n);   

   bool equal(o)
   {
      if (o; is sbyte n)
      {
         ^ _value == n;
      };

      ^ false
   }

   bool less(o)
      <= less(cast sbyte(o));

   bool equal(sbyte b)
            : info("Compares the value")
      = _value == b;

   bool less(sbyte b)
      = _value < b;

   bool equal(short s)
            : info("Compares the value")
      = new ShortNumber(self).equal(s);

   bool less(short s)
      = new ShortNumber(self).less(s);

   bool equal(int n)
            : info("Compares the value")
      = new IntNumber(self).equal(n);

   bool less(int n)
            : info("Compares the value")
      = new IntNumber(self).less(n);

   bool equal(long n)
            : info("Compares the value")
      = new LongNumber(self).equal(n);

   bool less(long n)
            : info("Compares the value")
      = new LongNumber(self).less(n);

   bool equal(real n)
            : info("Compares the value")
      = new RealNumber(self).equal(n);

   bool less(real n)
            : info("Compares the value")
      = new RealNumber(self).less(n);

   band(n)
      <= band(cast sbyte(n));

   bor(n)
      <= bor(cast sbyte(n));

   bxor(n)
      <= bxor(cast sbyte(n));

   sbyte band(sbyte n)
            : info("Returns the result of bitwise AND operation")
      = _value & n;

   sbyte bor(sbyte n)
            : info("Returns the result of bitwise OR operation")
      = _value | n;

   sbyte bxor(sbyte n)
            : info("Returns the result of bitwise XOR operation")
      = _value ^ n;

   short band(short n)
            : info("Returns the result of bitwise AND operation")
      = new ShortNumber(n).band(n);

   short bor(short n)
            : info("Returns the result of bitwise OR operation")
      = new ShortNumber(n).bor(n);

   short bxor(short n)
            : info("Returns the result of bitwise XOR operation")
      = new ShortNumber(n).bxor(n);

   int band(int n)
            : info("Returns the result of bitwise AND operation")
      = new IntNumber(n).band(n);

   int bor(int n)
            : info("Returns the result of bitwise OR operation")
      = new IntNumber(n).bor(n);

   int bxor(int n)
            : info("Returns the result of bitwise XOR operation")
      = new IntNumber(n).bxor(n);

   long band(long n)
            : info("Returns the result of bitwise AND operation")
      = new LongNumber(n).band(n);

   long bor(long n)
            : info("Returns the result of bitwise OR operation")
      = new LongNumber(n).bor(n);

   long bxor(long n)
            : info("Returns the result of bitwise XOR operation")
      = new LongNumber(n).bxor(n);

   short cast()
            : info("Returns the value as a short integer")
      = shortConvertor.convert(self);

   int cast()
            : info("returns the value as an integer")
      = intConvertor.convert(self);

   uint cast()
            : info("returns the value as an unsigned integer")
      = uintConvertor.convert(self);

   long cast()
            : info("Returns the value as a long integer")
      = longConvertor.convert(self);

   real cast()
            : info("Returns the value as a real number")
      = realConvertor.convert(self);

   sbyte shiftLeft(int val)
            : info("shifts an integer value to the left by a specified number of bits")
      = _value $shl val;

   sbyte shiftRight(int val)
            : info("shifts an integer value to the right by a specified number of bits")
      = _value $shr val;

   get sbyte BInverted()
            : info("bitwise inversion")
      = ~_value;

   string toPrintable()
            : info("returns the literal presentation")
      = stringConvertor.convert(self, 10);

   sbyte clone()
            : info("clones the value")
      = new SByteNumber(self);
}

// --- ShortNumber ---

public const struct ShortNumber : IntBaseNumber,
         info("a signed 16 bit integer")
{
   embeddable __int _value[2];

   static short MinValue
            : info("Returns the minimal value (-32768)")
      = -32768;

   static short MaxValue
            : info("Returns the maxial value (32767)")
      = 32767;

   static short Default
      = 0;

   constructor()
            : info("creates the object with a default value (0)")
      = 0;

   constructor(short s)
            : info("creates the object with specified value")
      = s;

   constructor(byte b)
            : info("creates the object with specified value")
      = shortConvertor.convert(b);

   constructor(sbyte b)
            : info("creates the object with specified value")
      = shortConvertor.convert(b);

   constructor(int n)
      = shortConvertor.convert(n);

   add(o)
     <= add(cast short(o));

   subtract(o)
     <= subtract(cast short(o));

   multiply(o)
     <= multiply(cast short(o));

   divide(o)
     <= divide(cast short(o));

   short add(short n)
            : info ("returns the sum")
      = _value + n;   

   short subtract(short n)
            : info ("returns the difference")
      = _value - n;   

   short multiply(short n)
            : info ("returns the product")
      = _value * n;   

   short divide(short n)
            : info("returns the result of integer division")
      = _value / n;   

   short add(byte n)
            : info ("returns the sum")
      <= add(new ShortNumber(n));

   short subtract(byte n)
            : info ("returns the difference")
      <= subtract(new ShortNumber(n));

   short multiply(byte n)
            : info ("returns the product")
      <= multiply(new ShortNumber(n));

   short divide(byte n)
            : info("returns the result of integer division")
      <= divide(new ShortNumber(n));

   int add(int n)
            : info("returns the sum")
      = new IntNumber(_value).add(n);   

   int subtract(int n)
            : info ("returns the difference")
      = new IntNumber(_value).subtract(n);   

   int multiply(int n)
            : info ("returns the product")
      = new IntNumber(_value).multiply(n);   

   int divide(int n)
            : info ("returns the result of integer division")
      = new IntNumber(_value).divide(n);   

   long add(long n)
            : info ("returns the sum")
      = new LongNumber(_value).add(n);   

   long subtract(long n)
            : info ("returns the difference")
      = new LongNumber(_value).subtract(n);   

   long multiply(long n)
            : info ("returns the product")
      = new LongNumber(_value).multiply(n);   

   long divide(long n)
            : info ("returns the result of integer division")
      = new LongNumber(_value).divide(n);   

   real add(real n)
            : info ("returns the sum")
      = new RealNumber(_value).add(n);   

   real subtract(real n)
            : info ("returns the difference")
      = new RealNumber(_value).subtract(n);   

   real multiply(real n)
            : info ("returns the product")
      = new RealNumber(_value).multiply(n);   

   real divide(real n)
            : info ("returns the result of integer division")
      = new RealNumber(_value).divide(n);   

   bool equal(o)
   {
      if (o; is short n)
      {
         ^ _value == n
      };

      ^ false
   }

   band(n)
      <= band(cast short(n));

   bor(n)
      <= bor(cast short(n));

   bxor(n)
      <= bxor(cast short(n));

   short band(short n)
            : info("Returns the result of bitwise AND operation")
      = _value & n;

   short bor(short n)
            : info("Returns the result of bitwise OR operation")
      = _value | n;

   short bxor(short n)
            : info("Returns the result of bitwise XOR operation")
      = _value ^ n;

   short band(byte n)
            : info("Returns the result of bitwise AND operation")
      <= band(new ShortNumber(n));

   short bor(byte n)
            : info("Returns the result of bitwise OR operation")
      <= bor(new ShortNumber(n));

   short bxor(byte n)
            : info("Returns the result of bitwise XOR operation")
      <= bxor(new ShortNumber(n));

   int band(int n)
            : info("Returns the result of bitwise AND operation")
      = new IntNumber(self).band(n);

   int bor(int n)
            : info("Returns the result of bitwise OR operation")
      = new IntNumber(self).bor(n);

   int bxor(int n)
            : info("Returns the result of bitwise XOR operation")
      = new IntNumber(self).bxor(n);

   long band(long n)
            : info("Returns the result of bitwise AND operation")
      = new LongNumber(self).band(n);

   long bor(long n)
            : info("Returns the result of bitwise OR operation")
      = new LongNumber(self).bor(n);

   long bxor(long n)
            : info("Returns the result of bitwise XOR operation")
      = new LongNumber(self).bxor(n);

   bool less(o)
      <= less(cast short(o));

   bool equal(short b)
         : info("compares the value")
      = _value == b;

   bool less(short b)
         : info("compares the value")
      = _value < b;

   bool equal(byte b)
         : info("compares the value")
      <= equal(new ShortNumber(b));

   bool less(byte b)
         : info("compares the value")
      <= less(new ShortNumber(b));

   bool equal(int n)
         : info("compares the value")
      = new IntNumber(_value).equal(n);

   bool less(int n)
         : info("compares the value")
      = new IntNumber(_value).less(n);

   bool equal(long n)
         : info("compares the value")
      = new LongNumber(_value).equal(n);

   bool less(long n)
         : info("compares the value")
      = new LongNumber(_value).less(n);

   bool equal(real n)
         : info("compares the value")
      = new RealNumber(_value).equal(n);

   bool less(real n)
         : info("compares the value")
      = new RealNumber(_value).less(n);

   byte cast()
         : info("Returns the byte value") 
      = byteConvertor.convert(self);

   int cast()
            : info("Returns the integer value") 
      = intConvertor.convert(self);

   uint cast()
            : info("Returns the unsigned integer value") 
      = uintConvertor.convert(self);

   long cast()
            : info("Returns the long integer value") 
      = longConvertor.convert(self);

   real cast()
            : info("Returns the real number value") 
      = realConvertor.convert(self);

   string toPrintable()
      = stringConvertor.convert(self, 10);

   short shiftLeft(int val)
            : info("shifts an integer value to the left by a specified number of bits")
      = _value $shl val;

   short shiftRight(int val)
            : info("shifts an integer value to the right by a specified number of bits")
      = _value $shr val;

   get short BInverted()
            : info("bitwise inversion")
      = ~_value;

   get short Negative()
//      = -_value;
   {
      short v := 0;
      v -= _value;

      ^ v
   }

   short clone()
            : info("clones the value")
      = new ShortNumber(self);
}

// --- UShortNumber ---

public const struct UShortNumber : IntBaseNumber,
         info("an unsigned 16 bit integer")
{
   embeddable __uint _value[2];

   static ushort MinValue
            : info("Returns the minimal value (0)")
      = 0;

   static ushort MaxValue
            : info("Returns the maxial value (65535)")
      = 65535;

   static ushort Default
      = 0;

   constructor()
            : info("creates the object with a default value (0)")
      = 0;

   constructor(ushort s)
            : info("creates the object with specified value")
      = s;

   constructor(short s)
            : info("creates the object with specified value")
      = ushortConvertor.convert(s);

   constructor(byte b)
            : info("creates the object with specified value")
      = ushortConvertor.convert(b);

   constructor(int n)
      = ushortConvertor.convert(n);

   add(o)
     <= add(cast ushort(o));

   subtract(o)
     <= subtract(cast ushort(o));

   multiply(o)
     <= multiply(cast ushort(o));

   divide(o)
     <= divide(cast ushort(o));

   ushort add(ushort n)
            : info ("returns the sum")
      = _value + n;   

   ushort subtract(ushort n)
            : info ("returns the difference")
      = _value - n;   

   ushort multiply(ushort n)
            : info ("returns the product")
      = _value * n;   

   ushort divide(ushort n)
            : info("returns the result of integer division")
      = _value / n;

   ushort add(byte n)
            : info ("returns the sum")
      <= add(new UShortNumber(n));

   ushort subtract(byte n)
            : info ("returns the difference")
      <= subtract(new UShortNumber(n));

   ushort multiply(byte n)
            : info ("returns the product")
      <= multiply(new UShortNumber(n));

   ushort divide(byte n)
            : info("returns the result of integer division")
      <= divide(new UShortNumber(n));

   int add(int n)
            : info("returns the sum")
      = new IntNumber(_value).add(n);   

   int subtract(int n)
            : info ("returns the difference")
      = new IntNumber(_value).subtract(n);   

   int multiply(int n)
            : info ("returns the product")
      = new IntNumber(_value).multiply(n);   

   int divide(int n)
            : info ("returns the result of integer division")
      = new IntNumber(_value).divide(n);   

   long add(long n)
            : info ("returns the sum")
      = new LongNumber(_value).add(n);   

   long subtract(long n)
            : info ("returns the difference")
      = new LongNumber(_value).subtract(n);   

   long multiply(long n)
            : info ("returns the product")
      = new LongNumber(_value).multiply(n);   

   long divide(long n)
            : info ("returns the result of integer division")
      = new LongNumber(_value).divide(n);   

   real add(real n)
            : info ("returns the sum")
      = new RealNumber(_value).add(n);   

   real subtract(real n)
            : info ("returns the difference")
      = new RealNumber(_value).subtract(n);   

   real multiply(real n)
            : info ("returns the product")
      = new RealNumber(_value).multiply(n);   

   real divide(real n)
            : info ("returns the result of integer division")
      = new RealNumber(_value).divide(n);   

   bool equal(o)
   {
      if (o; is ushort n)
      {
         ^ _value == n
      };

      ^ false
   }

   band(n)
      <= band(cast ushort(n));

   bor(n)
      <= bor(cast ushort(n));

   bxor(n)
      <= bxor(cast ushort(n));

   ushort band(ushort n)
            : info("Returns the result of bitwise AND operation")
      = _value & n;

   ushort bor(ushort n)
            : info("Returns the result of bitwise OR operation")
      = _value | n;

   ushort bxor(ushort n)
            : info("Returns the result of bitwise XOR operation")
      = _value ^ n;

   ushort band(byte n)
            : info("Returns the result of bitwise AND operation")
      <= band(new UShortNumber(n));

   ushort bor(byte n)
            : info("Returns the result of bitwise OR operation")
      <= bor(new UShortNumber(n));

   ushort bxor(byte n)
            : info("Returns the result of bitwise XOR operation")
      <= bxor(new UShortNumber(n));

   int band(int n)
            : info("Returns the result of bitwise AND operation")
      = new IntNumber(self).band(n);

   int bor(int n)
            : info("Returns the result of bitwise OR operation")
      = new IntNumber(self).bor(n);

   int bxor(int n)
            : info("Returns the result of bitwise XOR operation")
      = new IntNumber(self).bxor(n);

   long band(long n)
            : info("Returns the result of bitwise AND operation")
      = new LongNumber(self).band(n);

   long bor(long n)
            : info("Returns the result of bitwise OR operation")
      = new LongNumber(self).bor(n);

   long bxor(long n)
            : info("Returns the result of bitwise XOR operation")
      = new LongNumber(self).bxor(n);

   bool less(o)
      <= less(cast ushort(o));

   bool equal(ushort b)
         : info("compares the value")
      = _value == b;

   bool less(ushort b)
         : info("compares the value")
      = _value < b;

   bool equal(byte b)
         : info("compares the value")
      <= equal(new UShortNumber(b));

   bool less(byte b)
         : info("compares the value")
      <= less(new UShortNumber(b));

   bool equal(int n)
         : info("compares the value")
      = new IntNumber(_value).equal(n);

   bool less(int n)
         : info("compares the value")
      = new IntNumber(_value).less(n);

   bool equal(long n)
         : info("compares the value")
      = new LongNumber(_value).equal(n);

   bool less(long n)
         : info("compares the value")
      = new LongNumber(_value).less(n);

   bool equal(real n)
         : info("compares the value")
      = new RealNumber(_value).equal(n);

   bool less(real n)
         : info("compares the value")
      = new RealNumber(_value).less(n);

   byte cast()
         : info("Returns the byte value") 
      = byteConvertor.convert(self);

   short cast()
            : info("Returns the signed short value") 
      = shortConvertor.convert(self);

   int cast()
            : info("Returns the integer value") 
      = intConvertor.convert(self);

   uint cast()
            : info("Returns the unsigned integer value") 
      = uintConvertor.convert(self);

   long cast()
            : info("Returns the long integer value") 
      = longConvertor.convert(self);

   real cast()
            : info("Returns the real number value") 
      = realConvertor.convert(self);

   string toPrintable()
      = stringConvertor.convert(self, 10);

   ushort shiftLeft(int val)
            : info("shifts an integer value to the left by a specified number of bits")
      = _value $shl val;

   ushort shiftRight(int val)
            : info("shifts an integer value to the right by a specified number of bits")
      = _value $shr val;

   get ushort BInverted()
            : info("bitwise inversion")
      = ~_value;

   get ushort Negative()
//      = -_value;
   {
      ushort v := 0;
      v -= _value;

      ^ v
   }

   ushort clone()
            : info("clones the value")
      = new UShortNumber(self);
}

// --- IntNumber ---

public const struct IntNumber : IntBaseNumber,
   info("A signed 32 bit integer")
{
   embeddable __int _value[4];

   static int MinValue
            : info("Creates the object with the minimal value")
      = -2147483648;

   static int MaxValue
            : info("Creates the object with the maximal value")
      = 2147483647;

   static int Default
      = 0;

   constructor()
      = 0;

   constructor(int n)
            : info("Creates the object with specified value")
      = n;

   constructor(uint n)
            : info("Creates the object with specified value")
      = intConvertor.convert(n);

   constructor(byte b)
      = b;

   constructor(short s)
      = s;

   bool equal(o)
   {
      if (o; is int n)
      {
         ^ _value == n
      };

      ^ false
   }

   bool less(o)
      <= less(cast int(o));

   bool equal(byte b)
            : info("Compares the integer value")
      <= equal(new IntNumber(b));

   bool equal(long n)
            : info("Compares the integer value")
      = new LongNumber(_value).equal(n);

   bool equal(real n)
            : info("Compares the integer value")
   {
      real val := _value;

      ^ val == n;
   }

   bool less(byte b)
            : info("Compares the integer value")
      <= less(new IntNumber(b));

   bool equal(short s)
            : info("Compares the integer value")
      <= equal(new IntNumber(s));

   bool less(short s)
            : info("Compares the integer value")
      <= less(new IntNumber(s));

   bool less(long n)
            : info("Compares the integer value")
      = new LongNumber(_value).less(n);

   bool less(real n)
            : info("Compares the integer value")
   {
      real val := _value;

      ^ val < n;
   }

   bool equal(int n)
      = _value == n; 

   bool notequal(int n)
      = _value != n;

   bool less(int n)
      = _value < n;

   add(o)
     <= add(cast int(o));

   subtract(o)
     <= subtract(cast int(o));

   multiply(o)
     <= multiply(cast int(o));

   divide(o)
     <= divide(cast int(o));

   int add(int n)
            : info("Returns the sum")
      = _value + n;   

   int subtract(int n)
            : info("Returns the difference")
      = _value - n;   

   int multiply(int n)
            : info("Returns the product")
      = _value * n;   

   int divide(int n)
            : info("Returns the result of integer division")
      = _value / n;   

   int add(byte n)
            : info("Returns the sum")
      <= add(new IntNumber(n));

   int subtract(byte n)
            : info("Returns the difference")
      <= subtract(new IntNumber(n));

   int multiply(byte n)
            : info("Returns the product")
      <= multiply(new IntNumber(n));

   int divide(byte n)
            : info("Returns the result of integer division")
      <= divide(new IntNumber(n));

   int add(short n)
            : info("Returns the sum")
      <= add(new IntNumber(n));

   int subtract(short n)
            : info("Returns the difference")
      <= subtract(new IntNumber(n));

   int multiply(short n)
            : info("Returns the product")
      <= multiply(new IntNumber(n));

   int divide(short n)
            : info("Returns the result of integer division")
      <= divide(new IntNumber(n));

   long add(long n)
            : info("Returns the sum")
      = new LongNumber(self).add(n);

   long subtract(long n)
            : info("Returns the difference")
      = new LongNumber(self).subtract(n);

   long multiply(long n)
            : info("Returns the product")
      = new LongNumber(self).multiply(n);

   long divide(long n)
            : info("Returns the result of integer division")
      = new LongNumber(self).divide(n);

   real add(real n)
            : info("Returns the sum")
   {
      real val := _value;

      val := val + n;

      ^ val         
   }

   real subtract(real n)
            : info("Returns the difference")
   {
      real val := _value;

      val := val - n;

      ^ val         
   }

   real multiply(real n)
            : info("Returns the product")
   {
      real val := _value;

      val := val * n;

      ^ val         
   }

   real divide(real n)
            : info("Returns the result of integer division")
   {
      real val := _value;

      val := val / n;

      ^ val         
   }

   band(n)
      <= band(cast int(n));

   bor(n)
      <= bor(cast int(n));

   bxor(n)
      <= bxor(cast int(n));

   band(BaseVariable v)
      <= band(*v);

   bor(BaseVariable v)
      <= bor(*v);

   bxor(BaseVariable v)
      <= bxor(*v);

   int band(int n)
            : info("Returns the result of bitwise AND operation")
      = _value & n;

   int bor(int n)
            : info("Returns the result of bitwise OR operation")
      = _value | n;

   int bxor(int n)
            : info("Returns the result of bitwise XOR operation")
      = _value ^ n;

   int band(byte n)
            : info("Returns the result of bitwise AND operation")
      <= band(new IntNumber(n));

   int bor(byte n)
            : info("Returns the result of bitwise OR operation")
      <= bor(new IntNumber(n));

   int bxor(byte n)
            : info("Returns the result of bitwise XOR operation")
      <= bxor(new IntNumber(n));

   int band(short n)
            : info("Returns the result of bitwise AND operation")
      <= band(new IntNumber(n));

   int bor(short n)
            : info("Returns the result of bitwise OR operation")
      <= bor(new IntNumber(n));

   int bxor(short n)
            : info("Returns the result of bitwise XOR operation")
      <= bxor(new IntNumber(n));

   long band(long n)
            : info("Returns the result of bitwise AND operation")
      = new LongNumber(self).band(n);

   long bor(long n)
            : info("Returns the result of bitwise OR operation")
      = new LongNumber(self).bor(n);

   long bxor(long n)
            : info("Returns the result of bitwise XOR operation")
      = new LongNumber(self).bxor(n);

   byte cast()
            : info("Returns the byte value")
      = byteConvertor.convert(self);

   short cast()
            : info("Returns the value as a short integer")
      = shortConvertor.convert(self);

   long cast()
            : info("Returns the long integer")
      = longConvertor.convert(self);

   real cast()
            : info("Returns the real number")
      = realConvertor.convert(self);

   uint cast()
            : info("Returns the unsigned integer")
      = new UIntNumber(self);

   string toPrintable()
      = stringConvertor.convert(self, 10);

   int clone()
            : info("Clones the value")
      = new IntNumber(_value);

   int shiftLeft(int val)
      = _value $shl val;
   
   int shiftRight(int val)
      = _value $shr val;

   get int BInverted()
            : info("Bitwise inversion")
      = ~_value;

   get int Negative()
      = -_value;

   bool allMask(int operand)
         : info("Returns true if all the mask bits are set")
   {
      int v := operand & _value;
        
      ^ v == operand
   }
        
   bool anyMask(int operand)
         : info("Returns true if any of the mask bits are set")
   {
      int v := operand & _value;
        
      ^ v != 0
   }        
}

// --- UIntNumber ---

public sealed const struct UIntNumber : IntBaseNumber,
   info("A unsigned 32 bit integer")
{
   embeddable __uint _value[4];

   constructor(byte b)
   {
      _value := uintConvertor.convert(b);
   }

   constructor(short s)
   {
      _value := uintConvertor.convert(s);
   }

   constructor(int n)
   {
      _value := uintConvertor.convert(n);
   }

   constructor(uint n)
   {
      _value := n
   }

   constructor()
      = 0;

   cast u(string s)
   {
      _value := uintConvertor.convert(s);
   }

   cast H(string s)
   {
      _value := uintConvertor.convert(s, 16);
   }

   static uint MinValue
            : info("Creates the object with the minimal value")
      = 0;

   static uint MaxValue
            : info("Creates the object with the maximal value")
      = 0FFFFFFFFh;

   static uint Default
      = 0;

   string toPrintable()
            : info("Returns the literal presentation")
         = stringConvertor.convert(self, 16);

   short cast()
            : info("Returns the value as a short integer")
      = shortConvertor.convert(self);

   byte cast()
            : info("Returns the byte value")
      = byteConvertor.convert(self);

   int cast()
      = self;
   
   long cast()
            : info("Returns the long integer")
      = new LongNumber(self);

   real cast()
            : info("Returns the real number")
      = new RealNumber(self);

   bool equal(o)
   {
      if (o; is uint n)
      {
         ^ _value == n
      };

      ^ false
   }

   bool less(n)
      <= less(cast uint(n));

   bool equal(uint n)
            : info("Compares the value")
      = _value == n;

   bool less(uint n)
            : info("Compares the value")
      = _value < n;

   bool equal(int n)
            : info("Compares the value")
      <= equal(cast uint(n));

   bool less(int n)
            : info("Compares the value")
      <= less(cast uint(n));

   bool equal(short n)
            : info("Compares the value")
      = self.equal(new UIntNumber(n));

   bool equal(byte n)
            : info("Compares the value")
      = self.equal(new UIntNumber(n));

   bool less(byte n)
            : info("Compares the value")
      = self.less(new UIntNumber(n));

   bool equal(long n)
            : info("Compares the value")
      = new LongNumber(self).equal(n);

   bool less(long n)
            : info("Compares the value")
      = new LongNumber(self).less(n);

   bool equal(real n)
            : info("Compares the value")
      = new RealNumber(self).equal(n);

   bool less(real n)
            : info("Compares the value")
      = new RealNumber(self).less(n);

   add(n)
      <= add(cast uint(n));

   subtract(n)
      <= subtract(cast uint(n));

   multiply(n)
      <= multiply(cast uint(n));

   divide(n)
      <= divide(cast uint(n));

   uint add(BaseVariable v)
      <= add(*v);

   uint subtract(BaseVariable v)
      <= subtract(*v);

   uint multiply(BaseVariable v)
      <= multiply(*v);

   uint divide(BaseVariable v)
      <= divide(*v);

   uint add(uint n)
            : info("Returns the sum")
      = _value + n;

   uint subtract(uint n)
            : info("Returns the difference")
      = _value - n;

   uint multiply(uint n)
            : info("Returns the product")
      = _value * n;

   uint divide(uint n)
            : info("Returns the result of integer division")
      = _value / n;

   int add(int n)
            : info("Returns the sum")
      <= add(cast uint(n));

   int subtract(int n)
            : info("Returns the difference")
      <= subtract(cast uint(n));

   int multiply(int n)
            : info("Returns the product")
      <= multiply(cast uint(n));

   int divide(int n)
            : info("Returns the result of integer division")
      <= divide(cast uint(n));

   int add(byte n)
            : info("Returns the sum")
      <= add(cast uint(n));

   int subtract(byte n)
            : info("Returns the difference")
      <= subtract(cast uint(n));

   int multiply(byte n)
            : info("Returns the product")
      <= multiply(cast uint(n));

   int divide(byte n)
            : info("Returns the result of integer division")
      <= divide(cast uint(n));

   int add(short n)
            : info("Returns the sum")
      <= add(cast uint(n));

   int subtract(short n)
            : info("Returns the difference")
      <= subtract(cast uint(n));

   int multiply(short n)
            : info("Returns the product")
      <= multiply(cast uint(n));

   int divide(short n)
            : info("Returns the result of integer division")
      <= divide(cast uint(n));

   long add(long n)
      = new LongNumber(n).add(n);

   long subtract(long n)
      = new LongNumber(n).subtract(n);

   long multiply(long n)
      = new LongNumber(n).multiply(n);

   long divide(long n)
      = new LongNumber(n).divide(n);

   real add(real n)
      = new RealNumber(n).add(n);

   real subtract(real n)
      = new RealNumber(n).subtract(n);

   real multiply(real n)
      = new RealNumber(n).multiply(n);

   real divide(real n)
      = new RealNumber(n).divide(n);

   band(n)
      <= band(cast uint(n));

   bor(n)
      <= bor(cast uint(n));

   bxor(n)
      <= bxor(cast uint(n));

   uint band(uint n)
            : info("Returns the result of bitwise AND operation")
      = _value & n;

   uint bor(uint n)
            : info("Returns the result of bitwise OR operation")
      = _value | n;

   uint bxor(uint n)
            : info("Returns the result of bitwise XOR operation")
      = _value ^ n;

   int band(int n)
            : info("Returns the result of bitwise AND operation")
      <= band(cast uint(n));

   int bor(int n)
            : info("Returns the result of bitwise OR operation")
      <= bor(cast uint(n));

   int bxor(int n)
            : info("Returns the result of bitwise XOR operation")
      <= bxor(cast uint(n));

   uint band(short n)
            : info("Returns the result of bitwise AND operation")
      <= band(cast uint(n));

   uint bor(short n)
            : info("Returns the result of bitwise OR operation")
      <= bor(cast uint(n));

   uint bxor(short n)
            : info("Returns the result of bitwise XOR operation")
      <= bxor(cast uint(n));

   uint band(byte n)
            : info("Returns the result of bitwise AND operation")
      <= band(cast uint(n));

   uint bor(byte n)
            : info("Returns the result of bitwise OR operation")
      <= bor(cast uint(n));

   uint bxor(byte n)
            : info("Returns the result of bitwise XOR operation")
      <= bxor(cast uint(n));

   long band(long n)
            : info("Returns the result of bitwise AND operation")
      = new LongNumber(n).band(n);

   long bor(long n)
            : info("Returns the result of bitwise OR operation")
      = new LongNumber(n).bor(n);

   long bxor(long n)
            : info("Returns the result of bitwise XOR operation")
      = new LongNumber(n).bxor(n);

   uint shiftLeft(uint val)
      = _value $shl val;

   uint shiftLeft(int val)
      <= shiftLeft(cast uint(val));

   uint shiftRight(uint val)
      = _value $shr val;

   uint shiftRight(int val)
      <= shiftRight(cast uint(val));
   
   get uint BInverted()
            : info("Bitwise inversion")
      = ~_value;

   uint clone()
      = new UIntNumber(self);
}

// --- LongNumber ---

public sealed const struct LongNumber : IntBaseNumber,
   info("A signed 64 bit integer value")
{
   embeddable __int _value[8];

   static long MinValue
            : info("Returns the minimal value")
      = -9223372036854775808l;

   static long MaxValue
            : info("Returns the maximal value")
      = 9223372036854775807l;

   static long Default
      = 0l;

   constructor()
            : info("Creates the object with a default value (0)")
      = 0;

   constructor(long n)
            : info("Creates the object with specified value")
      = n;

   constructor(int n)
            : info("Creates the object with specified value")
      = longConvertor.convert(n);

   constructor(uint n)
            : info("Creates the object with specified value")
      = longConvertor.convert(n);

   constructor(short n)
            : info("Creates the object with specified value")
      = longConvertor.convert(n);

   constructor(byte b)
            : info("Creates the object with specified value")
      = longConvertor.convert(b);

   int cast()
            : info("Returns an integer value")
      = intConvertor.convert(self);

   uint cast()
            : info("Returns an integer value")
      = uintConvertor.convert(self);

   short cast()
            : info("Returns an integer value")
      = shortConvertor.convert(self);

   byte cast()
            : info("Returns an integer value")
      = byteConvertor.convert(self);

   real cast()
            : info("Returns a real value")
      = realConvertor.convert(self);

   bool equal(o)
   {
      if (o; is long n)
      {
         ^ _value == n
      };

      ^ false
   }

   bool less(o)
      <= less(cast long(o));

   bool equal(long n)
            : info("Compares the value")
      = _value == n;

   bool less(long n)
            : info("Compares the value")
      = _value < n;

   bool equal(byte b)
      <= equal(cast long(b));

   bool less(byte b)
      <= less(cast long(b));

   bool equal(short s)
      <= equal(cast long(s));

   bool less(short s)
      <= less(cast long(s));

   bool equal(int n)
      <= equal(cast long(n));

   bool less(int n)
      <= less(cast long(n));

   bool equal(real n)
      = new RealNumber(self).equal(n);

   bool less(real n)
      = new RealNumber(self).less(n);

   add(n)
      <= add(cast long(n));

   subtract(n)
      <= subtract(cast long(n));

   multiply(n)
      <= multiply(cast long(n));

   divide(n)
      <= divide(cast long(n));

   long add(long n)
      = _value + n;

   long subtract(long n)
      = _value - n;

   long multiply(long n)
      = _value * n;

   long divide(long n)
      = _value / n;

   long add(byte n)
      <= add(cast long(n));

   long subtract(byte n)
      <= subtract(cast long(n));

   long multiply(byte n)
      <= multiply(cast long(n));

   long divide(byte n)
      <= divide(cast long(n));

   long add(short n)
      <= add(cast long(n));

   long subtract(short n)
      <= subtract(cast long(n));

   long multiply(short n)
      <= multiply(cast long(n));

   long divide(short n)
      <= divide(cast long(n));

   long add(int n)
      <= add(cast long(n));

   long subtract(int n)
      <= subtract(cast long(n));

   long multiply(int n)
      <= multiply(cast long(n));

   long divide(int n)
      <= divide(cast long(n));

   long add(uint n)
      <= add(cast long(n));

   long subtract(uint n)
      <= subtract(cast long(n));

   long multiply(uint n)
      <= multiply(cast long(n));

   long divide(uint n)
      <= divide(cast long(n));

   real add(real n)
      = new RealNumber(self).add(n);

   real subtract(real n)
      = new RealNumber(self).subtract(n);

   real multiply(real n)
      = new RealNumber(self).multiply(n);

   real divide(real n)
      = new RealNumber(self).divide(n);

   band(n)
      <= band(cast long(n));

   bor(n)
      <= band(cast long(n));

   bxor(n)
      <= band(cast long(n));

   long band(long n)
      = _value & n;

   long bor(long n)
      = _value | n;

   long bxor(long n)
      = _value ^ n;

   long band(byte n)
      <= band(cast long(n));

   long bor(byte n)
      <= bor(cast long(n));

   long bxor(byte n)
      <= bxor(cast long(n));

   long band(short n)
      <= band(cast long(n));

   long bor(short n)
      <= bor(cast long(n));

   long bxor(short n)
      <= bxor(cast long(n));

   long band(int n)
      <= band(cast long(n));

   long bor(int n)
      <= bor(cast long(n));

   long bxor(int n)
      <= bxor(cast long(n));

   long band(uint n)
      <= band(cast long(n));

   long bor(uint n)
      <= bor(cast long(n));

   long bxor(uint n)
      <= bxor(cast long(n));

   long shiftLeft(int val)
      = _value $shl val;
   
   long shiftRight(int val)
      = _value $shr val;

   get long BInverted()
            : info("Bitwise inversion")
      = ~_value;

   get long Negative()
      = -1l * _value;

   string toPrintable()
      = stringConvertor.convert(self, 10);

   long clone()
      = new LongNumber(_value);
}

// --- RealNumber ---

public sealed const struct RealNumber : BaseNumber,
      info("A 64bit float numeric value")
{
   embeddable __float _value[8];

   static real MinValue
            : info("Returns the minimal value")
      = -1.7976931348623157e+308r;

   static real MaxValue
            : info("Returns the maximal value")
      = 1.7976931348623157e+308r;

   static real Default
      = 0.0;

   static real NegativeInfinity 
            : info("Returns the negative infinity")
      = -1.0 / 0.0;   

   static real PositiveInfinity 
            : info("Returns the positive infinity")
      = 1.0 / 0.0;   

   static real NaN 
            : info("Returns the positive infinity")
      = 0.0 / 0.0;   

   static real Pi
      = Pi_value;

   constructor(int n)
      = realConvertor.convert(n);
   
   constructor(byte n)
      = realConvertor.convert(n);
   
   constructor(short n)
      = realConvertor.convert(n);

   constructor()
            : info("Creates the object with a default value (0)")
      = 0.0;

   constructor(real r)
            : info("Creates the object with specified value")
      = r;
   
   constructor(uint n)
            : info("Creates the object with specified value")
      = realConvertor.convert(n);

   constructor(long n)
            : info("Creates the object with specified value")
      = realConvertor.convert(n);

   bool equal(o)
   {
      if (o; is real r)
      {
         ^ _value == r
      };

       ^ false
   }

   bool less(n)
      <= less(cast real(n));

   bool equal(real n)
            : info("Compares the value")
      = _value == n;
   
   bool less(real n)
            : info("Compares the value")
      = _value < n;

   add(n)
      <= add(cast real(n));

   subtract(n)
      <= subtract(cast real(n));

   multiply(n)
      <= multiply(cast real(n));

   divide(n)
      <= divide(cast real(n));

   real add(real n)
            : info("Returns the sum")
      = _value + n;

   real subtract(real n)
            : info("Returns the difference")
      = _value - n;

   real multiply(real n)
            : info("Returns the product")
      = _value * n;

   real divide(real n)
            : info("Returns the result of integer division")
      = _value / n;

   bool equal(int n)
            : info("Returns the result of integer division")
      = self.equal(new RealNumber(n));

   bool equal(byte n)
            : info("Returns the result of integer division")
      = self.equal(new RealNumber(n));

   bool equal(short n)
            : info("Returns the result of integer division")
      = self.equal(new RealNumber(n));

   bool equal(long n)
            : info("Returns the result of integer division")
      = self.equal(new RealNumber(n));

   bool less(int n)
            : info("Returns the result of integer division")
      = self.less(new RealNumber(n));

   bool less(byte n)
            : info("Returns the result of integer division")
      = self.less(new RealNumber(n));

   bool less(short n)
            : info("Returns the result of integer division")
      = self.less(new RealNumber(n));

   bool less(long n)
            : info("Returns the result of integer division")
      = self.less(new RealNumber(n));

   real add(int n)
            : info("Returns the sum of numbers")
      = self.add(new RealNumber(n));

   real add(long n)
            : info("Returns the sum of numbers")
      = self.add(new RealNumber(n));

   real add(byte n)
            : info("Returns the sum of numbers")
      = self.add(new RealNumber(n));

   real add(short n)
            : info("Returns the sum of numbers")
      = self.add(new RealNumber(n));

   real subtract(int n)
            : info("Returns the difference of numbers")
      = self.subtract(new RealNumber(n));

   real subtract(long n)
            : info("Returns the difference of numbers")
      = self.subtract(new RealNumber(n));

   real subtract(byte n)
            : info("Returns the difference of numbers")
      = self.subtract(new RealNumber(n));

   real subtract(short n)
            : info("Returns the difference of numbers")
      = self.subtract(new RealNumber(n));

   real multiply(int n)
            : info("Returns the product of numbers")
      = self.multiply(new RealNumber(n));

   real multiply(long n)
            : info("Returns the product of numbers")
      = self.multiply(new RealNumber(n));

   real multiply(byte n)
            : info("Returns the product of numbers")
      = self.multiply(new RealNumber(n));

   real multiply(short n)
            : info("Returns the product of numbers")
      = self.multiply(new RealNumber(n));

   real divide(int n)
            : info("Returns the result of the division")
      = self.divide(new RealNumber(n));

   real divide(long n)
            : info("Returns the result of the division")
      = self.divide(new RealNumber(n));

   real divide(byte n)
            : info("Returns the result of the division")
      = self.divide(new RealNumber(n));

   real divide(short n)
            : info("Returns the result of the division")
      = self.divide(new RealNumber(n));

   string toPrintable()
            : info("Returns the literal presentation")
      = stringConvertor.convert(self);

   real clone()
            : info("Clones the value")
      = new RealNumber(self);

   get real Negative()
            : info("Negates the value")
      = 0.0 - _value;
}

public real Pi_value = PrimitiveRealOperations.getPi();

public real E_value = PrimitiveRealOperations.getE();

// --- CharValue ---

public const struct CharValue : BaseValue,
   info("An UTF-32 character symbol")
{
   embeddable __word _value[4];

   static char MinValue
            : info("Returns the minimal possible character value (0)")
      = $0;

   static char MaxValue
            : info("Returns the maximal possible character value (010FFFFh)")
//      = $010FFFFh;
      = $1114111;

   static char Default
      = $0;

   constructor()
            : info("Creates the object with a default value (0)")
      = $0;

   constructor(char value)
            : info("Creates the object with specified value")
      = value;

   constructor load(int n)
            : info("Creates the object with specified numeric value")
      = charConvertor.convert(n);

   get internal int Value()
   {
      int val := intConvertor.convert(self);

      ^ val
   }

   get int Length()
         : info("Retuns the number of bytes to represent the symbol in UTF-8") 
   {
      int val := _value;
      int len := UTFOperations.getCharLength(val);
      
      ^ len
   }

   get int WideLength()
         : info("Retuns the number of bytes to represent the symbol in UTF-16") 
   {
      int len := UTFOperations.getCharWideLength(_value);
      
      ^ len
   }

   bool equal(o)
   {
      var ch := cast char(o) ?? nil;
      if (ch == nil) {
         ^ false
      };

      ^ self.equal(ch)
   }

   bool less(o)
      <= less(cast char(o));

   bool equal(char val)
      = _value == val;

   bool less(char val)
      = _value < val;

   bool equal(int n)
      = _value == n;

   bool equal(string s)
   {
      int sLen := s.Length;
      int chLen := self.Length;
      
      if (sLen == chLen)
      {
         char ch := s[0];

         ^ self == ch    
      };

      ^ false
   }

   bool equal(wide s)
   {
      int sLen := s.Length;
      int chLen := self.WideLength;
      
      if (sLen == chLen)
      {
         char ch := s[0];

         ^ self == ch    
      };

      ^ false
   }

   bool less(string s)
         => cast string(s);

   bool less(wide s)
         => cast wide(s);

   char clone()
            : info("Clones the value")
      = new CharValue(self);

   string toPrintable()
      = stringConvertor.convert(self);

   string cast()
      = stringConvertor.convert(self); 

   wide cast()
      = wideConvertor.convert(self); 
}

// --- nil constant ---

public singleton nilValue
   : info("A nil value")
{
   bool equal(object)
      : info("Returns true if the specified object is nil.")
   {
      if (nil == object)
      {
         ^ true
      };

      if (super.equal(object))
      {
         ^ true
      };

      ^ false
   }

   cast generic() 
            : info("Returns nil<br/>Any typecasting message will be handled and nil value returned")
      = nil;
}

// --- KeyValue ---

public sealed class KeyValue
   : info("A key - object pair<br/>Extends the object with a key property")
{
   object key;
   object value;

   constructor new(key, value)
      : info("Creates a new object")
   {
      this key := key;
      this value := value
   }

   Key : info("Returns the key")
      = key;

   Value : info("Returns the value")
      = value;

   set Value(value)
   {
      this value := value
   }

   bool equal(o)
      => value;

   string toPrintable()
      => value;

   dispatch() : info("Redirects the incoming messages to the object")
     => value;
}
