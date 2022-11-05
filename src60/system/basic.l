
// --- BaseValue ---
public abstract class BaseValue
{
   abstract bool less(v);

   bool greater(v)
      = v.less(self);

   bool notless(v) = !(self.less(v));

   bool notgreater(v) = !(self.greater(v));
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

public abstract class BaseNumber : BaseValue
{
   abstract add(o);

   abstract subtract(o);

   abstract multiply(o);

   abstract divide(o);
}

// --- IntBaseNumber ---

public abstract class IntBaseNumber : BaseNumber
{
}

// --- ByteNumber ---

public const struct ByteNumber : IntBaseNumber
{
   embeddable __int theValue[1];

   static byte MinValue
      = 0;

   static byte MaxValue
      = 255;

   constructor()
      = 0;

   constructor(byte b)
      = b;

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
      <= equal(cast byte(o));

   bool less(o)
      <= less(cast byte(o));

   bool equal(byte b)
      = theValue == b;

   bool less(byte b)
      = theValue < b;

   bool equal(short s)
      = new ShortNumber(theValue).equal(s);

   bool less(short s)
      = new ShortNumber(theValue).less(s);

   bool equal(int n)
      = new IntNumber(theValue).equal(n);

   bool less(int n)
      = new IntNumber(theValue).less(n);

   short cast()
      = shortConvertor.convert(self);

   int cast()
      = intConvertor.convert(self);
}

// --- ShortNumber ---

public const struct ShortNumber : IntBaseNumber
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

public const struct IntNumber : IntBaseNumber
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
