#define system.

// --- $windowClassControl ---

#symbol $windowClassControl =
{
    $fillWndClass &bytearray:anArray &wide:aName &int:aStyle &hinstance:anInstance &int:aBackground
        => system'internal'win_fillWndClass.
    
    createClass &name:aName &hinstance:anInstance &int:aStyle &int:aBackground
    [
        #var(type:bytearray,size:40)aStruct.
        #var(type:wide) aWideName := aName wide.
        
        $self $fillWndClass &bytearray:aStruct &wide:aWideName &int:aStyle &hinstance:anInstance &int:aBackground.
        
        system'external'USER32 RegisterClassW &bytearray:aStruct.        
        
        ^ aWideName.
    ]
}.

// --- SDIWindowClass ---

#static SDIWindowClass = $windowClassControl createClass &name:"ELENA.SDIWINDOW.3.1" &hinstance:CurrentInstance &int:WS_ELENAWINDOW &int:COLOR_BACKGROUND.

// --- BaseWindowHandle ---

#class BaseWindowHandle :: ControlHandle
{
    #method destroy
    [
        #var(type:int)anIndex := system'external'USER32 GetWindowLongW &handle:theHandle &int:GWL_USERDATA.
        
        Pointer new &index:anIndex close.
    ]
}

// --- SDIWindowCallback ---

#class SDIWindowCallback :: WindowCallback
{
    #field(type:win_sdiwindow)theOwner.
    
    #constructor new &win_sdiwindow:anOwner
    [
        theOwner := anOwner.
    ]
    
    #method eval &handle:anHWND &int:aMessage &vint:aRetVal
    [
        (aMessage == WM_DESTROY)
            ? [
                theOwner destroy.
                
                aRetVal << 0.
            ].
        
        $super eval &handle:anHWND &int:aMessage &vint:aRetVal.
    ]
    
//    #method eval &handle:anHWND &int:aMessage &int:aWParam &int:aLParam &vint:aRetVal
//    [
//        $super eval &handle:anHWND &int:aMessage &int:aWParam &int:aLParam &vint:aRetVal.
//    ]
}

// --- SDIWindowHandle ---
    
#class(sealed) SDIWindowHandle :: BaseWindowHandle
{
    #constructor new &wide:aCaption &int:x &int:y &int:width &int:height
    [
        $self create &int:EX_SDI_OVERLAPPEDWINDOW &wide:SDIWindowClass &wide:aCaption &int:SDI_OVERLAPPEDWINDOW 
            &int:x &int:y &int:width &int:height &handle:(Handle min) &hinstance:CurrentInstance &object:(SDIWindowCallback new &win_sdiwindow:$self).
    ]
    
    #method win_sdiwindow = $self.
    
    #method destroy
    [
        $super destroy.
        
        'program stop.
    ]
}