import extensions;

A
{
   constructor()
   {
      console.writeLine("new A()");
   }

   do()
   {
      console.writeLine("A.do()");
   }

   invoke()
   {
      self.do();
   }
}

public program()
{
   var a := $lazy new A();

   console.writeLine("a.invoke()");
   a.invoke();

//   var expr := $lazy console.write("Reading a number:").readLine().toInt();


//    // the expression is invoked only when it is used
//    var sum := expr + expr;
    
//    console.printLine("the result is ", sum);
}
