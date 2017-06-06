#subject pen    = system'drawing'Pen.
#subject brush  = system'drawing'Brush.
#subject canvas = system'drawing'Canvas.
#subject image  = system'drawing'Image.

// --- Pen ---

sealed dispatchable(pen) class Pen
{
    int  theColor.
    hpen thePen.
    
    constructor new &color:aColor &width:aWidth
    [
        theColor := aColor int.
        thePen := HPEN new &int:0 &int:(aWidth int) &int:theColor.
    ]

    hpen = thePen.
    
    int color = theColor.
}

// --- Brush ---

sealed dispatchable(brush) class Brush
{
    int    theColor.
    hbrush theBrush.
    
    constructor new &color:aColor
    [
        theColor := aColor int.
        theBrush := HBRUSH new &int:theColor.
    ]
    
    hbrush = theBrush.
    
    int color = theColor.
}

// --- WhitePen ---

pen WhitePen = Pen new &color:0FFFFFFh &width:1.

// --- BlackBrush ---

brush BlackBrush = Brush new &color:0.

// --- Image ---

sealed class Image
{
    wide      thePath.
    hdc       theDC.
    hbitmap   theBitmap.
    dimension theSize.
    
    constructor new &path:aPath &dimension:aSize
    [
        thePath := aPath wide.
        theSize := aSize.
    ]
    
    stacksafe open &hdc:aParentDC
    [
        int aWidth := 0.
        int aHeight := 0.
        theSize read &vint:aWidth &vint:aHeight.
 
        theBitmap := HBITMAP load &wide:thePath &int:aWidth &int:aHeight.
        
        theDC := HDC newCompatible &hdc:aParentDC.
        theBitmap select &hdc:theDC. 
    ]    
    
    hdc = theDC.

    free
    [
        theDC free.
        theBitmap free.
    ]
}

// --- Canvas ---

sealed struct Canvas
{
    hdc    theHDC.
    hbrush theBrush.
    hpen   thePen.
    
    constructor new
    [
        theHDC := 0.
        theBrush := 0.
        thePen := 0.
    ]
    
    stacksafe open &hdc:aHandle
    [
        theHDC := aHandle.
    ]
    
    set : anObject
        = anObject cast:%set &to:$self.
    
    set &pen:aPen
    [
        ifnot (0 == thePen)
            [ thePen free. ].
            
        thePen := aPen hpen.            
        
        theHDC setTextColor &int:(aPen color int).
        
        theHDC select &hpen:thePen.
    ]    
    
    set &brush:aBrush
    [
        hbrush aBrushCopy := theBrush.
        
        ifnot (0 == aBrushCopy)
            [ aBrushCopy free. ].
            
        aBrushCopy := aBrush hbrush.
        
        theHDC setBkColor &int:(aBrush color int).
        
        theHDC select &hbrush:aBrushCopy.        
        
        theBrush := aBrushCopy.
    ]    
    
    close
    [
        theHDC := 0.
    ]
 
    setCaret:p
        <= setCaret &coord:(p coord).
        
    setCaret &int:x &int:y
    [
        theHDC moveTo &int:x &int:y.
    ]
 
    setCaret &x:x &y:y
    [
        theHDC moveTo &int:(x int) &int:(y int).
    ]
 
    setCaret &coord:p
    [
        int x := 0.
        int y := 0.
        p read &vint:x &vint:y.
        
        theHDC moveTo &int:x &int:y.
    ]
 
    lineCaretTo &int:x &int:y
    [
        theHDC lineTo &int:x &int:y.
    ]
 
    lineCaretTo &x:x &y:y
    [
        theHDC lineTo &int:(x int) &int:(y int).
    ]
 
    lineCaretTo:p
        <= lineCaretTo &coord:(p coord).
        
    lineCaretTo &coord:p
    [
        int x := 0.
        int y := 0.
        p read &vint:x &vint:y.
        
        theHDC lineTo &int:x &int:y.
    ]
 
    writeText:aText &x:x &y:y        
    [
        theHDC drawText &wide:(aText wide) &int:(x int) &int:(y int).
    ]
 
    stacksafe write &wide:aText &int:x &int:y
    [
        theHDC drawText &wide:aText &int:x &int:y.
    ]
 
    stacksafe write &literal:aText &int:x &int:y
    [
        theHDC drawText &wide:(aText wide) &int:x &int:y.
    ]
 
    stacksafe write &image:anImage &int:x &int:y &int:aWidth &int:aHeight
    [
        hdc aDC := anImage hdc.
        
        theHDC copy &int:x &int:y &int:aWidth &int:aHeight &hdc:aDC &int:0 &int:0 &int:SRCCOPY.
    ] 
 
    free
    [
        ifnot (0 == thePen)
            [ thePen free. ].
           
        hbrush aBrushCopy := theBrush.
        ifnot (0 == aBrushCopy)
            [ aBrushCopy free. ].
            
        theBrush := 0.
        thePen := 0.
    ]
}