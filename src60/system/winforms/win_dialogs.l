public singleton messageBox
{
    int open(WindowHandle parent, wide caption, wide message, int flags)
    {
        int retVal := extern USER32.MessageBoxW(parent, message, caption, flags);
            
        ^ retVal
    }
    
    int open(string caption, string message, int flags)
    {
        WindowHandle handle := cast WindowHandle(forward program.MainWindow) ?? defaultHandle;
        
        ^ messageBox.open(handle, cast wide(caption), cast wide(message), flags)
    }
}
