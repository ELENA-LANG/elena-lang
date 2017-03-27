// --- $windowClassControl ---

class $windowClassControl =
{
    stacksafe $fillWndClass &bytearray:anArray &wide:aName &int:aStyle &hinstance:anInstance &int:aBackground
        => system'core_routines'__hwnd.
    
    createClass &name:aName &hinstance:anInstance &int:aStyle &int:aBackground
    [
        byte(40) aStruct := 0.
        wide aWideName := aName wide.
        
        $self $fillWndClass &bytearray:aStruct &wide:aWideName &int:aStyle &hinstance:anInstance &int:aBackground.
        
        system'external'USER32 RegisterClassW &bytearray:aStruct.        
        
        ^ aWideName.
    ]
}.

// --- WindowCallback ---

limited class WindowCallback
{
    win_callback = $self.

    stacksafe onpaint &hwnd:aControl &vint:aRetVal
    [
        int result := system'external'USER32 DefWindowProcW &int:aControl &int:WM_PAINT &int:0 &int:0.
        
        aRetVal := result.
    ]
    
    stacksafe onclick &hwnd:aControl &vint:aResult
    [
        aResult := 0.
    ]
    
    stacksafe onchange &hwnd:aControl &vint:aResult
    [
        aResult := 0.
    ]
    
    stacksafe onmenu &int:aCode &vint:aResult
    [
        aResult := 0.
    ]
    
    stacksafe onsize &hwnd:aControl &int:aWidth &int:aHeight &vint:aResult
    [
        aResult := 0.
    ]
    
    stacksafe ondestroy &hwnd:aControl &vint:aResult
    [
        int anIndex := system'external'USER32 SetWindowLongW &int:aControl &int:GWL_USERDATA &int:0.
        if (anIndex != 0)
            [
                rt_ptr ptr := anIndex.
                
                ptr free.
            ].

        aResult := 0.
    ]
        
    stacksafe wm_create &hwnd:anHWND &int:aMessage &int:aWParam &win_createstruct:aCreateStruct &vint:aRetVal
    [
        int param := 0.
        aCreateStruct readCreateParam &vint:param.
        
        system'external'USER32 SetWindowLongW &int:anHWND &int:GWL_USERDATA &int:param.
        
        system'external'USER32 DefWindowProcW &int:anHWND &int:aMessage &int:aWParam &win_createstruct:aCreateStruct.
        
        aRetVal := 0.
    ]
    
    stacksafe eval &hwnd:anHWND &int:aMessage &vint:aRetVal
    [
        if (aMessage == WM_DESTROY)
            [
                $self ondestroy &hwnd:anHWND &vint:aRetVal.                            
                
                ^ $self.
            ].                
        
        int result := system'external'USER32 DefWindowProcW &int:anHWND &int:aMessage &int:0 &int:0.
         
        aRetVal := result.
    ]
    
    stacksafe eval &hwnd:anHWND &int:aMessage &int:wParam &int:lParam &vint:aRetVal
    [
        if (aMessage == WM_SIZE)
            [
                int width := lParam && 0FFFFh.
                int height := lParam >> 16.
        
                $self onsize &hwnd:anHWND &int:width &int:height &vint:aRetVal.
                
                ^ $self.
            ].                
                
        int result := system'external'USER32 DefWindowProcW &int:anHWND &int:aMessage &int:wParam &int:lParam.
        
        aRetVal := result.
    ]
    
    stacksafe wm_paint &hwnd:anHWND &int:aMessage &vint:aRetVal
    [
        $self onpaint &hwnd:anHWND &vint:aRetVal.
    ]
    
    stacksafe wm_command &hwnd:anHWND &int:aMessage &int:wParam &hwnd:aControl &vint:aRetVal
    [
        if (0 == aControl)
            [ $self onmenu &int:wParam &vint:aRetVal. ^ $self. ].

        int aCommand := wParam >> 16.

        if (aCommand == BN_CLICKED)
            [ $self onclick &hwnd:aControl &vint:aRetVal. ^ $self. ].

        if (aCommand == EN_CHANGE)
            [ $self onchange &hwnd:aControl &vint:aRetVal. ^ $self. ].

        if (aCommand == CBN_SELCHANGE)
            [ $self onchange &hwnd:aControl &vint:aRetVal. ^ $self. ].
    ]

    stacksafe eval &hwnd:anHWND &int:aMessage &hwnd:aControl &int:aLParam &vint:aRetVal
    [
        if (aMessage == WM_SETCURSOR)
            [
                int code := aLParam && 0FFFFh.
                
                if (code == HTCLIENT)
                [
                    int h := system'external'USER32 LoadCursorW &int:0 &int:IDC_ARROW.
                    system'external'USER32 SetCursor &int:h.
                    
                    aRetVal := -1.
                    
                    ^ $self.
                ].
            ].
        
        int result := system'external'USER32 DefWindowProcW &int:anHWND &int:aMessage &hwnd:aControl &int:aLParam.
        
        aRetVal := result.
    ]

    eval &handle:anHWND &int:aMessage &int:aWParam &int:aLParam &vint:aRetVal
    [
        int result := system'external'USER32 DefWindowProcW &handle:anHWND &int:aMessage &int:aWParam &int:aLParam.
        
        aRetVal := result.
    ]
}

// --- SDIWindowCallback ---

class SDIWindowCallback :: WindowCallback
{
    eval &hwnd:anHWND &int:aMessage &vint:aRetVal
    [
        if (aMessage == WM_CLOSE)
            [
                anHWND free.
                
                aRetVal := 0.
                
                ^ $self.
            ].
        
        if (aMessage == WM_DESTROY)
            [
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

class ChildWindowCallback :: WindowCallback
{
    eval &hwnd:anHWND &int:aMessage &vint:aRetVal
    [
        if (aMessage == WM_CLOSE)
            [
                anHWND free.
                
                aRetVal := 0.
                
                ^ $self.
            ].
        
        if (aMessage == WM_DESTROY)
            [
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

wide static SDIWindowClass = $windowClassControl createClass &name:"ELENA.SDIWINDOW.3.1" &hinstance:CurrentInstance &int:WS_ELENAWINDOW &int:COLOR_BACKGROUND wide.
wide static PaintboxClass  = $windowClassControl createClass &name:"ELENA.Paintbox.3.1" &hinstance:CurrentInstance &int:(CS_VREDRAW or &int:CS_HREDRAW) &int:0 wide.
wide static PanelClass     = $windowClassControl createClass &name:"ELENA.PANEL.3.1" &hinstance:CurrentInstance &int:(CS_VREDRAW or &int:CS_HREDRAW) &int:COLOR_BACKGROUND wide.
