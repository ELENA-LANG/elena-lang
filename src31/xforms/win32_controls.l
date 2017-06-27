import system'collections.

// --- IControl ---

limited class IControl
{
}

// --- IContainer ---

limited class IContainer
{
    append xcontrol:control []
}

template icontrolof :: type
{
    type = $self.
    
    xcontrol = IControl::
    {
        
    }.
}

template icontainerof :: type
{    
    xcontainer = IContainer::
    {
        append xcontrol:control
        [
            theControls append xcontrol:control.
        ]
    }.
}

// --- BaseControl ---

class BaseControl
{
}

// --- BaseContainer ---

class BaseContainer :: BaseControl
{
    listof theControls(xcontrol).
    
    constructor new
    [
        theControls := 5.
    ]
}

// --- Label ---

class Label :: BaseControl, icontrolof(xlabel)
{
    constructor new xcontainer:parent
    [
        parent append xcontrol:$self
    ]
    
    constructor new : parent
        <= new xcontainer:parent;
    [
    ]
}

// --- Window ---

sealed class Window :: BaseContainer, icontainerof(xwindow)
{
    constructor new
        <= new;
    [
    ]
}