A
{
   constructor create(arg)
   {
      console.writeLine(arg)  
   }
}

B : A
{
   constructor create()
     <= super create("test");
}

//A;

public program()
{
   var b := B.create();
   console.writeLine(b)

//   try {
//      var a := new A();
//      a.fail()  
//   }
//   catch(MethodNotFoundException err) {
//     console.writeLine(err)
//   }
}
