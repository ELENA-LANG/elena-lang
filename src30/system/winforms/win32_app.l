//// ==== ver 3.4.1 ===
//
//#import system.
//
//// --- BaseApplication ---
//
//#class BaseApplication
//{
//}
//
//// --- SDIApplication ---
//
//#class SDIApplication :: BaseApplication
//{
//    #field            theMainWindow.
//    #field(type:hwnd) theHandle.
//    
//    #constructor new
//    [
//        RTPointer new:$self.
//        
//        theMainWindow := 'mainWindow new.
//    ]
//    
//    #method mainWindow = theMainWindow.
//    
//    #method start
//    [
//        theMainWindow open.
//
//        #var(type:win_msg)msg.
//        #var(type:int)proceeded.
//        #var(type:int)result.
//        #var(type:int)handle := theMainWindow hwnd.
//        
//        #extern
//        [
//            result := system'external'USER32 GetMessageW &win_msg:msg &int:handle &int:0 &int:0.
//            #loop (result != 0)?
//            [
//                proceeded := system'external'USER32 IsDialogMessageW &int:handle &win_msg:msg.
//                (proceeded == 0) ?
//                [
//                    system'external'USER32 TranslateMessage &win_msg:msg.
//                    system'external'USER32 DispatchMessageW &win_msg:msg.
//                ].
//                result := system'external'USER32 GetMessageW &win_msg:msg &int:handle &int:0 &int:0.
//            ].
//            
//        ].
//    ]
//
//    #method run &hwnd:handle
//    [
//        #var(type:win_msg)msg.
//        #var(type:int)proceeded.
//        #var(type:int)result.
//        #var(type:hwnd)handleCopy := handle.
//
//        #extern
//        [
//            #loop (handleCopy isVisible)?
//            [
//                result := system'external'USER32 PeekMessageW &win_msg:msg &int:0 &int:0 &int:0 &int:1.
//                (result != 0) ?
//                [
//                    proceeded := system'external'USER32 IsDialogMessageW &int:handleCopy &win_msg:msg.
//                    (proceeded == 0) ?
//                        [
//                            system'external'USER32 TranslateMessage &win_msg:msg.
//                            system'external'USER32 DispatchMessageW &win_msg:msg.
//                        ].
//                ]
//                ! [
//                    system'external'USER32 WaitMessage.
//                ].
//            ].
//        ].
//    ]
//            
//    #method stop
//    [
//        system'external'USER32 PostQuitMessage &int:0.
//    ]
//}
//
//#static program = SDIApplication new.
//
//#class $GUIStartUp :: StartUp
//{
//    #method start
//    [
//        'program start
//            | on:
//            {
//                Abort = e [].
//                
//                ! = e 
//                    [ 
//                        messageBox open &caption:"ELENA" &value:(e message) &int:MB_ICONEXCLAMATION.
//                    ].
//            }.
//            
//        $self $onStop:nil.
//    ]
//}
//
//#static startUp = $GUIStartUp new.