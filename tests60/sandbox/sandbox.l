import system'threading;
//
//singleton X
//{
////   Task someMethodAsync1()
////   {
////      ^ new Task(/*console.writeLine("someMethodAsync1"); Thread.delay(1000); }*/);
////   }
//
////   Task someMethodAsync2()
////   {
////      ^ new Task({ console.writeLine("someMethodAsync2"); Thread.delay(3000); });
////     
////   }
//
//   async Task test()
//   {
//      console.writeLine("Before");
//
//      $await Task.Run( { console.writeLine("someMethodAsync1"); Thread.delay(1000); });
//
//      console.writeLine("After");
//   }
//}
//
//public program()
//{
//   Task t := X.test();
//
//   t.Result;
//
//   console.readChar()
//}

//public abstract class AsyncStateEnumerator
//{
//   static Task proceed(Enumerator<Task> e)
//   {
//      Task t := /*new Task()*/nil;
//
//      Action<Task> proceeding := (Task tsk)
//      {
////         try
////         {
////             if (e.next()) {
//                (*e).continueWith(this self);
//
////                ^ this self;
////             }
////         }
////         catch (Exception e)
////         {
////            t.setException(e);
////         };
////
////         t.setResult(); 
//      };
//
//      proceeding(t);
//
//      ^ t;  
//   }
//}

A { int cast() = 2; }

public program()
{
//   var t := (int n)
//   {
//      $break;
//
//      console.writeLine("ERR")
//   };
//
//
//   t(2)

//   Action<int> t := (int n)
//   {
//      ^ this self;
//   };


   Func<int,int> f := (n => n + 1); // !! must be strong typed

   var o := f;
   var r := o(new A());
}
