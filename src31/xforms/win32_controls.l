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
}

// --- BaseControl ---

class BaseControl
{
    int    theHandle.
    
    /// properties
    int x      :: nprop(readXTo,writeX).
    int y      :: nprop(readXTo,writeY).
    int width  :: nprop(readWidthTo,writeWidth).
    int height :: nprop(readHeightTo,writeHeight).
    
    int xhandle = theHandle.
    
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
    listof theControls(control).
    
    constructor new
    [
        theControls := 5.
    ]
}

// --- Label ---

sealed class Label :: BaseControl, icontrolof(label)
{
    constructor new container:container int:hParent 
    [
        container append control:$self.
        
        int hRect := system'external'elenagm NewWidget int:hParent int:widgRect.
        int hText := system'external'elenagm NewWidget int:hRect int:widgText.
        
        theHandle := hText.
    ]
    
    constructor new : parent
        <= new container:parent int:(parent xhandle);
    [
    ]
}

// --- Window ---

sealed class Window :: BaseContainer, icontainerof(window)
{
    constructor new
        <= new;
    [
        int hRect := system'external'elenagm NewWidget int:0 int:widgRect.
        
        theHandle := hRect.
    ]
}