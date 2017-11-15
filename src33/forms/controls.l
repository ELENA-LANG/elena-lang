import system'drawing.

// --- Events ---

type index_changed = IndexChangedAction.

limited class IndexChangedAction
{
    action stacksafe onIndexChange object:sender int:index
    [
    ]
}

// --- BaseControl ---

class BaseControl
{
    bool   theVisible.
    bool   theEnabled.
    region theRegion.
    bool   theTabStop.
    
    object theCaption.
    object theParent.
    
    constructor new
    [
        theVisible := true.
        theEnabled := true.
        theRegion := Region new.
        theTabStop := false.
        theCaption := emptyWideLiteralValue.
    ]
    
    visible = theVisible.
    
    set visible:aValue
    [
        theVisible := aValue bool.
    ]
    
    enabled = theEnabled.
    
    set enabled : aValue
    [
        theEnabled := aValue bool.
    ]
    
    parent = theParent.
    
    $set parent:aParent
    [
        if($nil == aParent)
            [
                if($nil == theParent)
                    [ InvalidOperationException new:"A control already unbound"; raise ].
                
                theParent := $nil
            ];
            [
                if($nil != theParent)
                    [ InvalidOperationException new:"A control already bound"; raise ].
                
                theParent := aParent
            ]
    ]
    
    set caption : aValue
    [
        theCaption := aValue.
    ]
    
    append caption : aValue
    [    
        $self set caption:($self caption + aValue).
    ]
    
    caption = theCaption.
    
    stacksafe setLocation int:x int:y
    [
        theRegion writeLocation int:x int:y.
    ]
    
    set x:aValue
        <= set x:aValue y(theRegion y).
    
    set y:aValue
        <= set x(theRegion x) y:aValue.
    
    set x:x y:y
    [
        $self setLocation int(x int) int(y int).
    ]
    
    location = theRegion point.

    dimension = theRegion dimension.
    
    stacksafe setDimension int:aWidth int:aHeight
    [
        theRegion writeDimension int:aWidth int:aHeight.
    ]
    
    set width:aWidth height:aHeight
    [
        $self setDimension int:(aWidth int) int:(aHeight int)
    ]

    set width:aWidth
        <= set width:aWidth height(theRegion height).
    
    set height:aHeight
        <= set width(theRegion width) height:aHeight.
            
    stacksafe readDimensionTo vint:aWidth vint:aHeight
    [
        theRegion readDimensionTo vint:aWidth vint:aHeight.        
    ]
    
    stacksafe readLocationTo vint:x vint:y
    [
        theRegion readLocationTo vint:x vint:y
    ]
    
//    #method is &tabstop = theTabStop.
//    
//    #method set &tabstop:aValue
//    [
//        theTabStop := aValue bool.
//    ]
}
