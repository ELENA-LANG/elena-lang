#import system.
#import system'drawing.

// --- classes ---
#symbol(wide) StaticClass = "Static"w.
#symbol(wide) EditClass   = "Edit"w.
#symbol(wide) ButtonClass = "Button"w.
#symbol(wide) ComboboxClass = "Combobox"w.

// --- WindowHandle ---

#class(sealed,reference,const) WindowHandle
{
    #field(int)theHandle.
    
    #constructor new
    [
        theHandle := 0.
    ]

    #constructor min
        <= new.

    #constructor(stacksafe) new &hwnd:aHandle
    [
        theHandle := aHandle.
    ]

    #constructor(stacksafe) new &int:dwExStyle &wide:aClassName &wide:aWindowName &int:aStyle &int:x &int:y &int:width &int:height &hwnd:aParent &hinstance:instance &object:aParam
    [
        #var(int)aParamIndex := 0.
        ($nil != aParam) ?
        [
            #var(rt_ptr)aPointer := RTPointer new:aParam.
    
            aParamIndex := aPointer.
        ].
                
        #var(int)aHandle := 0.                
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

        (aHandle==0)
            ? [ #throw WinFormsException new. ].

        theHandle := aHandle.
    ]

    #method setVisible &int:aValue
    [
        system'external'USER32 ShowWindow &int:theHandle &int:aValue.
    ]
    
    #method(bool,stacksafe) isVisible
    [
        #var(int)visible := system'external'USER32 IsWindowVisible &int:theHandle.
        
        ^ (visible != 0).
    ]
    
    #method setEnabled &int:aValue
    [
        system'external'USER32 EnableWindow &int:theHandle &int:aValue.
    ]            
    
    #method isEnabled
    [
        #var(int)visible := system'external'USER32 IsWindowEnabled &int:theHandle.
        
        ^ (visible != 0).
    ]

    #method setChecked &int:aValue
    [
        system'external'USER32 SendMessageW &int:theHandle &int:BM_SETCHECK &int:aValue &int:0.
    ]

    #method(bool) isChecked
    [
        #var(int)aResult := system'external'USER32 SendMessageW &int:theHandle &int:BM_GETCHECK &int:0 &int:0.

		^ (aResult == BST_CHECKED).
    ]    
            
    #method setCaption &wide:aValue
    [
        system'external'USER32 SendMessageW &int:theHandle &int:WM_SETTEXT &int:0 &wide:aValue.
    ]
    
    #method getCaption
    [
        #var(int)aLength := system'external'USER32 SendMessageW &int:theHandle &int:WM_GETTEXTLENGTH &int:0 &int:0.
        
        (aLength == 0)
            ? [ ^ emptyWideLiteralValue. ]
            ! [
                #var(wide)aCaption := WideLiteralValue new &int:aLength.
                aLength := aLength + 1.
                
                system'external'USER32 SendMessageW &int:theHandle &int:WM_GETTEXT &int:aLength &wide:aCaption.
                
                ^ aCaption.
            ].
    ]

    #method sendMessage &int:aMessage &int:aWParam &wide:aLParam
    [
        system'external'USER32 SendMessageW &int:theHandle &int:aMessage &int:aWParam &wide:aLParam.            
    ]
            
    #method refresh
    [
        #var(int)handle := theHandle.
        
        #extern 
        [
            system'external'USER32 InvalidateRect &int:handle &int:0 &int:1.
            system'external'USER32 UpdateWindow &int:handle.
        ].
    ]
    
    #method(stacksafe) setPosition &int:x &int:y &int:cx &int:cy &int:flags
    [
        system'external'USER32 SetWindowPos &int:theHandle &int:0 &int:x &int:y &int:cx &int:cy &int:flags.
    ]

    #method(stacksafe) getClientRect &win_rect:aStruct
    [
        system'external'USER32 GetClientRect &int:theHandle &win_rect:aStruct.
    ]
  
    #method(stacksafe) read &vhdc:anHDC
    [
        #var(hdc)dc := system'external'USER32 GetDC &int:theHandle.
        
        anHDC := dc.
    ]

    #method hdc
    [
        #var(hdc)aDC.
        $self read &vhdc:aDC.

        ^ HDC new &hdc:aDC.
    ]

    #method(stacksafe) set &hmenu:aMenuHandle
    [
        system'external'USER32 SetMenu &int:theHandle &int:aMenuHandle.
    ]
            
    #method(stacksafe) free
    [
        system'external'USER32 DestroyWindow &int:theHandle.
    ]
}

// --- PAINTSTRUCT ---

#class(sealed,embeddable)PAINTSTRUCT
{
    #field(hdc)      theHDC.
    #field(int)      theErase.
    #field(win_rect) theRect. 
    #field(int)      theRestore.
    #field(int)      theIncUpdate.
    
    #field(int)      dummy0.
    #field(int)      dummy1.
    #field(int)      dummy2.
    #field(int)      dummy3.
    #field(int)      dummy4.
    #field(int)      dummy5.
    #field(int)      dummy6.
    #field(int)      dummy7.

    #method(stacksafe) read &vhdc:aHandle
    [
        aHandle := theHDC.
    ]
            
    #method hdc = HDC new &hdc:theHDC.            
            
    #method(stacksafe) begin &hwnd:aHandle
    [
        system'external'USER32 BeginPaint &int:aHandle &win_paintstruct:$self.
    ]
    
    #method(stacksafe) end &hwnd:aHandle
    [
        system'external'USER32 EndPaint &int:aHandle &win_paintstruct:$self.
    ]
}
