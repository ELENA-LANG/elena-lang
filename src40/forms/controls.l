import system'drawing;

//// --- Events ---
//
//type index_changed = IndexChangedAction.
//
//limited closure class IndexChangedAction
//{
//    stacksafe closure(object sender, int index)
//    [
//    ]
//}

// --- BaseControl ---

public class BaseControl
{
    bool   theVisible;
//    bool   theEnabled.
    Region theRegion;
//    bool   theTabStop.
    
    wide   theCaption;
//    object theParent.
    
    constructor new()
    {
        theVisible := true;
//        theEnabled := true.
        theRegion := new Region();
//        theTabStop := false.
        theCaption := emptyWideString;
    }
    
//    visible = theVisible.
//    
//    set visible:aValue
//    [
//        theVisible := aValue bool.
//    ]
//    
//    enabled = theEnabled.
//    
//    set enabled : aValue
//    [
//        theEnabled := aValue bool.
//    ]
//    
//    parent = theParent.
//    
//    $set parent:aParent
//    [
//        if($nil == aParent)
//            [
//                if($nil == theParent)
//                    [ InvalidOperationException new:"A control already unbound"; raise ].
//                
//                theParent := $nil
//            ];
//            [
//                if($nil != theParent)
//                    [ InvalidOperationException new:"A control already bound"; raise ].
//                
//                theParent := aParent
//            ]
//    ]
//    
    prop string Caption
    {
        string get() = theCaption;
        
        set(string s)
        {
            theCaption := s
        }        
    }

//    set caption : aValue
//    [
//        theCaption := aValue.
//    ]
//    
//    append caption : aValue
//    [    
//        $self set caption:($self caption + aValue).
//    ]
//    
//    caption = theCaption.
//    
//    stacksafe setLocation int:x int:y
//    [
//        theRegion writeLocation int:x int:y.
//    ]
    
    vprop int X
    {
        read(ref int retVal)
        {
            retVal := theRegion.X;
        }
        
        set(int n)
        {
            theRegion.X := n
        }
    }

    vprop int Y
    {
        read(ref int retVal)
        {
            retVal := theRegion.Y;
        }
        
        set(int n)
        {
            theRegion.Y := n
        }
    }

    vprop int Width
    {
        read(ref int retVal)
        {
            retVal := theRegion.Width;
        }
        
        set(int n)
        {
            theRegion.Width := n
        }
    }

    vprop int Height
    {
        read(ref int retVal)
        {
            retVal := theRegion.Height;
        }
        
        set(int n)
        {
            theRegion.Height := n
        }
    }

    setRegion(int x, int y, int width, int height)
    {
        theRegion.set(x, y, width, height);
    }

//    
//    location = theRegion point.
//
//    dimension = theRegion dimension.
//    
//    stacksafe setDimension int:aWidth int:aHeight
//    [
//        theRegion writeDimension int:aWidth int:aHeight.
//    ]
//            
//    stacksafe readDimensionTo vint:aWidth vint:aHeight
//    [
//        theRegion readDimensionTo vint:aWidth vint:aHeight.        
//    ]
//    
//    stacksafe readLocationTo vint:x vint:y
//    [
//        theRegion readLocationTo vint:x vint:y
//    ]
//    
////    #method is &tabstop = theTabStop.
////    
////    #method set &tabstop:aValue
////    [
////        theTabStop := aValue bool.
////    ]
}
