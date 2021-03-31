// --- ProgramLoop ---

class ProgramLoop
{
    bool   theActive;
    object theControl;
    
    constructor assign(control)
    {
        theActive := true;
        theControl := control;
        
        // if initializing is not defined - do nothing
        theControl.\initializing();
    }
    
    stop() { theActive := false }
    
    function()
    {
        // if grettings is not defined - do nothing
        console.printLine(theControl.greeting() ?? emptyString);
        
        var prompt := theControl.Prompt ?? emptyString;
        
        while (theActive)
        {
            console.write:prompt;

            console.printLine(theControl.proceed(console.readLine()))
        }
    }
}

public static programLoop = ProgramLoop.assign(forward program'control);
