#define system.

// --- HInstance ---

#class(sealed,integer:4) HInstance
{
    #constructor new &hinstance:aHandle
        => system'internal'int_copy.
        
    #constructor current
    //    <= new &hinstance:(system'external'KERNEL32 GetModuleHandleW &int:0).
    [
        #var(type:hinstance)aCurrent := system'external'KERNEL32 GetModuleHandleW &int:0.
        
        $self $write &hinstance:aCurrent.
    ]

    #method $write &hinstance:anInstance
        => system'internal'int_copy.

    #method $read &vint:aRetVal
        => system'internal'int_save.

    #method hinstance = $self.
}

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
    
    #method readCreateParam &vint:aRetVal
        => system'internal'int_save.
}