import extensions;

class Test
{
   static printFunctionResult(Func f)
   {
      Console.printLine(f," prints ", f()) 
   }

//   static printStrongFunctionResult(Func<string> f)
//   {
//      Console.printLine(f," prints ", f()) 
//   }
}

public program()
{
   Test.printFunctionResult(([] => "Hello from Func"))
//   Test.printFunctionResult::([] => "Hello from Func")
}
