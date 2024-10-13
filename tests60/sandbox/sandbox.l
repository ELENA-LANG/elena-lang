import extensions;

function variadicSum(params object[] args)
{ 
   var sum := 0;
   for (int i := 0; i < args.Length; i++) {
      sum := sum + args[i]
   };

   ^ sum
}

function sumAndPrint(params object[] args)
{ 
   // passing a variadic argument further    
   Console.printLine("The result is ", variadicSum(params args));
}

public program()
{
   sumAndPrint(1,2.3,4);
}