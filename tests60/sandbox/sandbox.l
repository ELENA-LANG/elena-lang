//import system'io;
//import system'threading;
//import system'collections;
//import system'collections'threadsafe;
//import system'routines;
//import extensions;
//import extensions'routines;
//
////singleton X
////{
////   Task someMethodAsync1()
////   {
////      ^ new Task(/*console.writeLine("someMethodAsync1"); Thread.delay(1000); }*/);
////   }
////
//////   Task someMethodAsync2()
//////   {
//////      ^ new Task({ console.writeLine("someMethodAsync2"); Thread.delay(3000); });
//////     
//////   }
////
////   async Task test()
////   {
////      console.writeLine("Before");
////
////      $await Task.Run( { console.writeLine("someMethodAsync1"); Thread.delay(1000); });
////
////      console.writeLine("After");
////   }
////}
////
////public program()
////{
////   Task t := X.test();
////
////   t.Result;
////
////   console.readChar()
////}
//
//public program()
//{
////   auto list := new ConcurrentQueue<string>();
////   auto dirNames := new string[]{ ".", ".." };
////   auto tasks := new List<Task>();
//
//   Task t := Task.Run( { foreach(auto path; in Directory.getFiles(".")) 
//                                    console.writeLine(path); Thread.sleep(4000) } );
//
//
////   Task t := Task.Run( { foreach(auto path; in Directory.getFiles(".")) 
////                                    list.push(path); } );
//
//   t.wait();
//
////   foreach(string dirName; in dirNames) {
////      Task t := Task.Run( { foreach(auto path; in Directory.getFiles(dirName)) 
////                                    list.push(path); } );
////      tasks.append(t);
////   };
////
////   Task.waitAll(tasks.Value);
//
////   foreach (Task t in tasks)
////      Console.WriteLine("Task {0} Status: {1}", t.Id, t.Status);
//
////   list.Snapshot.toArray().forEach(printingLn);
//
////   Console.WriteLine("Number of files read: {0}", list.Count);
//}

//public sealed class Task
//{
//   Exception      _exception;
//
//   setResult()
//      <= complete(nil);
//
//   private complete(Exception? ex)
//   {
//   }
//
//   setException(Exception ex)
//      <= complete(ex);
//}

A
{
   private setMe(int n)
   {
   } 

   set()
   {
      var n := 2; 
      setMe(n) 
   }
}

public program()
{
   A a := new A();
}
