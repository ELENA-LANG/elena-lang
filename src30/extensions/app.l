//#define system.
//
//// --- ProgramLoop ---
//
//#class $ProgramLoop
//{
//    #field(type:bool) theActive.
//    #field theControl.
//    
//    #constructor new : aControl
//    [
//        theActive := true.
//        theControl := aControl.
//    ]
//    
//    #method stop [ theActive := false. ]
//    
//    #method eval
//    [
//        control while:[ theActive ] &do:
//        [ 
//            'program'output write:(theControl prompt:'program'output read:'program'input). 
//        ].
//    ]
//}
//
//#static programLoop = $ProgramLoop new:'program'control.
