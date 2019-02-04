//// --- BaseApplication ---
//
//class BaseApplication
//{
//}

// --- SDIApplication ---

public class SDIApplication //:: BaseApplication
{
    object theMainWindow;
    
    constructor()
    {
        RTPointer.assign:self;
        
        theMainWindow := forward MainWindow.new();
    }
    
//    mainWindow = theMainWindow.
    
    closure()
    {
        theMainWindow.open();

        MSG          msg := new MSG();
        int          proceeded := 0;
        int          result := 0;
        WindowHandle handleCopy := cast WindowHandle(theMainWindow);
        
        extern
        {
            result := extern USER32.GetMessageW(msg, 0, 0, 0);
            while (result != 0)
            {
                proceeded := extern USER32.IsDialogMessageW(handleCopy, msg);
                if (proceeded == 0)
                {
                    extern USER32.TranslateMessage(msg);
                    extern USER32.DispatchMessageW(msg)
                };
                result := extern USER32.GetMessageW(msg, 0, 0, 0)
            }            
        }
    }

//    run hwnd:handle
//    [
//        win_msg msg := MSG new.
//        int     proceeded := 0.
//        int     result := 0.
//        hwnd    handleCopy := handle.
//
//        extern()
//        [
//            while (handleCopy isVisible)
//            [
//                result := system'external'USER32 PeekMessageW win_msg:msg int:0 int:0 int:0 int:1.
//                if (result != 0)
//                [
//                    proceeded := system'external'USER32 IsDialogMessageW int:handleCopy win_msg:msg.
//                    if(proceeded == 0)
//                        [
//                            system'external'USER32 TranslateMessage win_msg:msg.
//                            system'external'USER32 DispatchMessageW win_msg:msg.
//                        ]
//                ];
//                [
//                    system'external'USER32 WaitMessage
//                ].
//            ].
//        ].
//    ]
//            
//    stop
//    [
//        system'external'USER32 PostQuitMessage int:0.
//    ]
}

//// --- SDIApplication ---
//
//class SDIXApplication :: BaseApplication
//{
//    object       theMainWindow.
//    
//    constructor new
//    [
//        RTPointer new:$self.
//        
//        theMainWindow := 'MainWindow new.
//    ]
//    
//    mainWindow = theMainWindow.
//    
//    eval
//    [
//        theMainWindow visible := true.
//        
//        win_msg msg := MSG new.
//        int     proceeded := 0.
//        int     result := 0.
//    //    hwnd    handleCopy := handle.
//    
//        extern()
//        [
//            int message := msg message.
//            while (message != WM_QUIT)
//            [
//                result := system'external'USER32 PeekMessageW win_msg:msg int:0 int:0 int:0 int:1.
//                if (result != 0)
//                [
//                    message := msg message.
//                    
//    //                proceeded := system'external'USER32 IsDialogMessageW int:handleCopy win_msg:msg.
//                    if(proceeded == 0)
//                        [
//                            system'external'USER32 TranslateMessage win_msg:msg.
//                            system'external'USER32 DispatchMessageW win_msg:msg.
//                        ]
//                ];
//                [
//                    system'external'USER32 WaitMessage
//                ].
//            ].
//        ].    
//    ]
//            
//    stop
//    [
//        system'external'USER32 PostQuitMessage int:0.
//    ]
//}

public static program = new SDIApplication();
//static xprogram = SDIXApplication new.

public startUp = 
    {
        start(/*events*/)
        {
            intern system'core_routines'__exception.register();
            
            try
            {
                forward program()
            }
            catch(Exception e)
            {
                var l := e.Message
                
                //messageBox open literal:"ELENA" literal(e message) int:MB_ICONEXCLAMATION.
            }
            catch(AbortException e)
            {
            };
    
            //events $onStop:$nil.
        }        
    }
    .start(/*startUpEvents*/);