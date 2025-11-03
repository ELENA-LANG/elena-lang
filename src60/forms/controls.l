import system'drawing;

// --- BaseControl ---

public abstract class BaseControl
{
   bool    _visible;
   bool    _enabled;
   bool    _tabStop;

   Region  _region;   

   object? _parent;

   wide    _caption;

   string Caption
   {
      get() = _caption;
        
      set(s)
      {
         _caption := s
      }
   }

   bool Enabled
   {
      get() = _enabled;
        
      set(bool value)
      {
         _enabled := value
      }
   }

   bool Visible
   {
      get() = _visible;
        
        
      set(v)
      {
         _visible := v
      }
   }

   Parent = _parent;    

   set internal Parent(object o)
   {
      if(nil == o)
      {
         if(nil == _parent)
            { InvalidOperationException.new("A control already unbound").raise() };
            
         _parent := nil
      }
      else
      {
         if(nil != _parent)
            { InvalidOperationException.new("A control already bound").raise() };
            
         _parent := o
      }
   }

   int Width
   {
      get()
      {
         int retVal := _region.Width;
            
         ^ retVal
      }
        
      set(int n)
      {
         _region.Width := n
      }
   }

   int Height
   {
      get()
      {
         int retVal := _region.Height;
            
         ^ retVal
      }
        
      set(int n)
      {
         _region.Height := n
      }
   }

   int X
   {
      get()
      {
         int retVal := _region.X;
            
         ^ retVal
      }
        
      set(int n)
      {
         _region.X := n
      }
   }

   int Y
   {
      get()
      {
         int retVal := _region.Y;
            
         ^ retVal
      }
        
      set(int n)
      {
         _region.Y := n
      }
   }

   setRegion(int x, int y, int width, int height)
   {
      _region.set(x, y, width, height);
   }

   Region Region
      = _region;

   protected constructor new()
   {
      _visible := true;
      _enabled := true;
      _region := new Region();   
      _caption := emptyWideString;
      _tabStop := false;
   }
}