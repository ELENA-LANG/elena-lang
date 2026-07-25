//import system'math;
import forms;
import system'drawing;
import system'collections;
import extensions;

//const  int ScaleX   = 15;
//const  int ScaleY   = 15;
const  real starting = 0.0r;
symbol real ending   = Pi_value * 6;

//const  real H        = 0.005r;
const  real A        = 5.0r;
const  real C        = 2.0r / 3.0r;

//// --- function ---
//
//function(t) 
//{ 
//   ^ new Point(
//      (A * (C * t.cos() + (C*t).cos()) * ScaleX).RoundedInt,
//      (A * (C * t.sin() - (C*t).sin()) * ScaleY).RoundedInt)
//}

public singleton Factory : IFactory
{
   IFunctionRange Range
      = new IFunctionRange
      {
         real RangeX = 20;
         real RangeY = 20;
         real From = starting;
         real To = ending;
         
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

//singleton Plotter
//{
//   drawAxis(Dimension size, Canvas canvas)
//   {
//      var zeroX := size.Width / 2;
//      var zeroY := size.Height / 2 - 10;
//      
//      // X axis
//      canvas.setCaret(0, zeroY);
//      canvas.lineCaretTo(size.Width, zeroY);
//      
//      canvas.writeText("X", size.Width - 20, zeroY + 2);
//      
//      // Y axis
//      canvas.setCaret(zeroX, 0);
//      canvas.lineCaretTo(zeroX, size.Height);
//      
//      canvas.writeText("Y", zeroX + 2, 2);
//      
//      for(int i := -10; i <= 10; i += 2)
//      {
//         if(i != 0)
//         {
//            canvas.writeText(i.toString(), zeroX + (i * ScaleX) - 4, zeroY + 2);
//            
//            canvas.writeText(i.toString(), zeroX + 8, zeroY - (i * ScaleY) - 2);
//         }
//         else
//         { 
//            canvas.writeText("0", zeroX + 2, zeroY + 2) 
//         }
//      }
//   }
//   
//   drawGraph(Dimension size, Canvas canvas)
//   {
//      auto t := starting;
//      auto t2 := ending;
//      var zero := new Point(size.Width / 2, size.Height / 2 - 10);
//   
//      canvas.setCaret(zero + function(t));
//      while (t <= t2)
//      {
//         canvas.lineCaretTo(zero + function(t));
//   
//         t := t + H
//      }
//   }
//}

public class MainWindow : SDIDialog
{
   Imagebox       _imagebox;
   
   Union<IFunction, IPolarFunction> _function;
   IFunctionRange                   _range;   
   
   constructor new()
      <= super new()
   {
      _range := Factory.Range;
      _function := Factory.Function;
      
//      _range := new IFunctionRange
//      {
//         real RangeX = 20;
//         real RangeY = 20;
//         real From = starting;
//         real To = ending;
//         int  Steps = 300;
//      };
//      _function := new IPolarFunction
//      {
//         real x(real angle)
//            = A * (C * angle.cos() + (C*angle).cos());
//            
//         real y(real angle)
//            = A * (C * angle.sin() - (C*angle).sin());
//      };
      
      _imagebox := Imagebox.new();
      self.appendControl(_imagebox);

      self.Caption := "Graph sample";
      self.setRegion(50, 50, 400, 420);

      _imagebox.setRegion(0, 0, 400, 420);
      _imagebox.onPaint := (object sender, Canvas canvas)
      {
         auto plotter := Plotter.new(_imagebox.Region.Dimension, canvas);
         plotter.RangeWidth := _range.RangeX;
         plotter.RangeHeight := _range.RangeY;
         
         canvas.set(WhitePen);
         canvas.set(BlackBrush);
   
         Controller.draw(plotter, _range, new Union<IFunction, IPolarFunction>(_function));
   
//         Plotter.drawAxis(_imagebox.Region.Dimension, canvas);
//         Plotter.drawGraph(_imagebox.Region.Dimension, canvas)
      }
   }
}