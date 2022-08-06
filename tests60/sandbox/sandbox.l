import system;

singleton Tester
{
   run()
   {
       console.writeLine("Working");  
   }
}

public program() 
{
   Tester.run();
}
