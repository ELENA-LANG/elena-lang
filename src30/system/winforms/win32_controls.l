#define system.

// --- classes ---
#symbol(const,type:wide) StaticClass = "Static" wide. // !! temporal
#symbol(const,type:wide) EditClass   = "Edit" wide. // !! temporal
#symbol(const,type:wide) ButtonClass = "Button" wide. // !! temporal
//#symbol ComboboxClass = "Combobox" wide. // !! temporal
//#static PanelClass = $windowClassControl createClass &name:"ELENA.PANEL.3.1" &hinstance:CurrentInstance &int:(CS_VREDRAW or &int:CS_HREDRAW) &int:COLOR_BACKGROUND.

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
        system'external'USER32 BeginPaint &hwnd:aHandle &win_paintstruct:$self.
    ]
    
    #method end &hwnd:aHandle
    [
        system'external'USER32 EndPaint &hwnd:aHandle &win_paintstruct:$self.
    ]
}

// --- WindowHandle ---

#class(sealed,integer:4) WindowHandle
{
    #constructor min
        => system'internal'int_init.
        
    #constructor new &hwnd:aHandle
        => system'internal'int_copy.
        
    #method $save &int:aHandle
        => system'internal'int_copy.
        
    #constructor new &int:dwExStyle &wide:aClassName &wide:aWindowName &int:aStyle &int:x &int:y &int:width &int:height &hwnd:aParent &hinstance:instance &object:aParam
    [
        #var(type:vint)aParamIndex := 0.
        (nil != aParam) ?
        [
            #var(type:rt_ptr)aPointer := RTPointer new:aParam.
    
            aPointer read &vint:aParamIndex.
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
            &hwnd:      aParent
            &int:       0
            &hinstance: instance
            &int:       aParamIndex.

        (aHandle==0)
            ? [ #throw WinFormsException new. ].

        $self  $save &int:aHandle.
    ]

    #method hwnd = $self.        
    
    #method setVisible &int:aValue
    [
        system'external'USER32 ShowWindow &hwnd:$self &int:aValue.
    ]
    
    #method isVisible
    [
        #var(type:int)visible := system'external'USER32 IsWindowVisible &hwnd:$self.
        
        ^ (visible != 0).
    ]
    
    #method setEnabled &int:aValue
    [
        system'external'USER32 EnableWindow &hwnd:$self &int:aValue.
    ]            
    
    #method isEnabled
    [
        #var(type:int)visible := system'external'USER32 IsWindowEnabled &hwnd:$self.
        
        ^ (visible != 0).
    ]
    
    #method setCaption &wide:aValue
    [
        system'external'USER32 SendMessageW &hwnd:$self &int:WM_SETTEXT &int:0 &wide:aValue.
    ]
    
    #method getCaption
    [
        #var(type:int)aLength := system'external'USER32 SendMessageW &hwnd:$self &int:WM_GETTEXTLENGTH &int:0 &int:0.
        
        (aLength == 0)
            ? [ ^ emptyWideLiteralValue. ]
            ! [
                #var(type:wide)aCaption := WideLiteralValue new &length:aLength.
                aLength := aLength + 1.
                
                system'external'USER32 SendMessageW &hwnd:$self &int:WM_GETTEXT &int:aLength &wide:aCaption.
                
                ^ aCaption.
            ].
    ]
    
    #method refresh
    [
        system'external'USER32 InvalidateRect &hwnd:$self &int:0 &int:1.
        system'external'USER32 UpdateWindow &hwnd:$self.
    ]
    
    #method setPosition &int:x &int:y &int:cx &int:cy &int_attributes:flags
    [
        system'external'USER32 SetWindowPos &hwnd:$self &int:0 &int:x &int:y &int:cx &int:cy &int:flags.
    ]

    #method getClientRect &win_rect:aStruct
    [
        system'external'USER32 GetClientRect &hwnd:$self &win_rect:aStruct.
    ]
  
    #method read &hdc:anHDC
    [
        #var(type:hdc)dc := system'external'USER32 GetDC &hwnd:$self.
        
        anHDC $save &hdc:dc.
    ]
      
    #method close
    [
        system'external'USER32 DestroyWindow &hwnd:$self.
    ]
    
    #method $save &dirty_ptr:aPtr
        => system'internal'int_save.
}

//// --- ComboboxHandle ---
//
//#class(sealed) ComboboxHandle :: BaseComboboxHandle
//{
//    #constructor new &handle:aParent &wide:aText &int:x &int:y &int:width &int:height
//    [
//        $self create 
//            &int:WS_EX_CLIENTEDGE
//            &wide:ComboboxClass &wide:aText
//            &int:(WS_CHILD or:CBS_DROPDOWNLIST or:WS_VISIBLE or:WS_TABSTOP) 
//            &int:x &int:y &int:width &int:height 
//            &handle:aParent 
//            &hinstance:CurrentInstance
//            &object:nil.
//    ]
//}
//
//// --- PanelHandle ---
//
//#class PanelCallback :: WindowCallback
//{
//    #field(type:win_panel)theOwner.
//    
//    #constructor new &win_panel:anOwner
//    [
//        theOwner := anOwner.
//    ]
//}
//    
//#class(sealed) PanelHandle :: BasePanelHandle
//{
//    #constructor new &handle:aParent &int:x &int:y &int:width &int:height
//    [
//        $self create 
//            &int:WS_EX_CONTROLPARENT
//            &wide:PanelClass &int:0
//            &int:(WS_CHILD or:WS_CLIPSIBLINGS) 
//            &int:x &int:y &int:width &int:height 
//            &handle:aParent 
//            &hinstance:CurrentInstance
//            &object:(PanelCallback new &win_panel:$self).
//    ]
//}
