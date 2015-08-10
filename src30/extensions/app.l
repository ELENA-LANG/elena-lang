#define system.

// --- ProgramLoop ---

#class $ProgramLoop
{
    #field(type:bool) theActive.
    #field theControl.
    
    #constructor new : aControl
    [
        theActive := true.
        theControl := aControl.
    ]
    
    #method stop [ theActive := false. ]
    
    #method eval
    [
        // if grettings is not defined - do nothing
        console writeLine:(theControl greeting)
            | if &Error:e [].
        
        #var aPrompt := theControl prompt.
        
        #loop theActive?
        [
            console write:aPrompt.
            
            console writeLine:(theControl proceed:(console readLine)).
        ].
    ]
}

#static programLoop = $ProgramLoop new:'program'control.
