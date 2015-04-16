#define system.

// --- $windowClassControl ---

#symbol $windowClassControl =
{
    $fillWndClass &bytearray:anArray &wide:aName &int:aStyle &hinstance:anInstance &int:aBackground
        => system'internal'win_fillWndClass.
    
    createClass &name:aName &hinstance:anInstance &int:aStyle &int:aBackground
    [
        #var(type:bytearray,size:40)aStruct.
        #var(type:wide) aWideName := aName wide.
        
        $self $fillWndClass &bytearray:aStruct &wide:aWideName &int:aStyle &hinstance:anInstance &int:aBackground.
        
        system'external'USER32 RegisterClassW &bytearray:aStruct.        
        
        ^ aWideName.
    ]
}.

// --- WindowCallback ---

#class(limited) WindowCallback
{
    #field(type:win_listener)theListener.

    #method win_callback = $self.
    
    #method wm_create &hwnd:anHWND &int:aMessage &int:aWParam &win_createstruct:aCreateStruct &vint:aRetVal
    [
        #var(type:vint)param.
        aCreateStruct readCreateParam &vint:param.
        
        system'external'USER32 SetWindowLongW &hwnd:anHWND &int:GWL_USERDATA &int:param.
        
        system'external'USER32 DefWindowProcW &hwnd:anHWND &int:aMessage &int:aWParam &win_createstruct:aCreateStruct.
        
        aRetVal << 0.
    ]
    
    #method eval &hwnd:anHWND &int:aMessage &vint:aRetVal
    [
        #var(type:int)result := system'external'USER32 DefWindowProcW &hwnd:anHWND &int:aMessage &int:0 &int:0.
        
        aRetVal << result.
    ]

    #method wm_command &hwnd:anHWND &int:aMessage &int:aCode &hwnd:aControl &vint:aRetVal
    [
        aRetVal << -1.
    ]

    #method eval &hwnd:anHWND &int:aMessage &hwnd:aControl &int:aLParam &vint:aRetVal
    [
        (aMessage == WM_SETCURSOR)
            ? [
                #var(type:int)code := aLParam && 0FFFFh.
                
                (code == HTCLIENT) ?
                [
                    #var(type:int)h := system'external'USER32 LoadCursorW &int:0 &int:IDC_ARROW.
                    system'external'USER32 SetCursor &int:h.
                    
                    aRetVal << -1.
                    
                    ^ $self.
                ].
            ].
        
        #var(type:int)result := system'external'USER32 DefWindowProcW &hwnd:anHWND &int:aMessage &hwnd:aControl &int:aLParam.
        
        aRetVal << result.
    ]

//    #method eval &handle:anHWND &int:aMessage &int:aWParam &int:aLParam &vint:aRetVal
//    [
//        #var(type:int)result := system'external'USER32 DefWindowProcW &handle:anHWND &int:aMessage &int:aWParam &int:aLParam.
//        
//        aRetVal << result.
//    ]
}

// --- SDIWindowCallback ---

#class SDIWindowCallback :: WindowCallback
{
    #constructor new &win_listener:aListener
    [
        theListener := aListener.
    ]
    
    #method eval &hwnd:anHWND &int:aMessage &vint:aRetVal
    [
        (aMessage == WM_CLOSE)
            ? [
                anHWND close.
                
                aRetVal << 0.
                
                ^ $self.
            ].
        
        (aMessage == WM_DESTROY)
            ? [
                // NOTE: SDIWindow is the main program window, so closing it means exiting the program
                'program stop.
                
                #var(type:int)anIndex := system'external'USER32 SetWindowLongW &hwnd:anHWND &int:GWL_USERDATA &int:0.
                (anIndex != 0)
                    ? [ RTPointer new &index:anIndex close. ].
                
                aRetVal << 0.
                
                ^ $self.
            ].
        
        $super eval &hwnd:anHWND &int:aMessage &vint:aRetVal.
    ]
    
    #method wm_command &hwnd:anHWND &int:aMessage &int:aCode &hwnd:aControl &vint:aRetVal
    [
        (aControl == 0)
            ? [ ^ $self. ]. // !! temporal

        // !! note low word may contains the control ID, so only high word should be checked
        (aCode == 0)
            ? [ theListener onclick &hwnd:aControl &vint:aRetVal. ].
    ]
    
//    #method eval &handle:anHWND &int:aMessage &int:aWParam &int:aLParam &vint:aRetVal
//    [
//        $super eval &handle:anHWND &int:aMessage &int:aWParam &int:aLParam &vint:aRetVal.
//    ]
}

// --- WindowListener ---

#class(limited) WindowListener
{
    #method onclick &hwnd:aControl &vint:aResult
    [
        aResult << 0.
    ]
}

// --- SDIWindowClass ---

#static SDIWindowClass = $windowClassControl createClass &name:"ELENA.SDIWINDOW.3.1" &hinstance:CurrentInstance &int:WS_ELENAWINDOW &int:COLOR_BACKGROUND.

//// --- BaseWindowHandle ---
//
//#class BaseWindowHandle :: ControlHandle
//{
//    #field(type:win_listener)theListener.
//    
//    #method set &win_listener:aListener
//    [
//        theListener := aListener.
//    ]
//    
//    #method onclick &handle:aControl &vint:aRetVal
//    [
//        (nil == theListener)
//            ! [ theListener onclick &handle:aControl &vint:aRetVal. ].
//    ]
//    
//    #method destroy
//    [
//    ]
//}
