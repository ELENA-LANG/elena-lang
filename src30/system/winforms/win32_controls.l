#import system'drawing.

// --- classes ---
const wide StaticClass = "Static"w.
const wide EditClass   = "Edit"w.
const wide ButtonClass = "Button"w.
const wide ComboboxClass = "Combobox"w.

// --- WindowHandle ---

sealed const embeddable struct WindowHandle
{
    __ptr theHandle.
    
    constructor new
    [
        theHandle := 0.
    ]

    constructor min
        <= new.

    stacksafe constructor new &hwnd:aHandle
    [
        theHandle := aHandle.
    ]

    stacksafe constructor new &int:dwExStyle &wide:aClassName &wide:aWindowName &int:aStyle &int:x &int:y &int:width &int:height &hwnd:aParent &hinstance:instance &object:aParam
    [
        int aParamIndex := 0.
        if ($nil != aParam)
        [
            rt_ptr aPointer := RTPointer new:aParam.
    
            aParamIndex := aPointer.
        ].
                
        int aHandle := 0.                
        #extern 
        [
            aHandle := system'external'USER32 CreateWindowExW
                &int:       dwExStyle
                &wide:      aClassName
                &wide:      aWindowName  
                &int:       aStyle
                &int:       x
                &int:       y
                &int:       width
                &int:       height
                &int:       aParent
                &int:       0
                &int:       instance
                &int:       aParamIndex.
        ].

        if (aHandle==0)
            [ WinFormsException new raise. ].

        theHandle := aHandle.
    ]

    setVisible &int:aValue
    [
        system'external'USER32 ShowWindow &int:theHandle &int:aValue.
    ]
    
    bool stacksafe isVisible
    [
        int visible := system'external'USER32 IsWindowVisible &int:theHandle.
        
        ^ (visible != 0).
    ]
    
    setEnabled &int:aValue
    [
        system'external'USER32 EnableWindow &int:theHandle &int:aValue.
    ]            
    
    isEnabled
    [
        int visible := system'external'USER32 IsWindowEnabled &int:theHandle.
        
        ^ (visible != 0).
    ]

    setChecked &int:aValue
    [
        system'external'USER32 SendMessageW &int:theHandle &int:BM_SETCHECK &int:aValue &int:0.
    ]

    bool isChecked
    [
        int aResult := system'external'USER32 SendMessageW &int:theHandle &int:BM_GETCHECK &int:0 &int:0.

		^ (aResult == BST_CHECKED).
    ]    
            
    setCaption &wide:aValue
    [
        system'external'USER32 SendMessageW &int:theHandle &int:WM_SETTEXT &int:0 &wide:aValue.
    ]
    
    getCaption
    [
        int aLength := system'external'USER32 SendMessageW &int:theHandle &int:WM_GETTEXTLENGTH &int:0 &int:0.
        
        if (aLength == 0)
            [ ^ emptyWideLiteralValue. ];
            [
                wide aCaption := WideLiteralValue new &int:aLength.
                aLength := aLength + 1.
                
                system'external'USER32 SendMessageW &int:theHandle &int:WM_GETTEXT &int:aLength &wide:aCaption.
                
                ^ aCaption.
            ].
    ]

    sendMessage &int:aMessage &int:aWParam &wide:aLParam
    [
        system'external'USER32 SendMessageW &int:theHandle &int:aMessage &int:aWParam &wide:aLParam.            
    ]
            
    refresh
    [
        int handle := theHandle.
        
        #extern 
        [
            system'external'USER32 InvalidateRect &int:handle &int:0 &int:1.
            system'external'USER32 UpdateWindow &int:handle.
        ].
    ]
    
    stacksafe setPosition &int:x &int:y &int:cx &int:cy &int:flags
    [
        system'external'USER32 SetWindowPos &int:theHandle &int:0 &int:x &int:y &int:cx &int:cy &int:flags.
    ]

    stacksafe getClientRect &win_rect:aStruct
    [
        system'external'USER32 GetClientRect &int:theHandle &win_rect:aStruct.
    ]
  
    stacksafe read &vhdc:anHDC
    [
        hdc dc := system'external'USER32 GetDC &int:theHandle.
        
        anHDC := dc.
    ]

    hdc
    [
        hdc aDC := 0.
        $self read &vhdc:aDC.

        ^ HDC new &hdc:aDC.
    ]

    stacksafe set &hmenu:aMenuHandle
    [
        system'external'USER32 SetMenu &int:theHandle &int:aMenuHandle.
    ]
            
    stacksafe free
    [
        system'external'USER32 DestroyWindow &int:theHandle.
    ]
}

// --- PAINTSTRUCT ---

sealed embeddable struct PAINTSTRUCT
{
    hdc      theHDC.
    int      theErase.
    win_rect theRect. 
    int      theRestore.
    int      theIncUpdate.
    
    int      dummy0.
    int      dummy1.
    int      dummy2.
    int      dummy3.
    int      dummy4.
    int      dummy5.
    int      dummy6.
    int      dummy7.

    stacksafe read &vhdc:aHandle
    [
        aHandle := theHDC.
    ]
            
    hdc = HDC new &hdc:theHDC.            
            
    stacksafe begin &hwnd:aHandle
    [
        system'external'USER32 BeginPaint &int:aHandle &win_paintstruct:$self.
    ]
    
    stacksafe end &hwnd:aHandle
    [
        system'external'USER32 EndPaint &int:aHandle &win_paintstruct:$self.
    ]
}
