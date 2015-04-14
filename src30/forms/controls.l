#define system.

// --- BaseControl ---

#class BaseControl
{
    #field(type:caption)theCaption.
    #field(type:int)theX.
    #field(type:int)theY.
    #field(type:int)theWidth.
    #field(type:int)theHeight.
    
    #method caption = theCaption.
    
    #method x = theX.
    
    #method y = theY.
    
    #method width = theWidth.
    
    #method height = theHeight.
}

// --- BaseContainer ---

#class BaseContainer :: BaseControl
{
}

// --- BaseLabel ---

#class BaseLabel :: BaseControl
{
}