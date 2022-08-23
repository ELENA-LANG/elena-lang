import system;

A;

singleton Tester
{
   run()
   {
      var a := new A();

      a.fail() \\ on((e){ console.writeLine(e) })
   }
}

public program() 
{
   Tester.run();
}
