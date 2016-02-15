#import system.
#import extensions.

#import system'routines.
#import system'threading.

//#class LockTest
//{
//    #method(sync) eval : aLine
//    [
//        Loop &&from:0 &to:10000 run: =
//        [
//          'program'output << aLine.
//        ].
//        
//        $self fail.
//    ]
//}

//#static Seed = integer::0.
//
//#class SyncTest
//{
//    #method eval : aLine
//    [
//          #sync Seed
//          [
//              Loop::{ &for:0 &to:10000 } run: =>
//              [
//                  'program'output << aLine.
//              ].
//
//          ].
//                    
//          $self fail.
//    ]
//}
//

//#symbol TestCase1 =
//    [
//        #var aThread1 := BaseThread
//        {
//            eval
//            [
//                'program'output << "Thread1 starting...%n".
//                
//                Loop &&from:0 &to:1000000 run:
//                [
//                    Loop &&from:0 &to:10 run:
//                    [
//                        25 * 23 / 34 * 3435 / 3543636.
//                    ].
//                ].
//            ]
//        }.
//        
//        aThread1 start.
//        
//        #var aThread2 := BaseThread
//        {
//            eval
//            [
//                'program'output << "Thread2 starting...%n".
//                
//                Loop &&from:0 &to:1000000 run:
//                [
//                    Loop &&from:0 &to:10 run:
//                    [
//                        25 * 23 / 34 * 3435 / 3543636.
//                    ].
//                ].
//
//                //'program'Output << "Enter:".
//                //'program'input >> String.
//            ]
//        }.
//        
//        aThread2 start.
//
//        aThread1 stopped'wait.
//        aThread2 stopped'wait.
//
//        'program'Output << "stopped%n".
//    ].
//
//
////#symbol TestCase2 =>
////[
////    #var aTest := LockTest.
////  
////    #var aThread1 := ThreadControl run: => (aTest eval:"Thread 1%n").
////    #var aThread2 := ThreadControl run: => (aTest eval:"Thread 2%n").
////
////    aThread1 wait &stopped.
////    aThread2 wait &stopped.   
////]. 
//
//#symbol TestCase3 =>
//[
////    #var aTest := SyncTest.
////  
////    #var aThread1 := ThreadControl run: => (aTest eval:"Thread 1%n").
////    #var aThread2 := ThreadControl run: => (aTest eval:"Thread 2%n").
////
////    aThread1 wait &stopped.
////    aThread2 wait &stopped.   
//]. 

#symbol NewThreadTest =
[
    console writeLine:"Main thread started".
    #var aThread := threadControl start &func:
        [ 
            console writeLine:"New thread started. Press any key".

            console readChar.

            console writeLine:"New thread ended".
        ].
    
    aThread join.
    console writeLine:"Main thread ended".
].

#symbol _program =
[
    system'console writeLine:"mt testing...".
    
    NewThreadTest eval.
].

#class GCXTester
{
    #field theSync.
    
    #constructor new
    [
        theSync := Object new.
    ]
    
    #method proceed1
    [
        ("thread 1 line1", "thread 1 line2", "thread 1 line3", "thread 1 line4", "thread 1 line5", "thread 1 line6") run &each:word
        [
            #lock (theSync)
            [
                console writeLine:word.
            ].
        ].
        
        ^ true.
    ]
    
    #method proceed2
    [
        ("thread 2 line1", "thread 2 line2", "thread 2 line3", "thread 2 line4", "thread 2 line5", "thread 2 line6") run &each:word
        [
            #lock (theSync)
            [
                console writeLine:word.
            ].
        ].
        
        ^ true.
    ]
    
    #method start1
    [
        #var handle := Thread new &func:
        [
            #loop $self proceed1.
        ].
        
        handle start.
    ]
    
    #method start2
    [
        #var handle := Thread new &func:
        [
            #loop $self proceed2.
        ].
        
        handle start.
    ]
}

#symbol program =
[
    GCXTester new start1 start2.
    
    console readChar.
].