#import system.

#subject pen    = system'drawing'Pen.
#subject brush  = system'drawing'Brush.
#subject canvas = system'drawing'Canvas.
#subject image  = system'drawing'Image.

// --- Pen ---

#class(sealed,dispatchable:pen)Pen
{
    #field(int) theColor.
    #field(hpen)thePen.
    
    #constructor new &color:aColor &width:aWidth
    [
        theColor := aColor int.
        thePen := HPEN new &int:0 &int:(aWidth int) &int:theColor.
    ]

    #method hpen = thePen.
    
    #method(int) color = theColor.
}

// --- Brush ---

#class(sealed,dispatchable:brush)Brush
{
    #field(int) theColor.
    #field(hbrush)theBrush.
    
    #constructor new &color:aColor
    [
        theColor := aColor int.
        theBrush := HBRUSH new &int:theColor.
    ]
    
    #method hbrush = theBrush.
    
    #method(int) color = theColor.
}

// --- WhitePen ---

#symbol WhitePen = Pen new &color:0FFFFFFh &width:1.

// --- BlackBrush ---

#symbol BlackBrush = Brush new &color:0.

// --- Image ---

#class(sealed) Image
{
    #field(wide)thePath.
    #field(hdc)theDC.
    #field(hbitmap)theBitmap.
    #field(dimension)theSize.
    
    #constructor new &path:aPath &dimension:aSize
    [
        thePath := aPath wide.
        theSize := aSize.
    ]
    
    #method(stacksafe) open &hdc:aParentDC
    [
        #var(int)aWidth.
        #var(int)aHeight.
        theSize read &vint:aWidth &vint:aHeight.
 
        theBitmap := HBITMAP load &wide:thePath &int:aWidth &int:aHeight.
        
        theDC := HDC newCompatible &hdc:aParentDC.
        theBitmap select &hdc:theDC. 
    ]    
    
    #method hdc = theDC.

    #method free
    [
        theDC free.
        theBitmap free.
    ]
}

// --- Canvas ---

#class(sealed,struct) Canvas
{
    #field(hdc)    theHDC.
    #field(hbrush) theBrush.
    #field(hpen)   thePen.
    
    #constructor new
    [
        theHDC := 0.
        theBrush := 0.
        thePen := 0.
    ]
    
    #method(stacksafe) open &hdc:aHandle
    [
        theHDC := aHandle.
    ]
    
    #method set : anObject
        = anObject cast:%set &to:$self.
    
    #method set &pen:aPen
    [
        (0 == thePen)
            ! [ thePen free. ].
            
        thePen := aPen hpen.            
        
        theHDC setTextColor &int:(aPen color int).
        
        theHDC select &hpen:thePen.
    ]    
    
    #method set &brush:aBrush
    [
        #var(hbrush) aBrushCopy := theBrush.
        
        (0 == aBrushCopy)
            ! [ aBrushCopy free. ].
            
        aBrushCopy := aBrush hbrush.
        
        theHDC setBkColor &int:(aBrush color int).
        
        theHDC select &hbrush:aBrushCopy.        
        
        theBrush := aBrushCopy.
    ]    
    
    #method close
    [
        theHDC := 0.
    ]
 
    #method setCaret:p
        <= setCaret &coord:(p coord).
        
    #method setCaret &int:x &int:y
    [
        theHDC moveTo &int:x &int:y.
    ]
 
    #method setCaret &x:x &y:y
    [
        theHDC moveTo &int:(x int) &int:(y int).
    ]
 
    #method setCaret &coord:p
    [
        #var(int)x.
        #var(int)y.
        p read &vint:x &vint:y.
        
        theHDC moveTo &int:x &int:y.
    ]
 
    #method lineCaretTo &int:x &int:y
    [
        theHDC lineTo &int:x &int:y.
    ]
 
    #method lineCaretTo &x:x &y:y
    [
        theHDC lineTo &int:(x int) &int:(y int).
    ]
 
    #method lineCaretTo:p
        <= lineCaretTo &coord:(p coord).
        
    #method lineCaretTo &coord:p
    [
        #var(int)x.
        #var(int)y.
        p read &vint:x &vint:y.
        
        theHDC lineTo &int:x &int:y.
    ]
 
    #method writeText:aText &x:x &y:y        
    [
        theHDC drawText &wide:(aText wide) &int:(x int) &int:(y int).
    ]
 
    #method(stacksafe) write &wide:aText &int:x &int:y
    [
        theHDC drawText &wide:aText &int:x &int:y.
    ]
 
    #method(stacksafe) write &literal:aText &int:x &int:y
    [
        theHDC drawText &wide:(aText wide) &int:x &int:y.
    ]
 
    #method(stacksafe) write &image:anImage &int:x &int:y &int:aWidth &int:aHeight
    [
        #var(hdc)aDC := anImage hdc.
        
        theHDC copy &int:x &int:y &int:aWidth &int:aHeight &hdc:aDC &int:0 &int:0 &int:SRCCOPY.
    ] 
 
    #method free
    [
        (0 == thePen)
            ! [ thePen free. ].
           
        #var(hbrush) aBrushCopy := theBrush.
        (0 == aBrushCopy)
            ! [ aBrushCopy free. ].
            
        theBrush := 0.
        thePen := 0.
    ]
}