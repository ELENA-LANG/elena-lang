#define system.
#define system'threading.

// --- threadControl ---

#symbol threadControl =
{
    start &func:anAction
    [
        #var aThread := Thread new &func:anAction.
        
        aThread start.
        
        ^ aThread.
    ]
}.
