import system;

A;

singleton Tester
{
   run()
   {
      var a := new A();

      a.fail()
   }
}

public program() 
{
   Tester.run();
}
