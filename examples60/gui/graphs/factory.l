import system'collections;

const  real A        = 5.0r;
const  real C        = 2.0r / 3.0r;

public singleton Factory : IFactory
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

