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
      pointer wndPtr;
      self.assignWndPtr(wndPtr);

      WNDCLASS struct;

      struct.style := style;
      struct.hbrBackground := background;
      struct.hInstance := instance.Handle;
      struct.cbClsExtra := 0;
      struct.cbWndExtra := 0;
      struct.hIcon := 0;
      struct.hCursor := 0;
      struct.lpszMenuName := 0;
      struct.lpfnWndProc := wndPtr;      

      pointer namePtr;
      wide wideName := name;
      namePtr.assignPtr(wideName);
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

   onWMCreate(Handle hwnd, CREATESTRUCT struct)
   {
      pointer param := struct.CreateParam;

      extern USER32.SetWindowLongW(hwnd, GWL_USERDATA, param);
   }

   onWMDestoy(Handle hwnd)
   {
      extern USER32.DefWindowProcW(hwnd, WM_DESTROY, 0, 0);
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