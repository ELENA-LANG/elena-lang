//import system'drawing.

// --- classes ---
public const wide StaticClass = "Static"w;
//const wide EditClass   = "Edit"w.
public const wide ButtonClass = "Button"w;
//const wide ComboboxClass = "Combobox"w.

// --- WindowHandle ---

public sealed const struct WindowHandle
{
    embeddable __raw theHandle[4];
    
    constructor()
    {
        theHandle := 0
    }

    get constructor Default()
        <=();

//    stacksafe constructor new hwnd:aHandle
//    [
//        theHandle := aHandle.
//    ]

    constructor(int dwExStyle, wide className, wide windowName, int style, int x, int y, int width, int height, WindowHandle parent, HInstance instance, object param)
    {
        int paramIndex := 0;
        if (nil != param)
        {
            rt_ptr ptr := RTPointer.assign(param);
    
            paramIndex := ptr.Index
        };
                
        int h := 0;                
        extern
        {
            h := extern USER32.CreateWindowExW(
                dwExStyle,
                className,
                windowName,  
                style,
                x,
                y,
                width,
                height,
                parent,
                0,
                instance,
                paramIndex)
        };

//        int err := system'external'KERNEL32 GetLastError.

        if (h==0)
            { WinFormsException.raise() };

        theHandle := h
    }

//    setVisible int:aValue
//    [
//        system'external'USER32 ShowWindow int:theHandle int:aValue.
//    ]
//    
//    bool stacksafe isVisible
//    [
//        int visible := system'external'USER32 IsWindowVisible int:theHandle.
//        
//        ^ (visible != 0)
//    ]
//    
//    setEnabled int:aValue
//    [
//        system'external'USER32 EnableWindow int:theHandle int:aValue.
//    ]            
//    
//    isEnabled
//    [
//        int visible := system'external'USER32 IsWindowEnabled int:theHandle.
//        
//        ^ (visible != 0)
//    ]
//
//    setChecked int:aValue
//    [
//        system'external'USER32 SendMessageW int:theHandle int:BM_SETCHECK int:aValue int:0
//    ]
//
//    bool isChecked
//    [
//        int aResult := system'external'USER32 SendMessageW int:theHandle int:BM_GETCHECK int:0 int:0.
//
//		^ (aResult == BST_CHECKED)
//    ]    
            
    setCaption(wide s)
    {
        extern USER32.SendMessageW(theHandle, WM_SETTEXT, 0, s)
    }
    
//    getCaption
//    [
//        int aLength := system'external'USER32 SendMessageW int:theHandle int:WM_GETTEXTLENGTH int:0 int:0.
//        
//        if (aLength == 0)
//            [ ^ emptyWideLiteralValue ];
//            [
//                wide aCaption := WideLiteralValue new int:aLength.
//                aLength := aLength + 1.
//                
//                system'external'USER32 SendMessageW int:theHandle int:WM_GETTEXT int:aLength wide:aCaption.
//                
//                ^ aCaption
//            ].
//    ]
//
//    sendMessage int:aMessage int:aWParam wide:aLParam
//    [
//        system'external'USER32 SendMessageW int:theHandle int:aMessage int:aWParam wide:aLParam
//    ]
//            
//    refresh
//    [
//        int handle := theHandle.
//        
//        extern()
//        [
//            system'external'USER32 InvalidateRect int:handle int:0 int:1.
//            system'external'USER32 UpdateWindow int:handle
//        ].
//    ]
//    
//    stacksafe setPosition int:x int:y int:cx int:cy int:flags
//    [
//        system'external'USER32 SetWindowPos int:theHandle int:0 int:x int:y int:cx int:cy int:flags
//    ]
//
//    stacksafe getClientRect win_rect:aStruct
//    [
//        system'external'USER32 GetClientRect int:theHandle win_rect:aStruct
//    ]
//  
//    stacksafe read vhdc:anHDC
//    [
//        hdc dc := system'external'USER32 GetDC int:theHandle.
//        
//        anHDC hdc := dc.
//    ]
//
//    hdc
//    [
//        hdc aDC := 0.
//        $self read vhdc:aDC.
//
//        ^ HDC new hdc:aDC
//    ]
//
//    stacksafe set hmenu:aMenuHandle
//    [
//        system'external'USER32 SetMenu int:theHandle int:aMenuHandle
//    ]
            
    free()
    {
        extern USER32.DestroyWindow(theHandle)
    }
}

//// --- PAINTSTRUCT ---
//
//sealed struct PAINTSTRUCT
//{
//    hdc      theHDC.
//    int      theErase.
//    win_rect theRect. 
//    int      theRestore.
//    int      theIncUpdate.
//    
//    int      dummy0.
//    int      dummy1.
//    int      dummy2.
//    int      dummy3.
//    int      dummy4.
//    int      dummy5.
//    int      dummy6.
//    int      dummy7.
//
//    embeddable constructor new
//    [
//    ]
//
//    stacksafe read vhdc:aHandle
//    [
//        aHandle hdc := theHDC.
//    ]
//            
//    hdc = HDC new hdc:theHDC.            
//            
//    stacksafe begin hwnd:aHandle
//    [
//        system'external'USER32 BeginPaint int:aHandle win_paintstruct:$self
//    ]
//    
//    stacksafe end hwnd:aHandle
//    [
//        system'external'USER32 EndPaint int:aHandle win_paintstruct:$self
//    ]
//}
