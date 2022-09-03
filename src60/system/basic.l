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
{
   private constructor() {}

   static bool MinValue
     = false;

   abstract if(trueAction);

   abstract if(trueAction,falseAction);

   abstract ifnot(trueAction);

   abstract iif(object trueVal, object falseVal);

   abstract bool equal(bool f);

   abstract bool notequal(bool f);

   abstract bool or(expr);

   abstract bool and(expr);

   abstract bool xor(expr);

   get abstract bool Inverted();

   const static bool true = BoolValue
   {
      if(trueAction) = trueAction();

      if(trueAction,elseAction) = trueAction();

      ifnot(trueAction) = nil;

      iif(object trueVal, object falseVal) = trueVal;

      bool equal(bool b) 
         = b;

      bool notequal(bool b)
         = !b;

      bool or(expr)
         = this self;

      bool and(expr)
         = expr;

      bool xor(expr)
      {
         cast bool(expr) ? { ^ BoolValue.false };
      }

      bool Inverted = BoolValue.false;

      string toPrintable() = "true";
   };

   const static bool false = BoolValue
   {
      bool Inverted = BoolValue.true;

      if(trueAction) = nil;

      if(trueAction,elseAction) = elseAction();

      ifnot(trueAction) = trueAction();

      iif(object trueVal, object falseVal) = falseVal;

      bool equal(bool b) = !b;

      bool notequal(bool b)
         = b; 

      bool or(expr)
         = expr;

      bool and(expr)
          = this self;

      bool xor(expr)
      {
          cast bool(expr) ? { ^ BoolValue.true };
            
          ^ this self
      }

      string toPrintable() = "false";
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

public const struct CharValue //: BaseValue
{
   embeddable __word theValue[4];
}

