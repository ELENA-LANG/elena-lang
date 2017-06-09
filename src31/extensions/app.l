// --- ProgramLoop ---

class $ProgramLoop
{
    bool   theActive.
    object theControl.
    
    constructor new : aControl
    [
        theActive := true.
        theControl := aControl.
        
        // if initializing is not defined - do nothing
        theControl initializing \ back:$nil.
    ]
    
    stop [ theActive := false. ]
    
    eval
    [
        // if grettings is not defined - do nothing
        console writeLine:(theControl greeting) \ back:$nil.
        
        var aPrompt := theControl prompt.
        
        while (theActive)
        [
            console write:aPrompt.
            
            console writeLine:(theControl proceed:(console readLine)).
        ].
    ]
}

static programLoop = $ProgramLoop new:'program'control.
