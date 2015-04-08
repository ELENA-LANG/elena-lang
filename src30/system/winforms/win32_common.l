#define system.

#symbol(const,type:int)GWL_USERDATA           = -21.

#symbol(const,type:int)WS_OVERLAPPEDWINDOW    = 00CF0000h.
#symbol(const,type:int)SDI_OVERLAPPEDWINDOW   = 10CF0000h.

#symbol(const,type:int)WS_EX_OVERLAPPEDWINDOW = 00000300h.

// --- Window Color Constants

#symbol(const,type:int)COLOR_BACKGROUND = 5.

// --- Window Styles ---

#symbol(const,type:int)WS_ELENAWINDOW = 0100Bh.

// --- WindowCallback ---

#class(limited) WindowCallback
{
    #method win_callback = $self.
    
    #method wm_create &handle:anHWND &int:aMessage &int:aWParam &int:aLParam &vint:aRetVal
    [
        system'external'USER32 SetWindowLongW &handle:anHWND &int:/*GWL_USERDATA*/-21 &int:aLParam.
        
        aRetVal << 0.
    ]
    
    #method eval &handle:anHWND &int:aMessage &int:aWParam &int:aLParam &vint:aRetVal
    [
        #var(type:int)result := system'external'USER32 DefWindowProcW &handle:anHWND &int:aMessage &int:aWParam &int:aLParam.
        
        aRetVal << result.
    ]
}

// --- ControlHandle ---

#class ControlHandle
{
    #field(type:handle)theHandle.
    
    #constructor new &handle:aHandle
    [
        theHandle := Handle new &handle:aHandle.
    ]

    #constructor new
    [
        theHandle := 0.
    ]

    #method create &int:dwExStyle &wide:aClassName &wide:aWindowName &int:aStyle &int:x &int:y &int:width &int:height &handle:aParent &hinstance:instance &object:aParam
    [
        #var(type:int)lparam := 0.
        (nil != aParam) ?
        [
            #var(type:ptr)aPointer := Pointer new:aParam.
    
            #var(type:vint)aParamIndex.
            aPointer read &vint:aParamIndex.
            
            lparam := aParamIndex.
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
            &hinstance: instance
            &int:       lparam.

        (aHandle==0)
            ? [ #throw WinFormsException new. ].

        theHandle := Handle new &handle:aHandle.
    ]
            
    #method close
    [
        theHandle close.
    ]
}

// --- HINSTANCE ---

#static(type:hinstance) CurrentInstance = HInstance current.