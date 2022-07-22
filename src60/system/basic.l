// --- BoolValue ---

public interface BoolValue
{
   private constructor() {}

   get abstract bool Inverted();

   abstract if(trueAction);

   abstract bool equal(bool f);

   abstract bool notequal(bool f);

   const static bool true = BoolValue
   {
      bool Inverted = BoolValue.false;

      if(trueAction) = trueAction();

      bool equal(bool b) 
         = b;

      bool notequal(bool b)
         = !b;
   };

   const static bool false = BoolValue
   {
      bool Inverted = BoolValue.true;

      if(trueAction) = nil;

      bool equal(bool b) = !b;

      bool notequal(bool b)
         = b; 
   };
}

public const true = BoolValue.true;
public const false = BoolValue.false;

// --- BaseValue ---

public abstract class BaseValue
{
   abstract bool less(v);

   bool notless(o) = !(self.less(o));
}

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
}

// --- CharValue ---

public const struct CharValue //: BaseValue
{
   embeddable __word theValue[4];
}

