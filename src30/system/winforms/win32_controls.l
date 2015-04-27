#define system.

// --- classes ---
#symbol(const,type:wide) StaticClass = "Static" wide. // !! temporal
#symbol(const,type:wide) EditClass   = "Edit" wide. // !! temporal
#symbol(const,type:wide) ButtonClass = "Button" wide. // !! temporal
//#symbol ComboboxClass = "Combobox" wide. // !! temporal

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

    #method read &hdc:aHandle
        => system'internal'int_save.
            
    #method hdc = HDC new &hdc:theHDC.            
            
    #method begin &hwnd:aHandle
    [
        system'external'USER32 BeginPaint &handle:aHandle &win_paintstruct:$self.
    ]
    
    #method end &hwnd:aHandle
    [
        system'external'USER32 EndPaint &handle:aHandle &win_paintstruct:$self.
    ]
}

// --- WindowHandle ---

#class(sealed,struct:embeddable) WindowHandle
{
    #field(type:handle)theHandle.
    
    #constructor min
        => system'internal'int_init.
        
    #constructor new &hwnd:aHandle
        => system'internal'int_copy.
        
    #constructor new &int:dwExStyle &wide:aClassName &wide:aWindowName &int:aStyle &int:x &int:y &int:width &int:height &hwnd:aParent &hinstance:instance &object:aParam
    [
        #var(type:int)aParamIndex := 0.
        ($nil != aParam) ?
        [
            #var(type:rt_ptr)aPointer := RTPointer new:aParam.
    
            aParamIndex := aPointer.
        ].
                
        #var(type:handle)aHandle := system'external'USER32 CreateWindowExW
            &int:       dwExStyle
            &wide:      aClassName
            &wide:      aWindowName  
            &int:       aStyle
            &int:       x
            &int:       y
            &int:       width
            &int:       height
            &handle:    aParent
            &int:       0
            &handle:    instance
            &int:       aParamIndex.

        (aHandle==0)
            ? [ #throw WinFormsException new. ].

        theHandle := aHandle.
    ]

    #method hwnd = $self.        
    
    #method setVisible &int:aValue
    [
        system'external'USER32 ShowWindow &handle:theHandle &int:aValue.
    ]
    
    #method isVisible
    [
        #var(type:int)visible := system'external'USER32 IsWindowVisible &handle:theHandle.
        
        ^ (visible != 0).
    ]
    
    #method setEnabled &int:aValue
    [
        system'external'USER32 EnableWindow &handle:theHandle &int:aValue.
    ]            
    
    #method isEnabled
    [
        #var(type:int)visible := system'external'USER32 IsWindowEnabled &handle:theHandle.
        
        ^ (visible != 0).
    ]
    
    #method setCaption &wide:aValue
    [
        system'external'USER32 SendMessageW &handle:theHandle &int:WM_SETTEXT &int:0 &wide:aValue.
    ]
    
    #method getCaption
    [
        #var(type:int)aLength := system'external'USER32 SendMessageW &handle:theHandle &int:WM_GETTEXTLENGTH &int:0 &int:0.
        
        (aLength == 0)
            ? [ ^ emptyWideLiteralValue. ]
            ! [
                #var(type:wide)aCaption := WideLiteralValue new &length:aLength.
                aLength := aLength + 1.
                
                system'external'USER32 SendMessageW &handle:theHandle &int:WM_GETTEXT &int:aLength &wide:aCaption.
                
                ^ aCaption.
            ].
    ]
    
    #method refresh
    [
        system'external'USER32 InvalidateRect &handle:theHandle &int:0 &int:1.
        system'external'USER32 UpdateWindow &handle:theHandle.
    ]
    
    #method setPosition &int:x &int:y &int:cx &int:cy &int_attributes:flags
    [
        system'external'USER32 SetWindowPos &handle:theHandle &int:0 &int:x &int:y &int:cx &int:cy &int:flags.
    ]

    #method getClientRect &win_rect:aStruct
    [
        system'external'USER32 GetClientRect &handle:theHandle &win_rect:aStruct.
    ]
  
    #method read &hdc:anHDC
    [
        #var(type:handle)dc := system'external'USER32 GetDC &handle:theHandle.
        
        anHDC << dc.
    ]
                  
    #method close
    [
        system'external'USER32 DestroyWindow &handle:theHandle.
    ]
}
