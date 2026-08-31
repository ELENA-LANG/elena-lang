import system'collections;

public interface IFunctionRange
{
   get abstract real RangeX();
   get abstract real RangeY();
   
   get abstract real From();
   get abstract real To();
   get abstract int  Steps();
}

public interface IFunction
{
   abstract real y(real x);
}

public interface IPolarFunction
{
   abstract real x(real angle);
   abstract real y(real angle);
}

public interface IPlotter
{
   set abstract RangeWidth(real value);
   set abstract RangeHeight(real value);
   
   abstract start();
   abstract goTo(real x, real y);
   abstract moveTo(real x, real y);
}

public interface IFactory
{
   get abstract IFunctionRange Range();
   
   get abstract Union<IFunction, IPolarFunction> Function();
}

public interface IFunctionSelector
{
   get abstract string[] Names();
   
   abstract IFactory select(string name);
}
