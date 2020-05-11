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

public abstract class BaseControl
{
    bool   theVisible;
    bool   theEnabled;
    Region theRegion;
    bool   theTabStop;
    
    wide   theCaption;
    object theParent;
    
    protected constructor new()
    {
        theVisible := true;
        theEnabled := true;
        theRegion := new Region();
        theTabStop := false;
        theCaption := emptyWideString;
    }

    bool Visible
    {
        get() = theVisible;
        
        
        set(bool v)
        {
            theVisible := v
        }
    }

    bool Enabled
    {
        get() = theEnabled;
        
        set(bool value)
        {
            theEnabled := value
        }
    }
        
    Parent = theParent;    

    set internal Parent(object o)
    {
        if(nil == o)
        {
            if(nil == theParent)
                { InvalidOperationException.new:"A control already unbound".raise() };
            
            theParent := nil
        }
        else
        {
            if(nil != theParent)
                { InvalidOperationException.new:"A control already bound".raise() };
            
            theParent := o
        }
    }

    string Caption
    {
        get() = theCaption;
        
        set(string s)
        {
            theCaption := s
        }        
        
        set(v)
        {
            self.Caption := cast string(v)
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
    
    int X
    {
        get()
        {
            int retVal := theRegion.X;
            
            ^ retVal
        }
        
        set(int n)
        {
            theRegion.X := n
        }
        
        set(v)
        {
            self.X := cast int(v)
        }
    }

    int Y
    {
        get()
        {
            int retVal := theRegion.Y;
            
            ^ retVal
        }
        
        set(int n)
        {
            theRegion.Y := n
        }
        
        set(v)
        {
            self.Y := cast int(v)
        }
    }

    int Width
    {
        get()
        {
            int retVal := theRegion.Width;
            
            ^ retVal
        }
        
        set(int n)
        {
            theRegion.Width := n
        }
        
        set(o)
        {
            self.Width := cast int(o);
        }
    }

    int Height
    {
        get()
        {
            int retVal := theRegion.Height;
            
            ^ retVal
        }
        
        set(int n)
        {
            theRegion.Height := n
        }
        
        set(o)
        {
            self.Height := cast int(o);
        }
    }

    setRegion(int x, int y, int width, int height)
    {
        theRegion.set(x, y, width, height);
    }

    Dimension Dimension = theRegion.Dimension;


////    #method is &tabstop = theTabStop.
////    
////    #method set &tabstop:aValue
////    [
////        theTabStop := aValue bool.
////    ]
}
