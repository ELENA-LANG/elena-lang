#define system.
#define forms.
#define extensions.
#define extensions'math.

#symbol(const,type:int) ScaleX   = 15.
#symbol(const,type:int) ScaleY   = 15.
#symbol starting = 0.0r.
#symbol ending   = mathControl pi * 6.
#symbol H        = 0.005r.
#symbol A        = 5.0r.
#static C        = /*2.0r / 3.0r*/0.6666666666666666666667r.

// --- function ---

#symbol function = (:t) [ 
            Point new 
                &x:(mathControl roundInt:(A * (C * mathControl cos:t + mathControl cos:(C*t)) * ScaleX))
                &y:(mathControl roundInt:(A * (C * mathControl sin:t - mathControl sin:(C*t)) * ScaleY))
        ].

#symbol plotter =
{
    drawAxis &dimension:aSize &canvas:aCanvas
    [        
        #var(type:int) aZeroX :=  aSize width / 2.
        #var(type:int) aZeroY := aSize height / 2 - 10.

        // X axis
        aCanvas setCaret &x:0 &y:aZeroY.
        aCanvas lineCaretTo &x:(aSize width) &y:aZeroY.
        
        aCanvas write &caption:"X" &x:(aSize width - 20) &y:(aZeroY + 2).

        // Y axis
        aCanvas setCaret &x:aZeroX &y:0.
        aCanvas lineCaretTo &x:aZeroX &y:(aSize height).

        aCanvas write &caption:"Y" &x:(aZeroX + 2) &y:2.

        control run &int:-10 &int:10 &int:2 &forEvery: (&index:i)
        [
            (i != 0) ?
            [
                aCanvas write &caption:(i literal) &x:(aZeroX + (i * ScaleX) - 4) &y:(aZeroY + 2).
                
                aCanvas write &caption:(i literal) &x:(aZeroX + 8) &y:(aZeroY - (i * ScaleY) - 2).
            ]
            ! [ aCanvas write &caption:"0" &x:(aZeroX + 2) &y:(aZeroY + 2) ].
        ].
    ]
    
    drawGraph &dimension:aSize &canvas:aCanvas
    [
        #var(type:int) aZeroX :=  aSize width / 2.
        #var(type:int) aZeroY := aSize height / 2 - 10.

        #var t := starting.
        #var t2 := ending.
        #var aZero := Point new &x:aZeroX &y:aZeroY.

        #var p := mathControl pi.
        #var s := mathControl cos:(p / 2).
        #var s2 := mathControl roundInt:s.

        #var p2 := aZero + function eval:1.825r.

        aCanvas setCaret &point:(aZero + function eval:t).
        #loop (t <= t2)?
        [
            aCanvas lineCaretTo &point:(aZero + function eval:t).

            t := t + H.
        ].
    ]
}.

#class Window :: SDIDialog
{
    #field theImagebox.

    #constructor new
        <= (new)
    [
        theImagebox := Imagebox new.
        theControls append:theImagebox.

        $self set &caption:"Graph sample".
        $self set &x:50 &y:50.
        $self set &width:400 &height:420.

        theImagebox set &x:0 &y:0.
        theImagebox set &width:400 &height:420.
        theImagebox set &onPaint: (:sender &canvas:aCanvas)
        [
            aCanvas set &pen:WhitePen.
            aCanvas set &brush:BlackBrush.

            plotter drawAxis &dimension:(theImagebox dimension) &canvas:aCanvas.
            plotter drawGraph &dimension:(theImagebox dimension) &canvas:aCanvas.
        ].
    ]
}