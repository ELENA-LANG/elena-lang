#define system.
#define system'winforms.

// --- BaseForm ---

#class BaseWinForm :: BaseWinContainer
{
    #constructor new
        <= (new)
    [
    ]
    
    #method close
    [
        $super close.
        
        theHandle free.
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
        ($nil != aControl)
            ? [ aControl click. ].
        
        $super onclick &hwnd:aHandle &vint:aResult.
    ]
    
    #method onmenu &int:aCode &vint:aResult    
    [
        #var anItem := theOwner retrieve &menu_id:aCode.
        ($nil != anItem)
            ? [ anItem click. ].
        
        $super onmenu &int:aCode &vint:aResult.
    ]    
}

#class(limited) SDIForm :: BaseWinForm
{
    #field theMenu.
    
    #method sdiform = $self.
    
    #method set &menu:aMenu
    [
        theMenu := aMenu.
    ]
    
    #method retrieve &menu_id:anID
        => theMenu.
    
    #method $createHandle
    [
        #var(type:int)Styles := WS_OVERLAPPEDWINDOW.
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
        
        #var(type:int)anX.
        #var(type:int)anY.
        #var(type:int)aWidth.
        #var(type:int)aHeight.
        
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
        
        #var(type:int)anX.
        #var(type:int)anY.
        #var(type:int)aWidth.
        #var(type:int)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:EX_SDI_OVERLAPPEDWINDOW 
                        &wide:SDIWindowClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:(WindowHandle min) 
                        &hinstance:CurrentInstance 
                        &object:(SDIWindowCallback new &win_listener:(SDIWindowListener new &sdiform:$self)).
    ]
}

// --- Child ---

#class ChildWindowListener :: WindowListener
{
    #field(type:form)theOwner.
    
    #constructor new &form:anOwner
    [
        theOwner := anOwner.
    ]
    
    #method onclick &hwnd:aHandle &vint:aResult
    [
        #var aControl := theOwner retrieve &hwnd:aHandle.
        ($nil != aControl)
            ? [ aControl click. ].
        
        $super onclick &hwnd:aHandle &vint:aResult.
    ]
}

#class(limited) ChildForm :: BaseWinForm
{
    #constructor new &parent:aParent
        <= (new)
    [
        theParent := aParent.
    ]
    
    #method form = $self.
    
    #method run
    [
        $self open.
        theParent set &enabled:false.
        
        'program run &hwnd:theHandle.
        
        theParent set &enabled:true.
    ]    

    #method close 
    [
        $self set &visible:false.
        
        $super close.
    ]
            
    #method $createHandle
    [
        #var(type:int)Styles := WS_POPUPWINDOW || WS_DLGFRAME.
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
        
        #var(type:int)anX.
        #var(type:int)anY.
        #var(type:int)aWidth.
        #var(type:int)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:(WS_EX_DLGMODALFRAME || WS_EX_CONTROLPARENT)
                        &wide:SDIWindowClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:(theParent hwnd)
                        &hinstance:CurrentInstance 
                        &object:(ChildWindowCallback new &win_listener:(ChildWindowListener new &form:$self)).
    ]
}