import system'collections;

// -- Star --

const  real A        = 5.0r;
const  real C        = 2.0r / 3.0r;

public singleton PolarFactory : IFactory
{
   IFunctionRange Range
      = new IFunctionRange
      {
         real RangeX = 20;
         real RangeY = 20;
         real From   = 0.0r;
         real To     = Pi_value * 6;
         
         int  Steps = 300;
      };
      
   Union<IFunction, IPolarFunction> Function
      = new IPolarFunction
      {
         real x(real angle)
            = A * (C * angle.cos() + (C*angle).cos());
            
         real y(real angle)
            = A * (C * angle.sin() - (C*angle).sin());
      };
}

// -- Sin --

public singleton SinFactory : IFactory
{
   IFunctionRange Range
      = new IFunctionRange
      {
         real RangeX = 20;
         real RangeY = 20;
         real From   = (-Pi_value) * 6;
         real To     = Pi_value * 6;
         
         int  Steps = 300;
      };
      
   Union<IFunction, IPolarFunction> Function
      = new IFunction
      {
         real y(real x)
            = x.sin();
      };
}

// -- Cos --

public singleton CosFactory : IFactory
{
   IFunctionRange Range
      = new IFunctionRange
      {
         real RangeX = 20;
         real RangeY = 20;
         real From   = (-Pi_value) * 6;
         real To     = Pi_value * 6;
         
         int  Steps = 300;
      };
      
   Union<IFunction, IPolarFunction> Function
      = new IFunction
      {
         real y(real x)
            = x.cos();
      };
}

// -- SqrFunction --

public singleton SqrFactory : IFactory
{
   IFunctionRange Range
      = new IFunctionRange
      {
         real RangeX = 20;
         real RangeY = 20;
         real From   = -10;
         real To     = 10;
         
         int  Steps = 300;
      };
      
   Union<IFunction, IPolarFunction> Function
      = new IFunction
      {
         real y(real x)
            = x * x;
      };
}

// -- SqrtFunction --

public singleton SqrtFactory : IFactory
{
   IFunctionRange Range
      = new IFunctionRange
      {
         real RangeX = 20;
         real RangeY = 20;
         real From   = 0.01;
         real To     = 10;
         
         int  Steps = 300;
      };
      
   Union<IFunction, IPolarFunction> Function
      = new IFunction
      {
         real y(real x)
            = x.sqrt();
      };
}

// -- CubeFunction --

public singleton CubeFunction : IFactory
{
   IFunctionRange Range
      = new IFunctionRange
      {
         real RangeX = 20;
         real RangeY = 20;
         real From   = -10;
         real To     = 10;
         
         int  Steps = 300;
      };
      
   Union<IFunction, IPolarFunction> Function
      = new IFunction
      {
         real y(real x)
            = x * x * x;
      };
}

// -- LnFunction --

public singleton LnFunction : IFactory
{
   IFunctionRange Range
      = new IFunctionRange
      {
         real RangeX = 20;
         real RangeY = 20;
         real From   = 0.01;
         real To     = 10;
         
         int  Steps = 300;
      };
      
   Union<IFunction, IPolarFunction> Function
      = new IFunction
      {
         real y(real x)
            = x.ln();
      };
}
const int Choice = 6;

public singleton FactoryProvider
{
   IFactory new()
   {
      IFactory factory := PolarFactory;
      
      Choice =>
         2: { factory := SinFactory }
         3: { factory := CosFactory }
         4: { factory := SqrFactory }
         5: { factory := CubeFunction }
         6: { factory := LnFunction }
         7: { factory := SqrtFactory };
         
      ^ factory;
   }
}