// ==== ver 3.4.1 ===

#import system.
#import system'drawing.

// --- classes ---
#symbol(type:wide) StaticClass = "Static" wide. // !! temporal
#symbol(type:wide) EditClass   = "Edit" wide. // !! temporal
#symbol(type:wide) ButtonClass = "Button" wide. // !! temporal
#symbol(type:wide) ComboboxClass = "Combobox" wide. // !! temporal

// --- WindowHandle ---

#class(sealed,struct:embeddable) WindowHandle
{
    #field(type:int)theHandle.
    
    #constructor new
        => system'internal'int_init.
        
    // obsolete, new[0] should be used instead
    #constructor min
        => system'internal'int_init.
        
    #constructor(stacksafe) new &hwnd:aHandle
        => system'internal'int_copy.
        
    #constructor(stacksafe) new &int:dwExStyle &wide:aClassName &wide:aWindowName &int:aStyle &int:x &int:y &int:width &int:height &hwnd:aParent &hinstance:instance &object:aParam
    [
        #var(type:int)aParamIndex := 0.
        ($nil != aParam) ?
        [
            #var(type:rt_ptr)aPointer := RTPointer new:aParam.
    
            aParamIndex := aPointer.
        ].
                
        #var(type:int)aHandle := system'external'USER32 CreateWindowExW
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

        (aHandle==0)
            ? [ #throw WinFormsException new. ].

        theHandle := aHandle.
    ]

    #method hwnd = $self.        
    
    #method setVisible &int:aValue
    [
        system'external'USER32 ShowWindow &int:theHandle &int:aValue.
    ]
    
    #method(type:bool) isVisible
    [
        #var(type:int)visible := system'external'USER32 IsWindowVisible &int:theHandle.
        
        ^ (visible != 0).
    ]
    
    #method setEnabled &int:aValue
    [
        system'external'USER32 EnableWindow &int:theHandle &int:aValue.
    ]            
    
    #method isEnabled
    [
        #var(type:int)visible := system'external'USER32 IsWindowEnabled &int:theHandle.
        
        ^ (visible != 0).
    ]

    #method setChecked &int:aValue
    [
        system'external'USER32 SendMessageW &int:theHandle &int:BM_SETCHECK &int:aValue &int:0.
    ]

    #method isChecked
    [
        #var(type:int)aResult := system'external'USER32 SendMessageW &int:theHandle &int:BM_GETCHECK &int:0 &int:0.

		^ (aResult == BST_CHECKED).
    ]    
            
    #method setCaption &wide:aValue
    [
        system'external'USER32 SendMessageW &int:theHandle &int:WM_SETTEXT &int:0 &wide:aValue.
    ]
    
    #method getCaption
    [
        #var(type:int)aLength := system'external'USER32 SendMessageW &int:theHandle &int:WM_GETTEXTLENGTH &int:0 &int:0.
        
        (aLength == 0)
            ? [ ^ emptyWideLiteralValue. ]
            ! [
                #var(type:wide)aCaption := WideLiteralValue new &int:aLength.
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
        system'external'USER32 InvalidateRect &int:theHandle &int:0 &int:1.
        system'external'USER32 UpdateWindow &int:theHandle.
    ]
    
    #method(stacksafe) setPosition &int:x &int:y &int:cx &int:cy &int:flags
    [
        system'external'USER32 SetWindowPos &int:theHandle &int:0 &int:x &int:y &int:cx &int:cy &int:flags.
    ]

    #method(stacksafe) getClientRect &win_rect:aStruct
    [
        system'external'USER32 GetClientRect &int:theHandle &win_rect:aStruct.
    ]
  
    #method(stacksafe) read &hdc:anHDC
    [
        #var(type:int)dc := system'external'USER32 GetDC &int:theHandle.
        
        anHDC << dc.
    ]

    #method hdc
    [
        #var(type:hdc)aDC.
        $self read &hdc:aDC.

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

#class(sealed,struct:embeddable)PAINTSTRUCT
{
    #field(type:hdc)      theHDC.
    #field(type:int)      theErase.
    #field(type:win_rect) theRect. 
    #field(type:int)      theRestore.
    #field(type:int)      theIncUpdate.
    
    #field(type:int)      dummy0.
    #field(type:int)      dummy1.
    #field(type:int)      dummy2.
    #field(type:int)      dummy3.
    #field(type:int)      dummy4.
    #field(type:int)      dummy5.
    #field(type:int)      dummy6.
    #field(type:int)      dummy7.

    #method(stacksafe) read &hdc:aHandle
        => system'internal'int_save.
            
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
