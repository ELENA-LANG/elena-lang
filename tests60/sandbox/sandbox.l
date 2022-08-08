import system;

singleton Tester
{
   run()
   {
       console.writeLine("Failing");
       var o := nil;
       o.fail();
   }
}

public program() 
{
   Tester.run();
}
