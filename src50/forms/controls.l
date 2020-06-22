import system'drawing;

// --- BaseControl ---

public abstract class BaseControl
{
    bool   theVisible;
    bool   theEnabled;
    Region theRegion;
    bool   theTabStop;
    
    wide   theCaption;
    object theParent;
    
    string id;
    
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

    string ID
    {
        get()
            = id;
            
        set(string v)
        {
            id := v
        }
        
        set(v)
        {
            self.ID := cast string(v); 
        }
    }

    setRegion(int x, int y, int width, int height)
    {
        theRegion.set(x, y, width, height);
    }

    Dimension Dimension = theRegion.Dimension;
}
