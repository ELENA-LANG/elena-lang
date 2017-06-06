#import system'threading.

// --- threadControl ---

class threadControl =
{
    start &func:anAction
    [
        var aThread := Thread new &func:anAction.
        
        aThread start.
        
        ^ aThread.
    ]
}.
