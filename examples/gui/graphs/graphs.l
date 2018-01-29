import system'math.
import forms.
import system'drawing.
import extensions.

const  ScaleX   = 15.
const  ScaleY   = 15.
const  starting = 0.0r.
symbol ending   = pi_value * 6.
const  H        = 0.005r.
const  A        = 5.0r.
symbol C        = 2.0r / 3.0r.

// --- function ---

symbol function = (:t) [ 
            ^ Point new 
                x((A * (C * t cos + (C*t) cos) * ScaleX) roundedInt)
                y((A * (C * t sin - (C*t) sin) * ScaleY) roundedInt)
        ].

plotter =
{
    drawAxis dimension:aSize canvas:aCanvas
    [        
        var aZeroX :=  aSize width / 2.
        var aZeroY := aSize height / 2 - 10.

        // X axis
        aCanvas setCaret x:0 y:aZeroY.
        aCanvas lineCaretTo x:(aSize width) y:aZeroY.
        
        aCanvas writeText:"X" x:(aSize width - 20) y:(aZeroY + 2).

        // Y axis
        aCanvas setCaret x:aZeroX y:0.
        aCanvas lineCaretTo x:aZeroX y:(aSize height).

        aCanvas writeText:"Y" x:(aZeroX + 2) y:2.

        -10 to:10 by:2 do(:i)
        [
            if(i != 0)
            [
                aCanvas writeText:i x:(aZeroX + (i * ScaleX) - 4) y:(aZeroY + 2).
                
                aCanvas writeText:i x:(aZeroX + 8) y:(aZeroY - (i * ScaleY) - 2)
            ];
            [ aCanvas writeText:"0" x:(aZeroX + 2) y:(aZeroY + 2) ]
        ].
    ]
    
    drawGraph dimension:aSize canvas:aCanvas
    [
        var t := starting.
        var t2 := ending.
        var aZero := Point new x:(aSize width / 2) y:(aSize height / 2 - 10).

        aCanvas setCaret:(aZero + function eval:t).
        while (t <= t2)
        [
            aCanvas lineCaretTo:(aZero + function eval:t).

            t := t + H
        ]
    ]
}.

class Window :: SDIDialog
{
    object theImagebox.

    constructor new
        <= new;
    [
        theImagebox := Imagebox new.
        theControls append:theImagebox.

        $self set caption:"Graph sample".
        $self set x:50 y:50.
        $self set width:400 height:420.

        theImagebox set x:0 y:0.
        theImagebox set width:400 height:420.
        theImagebox set onPaint: (:sender:aCanvas)<object,canvas>
        [
            aCanvas set:WhitePen.
            aCanvas set:BlackBrush.

            plotter drawAxis dimension:(theImagebox dimension) canvas:aCanvas.
            plotter drawGraph dimension:(theImagebox dimension) canvas:aCanvas
        ].
    ]
}