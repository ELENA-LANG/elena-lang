import system'winforms;
import system'collections;

// --- IControl ---

public interface IControl
{
   abstract IControl retrieve(Handle handle);

   abstract open();

   abstract click();
}

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

   set Caption(string s)
   {
      _caption := s;
            
      ifnot(nil == _handle)
      {
         _handle.setCaption(cast wide(s))
      }
   }
    
   set Caption(wide s)
   {
      _caption := s;
            
      ifnot(nil == _handle)
      {
         _handle.setCaption(s)
      }
   }
        
   set Caption(v)
   {
      self.Caption := cast string(v)
   }

//   string Caption
//   {
//      set(string s)
//      {
//         theCaption := s;
//         //super.Caption := s;
//            
//         ifnot(nil == _handle)
//         {
//            _handle.setCaption(cast wide(s))
//         }
//      }
//    
//      set(wide s)
//      {
//         _caption := s;
//            
//         ifnot(nil == _handle)
//         {
//            _handle.setCaption(s)
//         }
//      }
//        
//      set(v)
//      {
//         self.Caption := cast string(v)
//      }
//   }

   get string Caption()
   {
      ifnot(nil == _handle)
      {
         _caption := _handle.getCaption()
      };
            
      ^ _caption
   }

   setRegion(int x, int y, int width, int height)
   {
      super.setRegion(x, y, width, height);
        
      ifnot(nil == _handle)
      {
         _handle.setPosition(x, y, 0, 0, SWP_NOSIZE);
         _handle.setPosition(0, 0, width, height, SWP_NOMOVE)
      }        
   }

   IControl retrieve(Handle handle)
   {
      if(nil == _handle)
         { ^ nil };
            
      if(_handle == handle)
         { ^ self };
            
      ^ nil 
   }

   WindowHandle cast() = _handle;
}

// --- BaseWinContainer ---

public abstract class BaseWinContainer : BaseWinControl
{
   List<IControl> _controls;

   protected constructor new()
      <= super new()
   {
      _controls := new List<IControl>();
   }

   IControl retrieve(Handle handle)
   {
      if(_handle == handle)
            { ^ self };

      int len := _controls.Length;
      IControl current;
      for (int i := 0, i < len, i += 1) {
         current := _controls[i].retrieve(handle);
         if (current != nil) {
            ^ current;
         }
      };
                            
      ^ nil
   }        

   open()
   {
      super.open();
        
      int len := _controls.Length;
      for (int i := 0, i < len, i += 1) {
         _controls[i].open()
      }
   }
}

// --- Label ---
public closed class Label : BaseWinControl, interface<IControl>
{
   constructor new()
      <= super new()
   {
   }

   internal createHandle()
   {
      WindowHandle parent := _parent;
   
      int styles := WS_CHILD;
      if(_visible)
         { styles := styles | WS_VISIBLE };
        
      _region.read(ref int x, ref int y, ref int width, ref int height);
        
      _handle := new WindowHandle(
                        0,
                        StaticClass, 
                        cast wide(_caption), 
                        styles,
                        x, y, width, height,
                        parent, 
                        CurrentInstance,
                        UnsafePointer.Default)
   }
}

// --- Label ---

public class StaticLabel : Label
{
   constructor new()
      <= super new()
   {
   }
    
   internal createHandle()
   {
      WindowHandle parent := _parent;
      if(parent == 0)
         { InvalidOperationException.new("The parent window is closed").raise() };
        
      int styles := WS_CHILD | SS_SIMPLE;
      if(_visible)
         { styles := styles | WS_VISIBLE };
        
      _region.read(ref int x, ref int y, ref int width, ref int height);
        
      _handle := new WindowHandle(
                        0,
                        StaticClass, 
                        cast wide(_caption), 
                        styles,
                        x, y, width, height,
                        parent, 
                        CurrentInstance,
                        UnsafePointer.Default)
                        
   }
}

// --- BaseEdit ---

abstract class BaseEdit : BaseWinControl
{
   protected constructor new()
      <= super new()
   {
   }

   string Value = self.Caption;

//   get retoverload Value(ref wide retVal)
//   {
//      retVal := self.Caption
//   }
    
   set Value(string value)
   {
      self.Caption := cast WideString(value);
   }   

   set Value(wide value)
   {
      self.Caption := value;
   }   

   internal updateStyles(ref int retVal)
   {
      int styles := retVal;
        
      if(_visible)
         { styles := styles | WS_VISIBLE };
      if(_tabStop)
         { styles := styles | WS_TABSTOP };
            
      retVal := styles
   }

}

// --- Edit ---

public sealed class Edit : BaseEdit, interface<IControl>
{
   constructor new()
      <= super new()
   {
      _tabStop := true
   }

   internal createHandle()
   {
      WindowHandle parent := _parent;
        
      int styles := WS_CHILD | WS_BORDER;
      self.updateStyles(ref styles);
        
      _region.read(ref int x, ref int y, ref int width, ref int height);
        
      _handle := new WindowHandle(
                     WS_EX_CLIENTEDGE,
                     EditClass, 
                     _caption, 
                     styles,
                     x, y, width, height,
                     parent, 
                     CurrentInstance,
                     UnsafePointer.Default)                        
   }
}

// --- BaseButton ---

public abstract class BaseButton : BaseWinControl
{
   Func1 onClick : event;

   protected constructor new()
      <= super new()
   {
      _tabStop := true
   }

   click()
   {
      onClick.?(self)
   }

   internal updateStyles(ref int retVal)
   {
      int styles := retVal;
        
      if(_visible)
         { styles := styles | WS_VISIBLE };
      if(_tabStop)
         { styles := styles | WS_TABSTOP };
            
      retVal := styles
   }
}

// --- Button ---

public sealed class Button : BaseButton, interface<IControl>
{
   constructor new()
      <= super new()
   {
   }

   internal createHandle()
   {
      WindowHandle parent := _parent;
        
      int styles := WS_CHILD | BS_PUSHBUTTON;
      self.updateStyles(ref styles);
        
      _region.read(ref int x, ref int y, ref int width, ref int height);
        
      _handle := new WindowHandle(
                        0,
                        ButtonClass, 
                        cast wide(_caption), 
                        styles,
                        x, y, width, height,
                        parent, 
                        CurrentInstance,
                        UnsafePointer.Default)                        
   }

}
