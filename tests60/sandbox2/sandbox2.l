import system'threading;
import extensions;
import extensions'threading;

static sync = new object();

public Program()
{
   threadControl.start(
   {
      try {
        int n := 1;
        while (true) {
           lock(sync) {
              Console.writeLine("Line from thread 1 : " + n.toPrintable());
              n++;
           };
           if (n == 50000)
             InvalidArgumentException.raise()
        }
      }
      catch(Exception ex)
      {
         Console.writeLine(ex);
      }
   });

   threadControl.start(
   {
      int n := 1000;
      while (true) {
         lock(sync) {
            Console.writeLine("Line from thread 2 : " + n.toPrintable());
            n++;
         }         
      }
   });

   threadControl.start(
   {
      int n := 3000;
      while (true) {
         lock(sync) {
            Console.writeLine("Line from thread 3 : " + n.toPrintable());
            n++;
         }         
      }
   });

   threadControl.start(
   {
      int n := 4000;
      while (true) {
         lock(sync) {
            Console.writeLine("Line from thread 4 : " + n.toPrintable());
            n++;
         }         
      }
   });

   Console.readChar()
}

/*public program()
{
   var list := new BlockingQueue<string>();

   Thread.start(
   {
      var s := list.pop();

      Console.printLine("Thread says ",s);
   });

   Console.printLineConcurrent("Send a message to a thread");
   var msg := Console.readLine();

   list.push(msg);

   Console.printLineConcurrent("Press any key");
   Console.readChar();
}*/
