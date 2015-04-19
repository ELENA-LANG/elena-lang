#define system.
#define system'winforms.

// --- BaseForm ---

#class BaseWinForm :: BaseWinContainer
{
    #constructor new
        <= (new)
    [
    ]
}

// --- SDI ---

#class SDIWindowListener :: WindowListener
{
    #field(type:sdiform)theOwner.
    
    #constructor new &sdiform:anOwner
    [
        theOwner := anOwner.
    ]
    
    #method onclick &hwnd:aHandle &vint:aResult
    [
        #var aControl := theOwner retrieve &hwnd:aHandle.
        (nil != aControl)
            ? [ aControl click. ].
        
        $super onclick &hwnd:aHandle &vint:aResult.
    ]
}

#class(limited) SDIForm :: BaseWinForm
{
    #method sdiform = $self.
    
    #method $createHandle
    [
        #var(type:int)Styles := WS_OVERLAPPEDWINDOW.
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
        
        #var(type:vint)anX.
        #var(type:vint)anY.
        #var(type:vint)aWidth.
        #var(type:vint)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:EX_SDI_OVERLAPPEDWINDOW 
                        &wide:SDIWindowClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:(WindowHandle min) 
                        &hinstance:CurrentInstance 
                        &object:(SDIWindowCallback new &win_listener:(SDIWindowListener new &sdiform:$self)).
    ]
}

// --- SDIDialog ---

#class SDIDialog :: SDIForm
{
    #method $createHandle
    [
        #var(type:int)Styles := WS_DIALOGWINDOW.
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
        
        #var(type:vint)anX.
        #var(type:vint)anY.
        #var(type:vint)aWidth.
        #var(type:vint)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:EX_SDI_OVERLAPPEDWINDOW 
                        &wide:SDIWindowClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:(WindowHandle min) 
                        &hinstance:CurrentInstance 
                        &object:(SDIWindowCallback new &win_listener:(SDIWindowListener new &sdiform:$self)).
    ]
}
