A
{
   constructor create(arg)
   {
      console.writeLine(arg)  
   }
}

B : A
{
   printMe()
   {
      console.writeLine("it's me") 
   }

   constructor create()
     <= super create("test");
}

//A;

public program()
{
   var b := B.create();
   var a := A.create("A");
   //console.writeLine(b);
   console.writeLine(b.__getClassName());
   console.writeLine(a.__getClassName());

//   try {
//      var a := new A();
//      a.fail()  
//   }
//   catch(MethodNotFoundException err) {
//     console.writeLine(err)
//   }
}
