// ==== ver 3.4.1 ===

#import system.
#import system'threading.

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
