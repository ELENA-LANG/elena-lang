#define system.
#define system'drawing.

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
        ($nil == aParent)
            ? [
                ($nil == theParent)
                    ? [ #throw InvalidOperationException new &message:"A control already unbound". ].
                
                theParent := $nil.
            ]
            ! [
                ($nil != theParent)
                    ? [ #throw InvalidOperationException new &message:"A control already bound". ].
                
                theParent := aParent.
            ].
    ]
    
    #method set &caption : aValue
    [
        theCaption := aValue.
    ]
    
    #method append &caption : aValue
    [    
        $self set &caption:($self caption + aValue).
    ]
    
    #method caption = theCaption.
    
    #method setLocation &int:x &int:y
    [
        theRegion writeLocation &int:x &int:y.
    ]
    
    #method set &x:x
    [
        #var(type:int)y.
        theRegion readY &vint:y.
        
        $self setLocation &int:(x int) &int:y.
    ]
    
    #method set &y:y
    [
        #var(type:int)x.
        theRegion readX &vint:x.
        
        $self setLocation &int:x &int:(y int).
    ]
    
    #method set &x:x &y:y
    [
        $self setLocation &int:(x int) &int:(y int).
    ]
    
    #method location = theRegion point.

    #method dimension = theRegion dimension.
    
    #method setDimension &int:aWidth &int:aHeight
    [
        theRegion writeDimension &int:aWidth &int:aHeight.
    ]
    
    #method set &width:aWidth &height:aHeight
    [
        $self setDimension &int:(aWidth int) &int:(aHeight int).
    ]

    #method set &width:aWidth
    [
        #var(type:int)aHeight.
        theRegion readHeight &vint:aHeight.
        
        $self setDimension &int:(aWidth int) &int:aHeight.
    ]
    
    #method set &height:aHeight
    [
        #var(type:int)aWidth.
        theRegion readWidth &vint:aWidth.
        
        $self setDimension &int:aWidth &int:(aHeight int).
    ]    
            
    #method readDimension &vint:aWidth &vint:aHeight
    [
        theRegion readDimension &vint:aWidth &vint:aHeight.        
    ]
    
    #method readLocation &vint:x &vint:y
    [
        theRegion readLocation&vint:x &vint:y.        
    ]
    
//    #method is &tabstop = theTabStop.
//    
//    #method set &tabstop:aValue
//    [
//        theTabStop := aValue bool.
//    ]
}
