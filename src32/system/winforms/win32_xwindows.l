type xwindow :: IWindow.

// --- XWindow ---

limited class IWindow
{
    stacksafe on_create hwnd:anHWND int:aMessage int:aWParam win_createstruct:aCreateStruct vint:aRetVal
    [
        system'external'USER32 DefWindowProcW int:anHWND int:aMessage int:aWParam win_createstruct:aCreateStruct.
        
        aRetVal int := 0
    ]
    
    stacksafe on_paint hwnd:anHWND vint:aRetVal
    [
        int result := system'external'USER32 DefWindowProcW int:anHWND int:WM_PAINT int:0 int:0.
        
        aRetVal int := result.
    ]
    
    stacksafe on_destroy hwnd:anHWND vint:aRetVal
    [
        aRetVal int := 0.
    ]
}

// --- XWindowCallback ---

sealed class XWindowCallback
{
    xwindow theWindow.
    
    constructor new xwindow:window
    [
        theWindow := window.
    ]
    
    stacksafe wm_create hwnd:anHWND int:aMessage int:aWParam win_createstruct:aCreateStruct vint:aRetVal
    [
        int param := 0.
        aCreateStruct readCreateParamTo vint:param.
        
        system'external'USER32 SetWindowLongW int:anHWND int:GWL_USERDATA int:param.
        
        theWindow on_create hwnd:anHWND int:aMessage int:aWParam win_createstruct:aCreateStruct vint:aRetVal.
    ]
    
    stacksafe wm_paint hwnd:anHWND int:aMessage vint:aRetVal
    [
        theWindow on_paint hwnd:anHWND vint:aRetVal.
    ]
    
    eval hwnd:anHWND int:aMessage int:aWParam int:aLParam vint:aRetVal
    [
        aMessage =>
            WM_DESTROY
            [
                theWindow on_destroy hwnd:anHWND vint:aRetVal.
            ];
            ! [
                int result := system'external'USER32 DefWindowProcW handle:anHWND int:aMessage int:aWParam int:aLParam.
                
                aRetVal int := result
            ].
    ]    
}

// --- $windowClassControl ---

singleton $classFactoryX =
{
    stacksafe $readXWndProc vint:retVal
        => system'core_routines'__hwnd.
        
    wide create wide:aName hinstance:anInstance int:aStyle int:aBackground
    [
        int cursor := system'external'USER32 LoadCursorW int:0 int:IDC_ARROW.
                
        int winproc_ptr := 0.
        $classFactoryX $readXWndProc vint:winproc_ptr.
                
        int wndclass(12).        
        intArrayHelper fill intarray:wndclass int:0 int:12 int:0.
        
        wndclass@0  := 48.
        wndclass@1  := aStyle.
        wndclass@2  := winproc_ptr.
        wndclass@5  := anInstance.
        wndclass@7  := cursor.
                
        dirty_ptr name_ptr := aName.
        name_ptr saveTo intarray:wndclass int:10.
        
        system'external'USER32 RegisterClassExW intarray:wndclass.        
        
        ^ aName
    ]
}.

// --- Window Classes ---

wide static XWindowClass    = $classFactoryX create wide:"ELENA.WINDOW.3.1"w hinstance:CurrentInstance int:(CS_VREDRAW || CS_HREDRAW) int:COLOR_BACKGROUND.
