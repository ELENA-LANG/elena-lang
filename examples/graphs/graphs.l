#define system.
#define system'math.
#define system'drawing.
#define forms.
#define extensions.

#symbol ScaleX   = 15.
#symbol ScaleY   = 15.
#symbol starting = 0.0r.
#symbol ending   = pi_value * 6.
#symbol H        = 0.005r.
#symbol A        = 5.0r.
#symbol C        = 2.0r / 3.0r.

// --- function ---

#symbol function = (:t) [ 
            Point new 
                &x:((A * (C * t cos + (C*t) cos) * ScaleX) roundedInt)
                &y:((A * (C * t sin - (C*t) sin) * ScaleY) roundedInt)
        ].

#symbol plotter =
{
    drawAxis &dimension:aSize &canvas:aCanvas
    [        
        #var aZeroX :=  aSize width / 2.
        #var aZeroY := aSize height / 2 - 10.

        // X axis
        aCanvas setCaret &x:0 &y:aZeroY.
        aCanvas lineCaretTo &x:(aSize width) &y:aZeroY.
        
        aCanvas writeText:"X" &x:(aSize width - 20) &y:(aZeroY + 2).

        // Y axis
        aCanvas setCaret &x:aZeroX &y:0.
        aCanvas lineCaretTo &x:aZeroX &y:(aSize height).

        aCanvas writeText:"Y" &x:(aZeroX + 2) &y:2.

        -10 to:10 &by:2 &doEach: i
        [
            (i != 0) ?
            [
                aCanvas writeText:i &x:(aZeroX + (i * ScaleX) - 4) &y:(aZeroY + 2).
                
                aCanvas writeText:i &x:(aZeroX + 8) &y:(aZeroY - (i * ScaleY) - 2).
            ]
            ! [ aCanvas writeText:"0" &x:(aZeroX + 2) &y:(aZeroY + 2) ].
        ].
    ]
    
    drawGraph &dimension:aSize &canvas:aCanvas
    [
        #var t := starting.
        #var t2 := ending.
        #var aZero := Point new &x:(aSize width / 2) &y:(aSize height / 2 - 10).

        aCanvas setCaret:(aZero + function eval:t).
        #loop (t <= t2)?
        [
            aCanvas lineCaretTo:(aZero + function eval:t).

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
            aCanvas set:WhitePen.
            aCanvas set:BlackBrush.

            plotter drawAxis &dimension:(theImagebox dimension) &canvas:aCanvas.
            plotter drawGraph &dimension:(theImagebox dimension) &canvas:aCanvas.
        ].
    ]
}