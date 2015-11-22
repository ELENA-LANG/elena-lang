// ==== ver 3.4.1 ===

#import system.

// --- BaseApplication ---

#class BaseApplication
{
}

// --- SDIApplication ---

#class SDIApplication :: BaseApplication
{
    #field            theMainWindow.
    #field(type:hwnd) theHandle.
    
    #constructor new
    [
        RTPointer new:$self.
        
        theMainWindow := 'mainWindow new.
    ]
    
    #method mainWindow = theMainWindow.
    
    #method start
    [
        #var(type:win_msg)aMsg.
        #var(type:int)proceeded.
        
        theMainWindow open.
        theHandle := theMainWindow hwnd.
        
        #var(type:int)aResult := system'external'USER32 GetMessageW &win_msg:aMsg &int:theHandle &int:0 &int:0.
        #loop (aResult != 0)?
        [
            proceeded := system'external'USER32 IsDialogMessageW &int:theHandle &win_msg:aMsg.
            (proceeded == 0) ?
                [
                    system'external'USER32 TranslateMessage &win_msg:aMsg.
                    system'external'USER32 DispatchMessageW &win_msg:aMsg.
                ].

            aResult := system'external'USER32 GetMessageW &win_msg:aMsg &int:theHandle &int:0 &int:0.
        ].
    ]

    #method run &hwnd:aHandle
    [
        #var(type:win_msg)aMsg.
        #var(type:int)proceeded.
        #var(type:int)aResult.

        #loop (aHandle isVisible)?
        [
            aResult := system'external'USER32 PeekMessageW &win_msg:aMsg &int:0 &int:0 &int:0 &int:1.
                (aResult != 0)
                    ? [
                        proceeded := system'external'USER32 IsDialogMessageW &int:aHandle &win_msg:aMsg.
                        (proceeded == 0) ?
                            [
                                system'external'USER32 TranslateMessage &win_msg:aMsg.
                                system'external'USER32 DispatchMessageW &win_msg:aMsg.
                            ].
                    ]
                    ! [
                        system'external'USER32 WaitMessage.
                    ].
        ].
    ]
            
    #method stop
    [
        theHandle := WindowHandle min.
        
        system'external'USER32 PostQuitMessage &int:0.
    ]
}

#static program = SDIApplication new.

#class $GUIStartUp :: StartUp
{
    #method start
    [
        'program start
            | on:
            {
                Abort = e [].
                
                ! = e 
                    [ 
                        messageBox open &caption:"ELENA" &value:(e message) &int:MB_ICONEXCLAMATION.
                    ].
            }.
            
        $self $onStop:nil.
    ]
}

#static startUp = $GUIStartUp new.