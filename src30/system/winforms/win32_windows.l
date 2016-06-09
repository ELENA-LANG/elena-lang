#import system.

// --- $windowClassControl ---

#symbol $windowClassControl =
{
    #method(stacksafe) $fillWndClass &bytearray:anArray &wide:aName &int:aStyle &hinstance:anInstance &int:aBackground
        => system'core_routines'__hwnd.
    
    createClass &name:aName &hinstance:anInstance &int:aStyle &int:aBackground
    [
        #var(byte:40)aStruct.
        #var(wide) aWideName := aName wide.
        
        $self $fillWndClass &bytearray:aStruct &wide:aWideName &int:aStyle &hinstance:anInstance &int:aBackground.
        
        system'external'USER32 RegisterClassW &bytearray:aStruct.        
        
        ^ aWideName.
    ]
}.

// --- WindowCallback ---

#class(limited) WindowCallback
{
    #method win_callback = $self.

    #method(stacksafe) onpaint &hwnd:aControl &vint:aRetVal
    [
        #var(int)result := system'external'USER32 DefWindowProcW &int:aControl &int:WM_PAINT &int:0 &int:0.
        
        aRetVal := result.
    ]
    
    #method(stacksafe) onclick &hwnd:aControl &vint:aResult
    [
        aResult := 0.
    ]
    
    #method(stacksafe) onchange &hwnd:aControl &vint:aResult
    [
        aResult := 0.
    ]
    
    #method(stacksafe) onmenu &int:aCode &vint:aResult
    [
        aResult := 0.
    ]
    
    #method(stacksafe) onsize &hwnd:aControl &int:aWidth &int:aHeight &vint:aResult
    [
        aResult := 0.
    ]
    
    #method(stacksafe) ondestroy &hwnd:aControl &vint:aResult
    [
        #var(int)anIndex := system'external'USER32 SetWindowLongW &int:aControl &int:GWL_USERDATA &int:0.
        (anIndex != 0)?
            [
                #var(rt_ptr)ptr := anIndex.
                
                ptr free.
            ].

        aResult := 0.
    ]
        
    #method(stacksafe) wm_create &hwnd:anHWND &int:aMessage &int:aWParam &win_createstruct:aCreateStruct &vint:aRetVal
    [
        #var(int)param.
        aCreateStruct readCreateParam &vint:param.
        
        system'external'USER32 SetWindowLongW &int:anHWND &int:GWL_USERDATA &int:param.
        
        system'external'USER32 DefWindowProcW &int:anHWND &int:aMessage &int:aWParam &win_createstruct:aCreateStruct.
        
        aRetVal := 0.
    ]
    
    #method(stacksafe) eval &hwnd:anHWND &int:aMessage &vint:aRetVal
    [
        (aMessage == WM_DESTROY)
            ? [
                $self ondestroy &hwnd:anHWND &vint:aRetVal.                            
                
                ^ $self.
            ].                
        
        #var(int)result := system'external'USER32 DefWindowProcW &int:anHWND &int:aMessage &int:0 &int:0.
         
        aRetVal := result.
    ]
    
    #method(stacksafe) eval &hwnd:anHWND &int:aMessage &int:wParam &int:lParam &vint:aRetVal
    [
        (aMessage == WM_SIZE)
            ? [
                #var(int)width := lParam && 0FFFFh.
                #var(int)height := lParam >> 16.
        
                $self onsize &hwnd:anHWND &int:width &int:height &vint:aRetVal.
                
                ^ $self.
            ].                
                
        #var(int)result := system'external'USER32 DefWindowProcW &int:anHWND &int:aMessage &int:wParam &int:lParam.
        
        aRetVal := result.
    ]
    
    #method(stacksafe) wm_paint &hwnd:anHWND &int:aMessage &vint:aRetVal
    [
        $self onpaint &hwnd:anHWND &vint:aRetVal.
    ]
    
    #method(stacksafe) wm_command &hwnd:anHWND &int:aMessage &int:wParam &hwnd:aControl &vint:aRetVal
    [
        (0 == aControl)
            ? [ $self onmenu &int:wParam &vint:aRetVal. ^ $self. ].

        #var(int)aCommand := wParam >> 16.

        (aCommand == BN_CLICKED)
            ? [ $self onclick &hwnd:aControl &vint:aRetVal. ^ $self. ].

        (aCommand == EN_CHANGE)
            ? [ $self onchange &hwnd:aControl &vint:aRetVal. ^ $self. ].

        (aCommand == CBN_SELCHANGE)
            ? [ $self onchange &hwnd:aControl &vint:aRetVal. ^ $self. ].
    ]

    #method(stacksafe) eval &hwnd:anHWND &int:aMessage &hwnd:aControl &int:aLParam &vint:aRetVal
    [
        (aMessage == WM_SETCURSOR)
            ? [
                #var(int)code := aLParam && 0FFFFh.
                
                (code == HTCLIENT) ?
                [
                    #var(int)h := system'external'USER32 LoadCursorW &int:0 &int:IDC_ARROW.
                    system'external'USER32 SetCursor &int:h.
                    
                    aRetVal := -1.
                    
                    ^ $self.
                ].
            ].
        
        #var(int)result := system'external'USER32 DefWindowProcW &int:anHWND &int:aMessage &hwnd:aControl &int:aLParam.
        
        aRetVal := result.
    ]

    #method eval &handle:anHWND &int:aMessage &int:aWParam &int:aLParam &vint:aRetVal
    [
        #var(int)result := system'external'USER32 DefWindowProcW &handle:anHWND &int:aMessage &int:aWParam &int:aLParam.
        
        aRetVal := result.
    ]
}

// --- SDIWindowCallback ---

#class SDIWindowCallback :: WindowCallback
{
    #method eval &hwnd:anHWND &int:aMessage &vint:aRetVal
    [
        (aMessage == WM_CLOSE)
            ? [
                anHWND free.
                
                aRetVal := 0.
                
                ^ $self.
            ].
        
        (aMessage == WM_DESTROY)
            ? [
                // NOTE: SDIWindow is the main program window, so closing it means exiting the program
                'program stop.
                
                $self ondestroy &hwnd:anHWND &vint:aRetVal.
                
                ^ $self.
            ].
        
        $super eval &hwnd:anHWND &int:aMessage &vint:aRetVal.
    ]

//    #method eval &handle:anHWND &int:aMessage &int:aWParam &int:aLParam &vint:aRetVal
//    [
//        $super eval &handle:anHWND &int:aMessage &int:aWParam &int:aLParam &vint:aRetVal.
//    ]
}

// --- ChildWindowCallback ---

#class ChildWindowCallback :: WindowCallback
{
    #method eval &hwnd:anHWND &int:aMessage &vint:aRetVal
    [
        (aMessage == WM_CLOSE)
            ? [
                anHWND free.
                
                aRetVal := 0.
                
                ^ $self.
            ].
        
        (aMessage == WM_DESTROY)
            ? [
                $self ondestroy &hwnd:anHWND &vint:aRetVal.
                
                ^ $self.
            ].
        
        $super eval &hwnd:anHWND &int:aMessage &vint:aRetVal.
    ]

//    #method eval &handle:anHWND &int:aMessage &int:aWParam &int:aLParam &vint:aRetVal
//    [
//        $super eval &handle:anHWND &int:aMessage &int:aWParam &int:aLParam &vint:aRetVal.
//    ]
}

// --- SDIWindowClass ---

#static(wide) SDIWindowClass = $windowClassControl createClass &name:"ELENA.SDIWINDOW.3.1" &hinstance:CurrentInstance &int:WS_ELENAWINDOW &int:COLOR_BACKGROUND wide.
#static(wide) PaintboxClass  = $windowClassControl createClass &name:"ELENA.Paintbox.3.1" &hinstance:CurrentInstance &int:(CS_VREDRAW or &int:CS_HREDRAW) &int:0 wide.
#static(wide) PanelClass     = $windowClassControl createClass &name:"ELENA.PANEL.3.1" &hinstance:CurrentInstance &int:(CS_VREDRAW or &int:CS_HREDRAW) &int:COLOR_BACKGROUND wide.
