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

//// --- Image ---
//
//sealed class Image
//{
//    wide      thePath.
//    hdc       theDC.
//    hbitmap   theBitmap.
//    dimension theSize.
//    
//    constructor new literal:aPath dimension:aSize
//    [
//        thePath := aPath wide.
//        theSize := aSize.
//    ]
//    
//    stacksafe open hdc:aParentDC
//    [
//        int aWidth := 0.
//        int aHeight := 0.
//        theSize read vint:aWidth vint:aHeight.
// 
//        theBitmap := HBITMAP load wide:thePath int:aWidth int:aHeight.
//        
//        theDC := HDC newCompatible hdc:aParentDC.
//        theBitmap select hdc:theDC. 
//    ]    
//    
//    hdc = theDC.
//
//    free
//    [
//        theDC free.
//        theBitmap free.
//    ]
//}

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
 
//    setCaret:p
//        <= setCaret coord:(p coord).
        
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
 
//    lineCaretTo:p
//        <= lineCaretTo coord:(p coord).
        
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
 
//    stacksafe write wide:aText int:x int:y
//    [
//        theHDC drawText wide:aText int:x int:y.
//    ]
// 
//    stacksafe write literal:aText int:x int:y
//    [
//        theHDC drawText wide:(aText wide) int:x int:y.
//    ]
// 
//    stacksafe write image:anImage int:x int:y int:aWidth int:aHeight
//    [
//        hdc aDC := anImage hdc.
//        
//        theHDC copy int:x int:y int:aWidth int:aHeight hdc:aDC int:0 int:0 int:SRCCOPY.
//    ] 
 
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