//import system'math;
import forms;
import system'drawing;
import system'dynamic;
import extensions;

registration : preloaded()
   = true.then(
   {
      DependencyInjector::registerSingleton<IFactory>(Factory);
   });

public sealed Plotter : IPlotter
{
   Dimension outputSize;
   Canvas    canvas;
   int       scaleX;
   int       scaleY; 
   
   int       zeroX;
   int       zeroY;
   
   constructor new(Dimension size, Canvas canvas)
   {
      this canvas := canvas;
      this outputSize := size;
      
      zeroX := this outputSize.Width / 2;
      zeroY := this outputSize.Height / 2 - 10;      
   }
   
   set RangeWidth(real value)
   {
      real scale := outputSize.Width / value;
      
      this scaleX := scale.RoundedInt;
   }
   
   set RangeHeight(real value)
   {
      real scale := outputSize.Height / value;
      
      this scaleY := scale.RoundedInt;
   }
   
   private drawAxis()
   {
      // X axis
      canvas.setCaret(0, zeroY);
      canvas.lineCaretTo(outputSize.Width, zeroY);
      
      canvas.writeText("X", outputSize.Width - 20, zeroY + 2);
      
      // Y axis
      canvas.setCaret(zeroX, 0);
      canvas.lineCaretTo(zeroX, outputSize.Height);
      
      canvas.writeText("Y", zeroX + 2, 2);
      
      for(int i := -10; i <= 10; i += 2)
      {
         if(i != 0)
         {
            canvas.writeText(i.toString(), zeroX + (i * scaleX) - 4, zeroY + 2);
            
            canvas.writeText(i.toString(), zeroX + 8, zeroY - (i * scaleY) - 2);
         }
         else
         { 
            canvas.writeText("0", zeroX + 2, zeroY + 2) 
         }
      }
   }
   
   start()
   {
      drawAxis();
   }
   
   goTo(real x, real y)
   {
      var zero := new Point(zeroX, zeroY);   
      Point p := new Point((x * scaleX).RoundedInt, (y * scaleY).RoundedInt);
   
      canvas.setCaret(zero + p);      
   }
   
   moveTo(real x, real y)
   {
      var zero := new Point(zeroX, zeroY);   
      Point p := new Point((x * scaleX).RoundedInt, (y * scaleY).RoundedInt);

      canvas.lineCaretTo(zero + p);
   }   
}

public class MainWindow : SDIDialog
{
   Imagebox       _imagebox;
   IFactory       _factory;
   
   constructor new(IFactory factory) : injectable_constructor()
      <= super new()
   {
      _factory := factory;
      
      _imagebox := Imagebox.new();
      self.appendControl(_imagebox);

      self.Caption := "Graph sample";
      self.setRegion(50, 50, 400, 420);

      _imagebox.setRegion(0, 0, 400, 420);
      _imagebox.onPaint := (object sender, Canvas canvas)
      {
         auto range := _factory.Range;
         auto plotter := Plotter.new(_imagebox.Region.Dimension, canvas);
         plotter.RangeWidth := range.RangeX;
         plotter.RangeHeight := range.RangeY;
         
         canvas.set(WhitePen);
         canvas.set(BlackBrush);
   
         Controller.draw(plotter, range, _factory.Function);
      }
   }
}
