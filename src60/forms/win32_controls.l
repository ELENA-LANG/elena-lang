import system'winforms;
import system'collections;
import system'runtime;
import system'drawing;

// --- IControl ---

public interface IControl
{
   get abstract object Control();

   abstract IControl retrieve(Handle handle);

   abstract setRegion(int x, int y, int width, int height);

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

   get Control() = self;

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

   bool Visible
   {
      set(bool v)
      {
         super.Visible := v;
            
         ifnot(nil == _handle)
         {
            _handle.setVisible(_visible ? 5 : 0);
         }
      }
   }

   bool Enabled
   {
      set(value)
      {
         super.Enabled := value;
            
         ifnot(nil == _handle)
         {
            int val := _enabled ? -1 : 0;
                
            _handle.setEnabled(val)
         }
            
      } 
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

   internal resize(Handle hwnd, int width, int height)
   {
      _region.writeDimension(width, height)
   }

   refresh()
   {
      if(nil != _handle)
         { _handle.refresh() }
   }

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
      onClick?.(self)
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

// --- RadioButton ---

public closed class RadioButton : BaseButton, interface<IControl>
{
   bool _checked;
    
   constructor new()
      <= super new()
   {
      _checked := false
   }
    
   constructor new(string caption)
      <= new()
   {
      _checked := false;
        
      self.Caption := caption
   }

   bool Checked
   {
      get()
      {
         ifnot(nil == _handle)
            { _checked := _handle.isChecked() };
                
         ^ _checked
      }
        
      set(v)
      {
         _checked := v;
            
         ifnot(nil == _handle)
         {
            _handle.setChecked(v ? BST_CHECKED : BST_UNCHECKED)
         }
      }
   }

   internal createHandle()
   {
      WindowHandle parent := _parent;
        
      int styles := WS_CHILD | BS_AUTORADIOBUTTON;
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
                        UnsafePointer.Default);
                        
      if(_checked)
         { self.Checked := true }
   }
}

// --- Paintbox ---

class PaintboxListener : WindowCallback
{
   Paintbox _owner;
    
   constructor(Paintbox owner)
   {
      _owner := owner
   }

   onSize(Handle control, int width, int height)
   {
      _owner.resize(control, width, height);
   }

   onWMPaint(Handle hwnd)
   {
      _owner.paint();
   }

   onWMDestoy(Handle hwnd)
   {
      _owner.destroy();
        
      super.onWMDestoy(hwnd)
   }
}

public closed class Paintbox : BaseWinControl, interface<IControl>
{
   Canvas  _canvas;

   Func<object,Canvas,object> onPaint : event;

   constructor new()
      <= super new()
   {
      _canvas := new Canvas()
   }

   internal destroy()
   {
      _canvas.free()
   }
 
   internal paint()
   {
      PAINTSTRUCT struct := new PAINTSTRUCT();
        
      struct.begin(_handle);
      HDC dc := struct.HDC;
        
      _canvas.open(dc);
        
      onPaint?.(self, _canvas);

      _canvas.close();

      struct.end(_handle)
   }

   internal createHandle()
   {
      WindowHandle parent := _parent;

      int styles := WS_CHILD;
      if(_visible)
         { styles := styles | WS_VISIBLE };
                        
      _region.read(ref int x, ref int y, ref int width, ref int height);
       
      pointer paramPtr := PermVectorTable.allocate(new PaintboxListener(self)); 

      _handle := new WindowHandle(
                        WS_EX_TRANSPARENT,
                        PaintboxClass,
                        cast wide(_caption),
                        styles,
                        x, y, width, height,
                        parent,
                        CurrentInstance,
                        paramPtr)
   }
}

public sealed class Imagebox : Paintbox
{
    HBITMAP  _image;
    HDC      _imageDC;
    bool     _needToRefresh;

   constructor new()
      <= super new()
   {
      _needToRefresh := false
   }

   internal destroy()
   {
      super.destroy();
        
      _image.free();
      _imageDC.free()
   }

   internal paint()
   {
      if(_needToRefresh)
         { self.refresh() };
        
      PAINTSTRUCT struct := new PAINTSTRUCT();
        
      struct.begin(_handle);
        
      _region.readDimensionTo(ref int width, ref int height);
        
      HDC dc := struct.HDC;
        
      dc.copy(0, 0, width, height, _imageDC, 0, 0, SRCCOPY);
//        int err := system'external'KERNEL32 GetLastError.
                
      struct.end(_handle)
   }

   refresh()
   {
      if(nil == _handle)
         { ^ self };
        
      onPaint?.(self, _canvas);

      super.refresh();
      _needToRefresh := false
   }
 
   internal resize(Handle handle, int width, int height)
   {
      WindowHandle hwnd := handle;

      ifnot(nil == _image)
      {
         _image.free();
         _imageDC.free()
      };
            
      HDC dc := hwnd.HDC;
            
      _image := HBITMAP.create(dc, width, height);
      _imageDC := HDC.newCompatible(dc);
      _image.select(_imageDC);

      _canvas.open(_imageDC);

      _needToRefresh := true
   }
}

// --- Frame ---

public sealed class Frame : BaseWinControl, interface<IControl>
{
   constructor new()
      <= super new()
   {
   }
   
   internal createHandle()
   {
      WindowHandle parent := _parent;
        
      int styles := WS_CHILD | BS_GROUPBOX;
      if(_visible)
         styles := styles | WS_VISIBLE;
        
      _region.read(ref int x, ref int y, ref int width, ref int height);
        
      _handle := new WindowHandle(
                        WS_EX_TRANSPARENT,
                        ButtonClass, 
                        cast wide(_caption), 
                        styles,
                        x, y, width, height,
                        parent, 
                        CurrentInstance,
                        UnsafePointer.Default)
   }
}

// --- BasePanel ---

public abstract class BasePanel : BaseWinContainer
{
   Frame _frame;

   protected constructor new()
      <= super new()
   {
      _frame := Frame.new();

      // NOTE : a frame should be the first element
      self.appendControlInternal(_frame)
   }

   internal appendControlInternal(BaseControl control)
   {
      control.Parent := self;

      _controls.append(control)        
   }

   internal appendControlInternal(ImageList control)
   {
      control.Parent := self;
        
      _controls.append(control)        
   }
    
//    int Width
//    {
//        set(int v)
//        {
//            super.Width := v;
//            
//            theFrame.Width := v
//        }        
//    }
//    
//    int Height
//    {
//        set(int h)
//        {
//            super.Height := h;
//            
//            theFrame.Height := h
//        }
//    }
            
   setRegion(int x, int y, int width, int height)
   {
      super.setRegion(x, y, width, height);
        
      _frame.setRegion(0, 0, width, height);
   }
    
//   string Caption
//   {
//      set(string caption)
//      {
//            theFrame.Caption := caption
//      }
//        
//      set(wide caption)
//      {
//         theFrame.Caption := caption
//      }
//   }    

   set Caption(string s)
   {
      _frame.Caption := s
   }
    
   set Caption(wide s)
   {
      _frame.Caption := s
   }

   internal resize(Handle handle, int width, int height)
   {
      _frame.Width := width;
      _frame.Height := height
   }    
}

class PanelListener : WindowCallback
{
   Panel _owner;
    
   constructor(Panel owner)
   {
      _owner := owner
   }    

   onSize(Handle control, int width, int height)
   {
      _owner.resize(control, width, height);
   }
}

public sealed class Panel : BasePanel, interface<IControl>
{
   constructor new()
      <= super new()
   {
   }
    
   appendControl(BaseControl control)
   {
      self.appendControlInternal(control);
   }
   appendControl(ImageList control)
   {
      self.appendControlInternal(control);
   }

   internal createHandle()
   {
      WindowHandle parent := _parent;

      int styles := WS_CHILD | WS_CLIPSIBLINGS;
      if(_visible)
         { styles := styles | WS_VISIBLE };

      _region.read(ref int x, ref int y, ref int width, ref int height);

      pointer paramPtr := PermVectorTable.allocate(new PanelListener(self));

      _handle := new WindowHandle(
                        WS_EX_CONTROLPARENT,
                        PanelClass,
                        cast wide(_caption),
                        styles,
                        x, y, width, height,
                        parent,
                        CurrentInstance,
                        paramPtr)
                        
   }
}

// --- RadioButtonGroup ---

class RadioButtonGroupListener : WindowCallback
{
   RadioButtonGroup _owner;

   constructor(RadioButtonGroup owner)
   {
      _owner := owner
   }

   onClick(Handle control)
   {
      _owner.onClick(control);
   }

   onSize(Handle control, int width, int height)
   {
      _owner.resize(control, width, height);
   }
}

public sealed class RadioButtonGroup : BasePanel, interface<IControl>
{
   int   _selected;

   Func1 onIndexChanged : event;
    
   constructor new()
        <= super new()
   {
      _selected := -1
   }

   internal createHandle()
   {
      WindowHandle parent := _parent;

      int styles := WS_CHILD | WS_CLIPSIBLINGS;
      if(_visible)
            { styles := styles | WS_VISIBLE };
                        
      _region.read(ref int x, ref int y, ref int width, ref int height);
        
      pointer paramPtr := PermVectorTable.allocate(new RadioButtonGroupListener(self));

      _handle := new WindowHandle(
                        WS_EX_CONTROLPARENT,
                        PanelClass,
                        cast wide(_caption),
                        styles,
                        x, y, width, height,
                        parent,
                        CurrentInstance,
                        paramPtr);
                                                
      if(_selected != -1 && nil != onIndexChanged)
         { onIndexChanged(self) }
   }

   int SelectedIndex
   {
      get() = _selected;
        
      set(n)
      {
         ifnot(-1 == _selected)
            { self.setChecked(false) };
            
         _selected := n;
        
         self.setChecked(true);
        
         onIndexChanged?.(self);
      }
   }

   private setChecked(value)
   {
      int len := _controls.Length;
      IControl current;
      for (int i := 1, i < len, i += 1) {
         current := _controls[i];
         int index := i - 1;
         if (index == _selected)
         {
            current.Control.Checked := value;
                
            ^ self
         }
      };
   }

   appendItem(string s)
   {
      self.appendControlInternal(RadioButton.new(s));
   }

   setRegion(int x, int y, int width, int height)
   {
      super.setRegion(x, y, width, height);
        
      self.resize()
   }

   internal resize()
   {
      int width := _region.Width;
      int height := _region.Height;

      if (_handle != nil) {
         self.resize(_handle, width, height)
      }
      else self.resize(Handle.Default, width, height)
   }
            
   internal resize(Handle handle, int width, int height)
   {
      int len := _controls.Length;
      IControl current;
      for (int i := 1, i < len, i += 1) {
         current := _controls[i];
         int index := i - 1;
            
         current.setRegion(4, 18 + index * 20, width - 6, 20);
      };
        
      super.resize(handle, width, height)
   }

   internal onClick(Handle handle)
   {
      var retVal := nil;
      int len := _controls.Length;
      IControl current;
      for (int i := 1, i < len, i += 1) {
         current := _controls[i];            
            
         retVal := current.retrieve(handle);
         if(nil != retVal)
         {
            int index := i - 1;
            _selected := index;
                
            onIndexChanged?.(self);
                
            ^ self
         }
      }
   }
}

// --- ImageList ---

public sealed class ImageList : Object, interface<IControl>
{
   Dimension    _size;
   object      _parent;
   List<Image> _images;
   int         _index;   

   constructor new()
   {
      _index := 0;
      _images := new List<Image>();

      _size :=  new Dimension(50, 50)
   }

   setRegion(int x, int y, int width, int height)
   {
      _size.Width := width;
      _size.Height := height
   }

   get Control() = self;

   Parent = _parent;    

   set internal Parent(object o)
   {
      _parent := o
   }

   assign(Paintbox paintbox)
   {
      paintbox.onPaint := (object sender, Canvas canvas)
      {
         int width := _size.Width;
         int height := _size.Height;
            
         auto image := _images[_index];
            
         canvas.write(image, 0, 0, width, height)
      }
   }

   IControl retrieve(Handle handle)
   {
      ^ nil
   }
    
   click() {}

   open()
   {
      WindowHandle parent := _parent;
      HDC parentDC := parent.HDC;        

      int len := _images.Length;
      for (int i := 0, i < len, i += 1) {
         _images[i].open(parentDC)
      }
   }

   appendImage(string path)
   {        
      _images.append(Image.load(path, _size))
   }

   int SelectedIndex
   {
      get() = _index;
        
      set(int n)
      {
         _index := n
      }
   }

}
