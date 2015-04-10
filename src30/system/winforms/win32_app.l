#define system.

// --- BaseApplication ---

#class BaseApplication
{
}

// --- SDIApplication ---

#class SDIApplication :: BaseApplication
{
    #field theMainWindow.
    
    #constructor new
    [
        RTPointer new:$self.
        
        theMainWindow := 'mainWindow new.
    ]
    
    #method start
    [
        #var(type:win_msg)aMsg.
        #var(type:handle)aHWND := theMainWindow handle.
        #var(type:int)proceeded.
        
        #var(type:int)aResult := system'external'USER32 GetMessageW &win_msg:aMsg &handle:aHWND &int:0 &int:0.
        #loop (aResult != 0)?
        [
            proceeded := system'external'USER32 IsDialogMessageW &handle:aHWND &win_msg:aMsg.
            (proceeded == 0) ?
                [
                    system'external'USER32 TranslateMessage &win_msg:aMsg.
                    system'external'USER32 DispatchMessageW &win_msg:aMsg.
                ].

            aResult := system'external'USER32 GetMessageW &win_msg:aMsg &handle:aHWND &int:0 &int:0.
        ].
    ]
    
    #method stop
    [
        system'external'USER32 PostQuitMessage &int:0.
    ]
}

#static program = SDIApplication new.