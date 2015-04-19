#define system.

// --- BaseControl ---

#class BaseControl
{
    #field(type:bool)theVisible.
    #field(type:bool)theEnabled.
    #field(type:region)theRegion.
    #field(type:bool)theTabStop.
    
    #field theCaption.
    #field theParent.
    
    #constructor new
    [
        theVisible := true.
        theEnabled := true.
        theRegion := Region new.
        theTabStop := false.
        theCaption := emptyWideLiteralValue.
    ]
    
    #method is &visible = theVisible.
    
    #method set &visible:aValue
    [
        theVisible := aValue bool.
    ]
    
    #method is &enabled = theEnabled.
    
    #method set &enabled : aValue
    [
        theEnabled := aValue bool.
    ]
    
    #method parent = theParent.
    
    #method $set &parent:aParent
    [
        (nil == aParent)
            ? [
                (nil == theParent)
                    ? [ #throw InvalidOperationException new &message:"A control already unbound". ].
                
                theParent := nil.
            ]
            ! [
                (nil != theParent)
                    ? [ #throw InvalidOperationException new &message:"A control already bound". ].
                
                theParent := aParent.
            ].
    ]
    
    #method set &caption : aValue
    [
        theCaption := aValue.
    ]
    
    #method caption = theCaption.
    
    #method set &location:aPoint
    [
        #var(type:vint)x.
        #var(type:vint)y.
        aPoint read &vint:x &vint:y.
        
        theRegion writeLocation &int:x &int:y.
    ]
    
    #method set &x:anX &y:anY
    [
        $self set &location:(Point new &x:anX &y:anY).
    ]
    
    #method location = theRegion point.

    #method dimension = theRegion dimension.
    
    #method set &dimension:aDimension
    [
        #var(type:vint)aWidth.
        #var(type:vint)aHeight.
        
        aDimension read &vint:aWidth &vint:aHeight.
        
        theRegion writeDimension &int:aWidth &int:aHeight.
    ]
    
    #method set &width:aWidth &height:aHeight
    [
        $self set &dimension:(Dimension new &width:aWidth &height:aHeight).
    ]
    
//    #method read &dimension:aDimension
//    [
//        #var(type:vint)aWidth.
//        #var(type:vint)aHeight.
//        
//        theRegion readDimension &vint:aWidth &vint:aHeight.
//        
//        aDimension write &int:aWidth &int:aHeight.
//    ]
//    
//    #method is &tabstop = theTabStop.
//    
//    #method set &tabstop:aValue
//    [
//        theTabStop := aValue bool.
//    ]
}
