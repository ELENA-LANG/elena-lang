// --- Pen ---

public sealed class Pen
{
   int  _color;
   HPEN _pen;
    
   constructor(int color, int width)
   {
      _color := color;
      _pen := HPEN.create(0, width, _color)
   }

   HPEN cast() = _pen;
    
   int Color = _color;
}

// --- Brush ---

public sealed class Brush
{
   int    _color;
   HBRUSH _brush;
    
   constructor(int color)
   {
      _color := color;
      _brush := HBRUSH.createSolid(color)
   }
    
   HBRUSH cast() = _brush;
    
   int Color = _color;
}

// --- WhitePen ---

public Pen WhitePen = new Pen(0FFFFFFh, 1);

// --- BlackBrush ---

public Brush BlackBrush = new Brush(0);

// --- Image ---

public sealed class Image
{
   wide      _path;
   HDC       _DC;
   HBITMAP   _bitmap;
   Dimension _size;
    
   constructor load(string path, Dimension size)
   {
      _path := path;
      _size := size
   }
    
   open(HDC parentDC)
   {
      int width := _size.Width;
      int height := _size.Height;
 
      _bitmap := HBITMAP.load(_path, width, height);
        
      _DC := HDC.newCompatible(parentDC);
      _bitmap.select(_DC) 
   }
    
   HDC cast() = _DC;

   free()
   {
      _DC.free();
      _bitmap.free();
   }
}

// --- Canvas ---

public sealed struct Canvas
{
   HDC    _HDC;
   HBRUSH _brush;
   HPEN   _pen;
    
   constructor()
   {
      _HDC := HDC.Default;
      _brush := HBRUSH.Default;
      _pen := HPEN.Default;
   }
    
   open(HDC handle)
   {
      _HDC := handle
   }
    
   set(Pen pen)
   {
      if:not (0 == _pen)
         { _pen.free() };
            
      _pen := pen;     
        
      _HDC.setTextColor(pen.Color);
        
      _HDC.select(_pen)
   }    
    
   set(Brush brush)
   {
      HBRUSH brushCopy := _brush;
        
      if:not (0 == brushCopy)
         { brushCopy.free() };
            
      brushCopy := brush;
        
      _HDC.setBkColor(brush.Color);
        
      _HDC.select(brushCopy);        
        
      _brush := brushCopy
   }
    
   close()
   {
      _HDC := HDC.Default
   }
 
   setCaret(int x, int y)
   {
      _HDC.moveTo(x, y)
   }

   setCaret(Point p)
   {
      int x := p.X;
      int y := p.Y;      
        
      _HDC.moveTo(x, y)
   }
 
   lineCaretTo(int x, int y)
   {
      _HDC.lineTo(x, y)
   }
 
   lineCaretTo(Point p)
   {
      int x := p.X;
      int y := p.Y;
        
      _HDC.lineTo(x, y)
   }
 
   writeText(wide s, int x, int y)
   {
      _HDC.drawText(s, x, y)
   }
 
   writeText(string s, int x, int y)
   {
      _HDC.drawText(cast wide(s), x, y)
   }
 
   write(Image image, int x, int y, int width, int height)
   {
      HDC dc := image;
        
      _HDC.copy(x, y, width, height, dc, 0, 0, SRCCOPY)
   } 
 
   free()
   {
      if:not (_pen.IsEmpty())
         { _pen.free() };
           
      HBRUSH brushCopy := _brush;
      if:not (0 == brushCopy)
         { brushCopy.free() };
            
      _brush := HBRUSH.Default;
      _pen := HPEN.Default;
   }
}
