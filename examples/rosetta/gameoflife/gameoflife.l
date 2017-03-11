#import cellular.

program =
[
    var model := Model new.

    model onUpdate &listener_func:
        (&space:sp) [ sp print ].

    model eval.

//    console clear.        
//    s print.
    
////    #var aSpace := Space new:(Cellular new &x:3 &y:3, Cellular new &x:5 &y:22, Cellular new &x:5 &y:13, Cellular new &x:13 &y:13) &scale:10.
].