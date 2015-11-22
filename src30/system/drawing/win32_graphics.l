// ==== ver 3.4.1 ===

#import system.

#subject(class:system'drawing'Pen)    pen.
#subject(class:system'drawing'Brush)  brush.
#subject(class:system'drawing'Canvas) canvas.
#subject(class:system'drawing'Image)  image.

// --- Pen ---

#class(sealed)Pen
{
    #field(type:int) theColor.
    #field(type:hpen)thePen.
    
    #constructor new &color:aColor &width:aWidth
    [
        theColor := aColor int.
        thePen := HPEN new &int:0 &int:(aWidth int) &int:theColor.
    ]

    #method pen = $self.
            
    #method hpen = thePen.
    
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &pen:$self.
    
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &pen:$self.
    
    #method(type:int) color = theColor.
}

// --- Brush ---

#class(sealed)Brush
{
    #field(type:int) theColor.
    #field(type:hbrush)theBrush.
    
    #constructor new &color:aColor
    [
        theColor := aColor int.
        theBrush := HBRUSH new &int:theColor.
    ]
    
    #method brush = $self.
    
    #method hbrush = theBrush.
    
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &brush:$self.
    
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &brush:$self.
    
    #method(type:int) color = theColor.
}

// --- WhitePen ---

#symbol WhitePen = Pen new &color:0FFFFFFh &width:1.

// --- BlackBrush ---

#symbol BlackBrush = Brush new &color:0.

// --- Image ---

#class(sealed) Image
{
    #field(type:wide)thePath.
    #field(type:hdc)theDC.
    #field(type:hbitmap)theBitmap.
    #field(type:dimension)theSize.
    
    #constructor new &path:aPath &dimension:aSize
    [
        thePath := aPath wide.
        theSize := aSize.
    ]
    
    #method image = $self.
    
    #method(stacksafe) open &hdc:aParentDC
    [
        #var(type:int)aWidth.
        #var(type:int)aHeight.
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
    #field(type:hdc)theHDC.
    #field(type:hbrush)theBrush.
    #field(type:hpen)thePen.
    
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
        #var(type:hbrush) aBrushCopy := theBrush.
        
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
        #var(type:int)x.
        #var(type:int)y.
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
        #var(type:int)x.
        #var(type:int)y.
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
        #var(type:hdc)aDC := anImage hdc.
        
        theHDC copy &int:x &int:y &int:aWidth &int:aHeight &hdc:aDC &int:0 &int:0 &int:SRCCOPY.
    ] 
 
    #method free
    [
        (0 == thePen)
            ! [ thePen free. ].
           
        #var(type:hbrush) aBrushCopy := theBrush.
        (0 == aBrushCopy)
            ! [ aBrushCopy free. ].
            
        theBrush := 0.
        thePen := 0.
    ]
}