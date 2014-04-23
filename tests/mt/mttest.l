#define system.
#define system'threading.

////#class LockTest
////{
////    #method(sync) eval : aLine
////    [
////        Loop &&from:0 &to:10000 run: =
////        [
////          'program'output << aLine.
////        ].
////        
////        $self fail.
////    ]
////}
//
////#static Seed = integer::0.
////
////#class SyncTest
////{
////    #method eval : aLine
////    [
////          #sync Seed
////          [
////              Loop::{ &for:0 &to:10000 } run: =>
////              [
////                  'program'output << aLine.
////              ].
////
////          ].
////                    
////          $self fail.
////    ]
////}
////
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
    #var aThread := threadControl start: 
        [ 
            console writeLine:"New thread started" 
        ].
    
    aThread join.
].

#symbol program =
[
    system'console writeLine:"mt testing...".
    
    NewThreadTest eval.
].