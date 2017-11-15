import extensions.
import system'collections.

// --- IControl ---

limited class IControl
{
}

// --- IContainer ---

limited class IContainer
{
    append control:control []
}

template icontrolof :: type
{
    type = $self.
    
    control = IControl::
    {
        
    }.
}

template icontainerof :: type
{    
    type = $self.
    
    container = IContainer::
    {
        append control:control
        [
            theControls append control:control.
        ]
    }.
    
    appendAndReturnControl : control
    [
        control $create container:$self int($self xhandle) object:self.
        
        ^ control
    ]
}

// --- BaseControl ---

class BaseControl
{
    object theOwner.
    
    int    theHandle.
    
    /// properties
    int vxprop x      :: readXTo,writeX.
    int vxprop y      :: readXTo,writeY.
    int vxprop width  :: readWidthTo,writeWidth.
    int vxprop height :: readHeightTo,writeHeight.
    
    int xhandle = theHandle.
    
    owner = theOwner.
    
    stacksafe readXTo vint:retVal
    [
        int val := system'external'elenagm SetNProperty int:theHandle int:propX int:0.
        
        retVal int := val.
    ]
    
    stacksafe writeX int:val
    [
        system'external'elenagm SetNProperty int:theHandle int:propX int:val.
    ]
    
    stacksafe readYTo vint:retVal
    [
        int val := system'external'elenagm SetNProperty int:theHandle int:propY int:0.
        
        retVal int := val.
    ]
    
    stacksafe writeY int:val
    [
        system'external'elenagm SetNProperty int:theHandle int:propY int:val.
    ]
    
    stacksafe readWidthTo vint:retVal
    [
        int val := system'external'elenagm SetNProperty int:theHandle int:propWidth int:0.
        
        retVal int := val.
    ]
    
    stacksafe writeWidth int:val
    [
        system'external'elenagm SetNProperty int:theHandle int:propWidth int:val.
    ]
    
    stacksafe readHeightTo vint:retVal
    [
        int val := system'external'elenagm SetNProperty int:theHandle int:propHeight int:0.
        
        retVal int := val.
    ]
    
    stacksafe writeHeight int:val
    [
        system'external'elenagm SetNProperty int:theHandle int:propHeight int:val.
    ]    
    
//    caption
//    [
//    ]
    
    set caption:val
    [
        system'external'elenagm SetText int:theHandle wide:val.
    ]
}

// --- BaseContainer ---

class BaseContainer :: BaseControl
{
    list<IControl> theControls.
    
    constructor new
    [
        theControls := list<IControl>(5).
    ]
}

// --- Label ---

sealed class Label :: BaseControl, icontrolof<label>
{
    constructor new container:container int:hParent object:owner
    [
        $self $create container:container int:hParent object:owner.
    ]
    
    constructor new : parent
        <= new container:parent int(parent xhandle) object:parent;
    [
    ]
    
    constructor new
    [
    ]
    
    $create container:container int:hParent object:owner
    [
        theOwner := owner.
        
        container append control:$self.
        
        int hRect := system'external'elenagm NewWidget int:hParent int:widgRect.
        int hText := system'external'elenagm NewWidget int:hRect int:widgText.
        
        theHandle := hText.
    ]
}

// --- Window ---

sealed class Window :: BaseContainer, icontainerof<window>
{
    constructor new
        <= new;
    [
        int hRect := system'external'elenagm NewWidget int:0 int:widgRect.
        
        theHandle := hRect.
    ]
}