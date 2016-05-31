// ==== ver 3.4.1 ===

#import system.

// --- ProgramLoop ---

#class $ProgramLoop
{
    #field(bool) theActive.
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
            | on:
            {
                Abort = e [ #throw e. ].
                
                ! = e [].
            }.
        
        #var aPrompt := theControl prompt.
        
        #loop theActive?
        [
            console write:aPrompt.
            
            console writeLine:(theControl proceed:(console readLine)).
        ].
    ]
}

#static programLoop = $ProgramLoop new:'program'control.
