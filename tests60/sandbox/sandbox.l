import system;

A
{
    add(int n) {}
}

//A;

singleton Tester
{
   run()
   {
       var inc := (x => x + 1 );

       console.writeLine(inc(2));

      //var a := new A();

      //a.fail() /*\\ on()*/
   }
}

public program() 
{
   Tester.run();
}
