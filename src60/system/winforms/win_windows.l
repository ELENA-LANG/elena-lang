import system;

public sealed struct WNDCLASS
{
   int       style : prop;
   pointer   lpfnWndProc : prop;
   int       cbClsExtra : prop;
   int       cbWndExtra : prop;
   handle    hInstance : prop;
   int       hIcon : prop;
   int       hCursor : prop;
   int       hbrBackground : prop;
   int       lpszMenuName : prop;
   pointer   lpszClassName : prop;
}

// --- windowClassControl ---

singleton windowClassControl
{
   private assignWndPtr(pointer wndPtr) : external(system'win_core_routines'__setWndPtr);

   wide createClass(string name, HInstance instance, int style, int background)
   {
      pointer wndPtr := default;
      self.assignWndPtr(wndPtr);

      WNDCLASS struct := default;

      struct.style := style;
      struct.hbrBackground := background;
      struct.hInstance := instance.Handle;
      struct.cbClsExtra := 0;
      struct.cbWndExtra := 0;
      struct.hIcon := 0;
      struct.hCursor := 0;
      struct.lpszMenuName := 0;
      struct.lpfnWndProc := wndPtr;      

      wide wideName := name;
      pointer namePtr := wideName;
      struct.lpszClassName := namePtr;
        
      extern {
         extern USER32.RegisterClassW(struct);        
      };      

      ^ wideName
   }
}

// --- WindowCallback ---
public closed class WindowCallback
{
   onClick(Handle control)
   {
   }

   onSize(Handle control, int width, int height)
   {
   }

   onWMCreate(Handle hwnd, CREATESTRUCT struct)
   {
      pointer param := struct.CreateParam;

      extern USER32.SetWindowLongW(hwnd, GWL_USERDATA, param);
   }

   onWMDestoy(Handle hwnd)
   {
      extern USER32.DefWindowProcW(hwnd, WM_DESTROY, 0, 0);
   }

   onWMPaint(Handle hwnd)
   {
      extern USER32.DefWindowProcW(hwnd, WM_PAINT, 0, 0);
   }

   onWMSize(Handle hwnd, int lParam)
   {
      int width := lParam & 0FFFFh;
      int height := lParam $shr 16;

      self.onSize(hwnd, width, height);
   }

   onWMCommand(Handle hwnd, int wParam, Handle control)
   {
      if (control == 0)
         { ^ self };

      int command := wParam $shr 16;

      if (command == BN_CLICKED)
         { self.onClick(control); ^ self };      
   }
}

// --- SDIWindowCallback ---
public class SDIWindowCallback : WindowCallback
{
   onWMDestoy(Handle hwnd)
   {
      super.onWMDestoy(hwnd);

      forward program.stop()
   }
}

// --- SDIWindowClass ---

public static wide SDIWindowClass = windowClassControl.createClass("ELENA.SDIWINDOW.6.0", CurrentInstance, WS_ELENAWINDOW, COLOR_BACKGROUND);
public static wide PaintboxClass  = windowClassControl.createClass("ELENA.Paintbox.6.0", CurrentInstance, CS_VREDRAW | CS_HREDRAW, 0);
public static wide PanelClass     = windowClassControl.createClass("ELENA.PANEL.6.0", CurrentInstance, CS_VREDRAW | CS_HREDRAW, COLOR_BACKGROUND);