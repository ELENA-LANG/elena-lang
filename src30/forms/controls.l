#import system.
#import system'drawing.

// --- Events ---

#template event : prop_name
{
    #method set &prop_name : value
    [
        target := value.
    ]
}

#subject index_changed = IndexChangedAction.

#class(limited,dispatchable:index_changed) IndexChangedAction
{
    #method(action,stacksafe)onIndexChange:sender &int:index
    [
    ]
}

// --- BaseControl ---

#class BaseControl
{
    #field(bool)   theVisible.
    #field(bool)   theEnabled.
    #field(region) theRegion.
    #field(bool)   theTabStop.
    
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
                    ? [ #throw InvalidOperationException new:"A control already unbound". ].
                
                theParent := $nil.
            ]
            ! [
                ($nil != theParent)
                    ? [ #throw InvalidOperationException new:"A control already bound". ].
                
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
    
    #method(stacksafe) setLocation &int:x &int:y
    [
        theRegion writeLocation &int:x &int:y.
    ]
    
    #method set &x:aValue
        <= set &x:aValue &y:(theRegion y).
    
    #method set &y:aValue
        <= set &x:(theRegion x) &y:aValue.
    
    #method set &x:x &y:y
    [
        $self setLocation &int:(x int) &int:(y int).
    ]
    
    #method location = theRegion point.

    #method dimension = theRegion dimension.
    
    #method(stacksafe) setDimension &int:aWidth &int:aHeight
    [
        theRegion writeDimension &int:aWidth &int:aHeight.
    ]
    
    #method set &width:aWidth &height:aHeight
    [
        $self setDimension &int:(aWidth int) &int:(aHeight int).
    ]

    #method set &width:aWidth
        <= set &width:aWidth &height:(theRegion height).
    
    #method set &height:aHeight
        <= set &width:(theRegion width) &height:aHeight.
            
    #method(stacksafe) readDimension &vint:aWidth &vint:aHeight
    [
        theRegion readDimension &vint:aWidth &vint:aHeight.        
    ]
    
    #method(stacksafe) readLocation &vint:x &vint:y
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
