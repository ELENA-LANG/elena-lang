
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
      = false;

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

public const struct ByteNumber : /*Int*/BaseNumber,
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
      = theValue + n;   

   byte subtract(byte n)
      = theValue - n;   

   byte multiply(byte n)
      = theValue * n;   

   byte divide(byte n)
      = theValue / n;   

   short add(short n)
      = new ShortNumber(theValue).add(n);   

   short subtract(short n)
      = new ShortNumber(theValue).subtract(n);   

   short multiply(short n)
      = new ShortNumber(theValue).multiply(n);   

   short divide(short n)
      = new ShortNumber(theValue).divide(n);   

   int add(int n)
      = new IntNumber(theValue).add(n);   

   int subtract(int n)
      = new IntNumber(theValue).subtract(n);   

   int multiply(int n)
      = new IntNumber(theValue).multiply(n);   

   int divide(int n)
      = new IntNumber(theValue).divide(n);   

   bool equal(o)
   {
/*      var n := cast byte(o) ?? nil;
      (nil == n)
          ? { ^ false }
          : { ^ self.equal(n) }*/
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
      = new IntNumber(theValue).less(n);

   bool equal(real n)
            : info<"Compares the value">
      = new RealNumber(theValue).equal(n);

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
}

// --- ShortNumber ---

public const struct ShortNumber //: IntBaseNumber
{
   embeddable __int theValue[2];

   static short MinValue
      = -32768;

   static short MaxValue
      = 32767;

   constructor()
      = 0;

   constructor(short s)
      = s;

   constructor(byte b)
      = shortConvertor.convert(b);

   add(o)
     <= add(cast short(o));

   subtract(o)
     <= subtract(cast short(o));

   multiply(o)
     <= multiply(cast short(o));

   divide(o)
     <= divide(cast short(o));

   short add(short n)
      = theValue + n;   

   short subtract(short n)
      = theValue - n;   

   short multiply(short n)
      = theValue * n;   

   short divide(short n)
      = theValue / n;   

   short add(byte n)
      <= add(new ShortNumber(n));

   short subtract(byte n)
      <= subtract(new ShortNumber(n));

   short multiply(byte n)
      <= multiply(new ShortNumber(n));

   short divide(byte n)
      <= divide(new ShortNumber(n));

   int add(int n)
      = new IntNumber(theValue).add(n);   

   int subtract(int n)
      = new IntNumber(theValue).subtract(n);   

   int multiply(int n)
      = new IntNumber(theValue).multiply(n);   

   int divide(int n)
      = new IntNumber(theValue).divide(n);   

   bool equal(o)
      <= equal(cast short(o));

   bool less(o)
      <= less(cast short(o));

   bool equal(short b)
      = theValue == b;

   bool less(short b)
      = theValue < b;

   bool equal(byte b)
      <= equal(new ShortNumber(b));

   bool less(byte b)
      <= less(new ShortNumber(b));

   bool equal(int n)
      = new IntNumber(theValue).equal(n);

   bool less(int n)
      = new IntNumber(theValue).less(n);

   byte cast()
      = byteConvertor.convert(self);

   int cast()
      = intConvertor.convert(self);

   string toPrintable()
      = stringConvertor.convert(self, 10);
}

// --- IntNumber ---

public const struct IntNumber : /*Int*/BaseNumber
{
   embeddable __int theValue[4];

   static int MinValue
      = -2147483648;

   static int MaxValue
      = 2147483647;

   constructor()
      = 0;

   constructor(int n)
      = n;

   constructor(byte b)
      = b;

   constructor(short s)
      = s;

   bool equal(o)
      <= equal(cast int(o));

   bool less(o)
      <= less(cast int(o));

   bool equal(byte b)
      <= equal(new IntNumber(b));

   bool less(byte b)
      <= less(new IntNumber(b));

   bool equal(short s)
      <= equal(new IntNumber(s));

   bool less(short s)
      <= less(new IntNumber(s));

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
      = theValue + n;   

   int subtract(int n)
      = theValue - n;   

   int multiply(int n)
      = theValue * n;   

   int divide(int n)
      = theValue / n;   

   int add(byte n)
      <= add(new IntNumber(n));

   int subtract(byte n)
      <= subtract(new IntNumber(n));

   int multiply(byte n)
      <= multiply(new IntNumber(n));

   int divide(byte n)
      <= divide(new IntNumber(n));

   int add(short n)
      <= add(new IntNumber(n));

   int subtract(short n)
      <= subtract(new IntNumber(n));

   int multiply(short n)
      <= multiply(new IntNumber(n));

   int divide(short n)
      <= divide(new IntNumber(n));

   byte cast()
      = byteConvertor.convert(self);

   short cast()
      = shortConvertor.convert(self);

   string toPrintable()
      = stringConvertor.convert(self, 10);
}

// --- UIntNumber ---

public class UIntNumber;

// --- LongNumber ---

public class LongNumber;

// --- RealNumber ---

public class RealNumber
{
   constructor(byte b)
   {
   }
}

// --- CharValue ---

public const struct CharValue : BaseValue
{
   embeddable __word theValue[4];

   static char MinValue
      = $0;

   static char MaxValue
//      = $010FFFFh;
      = $1114111;

   constructor()
      = $0;

   constructor(char value)
      = value;

   constructor load(int n)
      = charConvertor.convert(n);

   bool equal(char val)
      = theValue == val;

   bool less(char val)
      = theValue < val;
}
