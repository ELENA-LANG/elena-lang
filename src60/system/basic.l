
// --- BaseValue ---
public abstract class BaseValue
   : info<"a base value">
{
   bool equal(BaseVariable var)
            : info<"Returns true if a value of var is equal to the object value; otherwise, false">
      <= equal(*var);

   bool less(BaseVariable var)
            : info<"Returns true if the object value is less than a value of var; otherwise, false">
      <= less(*var);

   abstract bool less(v)
            : info<"Returns true if the object value is less than o; otherwise, false;<br/>should be overridden">;

   bool greater(v)
            : info<"Returns true if the object value is greater than o; otherwise, false;<br/>by default sends less[2] to o with the object as an argument">
      = v.less(self);

   bool notless(v) 
            : info<"Returns true if the object value is not less than o; otherwise, false;<br/>by default inverts the result of less[2] operation">
      = !(self.less(v));

   bool notgreater(v) 
            : info<"Returns true if the object value is not greater than o; otherwise, false;<br/>by default inverts the result of greater[2] operation">
      = !(self.greater(v));
}

// --- BoolValue ---
public interface BoolValue
   : info<"a common boolean value">
{
   private constructor() {}

   static bool MinValue
                  : info<"Returns the default value">
      = BoolValue.false;

   abstract if(trueAction)
                  : info<"Executes trueAction if the value is true;<br/>has to be implemented">;

   abstract if(trueAction,falseAction)
                  : info<"Executes trueAction if the value is true or falseAction otherwise;<br/>has to be implemented">;

   abstract ifnot(trueAction)
                  : info<"Executes an action if the value is false;<br/>has to be implemented">;

   abstract iif(object trueVal, object falseVal)
                  : info<"Returns trueVal if the value is true or falseVal otherwise;<br/>has to be implemented">;

   abstract bool equal(bool f)
                  : info<"Determines whether the specified object is equal to the current object boolean value;<br/>has to be implemented">;

   abstract bool notequal(bool f);

   abstract bool or(expr)
                  : info<"Executes logical OR operation<;br/>has to be implemented">;

   abstract bool and(expr)
                  : info<"Executes logical AND operation;<br/>has to be implemented">;

   abstract bool xor(expr)
                  : info<"Executes logical XOR operation;<br/>has to be implemented">;

   get abstract bool Inverted()
                  : info<"Inverts the value;<br/>has to be implemented">;

   const static bool true : info<"Returns true value">
      = BoolValue
      {
         if(trueAction)
                  : info<"Executes trueAction.">
            = trueAction();

         if(trueAction,elseAction)
                  : info<"Executes trueAction.">
            = trueAction();

         ifnot(trueAction)
                  : info<"Always returns nil.">
            = nil;

         iif(object trueVal, object falseVal) 
                  : info<"Always returns trueVal.">
            = trueVal;

         bool equal(bool b)
                  : info<"Determines whether the specified object is equal to the current object boolean value.">
            = b;

         bool notequal(bool b)
            = !b;

         bool or(expr)
                  : info<"Executes logical OR operation;<br/>Always return true">
            = this self;

         bool and(expr)
                  : info<"executes logical AND operation.">
            = expr;

         bool xor(expr)
                  : info<"Executes logical XOR operation.">
         {
            cast bool(expr) ? { ^ BoolValue.false };
         }

         bool Inverted 
                  : info<"Always returns false.">
            = BoolValue.false;

         string toPrintable()
                  : info<"Returns literal representation.">
            = "true";
      };

   const static bool false
                  : info<"Returns false value.">
      = BoolValue
      {
         bool Inverted
                  : info<"Always returns true">
            = BoolValue.true;

         if(trueAction)
                  : info<"Always returns nil">
            = nil;

         if(trueAction,elseAction)
                  : info<"Executes else-action">
            = elseAction();

         ifnot(trueAction)
                  : info<"Executes the action">
            = trueAction();

         iif(object trueVal, object falseVal)
                  : info<"Always returns falseVal.">
            = falseVal;

         bool equal(bool b)
                  : info<"Determines whether the specified object is equal to the current object boolean value">
            = !b;

         bool notequal(bool b)
            = b; 

         bool or(expr)
                  : info<"Executes logical OR operation">
            = expr;

         bool and(expr)
                  : info<"Executes logical AND operation;<br/>Always return false">
            = this self;

         bool xor(expr)
                  : info<"Executes logical XOR operation">
         {
            cast bool(expr) ? { ^ BoolValue.true };
            
            ^ this self
         }

         string toPrintable()
                  : info<"Returns literal representation">
            = "false";
   };
}

public const true = BoolValue.true;
public const false = BoolValue.false;

// --- BaseNumber ---

public abstract class BaseNumber : BaseValue,
   info<"a base numeric value">
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
      : info<"Adds the value;<br/>should be overridden">;

   abstract subtract(o)
      : info<"Subtracts the value;<br/>should be overridden">;

   abstract multiply(o)
      : info<"Multiplies by the value;<br/>should be overridden">;

   abstract divide(o)
      : info<"Divides by the value;<br/>should be overridden">;

   Negative
         : info<"Returns the negative value of a number">
      = 0 - self;
}

// --- IntBaseNumber ---

public abstract class IntBaseNumber : BaseNumber,
   info<"a base integer value">
{
   band(BaseVariable var)
      <= band(*var);

   bor(BaseVariable var)
      <= bor(*var);

   bxor(BaseVariable var)
      <= bxor(*var);

   abstract band(o)
      : info<"Executes bitwise AND operation;<br/>should be overridden">;

   abstract bor(o)
      : info<"Executes bitwise OR operation;<br/>should be overridden">;

   abstract bxor(o)
      : info<"Executes bitwise XOR operation;<br/>should be overridden">;

   bool allMask(operand)
         : info<"Returns true if all the mask bits are set">
      = self.band:operand == operand;

   bool anyMask(operand)
         : info<"Returns true if any of the mask bits are set">
      = 0 != self.band:operand;

   get abstract BInverted();
}

// --- ByteNumber ---

public const struct ByteNumber : IntBaseNumber,
   info<"an unsigned 8 bit integer">
{
   embeddable __int theValue[1];

   static byte MinValue
            : info<"returns the minimal value (0)">
      = 0;

   static byte MaxValue
            : info<"Returns the maxial value (255)">
      = 255;

   constructor()
            : info<"Returns the default value">
      = 0;

   constructor(byte b)
            : info<"Creates the object with a specified value">
      = b;

   constructor(int n)
            : info<"Creates the object with a specified value">
      = n & 255;         

   add(o)
     <= add(cast byte(o));

   subtract(o)
     <= subtract(cast byte(o));

   multiply(o)
     <= multiply(cast byte(o));

   divide(o)
     <= divide(cast byte(o));

   byte add(byte n)
            : info<"Returns the sum">
      = theValue + n;   

   byte subtract(byte n)
            : info<"Returns the difference">
      = theValue - n;   

   byte multiply(byte n)
            : info<"Returns the product">
      = theValue * n;   

   byte divide(byte n)
            : info<"Returns the result of integer division">
      = theValue / n;   

   short add(short n)
            : info<"Returns the sum">
      = new ShortNumber(theValue).add(n);   

   short subtract(short n)
            : info<"Returns the difference">
      = new ShortNumber(theValue).subtract(n);   

   short multiply(short n)
            : info<"Returns the product">
      = new ShortNumber(theValue).multiply(n);   

   short divide(short n)
            : info<"Returns the result of integer division">
      = new ShortNumber(theValue).divide(n);   

   int add(int n)
            : info<"Returns the sum">
      = new IntNumber(theValue).add(n);   

   int subtract(int n)
            : info<"Returns the difference">
      = new IntNumber(theValue).subtract(n);   

   int multiply(int n)
            : info<"Returns the product">
      = new IntNumber(theValue).multiply(n);   

   int divide(int n)
            : info<"Returns the result of integer division">
      = new IntNumber(theValue).divide(n);   

   int add(long n)
            : info<"Returns the sum">
      = new LongNumber(theValue).add(n);   

   int subtract(long n)
            : info<"Returns the difference">
      = new LongNumber(theValue).subtract(n);   

   int multiply(long n)
            : info<"Returns the product">
      = new LongNumber(theValue).multiply(n);   

   int divide(long n)
            : info<"Returns the result of integer division">
      = new LongNumber(theValue).divide(n);   

   real add(real n)
            : info<"Returns the sum">
      = new RealNumber(theValue).add(n);   

   real subtract(real n)
            : info<"Returns the difference">
      = new RealNumber(theValue).subtract(n);   

   real multiply(real n)
            : info<"Returns the product">
      = new RealNumber(theValue).multiply(n);   

   real divide(real n)
            : info<"Returns the result of integer division">
      = new RealNumber(theValue).divide(n);   

   bool equal(o)
   {
      var n := cast byte(o) ?? nil;
      (nil == n)
          ? { ^ false };

      ^ self.equal(n)
   }

   bool less(o)
      <= less(cast byte(o));

   bool equal(byte b)
            : info<"Compares the value">
      = theValue == b;

   bool less(byte b)
      = theValue < b;

   bool equal(short s)
            : info<"Compares the value">
      = new ShortNumber(theValue).equal(s);

   bool less(short s)
      = new ShortNumber(theValue).less(s);

   bool equal(int n)
            : info<"Compares the value">
      = new IntNumber(theValue).equal(n);

   bool less(int n)
            : info<"Compares the value">
      = new IntNumber(theValue).less(n);

   bool equal(long n)
            : info<"Compares the value">
      = new LongNumber(self).equal(n);

   bool less(long n)
            : info<"Compares the value">
      = new LongNumber(self).less(n);

   bool equal(real n)
            : info<"Compares the value">
      = new RealNumber(theValue).equal(n);

   bool less(real n)
            : info<"Compares the value">
      = new RealNumber(theValue).less(n);

   band(n)
      <= band(cast byte(n));

   bor(n)
      <= bor(cast byte(n));

   bxor(n)
      <= bxor(cast byte(n));

   byte band(byte n)
            : info<"Returns the result of bitwise AND operation">
      = theValue & n;

   byte bor(byte n)
            : info<"Returns the result of bitwise OR operation">
      = theValue | n;

   byte bxor(byte n)
            : info<"Returns the result of bitwise XOR operation">
      = theValue ^ n;

   short band(short n)
            : info<"Returns the result of bitwise AND operation">
      = new ShortNumber(n).band(n);

   short bor(short n)
            : info<"Returns the result of bitwise OR operation">
      = new ShortNumber(n).bor(n);

   short bxor(short n)
            : info<"Returns the result of bitwise XOR operation">
      = new ShortNumber(n).bxor(n);

   int band(int n)
            : info<"Returns the result of bitwise AND operation">
      = new IntNumber(n).band(n);

   int bor(int n)
            : info<"Returns the result of bitwise OR operation">
      = new IntNumber(n).bor(n);

   int bxor(int n)
            : info<"Returns the result of bitwise XOR operation">
      = new IntNumber(n).bxor(n);

   long band(long n)
            : info<"Returns the result of bitwise AND operation">
      = new LongNumber(n).band(n);

   long bor(long n)
            : info<"Returns the result of bitwise OR operation">
      = new LongNumber(n).bor(n);

   long bxor(long n)
            : info<"Returns the result of bitwise XOR operation">
      = new LongNumber(n).bxor(n);

   short cast()
            : info<"Returns the value as a short integer">
      = shortConvertor.convert(self);

   int cast()
            : info<"returns the value as an integer">
      = intConvertor.convert(self);

   uint cast()
            : info<"returns the value as an unsigned integer">
      = uintConvertor.convert(self);

   long cast()
            : info<"Returns the value as a long integer">
      = longConvertor.convert(self);

   real cast()
            : info<"Returns the value as a real number">
      = realConvertor.convert(self);

   byte shiftLeft(int val)
            : info<"shifts an integer value to the left by a specified number of bits">
      = theValue $shl val;

   byte shiftRight(int val)
            : info<"shifts an integer value to the right by a specified number of bits">
      = theValue $shr val;

   get byte BInverted()
            : info<"bitwise inversion">
      = ~theValue;

   string toPrintable()
            : info<"returns the literal presentation">
      = stringConvertor.convert(self, 10);

   byte clone()
            : info<"clones the value">
      = new ByteNumber(self);
}

// --- ShortNumber ---

public const struct ShortNumber : IntBaseNumber,
         info<"a signed 16 bit integer">
{
   embeddable __int theValue[2];

   static short MinValue
            : info<"Returns the minimal value (-32768)">
      = -32768;

   static short MaxValue
            : info<"Returns the maxial value (32767)">
      = 32767;

   constructor()
            : info<"creates the object with a default value (0)">
      = 0;

   constructor(short s)
            : info<"creates the object with specified value">
      = s;

   constructor(byte b)
            : info<"creates the object with specified value">
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
            : info <"returns the sum">
      = theValue + n;   

   short subtract(short n)
            : info <"returns the difference">
      = theValue - n;   

   short multiply(short n)
            : info <"returns the product">
      = theValue * n;   

   short divide(short n)
            : info <"returns the result of integer division">
      = theValue / n;   

   short add(byte n)
            : info <"returns the sum">
      <= add(new ShortNumber(n));

   short subtract(byte n)
            : info <"returns the difference">
      <= subtract(new ShortNumber(n));

   short multiply(byte n)
            : info <"returns the product">
      <= multiply(new ShortNumber(n));

   short divide(byte n)
            : info <"returns the result of integer division">
      <= divide(new ShortNumber(n));

   int add(int n)
            : info <"returns the sum">
      = new IntNumber(theValue).add(n);   

   int subtract(int n)
            : info <"returns the difference">
      = new IntNumber(theValue).subtract(n);   

   int multiply(int n)
            : info <"returns the product">
      = new IntNumber(theValue).multiply(n);   

   int divide(int n)
            : info <"returns the result of integer division">
      = new IntNumber(theValue).divide(n);   

   long add(long n)
            : info <"returns the sum">
      = new LongNumber(theValue).add(n);   

   long subtract(long n)
            : info <"returns the difference">
      = new LongNumber(theValue).subtract(n);   

   long multiply(long n)
            : info <"returns the product">
      = new LongNumber(theValue).multiply(n);   

   long divide(long n)
            : info <"returns the result of integer division">
      = new LongNumber(theValue).divide(n);   

   real add(real n)
            : info <"returns the sum">
      = new RealNumber(theValue).add(n);   

   real subtract(real n)
            : info <"returns the difference">
      = new RealNumber(theValue).subtract(n);   

   real multiply(real n)
            : info <"returns the product">
      = new RealNumber(theValue).multiply(n);   

   real divide(real n)
            : info <"returns the result of integer division">
      = new RealNumber(theValue).divide(n);   

   bool equal(o)
   {
      var n := cast short(o) ?? nil;
      (nil == n)
         ? { ^ false };

      ^ self.equal(n)
   }

   band(n)
      <= band(cast short(n));

   bor(n)
      <= bor(cast short(n));

   bxor(n)
      <= bxor(cast short(n));

   short band(short n)
            : info<"Returns the result of bitwise AND operation">
      = theValue & n;

   short bor(short n)
            : info<"Returns the result of bitwise OR operation">
      = theValue | n;

   short bxor(short n)
            : info<"Returns the result of bitwise XOR operation">
      = theValue ^ n;

   short band(byte n)
            : info<"Returns the result of bitwise AND operation">
      <= band(new ShortNumber(n));

   short bor(byte n)
            : info<"Returns the result of bitwise OR operation">
      <= bor(new ShortNumber(n));

   short bxor(byte n)
            : info<"Returns the result of bitwise XOR operation">
      <= bxor(new ShortNumber(n));

   int band(int n)
            : info<"Returns the result of bitwise AND operation">
      = new IntNumber(self).band(n);

   int bor(int n)
            : info<"Returns the result of bitwise OR operation">
      = new IntNumber(self).bor(n);

   int bxor(int n)
            : info<"Returns the result of bitwise XOR operation">
      = new IntNumber(self).bxor(n);

   long band(long n)
            : info<"Returns the result of bitwise AND operation">
      = new LongNumber(self).band(n);

   long bor(long n)
            : info<"Returns the result of bitwise OR operation">
      = new LongNumber(self).bor(n);

   long bxor(long n)
            : info<"Returns the result of bitwise XOR operation">
      = new LongNumber(self).bxor(n);

   bool less(o)
      <= less(cast short(o));

   bool equal(short b)
         : info<"compares the value">
      = theValue == b;

   bool less(short b)
         : info<"compares the value">
      = theValue < b;

   bool equal(byte b)
         : info<"compares the value">
      <= equal(new ShortNumber(b));

   bool less(byte b)
         : info<"compares the value">
      <= less(new ShortNumber(b));

   bool equal(int n)
         : info<"compares the value">
      = new IntNumber(theValue).equal(n);

   bool less(int n)
         : info<"compares the value">
      = new IntNumber(theValue).less(n);

   bool equal(long n)
         : info<"compares the value">
      = new LongNumber(theValue).equal(n);

   bool less(long n)
         : info<"compares the value">
      = new LongNumber(theValue).less(n);

   bool equal(real n)
         : info<"compares the value">
      = new RealNumber(theValue).equal(n);

   bool less(real n)
         : info<"compares the value">
      = new RealNumber(theValue).less(n);

   byte cast()
         : info<"Returns the byte value"> 
      = byteConvertor.convert(self);

   int cast()
            : info<"Returns the integer value"> 
      = intConvertor.convert(self);

   uint cast()
            : info<"Returns the unsigned integer value"> 
      = uintConvertor.convert(self);

   long cast()
            : info<"Returns the long integer value"> 
      = longConvertor.convert(self);

   real cast()
            : info<"Returns the real number value"> 
      = realConvertor.convert(self);

   string toPrintable()
      = stringConvertor.convert(self, 10);

   short shiftLeft(int val)
            : info<"shifts an integer value to the left by a specified number of bits">
      = theValue $shl val;

   short shiftRight(int val)
            : info<"shifts an integer value to the right by a specified number of bits">
      = theValue $shr val;

   get short BInverted()
            : info<"bitwise inversion">
      = ~theValue;

   short clone()
            : info<"clones the value">
      = new ShortNumber(self);
}

// --- IntNumber ---

public const struct IntNumber : IntBaseNumber,
   info<"A signed 32 bit integer">
{
   embeddable __int theValue[4];

   static int MinValue
            : info<"Creates the object with the minimal value">
      = -2147483648;

   static int MaxValue
            : info<"Creates the object with the maximal value">
      = 2147483647;

   constructor()
      = 0;

   constructor(int n)
            : info<"Creates the object with specified value">
      = n;

   constructor(uint n)
            : info<"Creates the object with specified value">
      = n;

   constructor(byte b)
      = b;

   constructor(short s)
      = s;

   bool equal(o)
   {
      var n := cast int(o) ?? nil;
      (nil == n)
         ? { ^ false };

      ^ self.equal(n)
   }

   bool less(o)
      <= less(cast int(o));

   bool equal(byte b)
            : info<"Compares the integer value">
      <= equal(new IntNumber(b));

   bool equal(long n)
            : info<"Compares the integer value">
      = new LongNumber(theValue).equal(n);

   bool equal(real n)
            : info<"Compares the integer value">
      = new RealNumber(theValue).equal(n);

   bool less(byte b)
            : info<"Compares the integer value">
      <= less(new IntNumber(b));

   bool equal(short s)
            : info<"Compares the integer value">
      <= equal(new IntNumber(s));

   bool less(short s)
            : info<"Compares the integer value">
      <= less(new IntNumber(s));

   bool less(long n)
            : info<"Compares the integer value">
      = new LongNumber(theValue).less(n);

   bool less(real n)
            : info<"Compares the integer value">
      = new RealNumber(theValue).less(n);

   bool equal(int n)
      = theValue == n; 

   bool notequal(int n)
      = theValue != n;

   bool less(int n)
      = theValue < n;

   add(o)
     <= add(cast int(o));

   subtract(o)
     <= subtract(cast int(o));

   multiply(o)
     <= multiply(cast int(o));

   divide(o)
     <= divide(cast int(o));

   int add(int n)
            : info<"Returns the sum">
      = theValue + n;   

   int subtract(int n)
            : info<"Returns the difference">
      = theValue - n;   

   int multiply(int n)
            : info<"Returns the product">
      = theValue * n;   

   int divide(int n)
            : info<"Returns the result of integer division">
      = theValue / n;   

   int add(byte n)
            : info<"Returns the sum">
      <= add(new IntNumber(n));

   int subtract(byte n)
            : info<"Returns the difference">
      <= subtract(new IntNumber(n));

   int multiply(byte n)
            : info<"Returns the product">
      <= multiply(new IntNumber(n));

   int divide(byte n)
            : info<"Returns the result of integer division">
      <= divide(new IntNumber(n));

   int add(short n)
            : info<"Returns the sum">
      <= add(new IntNumber(n));

   int subtract(short n)
            : info<"Returns the difference">
      <= subtract(new IntNumber(n));

   int multiply(short n)
            : info<"Returns the product">
      <= multiply(new IntNumber(n));

   int divide(short n)
            : info<"Returns the result of integer division">
      <= divide(new IntNumber(n));

   long add(long n)
            : info<"Returns the sum">
      = new LongNumber(self).add(n);

   long subtract(long n)
            : info<"Returns the difference">
      = new LongNumber(self).subtract(n);

   long multiply(long n)
            : info<"Returns the product">
      = new LongNumber(self).multiply(n);

   long divide(long n)
            : info<"Returns the result of integer division">
      = new LongNumber(self).divide(n);

   real add(real n)
            : info<"Returns the sum">
      = new RealNumber(self).add(n);

   real subtract(real n)
            : info<"Returns the difference">
      = new RealNumber(self).subtract(n);

   real multiply(real n)
            : info<"Returns the product">
      = new RealNumber(self).multiply(n);

   real divide(real n)
            : info<"Returns the result of integer division">
      = new RealNumber(self).divide(n);

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
            : info<"Returns the result of bitwise AND operation">
      = theValue & n;

   int bor(int n)
            : info<"Returns the result of bitwise OR operation">
      = theValue | n;

   int bxor(int n)
            : info<"Returns the result of bitwise XOR operation">
      = theValue ^ n;

   int band(byte n)
            : info<"Returns the result of bitwise AND operation">
      <= band(new IntNumber(n));

   int bor(byte n)
            : info<"Returns the result of bitwise OR operation">
      <= bor(new IntNumber(n));

   int bxor(byte n)
            : info<"Returns the result of bitwise XOR operation">
      <= bxor(new IntNumber(n));

   int band(short n)
            : info<"Returns the result of bitwise AND operation">
      <= band(new IntNumber(n));

   int bor(short n)
            : info<"Returns the result of bitwise OR operation">
      <= bor(new IntNumber(n));

   int bxor(short n)
            : info<"Returns the result of bitwise XOR operation">
      <= bxor(new IntNumber(n));

   long band(long n)
            : info<"Returns the result of bitwise AND operation">
      = new LongNumber(self).band(n);

   long bor(long n)
            : info<"Returns the result of bitwise OR operation">
      = new LongNumber(self).bor(n);

   long bxor(long n)
            : info<"Returns the result of bitwise XOR operation">
      = new LongNumber(self).bxor(n);

   byte cast()
            : info<"Returns the byte value">
      = byteConvertor.convert(self);

   short cast()
            : info<"Returns the value as a short integer">
      = shortConvertor.convert(self);

   long cast()
            : info<"Returns the long integer">
      = longConvertor.convert(self);

   real cast()
            : info<"Returns the real number">
      = realConvertor.convert(self);

   uint cast()
            : info<"Returns the unsigned integer">
      = new UIntNumber(self);

   string toPrintable()
      = stringConvertor.convert(self, 10);

   int clone()
            : info<"Clones the value">
      = new IntNumber(theValue);

   int shiftLeft(int val)
      = theValue $shl val;
   
   int shiftRight(int val)
      = theValue $shr val;

   get int BInverted()
            : info<"Bitwise inversion">
      = ~theValue;

   get int Negative()
      = -theValue;

   bool allMask(int operand)
         : info<"Returns true if all the mask bits are set">
   {
      int v := operand & theValue;
        
      ^ v == operand
   }
        
   bool anyMask(int operand)
         : info<"Returns true if any of the mask bits are set">
   {
      int v := operand & theValue;
        
      ^ v != 0
   }        
}

// --- UIntNumber ---

public sealed const struct UIntNumber : IntBaseNumber,
   info<"A unsigned 32 bit integer">
{
   embeddable __int theValue[4];

   constructor(byte b)
   {
      theValue := b;
   }

   constructor(short s)
   {
      theValue := s;
   }

   constructor(int n)
   {
      theValue := n
   }

   constructor(uint n)
   {
      theValue := n
   }

   constructor()
      = 0;

   cast u(string s)
   {
      theValue := uintConvertor.convert(s);
   }

   cast H(string s)
   {
      theValue := uintConvertor.convert(s, 16);
   }

   static uint MinValue
            : info<"Creates the object with the minimal value">
      = 0;

   static uint MaxValue
            : info<"Creates the object with the maximal value">
      = 0FFFFFFFFh;

   string toPrintable()
            : info<"Returns the literal presentation">
      = stringConvertor.convert(self, 10);

   short cast()
            : info<"Returns the value as a short integer">
      = shortConvertor.convert(self);

   byte cast()
            : info<"Returns the byte value">
      = byteConvertor.convert(self);

   int cast()
      = self;
   
   long cast()
            : info<"Returns the long integer">
      = new LongNumber(self);

   real cast()
            : info<"Returns the real number">
      = new RealNumber(self);

   bool equal(o)
   {
      var n := cast uint(o) ?? nil;
      (nil == n)
         ? { ^ false };

      ^ self.equal(n)
   }

   bool less(n)
      <= less(cast uint(n));

   bool equal(uint n)
            : info<"Compares the value">
      = theValue == n;

   bool less(uint n)
            : info<"Compares the value">
      = theValue < n;

   bool equal(int n)
            : info<"Compares the value">
      = theValue == n;

   bool less(int n)
            : info<"Compares the value">
      <= less(cast uint(n));

   bool equal(short n)
            : info<"Compares the value">
      = self.equal(new UIntNumber(n));

   bool equal(byte n)
            : info<"Compares the value">
      = self.equal(new UIntNumber(n));

   bool less(byte n)
            : info<"Compares the value">
      = self.less(new UIntNumber(n));

   bool equal(long n)
            : info<"Compares the value">
      = new LongNumber(self).equal(n);

   bool less(long n)
            : info<"Compares the value">
      = new LongNumber(self).less(n);

   bool equal(real n)
            : info<"Compares the value">
      = new RealNumber(self).equal(n);

   bool less(real n)
            : info<"Compares the value">
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
            : info<"Returns the sum">
      = theValue + n;

   uint subtract(uint n)
            : info<"Returns the difference">
      = theValue - n;

   uint multiply(uint n)
            : info<"Returns the product">
      = theValue * n;

   uint divide(uint n)
            : info<"Returns the result of integer division">
      = theValue / n;

   int add(int n)
            : info<"Returns the sum">
      = theValue + n;

   int subtract(int n)
            : info<"Returns the difference">
      = theValue - n;

   int multiply(int n)
            : info<"Returns the product">
      = theValue * n;

   int divide(int n)
            : info<"Returns the result of integer division">
      = theValue / n;

   int add(byte n)
            : info<"Returns the sum">
      <= add(cast uint(n));

   int subtract(byte n)
            : info<"Returns the difference">
      <= subtract(cast uint(n));

   int multiply(byte n)
            : info<"Returns the product">
      <= multiply(cast uint(n));

   int divide(byte n)
            : info<"Returns the result of integer division">
      <= divide(cast uint(n));

   int add(short n)
            : info<"Returns the sum">
      <= add(cast uint(n));

   int subtract(short n)
            : info<"Returns the difference">
      <= subtract(cast uint(n));

   int multiply(short n)
            : info<"Returns the product">
      <= multiply(cast uint(n));

   int divide(short n)
            : info<"Returns the result of integer division">
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
            : info<"Returns the result of bitwise AND operation">
      = theValue & n;

   uint bor(uint n)
            : info<"Returns the result of bitwise OR operation">
      = theValue | n;

   uint bxor(uint n)
            : info<"Returns the result of bitwise XOR operation">
      = theValue ^ n;

   int band(int n)
            : info<"Returns the result of bitwise AND operation">
      = theValue & n;

   int bor(int n)
            : info<"Returns the result of bitwise OR operation">
      = theValue | n;

   int bxor(int n)
            : info<"Returns the result of bitwise XOR operation">
      = theValue ^ n;

   uint band(short n)
            : info<"Returns the result of bitwise AND operation">
      <= band(cast uint(n));

   uint bor(short n)
            : info<"Returns the result of bitwise OR operation">
      <= bor(cast uint(n));

   uint bxor(short n)
            : info<"Returns the result of bitwise XOR operation">
      <= bxor(cast uint(n));

   uint band(byte n)
            : info<"Returns the result of bitwise AND operation">
      <= band(cast uint(n));

   uint bor(byte n)
            : info<"Returns the result of bitwise OR operation">
      <= bor(cast uint(n));

   uint bxor(byte n)
            : info<"Returns the result of bitwise XOR operation">
      <= bxor(cast uint(n));

   long band(long n)
            : info<"Returns the result of bitwise AND operation">
      = new LongNumber(n).band(n);

   long bor(long n)
            : info<"Returns the result of bitwise OR operation">
      = new LongNumber(n).bor(n);

   long bxor(long n)
            : info<"Returns the result of bitwise XOR operation">
      = new LongNumber(n).bxor(n);

   uint shiftLeft(int val)
      = theValue $shl val;

   uint shiftRight(int val)
      = theValue $shr val;
   
   get uint BInverted()
            : info<"Bitwise inversion">
      = ~theValue;

   uint clone()
      = new UIntNumber(self);
}

// --- LongNumber ---

public sealed const struct LongNumber : IntBaseNumber,
   info<"A signed 64 bit integer value">
{
   embeddable __int theValue[8];

   static long MinValue
            : info<"Returns the minimal value">
      = -9223372036854775808l;

   static long MaxValue
            : info<"Returns the maximal value">
      = 9223372036854775807l;

   constructor()
            : info<"Creates the object with a default value (0)">
      = 0;

   constructor(long n)
            : info<"Creates the object with specified value">
      = n;

   construcot(int n)
            : info<"Creates the object with specified value">
      = longConvertor.convert(n);

   construcot(uint n)
            : info<"Creates the object with specified value">
      = longConvertor.convert(n);

   construcot(short n)
            : info<"Creates the object with specified value">
      = longConvertor.convert(n);

   constructor(byte b)
            : info<"Creates the object with specified value">
      = longConvertor.convert(b);

   int cast()
            : info<"Returns an integer value">
      = intConvertor.convert(self);

   uint cast()
            : info<"Returns an integer value">
      = uintConvertor.convert(self);

   short cast()
            : info<"Returns an integer value">
      = shortConvertor.convert(self);

   byte cast()
            : info<"Returns an integer value">
      = byteConvertor.convert(self);

   real cast()
            : info<"Returns a real value">
      = realConvertor.convert(self);

   bool equal(o)
   {
      var n := cast long(o) ?? nil;
      (nil == n)
         ? { ^ false };


       ^ self.equal(n)
   }

   bool less(o)
      <= less(cast long(o));

   bool equal(long n)
            : info<"Compares the value">
      = theValue == n;

   bool less(long n)
            : info<"Compares the value">
      = theValue < n;

   bool equal(byte b)
      <= equal(cast long(b));

   bool less(byte b)
      <= less(cast long(b));

   bool equal(short s)
      <= equal(cast long(s));

   bool less(short s)
      <= less(cast long(s));

   bool equal(int n)
      <= equal(cast int(n));

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

   long add(long n)
      = theValue + n;

   long subtract(long n)
      = theValue - n;

   long multiply(long n)
      = theValue * n;

   long divide(long n)
      = theValue / n;

   long add(byte n)
      <= add(cast long(n));

   long subtract(byte n)
      <= add(cast long(n));

   long multiply(byte n)
      <= add(cast long(n));

   long divide(byte n)
      <= add(cast long(n));

   long add(short n)
      <= add(cast long(n));

   long subtract(short n)
      <= add(cast long(n));

   long multiply(short n)
      <= add(cast long(n));

   long divide(short n)
      <= add(cast long(n));

   long add(int n)
      <= add(cast long(n));

   long subtract(int n)
      <= add(cast long(n));

   long multiply(int n)
      <= add(cast long(n));

   long divide(int n)
      <= add(cast long(n));

   long add(uint n)
      <= add(cast long(n));

   long subtract(uint n)
      <= add(cast long(n));

   long multiply(uint n)
      <= add(cast long(n));

   long divide(uint n)
      <= add(cast long(n));

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
      = theValue & n;

   long bor(long n)
      = theValue | n;

   long bxor(long n)
      = theValue ^ n;

   long band(byte n)
      <= band(cast long(n));

   long bor(byte n)
      <= band(cast long(n));

   long bxor(byte n)
      <= band(cast long(n));

   long band(short n)
      <= band(cast long(n));

   long bor(short n)
      <= band(cast long(n));

   long bxor(short n)
      <= band(cast long(n));

   long band(int n)
      <= band(cast long(n));

   long bor(int n)
      <= band(cast long(n));

   long bxor(int n)
      <= band(cast long(n));

   long band(uint n)
      <= band(cast long(n));

   long bor(uint n)
      <= band(cast long(n));

   long bxor(uint n)
      <= band(cast long(n));

   long shiftLeft(int val)
      = theValue $shl val;
   
   long shiftRight(int val)
      = theValue $shr val;

   get long BInverted()
            : info<"Bitwise inversion">
      = ~theValue;

   get long Negative()
      = -theValue;

   string toPrintable()
      = stringConvertor.convert(self, 10);

   long clone()
      = new LongNumber(theValue);
}

// --- RealNumber ---

public sealed const struct RealNumber : BaseNumber,
      info<"A 64bit float numeric value">
{
   embeddable __float theValue[8];

   static real MinValue
            : info<"Returns the minimal value">
      = -1.7976931348623157e+308r;

   static real MaxValue
            : info<"Returns the maximal value">
      = 1.7976931348623157e+308r;

    static real NegativeInfinity 
            : info<"Returns the negative infinity">
        = -1.0 / 0.0;   

    static real PositiveInfinity 
            : info<"Returns the positive infinity">
        = 1.0 / 0.0;   

    static real NaN 
            : info<"Returns the positive infinity">
        = 0.0 / 0.0;   

   constructor(int n)
      = realConvertor.convert(n);
   
   constructor(byte n)
      = realConvertor.convert(n);
   
   constructor(short n)
      = realConvertor.convert(n);

   constructor()
            : info<"Creates the object with a default value (0)">
      = 0.0;

   constructor(real r)
            : info<"Creates the object with specified value">
      = r;
   
   constructor(uint n)
            : info<"Creates the object with specified value">
      = realConvertor.convert(n);

   constructor(long n)
            : info<"Creates the object with specified value">
      = realConvertor.convert(n);

   bool equal(o)
   {
      var n := cast real(o) ?? nil;
      (nil == n)
         ? { ^ false };


       ^ self.equal(n)
   }

   bool less(n)
      <= less(cast real(n));

   bool equal(real n)
            : info<"Compares the value">
      = theValue == n;
   
   bool less(real n)
            : info<"Compares the value">
      = theValue < n;

   add(n)
      <= add(cast real(n));

   subtract(n)
      <= subtract(cast real(n));

   multiply(n)
      <= multiply(cast real(n));

   divide(n)
      <= divide(cast real(n));

   real add(real n)
            : info<"Returns the sum">
      = theValue + n;

   real subtract(real n)
            : info<"Returns the difference">
      = theValue - n;

   real multiply(real n)
            : info<"Returns the product">
      = theValue * n;

   real divide(real n)
            : info<"Returns the result of integer division">
      = theValue / n;

   bool equal(int n)
            : info<"Returns the result of integer division">
      = self.equal(new RealNumber(n));

   bool equal(byte n)
            : info<"Returns the result of integer division">
      = self.equal(new RealNumber(n));

   bool equal(short n)
            : info<"Returns the result of integer division">
      = self.equal(new RealNumber(n));

   bool equal(long n)
            : info<"Returns the result of integer division">
      = self.equal(new RealNumber(n));

   bool less(int n)
            : info<"Returns the result of integer division">
      = self.less(new RealNumber(n));

   bool less(byte n)
            : info<"Returns the result of integer division">
      = self.less(new RealNumber(n));

   bool less(short n)
            : info<"Returns the result of integer division">
      = self.less(new RealNumber(n));

   bool less(long n)
            : info<"Returns the result of integer division">
      = self.less(new RealNumber(n));

   real add(int n)
            : info<"Returns the sum of numbers">
      = self.add(new RealNumber(n));

   real add(long n)
            : info<"Returns the sum of numbers">
      = self.add(new RealNumber(n));

   real add(byte n)
            : info<"Returns the sum of numbers">
      = self.add(new RealNumber(n));

   real add(short n)
            : info<"Returns the sum of numbers">
      = self.add(new RealNumber(n));

   real subtract(int n)
            : info<"Returns the difference of numbers">
      = self.subtract(new RealNumber(n));

   real subtract(long n)
            : info<"Returns the difference of numbers">
      = self.subtract(new RealNumber(n));

   real subtract(byte n)
            : info<"Returns the difference of numbers">
      = self.subtract(new RealNumber(n));

   real subtract(short n)
            : info<"Returns the difference of numbers">
      = self.subtract(new RealNumber(n));

   real multiply(int n)
            : info<"Returns the product of numbers">
      = self.multiply(new RealNumber(n));

   real multiply(long n)
            : info<"Returns the product of numbers">
      = self.multiply(new RealNumber(n));

   real multiply(byte n)
            : info<"Returns the product of numbers">
      = self.multiply(new RealNumber(n));

   real multiply(short n)
            : info<"Returns the product of numbers">
      = self.multiply(new RealNumber(n));

   real divide(int n)
            : info<"Returns the result of the division">
      = self.divide(new RealNumber(n));

   real divide(long n)
            : info<"Returns the result of the division">
      = self.divide(new RealNumber(n));

   real divide(byte n)
            : info<"Returns the result of the division">
      = self.divide(new RealNumber(n));

   real divide(short n)
            : info<"Returns the result of the division">
      = self.divide(new RealNumber(n));

   string toPrintable()
            : info<"Returns the literal presentation">
      = stringConvertor.convert(self);

   real clone()
            : info<"Clones the value">
      = new RealNumber(self);

   get real Negative()
            : info<"Negates the value">
      = 0.0 - theValue;
}

// --- CharValue ---

public const struct CharValue : BaseValue,
   info<"An UTF-32 character symbol">
{
   embeddable __word theValue[4];

   static char MinValue
            : info<"Returns the minimal possible character value (0)">
      = $0;

   static char MaxValue
            : info<"Returns the maximal possible character value (010FFFFh)">
//      = $010FFFFh;
      = $1114111;

   constructor()
            : info<"Creates the object with a default value (0)">
      = $0;

   constructor(char value)
            : info<"Creates the object with specified value">
      = value;

   constructor load(int n)
            : info<"Creates the object with specified numeric value">
      = charConvertor.convert(n);

   get internal int Value()
   {
      int val := intConvertor.convert(self);

      ^ val
   }

   get int Length()
         : info<"Retuns the number of bytes to represent the symbol in UTF-8"> 
   {
      int len := PrimitiveOperations.getCharLength(theValue);
      
      ^ len
   }

   get int WideLength()
         : info<"Retuns the number of bytes to represent the symbol in UTF-16"> 
   {
      int len := PrimitiveOperations.getCharWideLength(theValue);
      
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
      = theValue == val;

   bool less(char val)
      = theValue < val;

   bool equal(int n)
      = theValue == n;

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
            : info<"Clones the value">
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
   : info<"A nil value">
{
   bool equal(object)
      : info<"Returns true if the specified object is nil.">
   {
      if (nil == object)
      {
         ^ true
      };

      if (super.equal:object)
      {
         ^ true
      };

      ^ false
   }

   cast generic() 
            : info<"Returns nil<br/>Any typecasting message will be handled and nil value returned">
      = nil;
}

// --- KeyValue ---

public sealed class KeyValue
   : info<"A key - object pair<br/>Extends the object with a key property">
{
   object key;
   object value;

   constructor new(key, value)
      : info<"Creates a new object">
   {
      this key := key;
      this value := value
   }

   Key : info<"Returns the key">
      = key;

   Value : info<"Returns the value">
      = value;

   set Value(value)
   {
      this value := value
   }

   bool equal(o)
      => object;

   string toPrintable()
      => object;

   dispatch() : info<"Redirects the incoming messages to the object">
     => object;
}