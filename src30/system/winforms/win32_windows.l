#define system.

// --- SDIWindowClass ---

#static SDIWindowClass = $windowClassControl createClass &name:"ELENA.SDIWINDOW.3.1" &hinstance:CurrentInstance &int:WS_ELENAWINDOW &int:COLOR_BACKGROUND.

// --- BaseWindowHandle ---

#class BaseWindowHandle :: ControlHandle
{
    #field(type:win_listener)theListener.
    
    #method set &win_listener:aListener
    [
        theListener := aListener.
    ]
    
    #method onclick &handle:aControl &vint:aRetVal
    [
        (nil == theListener)
            ! [ theListener onclick &handle:aControl &vint:aRetVal. ].
    ]
    
    #method close
    [
        system'external'USER32 DestroyWindow &handle:theHandle.
    ]
    
    #method destroy
    [
        #var(type:int)anIndex := system'external'USER32 SetWindowLongW &handle:theHandle &int:GWL_USERDATA &int:0.
        (anIndex != 0)
            ? [ RTPointer new &index:anIndex close. ].
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
        (aMessage == WM_CLOSE)
            ? [
                theOwner close.
                
                aRetVal << 0.
                
                ^ $self.
            ].
        
        (aMessage == WM_DESTROY)
            ? [
                theOwner destroy.
                
                aRetVal << 0.
                
                ^ $self.
            ].
        
        $super eval &handle:anHWND &int:aMessage &vint:aRetVal.
    ]
    
    #method wm_command &handle:anHWND &int:aMessage &int:aCode &handle:aControl &vint:aRetVal
    [
        (aControl == 0)
            ? [ ^ $self. ]. // !! temporal

        // !! note low word may contains the control ID, so only high word should be checked
        (aCode == 0)
            ? [ theOwner onclick &handle:aControl &vint:aRetVal. ].
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
        // NOTE: SDIWindow is the main program window, so closing it means exiting the program
        'program stop.
        
        $super destroy.        
    ]
}