//// ==== ver 3.4.1 ===
//
//#import system.
//
//#subject(class:Thread)thread.
//
//// --- Thread ---
//
//#class Thread
//{
//    #field theHandle.
//    
//    #constructor new &func:anAction
//    [
//        theHandle := ThreadHandle new &func:anAction.
//    ]
//    
//    #method thread = $self.
//    
//    #method start
//    [
//        theHandle start.
//    ]
//
//    #method join
//    [
//        theHandle join.
//    ]
//            
//    #method close
//    [
//        theHandle close.
//    ]
//}
