import system'winforms.
import system'drawing.

template iform :: type
{
    xwindow = IWindow::
    {
        stacksafe on_create hwnd:anHWND int:aMessage int:aWParam win_createstruct:aCreateStruct vint:aRetVal
        [
            $super on_create hwnd:anHWND int:aMessage int:aWParam win_createstruct:aCreateStruct vint:aRetVal.
        ]
        
        stacksafe on_paint hwnd:anHWND vint:aRetVal
        [
            $super on_paint hwnd:anHWND vint:aRetVal.
        ]
        
        stacksafe on_destroy hwnd:anHWND vint:aRetVal
        [
            $owner $on_destroy vint:aRetVal.
        ]
    }.
}

sealed class Form :: iform(object)
{
    hwnd theHandle.
    
    int width  :: nprop(readWidthTo,writeWidth).
    int height :: nprop(readHeightTo,writeHeight).
    
    constructor new
    [
        int width := $self width.
        int height := $self height.
     
        win_rect reg := RECT new.
        reg right := width.
        reg bottom := height.
        system'external'USER32 AdjustWindowRect win_rect:reg int:WS_OVERLAPPEDWINDOW int:0.
        
        width := reg width.
        height := reg height.
                                          
        theHandle := WindowHandle new 
                        int:0
                        wide:XWindowClass 
                        wide:"Test" 
                        int:WS_OVERLAPPEDWINDOW
                        int:CW_USEDEFAULT
                        int:CW_USEDEFAULT
                        int:width
                        int:height
                        hwnd:0
                        hinstance:CurrentInstance 
                        object:(XWindowCallback new xwindow:$self ).
    ]
    
    stacksafe $on_destroy vint:aRetVal
    [
        system'external'USER32 PostQuitMessage int:0.
        
        aRetVal int := 0.
    ]

    stacksafe readWidthTo vint:retVal
    [
        retVal int := 300.
    ]
    
    stacksafe writeWidth int:val
    [
    ]
    
    stacksafe readHeightTo vint:retVal
    [
        retVal int := 300.
    ]
    
    stacksafe writeHeight int:val
    [
    ]
        
    set visible:value
    [
        if(value)
            [ theHandle setVisible int:5 ];
            [ theHandle setVisible int:0 ]        
    ]
}
