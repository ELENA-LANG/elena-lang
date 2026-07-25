public interface IFunctionRange
{
   get abstract real From();
   get abstract real To();
   get abstract int Steps();
}

public interface IFunction
{
   get abstract bool IsPolar();
   
   abstract real y(real x);
}

public interface IPolarFunction
{
   abstract real x(real angle);
   abstract real y(real angle);
}
