// --- BaseValue ---

public abstract class BaseValue
{
   abstract bool less(v);

   bool notless(o) = !(self.less(o));
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
}

// --- IntBaseNumber ---

public abstract class IntBaseNumber : BaseNumber
{
}

// --- ByteNumber ---

public const struct ByteNumber //: IntBaseNumber
{
   embeddable __int theValue[1];
}

// --- IntNumber ---

public const struct IntNumber : IntBaseNumber
{
   embeddable __int theValue[4];

   bool equal(int n)
      = theValue == n; 

   bool notequal(int n)
      = theValue != n;

   bool less(int n)
      = theValue < n;

   int add(int n)
      = theValue + n;   

   int subtract(int n)
      = theValue - n;   

   int multiply(int n)
      = theValue * n;   

   int divide(int n)
      = theValue / n;   

   byte cast()
      = byteConvertor.convert(self);

   string toPrintable()
      = stringConvertor.convert(self, 10);
}

// --- CharValue ---

public const struct CharValue //: BaseValue
{
   embeddable __word theValue[4];
}

