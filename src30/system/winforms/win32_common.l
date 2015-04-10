#define system.

#symbol(const,type:int)GWL_USERDATA            = -21.

// --- styles ---
#symbol(const,type:int)WS_OVERLAPPEDWINDOW     = 00CF0000h.
#symbol(const,type:int)SDI_OVERLAPPEDWINDOW    = 10CF0000h.
#symbol(const,type:int)WS_CHILD                = 40000000h.
#symbol(const,type:int)WS_VISIBLE              = 10000000h.
#symbol(const,type:int)WS_BORDER               = 00800000h.
#symbol(const,type:int)WS_TABSTOP              = 00010000h.

// --- ex styles ---
#symbol(const,type:int)WS_EX_OVERLAPPEDWINDOW  = 00000300h.
#symbol(const,type:int)EX_SDI_OVERLAPPEDWINDOW = 00040101h.
#symbol(const,type:int)WS_EX_CLIENTEDGE        = 00000200h.

// --- static styles ---
#symbol(const,type:int)SS_SIMPLE               = 0000000Bh.

// --- button styles ---
#symbol BS_DEFPUSHBUTTON                       = 1. 
#symbol BS_PUSHBUTTON                          = 0. 

// --- messages ---
#symbol(const,type:int)WM_DESTROY = 02h.
#symbol(const,type:int)WM_CLOSE   = 10h.

// --- Notifications ---
#symbol(const,type:int)BN_CLICKED = 0.

// --- Window Color Constants

#symbol(const,type:int)COLOR_BACKGROUND = 5.

// --- Window Styles ---

#symbol(const,type:int)WS_ELENAWINDOW = 0100Bh.

// --- WindowCallback ---

#class(limited) WindowCallback
{
    #method win_callback = $self.
    
    #method wm_create &handle:anHWND &int:aMessage &int:aWParam &win_createstruct:aCreateStruct &vint:aRetVal
    [
        #var(type:vint)param.
        aCreateStruct readCreateParam &vint:param.
        
        #var(type:int)tmp := param.
        system'external'USER32 SetWindowLongW &handle:anHWND &int:/*GWL_USERDATA*/-21 &int:tmp.
        
        system'external'USER32 DefWindowProcW &handle:anHWND &int:aMessage &int:aWParam &win_createstruct:aCreateStruct.
        
        aRetVal << 0.
    ]
    
    #method eval &handle:anHWND &int:aMessage &vint:aRetVal
    [
        #var(type:int)result := system'external'USER32 DefWindowProcW &handle:anHWND &int:aMessage &int:0 &int:0.
        
        aRetVal << result.
    ]

    #method wm_command &handle:anHWND &int:aMessage &int:aCode &handle:aControl &vint:aRetVal
    [
        aRetVal << -1.
    ]

//    #method eval &handle:anHWND &int:aMessage &int:aWParam &int:aLParam &vint:aRetVal
//    [
//        #var(type:int)result := system'external'USER32 DefWindowProcW &handle:anHWND &int:aMessage &int:aWParam &int:aLParam.
//        
//        aRetVal << result.
//    ]
}

// --- WindowListener ---

#class(limited) WindowListener
{
    #method onclick &handle:aControl &vint:aResult
    [
        aResult << 0.
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

    #method handle = theHandle.

    #constructor new
    [
        theHandle := 0.
    ]

    #method create &int:dwExStyle &wide:aClassName &wide:aWindowName &int:aStyle &int:x &int:y &int:width &int:height &handle:aParent &hinstance:instance &object:aParam
    [
        #var(type:int)lparam := 0.
        (nil != aParam) ?
        [
            #var(type:rt_ptr)aPointer := RTPointer new:aParam.
    
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
}

// --- HINSTANCE ---

#static(type:hinstance) CurrentInstance = HInstance current.