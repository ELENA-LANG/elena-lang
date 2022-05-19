// --- BoolValue ---

public interface BoolValue
{
   private constructor() {}

   abstract if(trueAction);

//   abstract bool equal(bool f);

   const static true = BoolValue
   {
      if(trueAction) = trueAction();

//      bool equal(bool f) {}  
   };

   const static false = BoolValue
   {
      if(trueAction) = nil;

//      bool equal(bool f) {}  
   };
}

public const true = BoolValue.true;
public const false = BoolValue.false;

// --- BaseValue ---

public abstract class BaseValue
{
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

public const struct ByteNumber : IntBaseNumber
{
   embeddable __int theValue[1];
}

// --- IntNumber ---

public const struct IntNumber : IntBaseNumber
{
   embeddable __int theValue[4];

    int add(int n)
        = theValue + n;   

    int subtract(int n)
        = theValue - n;   
}
