import system;

//A;

A
{
   feedback(prober)
   {
      console.writeLine("A.default")
   }
}

//Prober
//{
//   function(string s)
//   {
//      console.writeLine(s)
//   }
//}

singleton Tester
{
   run()
   {
//      console.writeLine("Start");

      var a := new A();
//      var prober := new Prober();

//      __probe prober("string", mssgconst feedback[2]);
//      __probe prober(a, mssgconst feedback[1]);

//      a.fail() \\ on((e){ console.writeLine(e) })
      a.fail() \\ on((Exception e){ console.writeLine(e) })
   }
}

public program() 
{
   Tester.run();
}
