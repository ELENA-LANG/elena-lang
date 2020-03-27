// --- $windowClassControl ---

singleton windowClassControl
{
    internal fillWndClass(byte[] array, wide name, int style, HInstance instance, int background)
        => system'core_routines'__hwnd;
    
    wide createClass(string name, HInstance instance, int style, int background)
    {
        byte struct[40];
        wide wideName := name;
        
        self.fillWndClass(struct, wideName, style, instance, background);
        
        extern USER32.RegisterClassW(struct);        
        
        ^ wideName
    }
}

// --- WindowCallback ---

public closed class WindowCallback
{
    onpaint(WindowHandle hwnd, ref int retVal)
    {
        int result := extern USER32.DefWindowProcW(hwnd, WM_PAINT, 0, 0);
        
        retVal := result;
    }
    
    onclick(WindowHandle hwnd, ref int retVal)
    {
        retVal := 0
    }
    
//    stacksafe onchange hwnd:aControl vint:aResult
//    [
//        aResult int := 0.
//    ]
//    
//    stacksafe onmenu int:aCode vint:aResult
//    [
//        aResult int := 0.
//    ]
    
    onsize(WindowHandle hwnd, int width, int height, ref int result)
    {
        result := 0
    }
    
    ondestroy(WindowHandle hwnd, ref int result)
    {
        int index := extern USER32.SetWindowLongW(hwnd, GWL_USERDATA, 0);
        if (index != 0)
        {
            rt_ptr ptr := index;
            
            ptr.free()
        };

        result := 0
    }
        
    wm_create(WindowHandle hwnd, int message, int wParam, CREATESTRUCT struct, ref int retVal)
    {
        int param := struct.CreateParam;
        
        extern USER32.SetWindowLongW(hwnd, GWL_USERDATA, param);
        
        extern USER32.DefWindowProcW(hwnd, message, wParam, struct);
        
        retVal := 0
    }
    
    eval(WindowHandle hwnd, int message, ref int retVal)
    {
        if (message == WM_DESTROY)
        {
            self.ondestroy(hwnd, ref retVal);                            
            
            ^ self
        };                
        
        int result := extern USER32.DefWindowProcW(hwnd, message, 0, 0);
         
        retVal := result;
    }
    
    eval(WindowHandle hwnd, int message, int wParam, int lParam, ref int retVal)
    {
        if (message == WM_SIZE)
        {
            int width := lParam && 0FFFFh;
            int height := lParam $shr 16;
    
            self.onsize(hwnd, width, height, ref retVal);
            
            ^ self
        };
                
        int result := extern USER32.DefWindowProcW(hwnd, message, wParam, lParam);
        
        retVal := result
    }
    
    wm_paint(WindowHandle hwnd, int message, ref int retVal)
    {
        self.onpaint(hwnd, ref retVal)
    }
    
    wm_command(WindowHandle hwnd, int message, int wParam, WindowHandle control, ref int retVal)
    {
//        if (0 == aControl)
//            [ $self onmenu int:wParam vint:aRetVal. ^ $self ].

        int command := wParam $shr 16;

        if (command == BN_CLICKED)
            { self.onclick(control, ref retVal); ^ self };

//        if (aCommand == EN_CHANGE)
//            [ $self onchange hwnd:aControl vint:aRetVal. ^ $self ].
//
//        if (aCommand == CBN_SELCHANGE)
//            [ $self onchange hwnd:aControl vint:aRetVal. ^ $self ]
    }

    eval(WindowHandle hwnd, int message, WindowHandle control, int lParam, ref int retVal)
    {
        if (message == WM_SETCURSOR)
        {
            int code := lParam && 0FFFFh;
            
            if (code == HTCLIENT)
            {
                int h := extern USER32.LoadCursorW(0, IDC_ARROW);
                extern USER32.SetCursor(h);
                
                retVal := -1;
                
                ^ self
            }
        };
        
        int result := extern USER32.DefWindowProcW(hwnd, message, control, lParam);
        
        retVal := result
    }

//    eval handle:anHWND int:aMessage int:aWParam int:aLParam vint:aRetVal
//    [
//        int result := system'external'USER32 DefWindowProcW handle:anHWND int:aMessage int:aWParam int:aLParam.
//        
//        aRetVal int := result
//    ]
}

// --- SDIWindowCallback ---

public class SDIWindowCallback : WindowCallback
{
    eval(WindowHandle hwnd, int message, ref int retVal)
    {
        if (message == WM_CLOSE)
        {
            hwnd.free();
            
            retVal := 0;
            
            ^ self
        };
        
        if (message == WM_DESTROY)
        {
            // NOTE: SDIWindow is the main program window, so closing it means exiting the program
            forward program.stop();
            
            self.ondestroy(hwnd, ref retVal);
            
            ^ self
        };
        
//        $super eval hwnd:anHWND int:aMessage vint:aRetVal.
    }

////    #method eval &handle:anHWND &int:aMessage &int:aWParam &int:aLParam &vint:aRetVal
////    [
////        $super eval &handle:anHWND &int:aMessage &int:aWParam &int:aLParam &vint:aRetVal.
////    ]
}

//// --- ChildWindowCallback ---
//
//class ChildWindowCallback :: WindowCallback
//{
//    eval hwnd:anHWND int:aMessage vint:aRetVal
//    [
//        if (aMessage == WM_CLOSE)
//            [
//                anHWND free.
//                
//                aRetVal int := 0.
//                
//                ^ $self
//            ].
//        
//        if (aMessage == WM_DESTROY)
//            [
//                $self ondestroy hwnd:anHWND vint:aRetVal.
//                
//                ^ $self
//            ].
//        
//        $super eval hwnd:anHWND int:aMessage vint:aRetVal.
//    ]
//
////    #method eval &handle:anHWND &int:aMessage &int:aWParam &int:aLParam &vint:aRetVal
////    [
////        $super eval &handle:anHWND &int:aMessage &int:aWParam &int:aLParam &vint:aRetVal.
////    ]
//}

// --- SDIWindowClass ---

public static wide SDIWindowClass = windowClassControl.createClass("ELENA.SDIWINDOW.4.0", CurrentInstance, WS_ELENAWINDOW, COLOR_BACKGROUND);
public static wide PaintboxClass  = windowClassControl.createClass("ELENA.Paintbox.4.0", CurrentInstance, CS_VREDRAW || CS_HREDRAW, 0);
public static wide PanelClass     = windowClassControl.createClass("ELENA.PANEL.4.0", CurrentInstance, CS_VREDRAW || CS_HREDRAW, COLOR_BACKGROUND);
