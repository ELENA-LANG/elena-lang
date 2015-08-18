#import system.
#import system'drawing.

// --- HInstance ---

#class(sealed,struct:embeddable) HInstance
{
    #field(type:handle)theHandle.
    
    #constructor new &handle:aHandle
    [
        theHandle := aHandle.
    ]
        
    #constructor current
    //    <= new &hinstance:(system'external'KERNEL32 GetModuleHandleW &int:0).
    [
        #var(type:handle)aCurrent := system'external'KERNEL32 GetModuleHandleW &int:0.
        
        theHandle := aCurrent.
    ]

    #method hinstance = $self.
}

// --- HINSTANCE ---

#static(type:hinstance) CurrentInstance = HInstance current.

// --- MSG ---

#class(sealed,struct:embeddable) MSG 
{
    #field(type:handle)theHWND.
    #field(type:int)   theMessage.
    #field(type:int)   theWParam.
    #field(type:int)   theLParam.
    #field(type:int)   theTime.
    #field(type:int)   theX.
    #field(type:int)   theY.    
    
    #method readMessage &vint:aMessage
    [
    ]
    
    #method win_msg = $self.
}

// --- CREATESTRUCT ---

#class(sealed,struct:embeddable)CREATESTRUCT
{
    #field(type:int)       theCreateParam.
    #field(type:hinstance) theInstance.    
    #field(type:int)       theMenu.
    #field(type:int)       theParent.
    #field(type:int)       theHeight.
    #field(type:int)       theWidth.
    #field(type:int)       theY.
    #field(type:int)       theX.
    #field(type:int)       theStyle.
    #field(type:int)       theNamePtr.
    #field(type:int)       theClassPtr.
    #field(type:int)       theExStyle.
    
    #method(stacksafe) readCreateParam &vint:aRetVal
        => system'internal'int_save.
}

// --- HMENU ---

#class(sealed,struct:embeddable) HMENU
{
    #field(type:handle)theHandle.
    
    #constructor new
    [
        #var(type:handle)h := system'external'USER32 CreateMenu.
        
        theHandle := h.
    ]

    #constructor newPopup
    [
        #var(type:handle)h := system'external'USER32 CreatePopupMenu.
        
        theHandle := h.
    ]

    #method(stacksafe) append &int:Flags &int:anID &wide:aCaption
    [
        system'external'USER32 AppendMenuW &handle:theHandle &int:Flags &int:anID &wide:aCaption.
    ]
            
    #method(stacksafe) append &int:Flags &handle:aParent &wide:aCaption
    [
        system'external'USER32 AppendMenuW &handle:theHandle &int:(Flags || MF_POPUP) &handle:aParent &wide:aCaption.
    ]
            
    #method(stacksafe) free
    [
        system'external'USER32 DestroyMenu &handle:theHandle.
    ]
}
