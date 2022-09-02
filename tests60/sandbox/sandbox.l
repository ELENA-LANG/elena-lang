import system;

A;

singleton Tester
{
   run()
   {
      var a := new A();
      try
      {
        a.fail()
      } 
      catch(Exception e) 
      {
         console.writeLine(e)
      }


      //a.fail() \\ on((e){ console.writeLine(e) });
      //a.fail() \\ on((Exception e){ console.writeLine(e) });
   }
}

public program() 
{
   Tester.run();
}
