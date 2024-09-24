import system'threading;

singleton X
{
//   Task someMethodAsync1()
//   {
//      ^ new Task(/*console.writeLine("someMethodAsync1"); Thread.delay(1000); }*/);
//   }

//   Task someMethodAsync2()
//   {
//      ^ new Task({ console.writeLine("someMethodAsync2"); Thread.delay(3000); });
//     
//   }

   async Task test()
   {
      console.writeLine("Before");

      $await Task.Run( { console.writeLine("someMethodAsync1"); Thread.sleep(3000); });

      console.writeLine("After");
   }
}

public program()
{
   Task t := X.test();

   console.readChar();

   console.writeLine("Press any key..");
   t.wait();

   console.readChar();
}
