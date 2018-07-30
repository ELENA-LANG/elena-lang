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
    
    closure
    [
        // if grettings is not defined - do nothing
        console printLine(theControl greeting \ back:emptyLiteral).
        
        var aPrompt := theControl prompt.
        
        while (theActive)
        [
            console write:aPrompt.

            console printLine(theControl proceed(console readLine)).
        ].
    ]
}

public static programLoop = ProgramLoop new:forwards'program'control.
