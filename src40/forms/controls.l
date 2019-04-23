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
    bool   theEnabled;
    Region theRegion;
    bool   theTabStop;
    
    wide   theCaption;
    object theParent;
    
    constructor new()
    {
        theVisible := true;
        theEnabled := true;
        theRegion := new Region();
        theTabStop := false;
        theCaption := emptyWideString;
    }

    bool Visible
    {
        bool get() = theVisible;
        
        
        set(bool v)
        {
            theVisible := v
        }
    }

    bool Enabled
    {
        bool get() = theEnabled;
        
        set(bool value)
        {
            theEnabled := value
        }
    }
        
    Parent = theParent;    

    internal set Parent(object o)
    {
        theParent := o
    }

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
    string Caption
    {
        string get() = theCaption;
        
        set(string s)
        {
            theCaption := s
        }        
    }

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
    
    X
    {
        int get()
        {
            int retVal := theRegion.X;
            
            ^ retVal
        }
        
        set(int n)
        {
            theRegion.X := n
        }
    }

    Y
    {
        int get()
        {
            int retVal := theRegion.Y;
            
            ^ retVal
        }
        
        set(int n)
        {
            theRegion.Y := n
        }
    }

    Width
    {
        int get()
        {
            int retVal := theRegion.Width;
            
            ^ retVal
        }
        
        set(int n)
        {
            theRegion.Width := n
        }
    }

    Height
    {
        int get()
        {
            int retVal := theRegion.Height;
            
            ^ retVal
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

    Dimension Dimension = theRegion.Dimension;
//    
////    #method is &tabstop = theTabStop.
////    
////    #method set &tabstop:aValue
////    [
////        theTabStop := aValue bool.
////    ]
}
