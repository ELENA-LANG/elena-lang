import system'winforms;

// --- BaseWindowControl ---

public abstract class BaseWinControl : BaseControl
{
   WindowHandle _handle;

   protected constructor new()
      <= super new()
   {
   }

   internal abstract createHandle();

   internal onCreate()
   {
      ifnot(_enabled )
         { _handle.setEnabled(0) };
   }

   open()
   {
      if(nil != _handle)
         { InvalidOperationException.new("The form is already open").raise() };
            
      self.createHandle();
        
      self.onCreate()
   }   
}

// --- BaseWinContainer ---

public abstract class BaseWinContainer : BaseWinControl
{
   protected constructor new()
      <= super new()
   {
   }

   open()
   {
      super.open();
   }
}