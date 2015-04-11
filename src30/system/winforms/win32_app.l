#define system.

// --- BaseApplication ---

#class BaseApplication
{
}

// --- SDIApplication ---

#class SDIApplication :: BaseApplication
{
    #field              theMainWindow.
    #field(type:handle) theHandle.
    
    #constructor new
    [
        RTPointer new:$self.
        
        theMainWindow := 'mainWindow new.
        theHandle := theMainWindow handle.
    ]
    
    #method start
    [
        #var(type:win_msg)aMsg.
        #var(type:int)proceeded.
        
        #var(type:int)aResult := system'external'USER32 GetMessageW &win_msg:aMsg &handle:theHandle &int:0 &int:0.
        #loop (aResult != 0)?
        [
            proceeded := system'external'USER32 IsDialogMessageW &handle:theHandle &win_msg:aMsg.
            (proceeded == 0) ?
                [
                    system'external'USER32 TranslateMessage &win_msg:aMsg.
                    system'external'USER32 DispatchMessageW &win_msg:aMsg.
                ].

            aResult := system'external'USER32 GetMessageW &win_msg:aMsg &handle:theHandle &int:0 &int:0.
        ].
    ]
    
    #method stop
    [
        theHandle := Handle min.
        
        system'external'USER32 PostQuitMessage &int:0.
    ]
}

#static program = SDIApplication new.