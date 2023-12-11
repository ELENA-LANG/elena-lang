import extensions;

A
{
   eval() {}
}

extension AOp : A
{
   extendEval() {}
}

retValTests()
{
   auto a := new A();

   console.writeLine(a.eval());
   console.writeLine(a.extendEval());

   Assert.ifInstanceOf(a.eval(), A);
   Assert.ifInstanceOf(a.extendEval(), A);
}


public program()
{
   retValTests()

//   console.print("Enter ").readLine()/*.toReal()*/
}