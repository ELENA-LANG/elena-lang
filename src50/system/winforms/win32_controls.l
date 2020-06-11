import system'drawing;

// --- classes ---
public const wide StaticClass = "Static"w;
public const wide EditClass   = "Edit"w;
public const wide ButtonClass = "Button"w;
public const wide ComboboxClass = "Combobox"w;

// --- WindowHandle ---

public sealed const struct WindowHandle
{
    embeddable __raw theHandle[4];
    
    constructor()
    {
        theHandle := 0
    }

    get constructor Default()
    {
    }

    constructor(int dwExStyle, wide className, wide windowName, int style, int x, int y, int width, int height, WindowHandle parent, HInstance instance, object param)
    {
        int paramIndex := 0;
        if (nil != param)
        {
            rt_ptr ptr := RTPointer.assign(param);
    
            paramIndex := ptr.Index
        };
                
        int h := 0;                
        extern
        {
            h := extern USER32.CreateWindowExW(
                dwExStyle,
                className,
                windowName,  
                style,
                x,
                y,
                width,
                height,
                parent,
                0,
                instance,
                paramIndex)
        };

//        int err := system'external'KERNEL32 GetLastError.

        if (h==0)
            { WinFormsException.raise() };

        theHandle := h
    }

    setVisible(int n)
    {
        extern USER32.ShowWindow(theHandle, n)
    }
    
    bool isVisible()
    {
        int visible := extern USER32.IsWindowVisible(theHandle);
        
        ^ (visible != 0)
    }
    
    setEnabled(int value)
    {
        extern USER32.EnableWindow(theHandle, value)
    }            
    
    bool isEnabled()
    {
        int visible := extern USER32.IsWindowEnabled(theHandle);
        
        ^ (visible != 0)
    }

    setChecked(int n)
    {
        extern USER32.SendMessageW(theHandle, BM_SETCHECK, n, 0)
    }

    bool isChecked()
    {
        int r := extern USER32.SendMessageW(theHandle, BM_GETCHECK, 0, 0);

		^ (r == BST_CHECKED)
    }    
            
    setCaption(wide s)
    {
        extern USER32.SendMessageW(theHandle, WM_SETTEXT, 0, s)
    }
    
    wide getCaption()
    {
        int length := extern USER32.SendMessageW(theHandle, WM_GETTEXTLENGTH, 0, 0);
        
        if (length == 0)
        { 
            ^ emptyWideString 
        }
        else
        {
            wide caption := WideString.allocate(length);
            length := length + 1;
            
            extern USER32.SendMessageW(theHandle, WM_GETTEXT, length, caption);
            
            ^ caption
        }
    }

    sendMessage(int message, int wParam, wide lParam)
    {
        extern USER32.SendMessageW(theHandle, message, wParam, lParam)
    }
            
    refresh()
    {
        WindowHandle copy := theHandle;
        
        extern
        {
            extern USER32.InvalidateRect(copy, 0, 1);
            extern USER32.UpdateWindow(copy)
        }
    }
    
    setPosition(int x, int y, int cx, int cy, int flags)
    {
        extern USER32.SetWindowPos(theHandle, 0, x, y, cx, cy, flags)
    }

    getClientRect(RECT struct)
    {
        extern USER32.GetClientRect(theHandle, struct)
    }
  
    get HDC HDC()
    {
        HDC dc := extern USER32.GetDC(theHandle);

        ^ dc
    }

    setMenu(HMENU menuHandle)
    {
        extern USER32.SetMenu(theHandle, menuHandle)
    }
            
    free()
    {
        extern USER32.DestroyWindow(theHandle)
    }
}

// --- PAINTSTRUCT ---

public sealed struct PAINTSTRUCT
{
    HDC  theHDC;
    int  theErase;
    RECT theRect;
    int  theRestore;
    int  theIncUpdate;
    
    int  dummy0;
    int  dummy1;
    int  dummy2;
    int  dummy3;
    int  dummy4;
    int  dummy5;
    int  dummy6;
    int  dummy7;

    embeddable constructor()
    {
    }

    get HDC HDC()
    {
        ^ theHDC
    }
            
    begin(WindowHandle handle)
    {
        extern USER32.BeginPaint(handle, self)
    }
    
    end(WindowHandle handle)
    {
        extern USER32.EndPaint(handle, self)
    }
}
