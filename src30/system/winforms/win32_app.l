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
        Pointer new:$self.
        
        theMainWindow := 'mainWindow new.
    ]
    
    #method start
    [
        #var(type:win_msg)aMsg.
        
        #var(type:int)aResult := system'external'USER32 GetMessageW &win_msg:aMsg &int:0 &int:0 &int:0.
        #loop (aResult != 0)?
        [
            system'external'USER32 TranslateMessage &win_msg:aMsg.
            system'external'USER32 DispatchMessageW &win_msg:aMsg.

            aResult := system'external'USER32 GetMessageW &win_msg:aMsg &int:0 &int:0 &int:0.
        ].
    ]
    
    #method stop
    [
        system'external'USER32 PostQuitMessage &int:0.
    ]
}

#static program = SDIApplication new.