//import extensions;
//import system'threading;
//
//import system'runtime;
//
//myFunction()
//{
//   console.writeLine("Hello from the thread");
//}

//public program()
//{
//   auto myThread := Thread.assign(&myFunction);
//
//   var priority := myThread.Priority;
//
//   console.printLine("Thread priority ", priority);
//
//   myThread.start();
//
//   console.readChar()
//}

//myFunction(arg)
//{
//   console.writeLine($"{arg} from the thread");
//}
//
//public program()
//{
//   auto myThread := Thread.assign(&myFunction);
//
//   var priority := myThread.Priority;
//
//   console.printLine("Thread priority ", priority);
//
//   myThread.start("Good bay");
//
//   console.readChar()
//}

//public program()
//{
//   object f := myFunction;
//
//   pointer actionPtr := PermVectorTable.allocate(f);
//}

singleton Helper
{
   test(object arg)
   {
   }
}

public program()
{
   object arg := 2;

   Helper.test(arg)
}