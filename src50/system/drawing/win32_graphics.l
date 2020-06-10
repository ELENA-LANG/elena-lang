// --- Pen ---

public sealed class Pen
{
    int  theColor;
    HPEN thePen;
    
    constructor(int color, int width)
    {
        theColor := color;
        thePen := HPEN.create(0, width, theColor)
    }

    HPEN cast() = thePen;
    
    int Color = theColor;
}

// --- Brush ---

public sealed class Brush
{
    int    theColor;
    HBRUSH theBrush;
    
    constructor(int color)
    {
        theColor := color;
        theBrush := HBRUSH.createSolid(theColor)
    }
    
    HBRUSH cast() = theBrush;
    
    int Color = theColor;
}

// --- WhitePen ---

public Pen WhitePen = new Pen(0FFFFFFh, 1);

// --- BlackBrush ---

public Brush BlackBrush = new Brush(0);

// --- Image ---

public sealed class Image
{
    wide      thePath;
    HDC       theDC;
    HBITMAP   theBitmap;
    Dimension theSize;
    
    constructor load(string path, Dimension size)
    {
        thePath := path;
        theSize := size
    }
    
    open(HDC parentDC)
    {
        int width := theSize.Width;
        int height := theSize.Height;
 
        theBitmap := HBITMAP.load(thePath, width, height);
        
        theDC := HDC.newCompatible(parentDC);
        theBitmap.select(theDC) 
    }
    
    HDC cast() = theDC;

    free()
    {
        theDC.free();
        theBitmap.free();
    }
}

// --- Canvas ---

public sealed struct Canvas
{
    HDC    theHDC;
    HBRUSH theBrush;
    HPEN   thePen;
    
    constructor()
    {
        theHDC := HDC.Default;
        theBrush := HBRUSH.Default;
        thePen := HPEN.Default;
    }
    
    open(HDC handle)
    {
        theHDC := handle
    }
    
    set(Pen pen)
    {
        ifnot (0 == thePen)
            { thePen.free() };
            
        thePen := pen;     
        
        theHDC.setTextColor(pen.Color);
        
        theHDC.select(thePen)
    }    
    
    set(Brush brush)
    {
        HBRUSH brushCopy := theBrush;
        
        ifnot (0 == brushCopy)
            { brushCopy.free() };
            
        brushCopy := brush;
        
        theHDC.setBkColor(brush.Color);
        
        theHDC.select(brushCopy);        
        
        theBrush := brushCopy
    }
    
    close()
    {
        theHDC := 0
    }
 
    setCaret(int x, int y)
    {
        theHDC.moveTo(x, y)
    }

    setCaret(Point p)
    {
        int x := p.X;
        int y := p.Y;
        //p read vint:x vint:y.
        
        theHDC.moveTo(x, y)
    }
 
    lineCaretTo(int x, int y)
    {
        theHDC.lineTo(x, y)
    }
 
    lineCaretTo(Point p)
    {
        int x := p.X;
        int y := p.Y;
        //p read vint:x vint:y.
        
        theHDC.lineTo(x, y)
    }
 
    writeText(wide s, int x, int y)
    {
        theHDC.drawText(s, x, y)
    }
 
    writeText(string s, int x, int y)
    {
        theHDC.drawText(cast wide(s), x, y)
    }
 
    write(Image image, int x, int y, int width, int height)
    {
        HDC dc := image;
        
        theHDC.copy(x, y, width, height, dc, 0, 0, SRCCOPY)
    } 
 
    free()
    {
        ifnot (thePen.IsEmpty())
            { thePen.free() };
           
        HBRUSH brushCopy := theBrush;
        ifnot (0 == brushCopy)
            { brushCopy.free() };
            
        theBrush := HBRUSH.Default;
        thePen := HPEN.Default;
    }
}