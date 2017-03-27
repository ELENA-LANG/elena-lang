// --- BaseApplication ---

class BaseApplication
{
}

// --- SDIApplication ---

class SDIApplication :: BaseApplication
{
    object       theMainWindow.
    
    constructor new
    [
        RTPointer new:$self.
        
        theMainWindow := 'mainWindow new.
    ]
    
    mainWindow = theMainWindow.
    
    eval
    [
        theMainWindow open.

        win_msg msg := 0.
        int     proceeded := 0.
        int     result := 0.
        hwnd    handleCopy := theMainWindow hwnd.
        
        #extern
        [
            result := system'external'USER32 GetMessageW &win_msg:msg &int:0 &int:0 &int:0.
            while (result != 0)
            [
                proceeded := system'external'USER32 IsDialogMessageW &int:handleCopy &win_msg:msg.
                if (proceeded == 0)
                [
                    system'external'USER32 TranslateMessage &win_msg:msg.
                    system'external'USER32 DispatchMessageW &win_msg:msg.
                ].
                result := system'external'USER32 GetMessageW &win_msg:msg &int:0 &int:0 &int:0.
            ].
            
        ].
    ]

    run &hwnd:handle
    [
        win_msg msg := 0.
        int     proceeded := 0.
        int     result := 0.
        hwnd    handleCopy := handle.

        #extern
        [
            while (handleCopy isVisible)
            [
                result := system'external'USER32 PeekMessageW &win_msg:msg &int:0 &int:0 &int:0 &int:1.
                if (result != 0)
                [
                    proceeded := system'external'USER32 IsDialogMessageW &int:handleCopy &win_msg:msg.
                    (proceeded == 0) ?
                        [
                            system'external'USER32 TranslateMessage &win_msg:msg.
                            system'external'USER32 DispatchMessageW &win_msg:msg.
                        ].
                ];
                [
                    system'external'USER32 WaitMessage.
                ].
            ].
        ].
    ]
            
    stop
    [
        system'external'USER32 PostQuitMessage &int:0.
    ]
}

static program = SDIApplication new.

startUp = 
    {
        start : events
        [
            try('program eval)
            {
                Abort : e []
                
                ! e 
                    [ 
                        messageBox open &caption:"ELENA" &value:(e message) &int:MB_ICONEXCLAMATION.
                    ]
            }.
    
            events $onStop:$nil.
        ]        
    }
    start:startUpEvents.
