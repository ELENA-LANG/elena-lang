// --- BoolValue ---

public interface BoolValue
{
   private constructor() {}

//   abstract bool equal(bool f);

   const static true = BoolValue
   {
//      bool equal(bool f) {}  
   };

   const static false = BoolValue
   {
//      bool equal(bool f) {}  
   };
}

//public /*const*/ true = BoolValue.true;
//public /*const*/ false = BoolValue.false;

// --- BaseValue ---

public abstract class BaseValue
{
}

// --- BaseNumber ---

public abstract class BaseNumber : BaseValue
{
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
}
