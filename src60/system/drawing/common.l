import system;

// --- Point ---

public sealed struct Point
{
   int _x;
   int _y;

   constructor()
   {
      _x := 0;
      _y := 0
   }

   constructor(int x, int y)
   {
      _x := x;
      _y := y
   }
   
   int X
   {
      get() = _x;
        
      set(int value)
      {
         _x := value
      }
   }
    
   int Y
   {
      get() = _y;
        
      set(int value)
      {
         _y := value
      }
   }

   Point clone() = new Point(_x, _y);
    
   bool equal(Point p)
   {
      int x := p.X;
      int y := p.Y;
        
      ^ (_x == x) && (_y == y)
   }
    
   bool less(Point p)
   {
      int x := p.X;
      int y := p.Y;
        
      if (_y < y)
         { ^ true };
            
      if (_y == y)
         { ^ _x < x };

      ^ false  
   }

   Point add(Point p)
   {
      int x := p.X;
      int y := p.Y;
        
      ^ new Point(_x + x, _y + y)
   } 
    
   Point subtract(Point p)
   {
      int x := p.X;
      int y := p.Y;
        
      ^ new Point(_x - x, _y - y)
    }
}

public sealed struct Region //: BaseValue
{
   int _x;
   int _y;
   int _width;
   int _height;
    
   constructor()
   {
      _x := 0;
      _y := 0;
      _width := 0;
      _height := 0;
   }
    
   int X
   {
      get()
      {
         ^ _x
      }
        
      set(int val)
      {
         _x := val
      }
   }

   int Y
   {
      get()
      {
         ^ _y
      }
        
      set(int val)
      {
         _y := val
      }
   }

   int Width
   {
      get()
      {
         ^ _width
      }
        
      set(int val)
      {
         _width := val
      }
   }

   int Height
   {
      get()
      {
         ^ _height
      }
        
      set(int val)
      {
         _height := val
      }
   }

   set(int x, int y, int width, int height)
   {
      _x := x;
      _y := y;
      _width := width;
      _height := height
   }

   read(ref int x, ref int y, ref int width, ref int height)
   {
      x := _x;
      y := _y;
      width := _width;
      height := _height;
   }
   
}
