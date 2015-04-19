#define system.
#define system'winforms.

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
    
    #method(type:int) color = theColor.
}

// --- WhitePen ---

#symbol WhitePen = Pen new &color:0FFFFFFh &width:1.

// --- BlackBrush ---

#symbol BlackBrush = Brush new &color:0.

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
    
    #method open &hdc:aHandle
    [
        theHDC := aHandle.
    ]
    
    #method open &win_paintstruct:aPaintStruct
    [
        #var(type:hdc)dc.
        aPaintStruct read &hdc:dc.
        
        $self open &hdc:dc.
    ]
    
    #method set &pen:aPen
    [
        (0 == thePen)
            ! [ thePen free. ].
            
        thePen := aPen hpen.            
        
        theHDC setTextColor &int:(aPen color).
        
        theHDC select &hpen:thePen.        
    ]    
    
    #method set &brush:aBrush
    [
        (0 == theBrush)
            ! [ theBrush free. ].
            
        theBrush := aBrush hbrush.
        
        theHDC setBkColor &int:(aBrush color).
        
        theHDC select &hbrush:theBrush.        
    ]    
    
    #method close
    [
        theHDC := 0.
    ]
 
    #method setCaret &int:x &int:y
    [
        theHDC moveTo &int:x &int:y.
    ]
 
    #method setCaret &x:x &y:y
    [
        theHDC moveTo &int:(x int) &int:(y int).
    ]
 
    #method setCaret &point:p
    [
        #var(type:vint)x.
        #var(type:vint)y.
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
 
    #method lineCaretTo &point:p
    [
        #var(type:vint)x.
        #var(type:vint)y.
        p read &vint:x &vint:y.
        
        theHDC lineTo &int:x &int:y.
    ]
 
    #method write &caption:aText &x:x &y:y
    [
        theHDC drawText &wide:(aText wide) &int:(x int) &int:(y int).
    ]
 
    #method write &wide:aText &int:x &int:y
    [
        theHDC drawText &wide:aText &int:x &int:y.
    ]
 
    #method free
    [
        (0 == thePen)
            ! [ thePen free. ].
            
        (0 == theBrush)
            ! [ theBrush free. ].
            
        theBrush := 0.
        thePen := 0.
    ]
}