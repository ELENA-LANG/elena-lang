import system'drawing;

// --- BaseControl ---

public abstract class BaseControl
{
   bool   _visible;
   bool   _enabled;
   bool   _tabStop;

   Region _region;   

   object _parent;

   wide   _caption;

   string Caption
   {
      get() = _caption;
        
      set(s)
      {
         _caption := s
      }
   }

   Parent = _parent;    

   set internal Parent(object o)
   {
      if(nil == o)
      {
         if(nil == _parent)
            { InvalidOperationException.new:"A control already unbound".raise() };
            
         _parent := nil
      }
      else
      {
         if(nil != _parent)
            { InvalidOperationException.new:"A control already bound".raise() };
            
         _parent := o
      }
   }

   setRegion(int x, int y, int width, int height)
   {
      _region.set(x, y, width, height);
   }

   protected constructor new()
   {
      _visible := true;
      _enabled := true;
      _region := new Region();   
      _caption := emptyWideString;
      _tabStop := false;
   }
}