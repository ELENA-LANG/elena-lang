// --- ProgramLoop ---

class ProgramLoop
{
    bool   theActive.
    object theControl.
    
    constructor new : aControl
    [
        theActive := true.
        theControl := aControl.
        
        // if initializing is not defined - do nothing
        theControl~__prober initializing.
    ]
    
    stop [ theActive := false. ]
    
    closure eval
    [
        // if grettings is not defined - do nothing
        console printLine(theControl greeting \ back:emptyLiteral).
        
        var aPrompt := theControl prompt.
        
        while (theActive)
        [
            console write:aPrompt.

            var s := console readLine.
            var c1 := console.
            var s2 := theControl proceed(s).
            var c2 := console.
                                    
            console printLine(/*theControl proceed(console readLine)*/s2).
        ].
    ]
}

public static programLoop = ProgramLoop new:forwards'program'control.
