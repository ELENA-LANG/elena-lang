import extensions.

import system'routines.
import system'threading.

//class GCXTester
//{
//    object theSync.
//    
//    constructor new
//    [
//        theSync := Object new.
//    ]
//    
//    proceed1
//    [
//        ("thread 1 line1", "thread 1 line2", "thread 1 line3", "thread 1 line4", "thread 1 line5", "thread 1 line6") forEach(:word)
//        [
//            lock (theSync)
//            [
//                console writeLine:word.
//            ].
//        ].
//        
//        ^ true.
//    ]
//    
//    proceed2
//    [
//        ("thread 2 line1", "thread 2 line2", "thread 2 line3", "thread 2 line4", "thread 2 line5", "thread 2 line6") forEach(:word)
//        [
//            lock (theSync)
//            [
//                console writeLine:word.
//            ].
//        ].
//        
//        ^ true.
//    ]
//    
//    start1
//    [
//        var handle := Thread new:
//        [
//            while (true)
//            [
//                self proceed1.
//            ].
//        ].
//        
//        handle start.
//    ]
//
//    start2
//    [
//        var handle := Thread new:
//        [
//            while (true)
//            [
//                self proceed2.
//            ].
//        ].
//        
//        handle start.
//    ]
//}

public program
[
    var handle := Thread new:
    [
        console writeLine:"Test".
    ].
    
    handle start.
    
//    GCXTester new; start1; start2.

    console readChar.
]