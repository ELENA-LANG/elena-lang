#define std'dictionary'*.
#define gui'dictionary'*.
//#define std'basic'*.
#define std'patterns'*.
#define ext'text'*.
//#define ext'patterns'*.

#define math'* = std'math'*.
#define gui'* = gui'common'*.
#define controls'* = gui'controls'*.
#define forms'* = gui'forms'*.
#define graph'* = gui'graphics'*.

// --- private subjects ---
#subject $drawAxis, $drawGraph.

// === code ===
#symbol ScaleX   = 15.
#symbol ScaleY   = 15.
#symbol starting = 0.0r.
#symbol ending   = math'Pi * 6.
#symbol H        = 0.005r.
#symbol A        = 5.0r.

#static C        = 2.0r / 3.0r.

// --- function ---

#symbol Function : T = gui'Point
            &&x:(math'Rounded::(A * (c * math'cos::T + math'cos::(c*T)) * ScaleX))
             &y:(math'Rounded::(A * (c * math'sin::T - math'sin::(c*T)) * ScaleY)).

#class Window (forms'SDIDialog)
{
    #field theImagebox.
    #field theZeroX.
    #field theZeroY.

    #method $drawAxis : aCanvas
    [
        #var aSize := theImagebox dimensions.
        theZeroX :=  aSize width / 2.
        theZeroY := aSize height / 2 - 10.

        #var aPlotter := graph'Plotter::aCanvas.

        // X axis
        aPlotter write &x:0 &y:theZeroY.
        aPlotter append &x:(aSize width) &y:theZeroY.

        aCanvas write &caption:"X" &x:(aSize width - 20) &y:(theZeroY + 2) &:graph'atextdrawing.

        // Y axis
        aPlotter write &x:theZeroX &y:0.
        aPlotter append &x:theZeroX &y:(aSize height).

        aCanvas write &caption:"Y" &x:(theZeroX + 2) &y:2 &:graph'atextdrawing.

        loop &&from:-10 &to:10 &step:2 run: anIndex =
        [
            #if (anIndex != 0)?
            [
                aCanvas write 
                    &caption:PrintableValue::anIndex &x:(theZeroX + (anIndex * ScaleX) - 4) &y:(theZeroY + 2) &:graph'atextdrawing.
                
                aCanvas write 
                    &caption:PrintableValue::anIndex &x:(theZeroX + 8) &y:(theZeroY - (anIndex * ScaleY) - 2) &:graph'atextdrawing.
            ]
            | [  
                aCanvas write 
                    &caption:"0" &x:(theZeroX + 2) &y:(theZeroY + 2) &:graph'atextdrawing.
            ].
        ].
    ]

    #method $drawGraph : aCanvas
    [
        #var aPlotter := graph'Plotter::aCanvas.

        #var t := starting.
        #var t2 := ending.
        #var aZero := gui'PointVar &&x:theZeroX &y:theZeroY.

        #var B := H.

        aPlotter << aZero add:(Function::t).
        #while (t <= t2)?
        [
            aPlotter += aZero add:(Function::t).

            t := t + H.
        ].
    ]

    #method painting &canvas:aCanvas
    [
        aCanvas 
            graph'pen'set:graph'WhitePen
            graph'brush'set:graph'BlackBrush.

        self $drawAxis:aCanvas $drawGraph:aCanvas.
    ]

    #method initializing : Args
    [
        super initializing:Args.

        theImagebox := controls'Imagebox::self.

        theImagebox event_handle'append:$self.

        self
            dimensions'set &width:400 &height:420
            coordinates'set &x:50 &y:50
            caption'set: "Graph sample".

        theImagebox
            dimensions'set &width:400 &height:420
            coordinates'set &x:0 &y:0
            open.
    ]
}