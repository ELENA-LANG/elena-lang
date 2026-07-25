import system'collections;

public singleton Controller
{
   private drawFunction(IPlotter plotter, IFunctionRange range, IFunction f)
   {
      auto x := range.From;
      auto x_end := range.To;
      auto h := (x_end - x) / range.Steps;
      
      plotter.goTo(x, f.y(x));
      while (x <= x_end)
      {
         plotter.moveTo(x, f.y(x));
   
         x := x + h
      }      
   }
   
   private drawPolarFunction(IPlotter plotter, IFunctionRange range, IPolarFunction f)
   {
      auto t := range.From;
      auto t2 := range.To;
      auto h := (t2 - t) / range.Steps;
   
      plotter.goTo(f.x(t), f.y(t));
      while (t <= t2)
      {
         plotter.moveTo(f.x(t), f.y(t));
   
         t := t + h
      }      
   }   
   
   draw(IPlotter plotter, IFunctionRange range, Union<IFunction, IPolarFunction> functionUnion)
   {
      plotter.start();
      
      if:not:nil(var IFunction? f := *functionUnion) {
         drawFunction(plotter, range, f);
      }
      else if:not:nil(var IPolarFunction? f := *functionUnion) {
         drawPolarFunction(plotter, range, f);
      }
      else InvalidArgumentException.raise(functionUnion);      
   }
}