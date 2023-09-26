import system;

public sealed struct WNDCLASS
{
   int       style : prop;
   pointer   lpfnWndProc : prop;
   int       cbClsExtra : prop;
   int       cbWndExtra : prop;
   HInstance hInstance : prop;
   Handle    hIcon : prop;
   Handle    hCursor : prop;
   Handle    hbrBackground : prop;
   pointer   lpszMenuName : prop;
   pointer   lpszClassName : prop;
}

// --- windowClassControl ---

singleton windowClassControl
{
   wide createClass(string name, HInstance instance, int style, int background)
   {
      WNDCLASS struct;

      wide wideName := name;

//      self.fillWndClass(struct, wideName, style, instance, background);
//        
//        extern USER32.RegisterClassW(struct);        

        ^ wideName
   }
}

// --- SDIWindowClass ---

public static wide SDIWindowClass = windowClassControl.createClass("ELENA.SDIWINDOW.6.0", CurrentInstance, WS_ELENAWINDOW, COLOR_BACKGROUND);
