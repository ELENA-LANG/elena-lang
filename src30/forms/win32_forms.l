#import system.
#import system'drawing.
#import system'winforms.

// --- BaseForm ---

#class BaseWinForm :: BaseWinContainer
{
    #field(func1)theOpenEvent.
    
    #constructor new
        <= (new)
    [
    ]
    
    #method controls = theControls.

    #method set &onOpen:aFunction
    [
        theOpenEvent := aFunction func1.
    ]        
    
    #method open
    [
        $super open.
        
        ($nil != theOpenEvent)
            ? [ theOpenEvent eval:self. ].
    ]
    
    #method close
    [
        $super close.
        
        theHandle free.
    ]
}

// --- SDI ---

#class SDIWindowListener :: SDIWindowCallback
{
    #field(sdiform)theOwner.
    
    #constructor new &sdiform:anOwner
    [
        theOwner := anOwner.
    ]
    
    #method onsize &hwnd:aControl &int:width &int:height &vint:aResult
    [
        theOwner $resize &int:width &int:height.
        
        aResult := 0.
    ]
    
    #method onclick &hwnd:aHandle &vint:aResult
    [
        #var aControl := theOwner retrieve &hwnd:aHandle.
        ($nil != aControl)
            ? [ aControl click. ].
        
        $super onclick &hwnd:aHandle &vint:aResult.
    ]
    
    #method onchange &hwnd:aHandle &vint:aResult
    [
        #var aControl := theOwner retrieve &hwnd:aHandle.
        ($nil != aControl)
            ? [ aControl change. ].
        
        $super onchange &hwnd:aHandle &vint:aResult.
    ]
    
    #method onmenu &int:aCode &vint:aResult    
    [
        #var anItem := theOwner retrieveMenu &int:aCode.
        ($nil != anItem)
            ? [ anItem click. ].
        
        $super onmenu &int:aCode &vint:aResult.
    ]    
}

#class(limited) SDIForm :: BaseWinForm
{
    #field theMenu.
    
    #constructor new
        <= (new)
    [
    ]
    
    #method sdiform = $self.
    
    #method set &menu:aMenu
    [
        theMenu := aMenu.
    ]
    
    #method(stacksafe) retrieveMenu &int:anID
        => theMenu.
    
    #method $createHandle
    [
        #var(int)Styles := WS_OVERLAPPEDWINDOW.
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
        
        #var(int)anX.
        #var(int)anY.
        #var(int)aWidth.
        #var(int)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:EX_SDI_OVERLAPPEDWINDOW 
                        &wide:SDIWindowClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:(WindowHandle min) 
                        &hinstance:CurrentInstance 
                        &object:(SDIWindowListener new &sdiform:$self).
    ]
}

// --- SDIDialog ---

#class SDIDialog :: SDIForm
{
    #constructor new
        <= (new)
    [
    ]
    
    #method $createHandle
    [
        #var(int)Styles := WS_DIALOGWINDOW.
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
        
        #var(int)anX.
        #var(int)anY.
        #var(int)aWidth.
        #var(int)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:EX_SDI_OVERLAPPEDWINDOW 
                        &wide:SDIWindowClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:(WindowHandle min) 
                        &hinstance:CurrentInstance 
                        &object:(SDIWindowListener new &sdiform:$self).
    ]
}

// --- Child ---

#class ChildWindowListener :: ChildWindowCallback
{
    #field(form)theOwner.
    
    #constructor new &form:anOwner
    [
        theOwner := anOwner.
    ]
    
    #method onsize &hwnd:aControl &int:width &int:height &vint:aResult
    [
        theOwner $resize &int:width &int:height.
        
        aResult := 0.
    ]
    
    #method onclick &hwnd:aHandle &vint:aResult
    [
        #var aControl := theOwner retrieve &hwnd:aHandle.
        ($nil != aControl)
            ? [ aControl click. ].
        
        $super onclick &hwnd:aHandle &vint:aResult.
    ]
    
    #method onchange &hwnd:aHandle &vint:aResult
    [
        #var aControl := theOwner retrieve &hwnd:aHandle.
        ($nil != aControl)
            ? [ aControl change. ].
        
        $super onchange &hwnd:aHandle &vint:aResult.
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
        #var(int)Styles := WS_POPUPWINDOW || WS_DLGFRAME.
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
        
        #var(int)anX.
        #var(int)anY.
        #var(int)aWidth.
        #var(int)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:(WS_EX_DLGMODALFRAME || WS_EX_CONTROLPARENT)
                        &wide:SDIWindowClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:(theParent hwnd)
                        &hinstance:CurrentInstance 
                        &object:(ChildWindowListener new &form:$self).
    ]
}