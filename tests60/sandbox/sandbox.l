import system;

A;

singleton Tester
{
   run()
   {
//      console.writeLine("Start");

      var a := new A();

      a.fail() \\ on((e){ console.writeLine(e) });
      a.fail() \\ on((Exception e){ console.writeLine(e) });
   }
}

public program() 
{
   Tester.run();
}
