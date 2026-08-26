//import system'math;
import forms;
import system'drawing;
import system'dynamic;
import system'routines;
import extensions;

registration : preloaded()
   = true.then(
   {
      DependencyInjector::registerSingleton<IFunctionSelector>(Provider);
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
      canvas.fillRecangle(0, 0, outputSize.Width, outputSize.Height);
      
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
      Point p := new Point(zeroX + (x * scaleX).RoundedInt, zeroY - (y * scaleY).RoundedInt);
   
      canvas.setCaret(p);      
   }
   
   moveTo(real x, real y)
   {
      Point p := new Point(zeroX + (x * scaleX).RoundedInt, zeroY - (y * scaleY).RoundedInt);

      canvas.lineCaretTo(p);
   }   
}

public class MainWindow : SDIForm
{
   DockingBox        _dockingBox;
   Imagebox          _imagebox;
   Panel             _funPanel; 
   RadioButtonGroup  _selector;
   
   string[]          _names;
   IFactory          _factory;
   
   constructor new(IFunctionSelector provider) : injectable_constructor()
      <= super new()
   {
      _names := provider.Names;
      
      _dockingBox := DockingBox.new();
      self.appendControl(_dockingBox);
      
      _imagebox := Imagebox.new();
      self.appendControl(_imagebox);

      _funPanel := Panel.new();
      self.appendControl(_funPanel);

      _selector := RadioButtonGroup.new();
      _funPanel.appendControl(_selector);

      self.Caption := "Graph sample";
      self.setRegion(50, 50, 500, 420);

      _imagebox.setRegion(0, 0, 400, 420);
      
      _funPanel.Caption := "Function";
      _funPanel.setRegion(2, 2, 100, 420);
      
      _selector.Caption := "select..";
      _selector.setRegion(5, 20, 90, 355);
      _names.forEach::(name => _selector.appendItem(name));
      _selector.onIndexChanged := (args){ 
         _factory := provider.select(_names[_selector.SelectedIndex]);
         
         _imagebox.refresh();
      };
      _selector.SelectedIndex := 0;
                                    
      _dockingBox.Client := _imagebox;
      _dockingBox.Right := _funPanel;
      
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
