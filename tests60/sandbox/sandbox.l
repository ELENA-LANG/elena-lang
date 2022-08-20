import system;

A;

singleton Tester
{
   run()
   {
      var a := new A();

      a \\ on((e){ console.writeLine(e) })

      //a.fail() \\ on((e){ console.writeLine(e) })
   }
}

public program() 
{
   Tester.run();
}
