import system;

//A;

A
{
   feedback(prober,arg)
   {
      console.writeLine("A.default")
   }
}

Prober
{
   function(string s)
   {
      console.writeLine(s)
   }
}


singleton Tester
{
   run()
   {
      console.writeLine("Start")

//      var a := new A();
      var prober := new Prober();

      __probe prober("string", mssgconst feedback[2]);
//      __try prober(a, mssgconst feedback[2]);

//      a.fail() \\ on((e){ console.writeLine(e) })
   }
}

public program() 
{
   Tester.run();
}
