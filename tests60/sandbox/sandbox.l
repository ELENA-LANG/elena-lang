A
{
   test()
   {
     console.writeLine("A.test"); 
   }
}

B : A
{
   test()
   {
      super.test();

      console.writeLine("B.test"); 
   }
}

//A;

public program()
{
   var b := new B();
   b.test()

//   try {
//      var a := new A();
//      a.fail()  
//   }
//   catch(MethodNotFoundException err) {
//     console.writeLine(err)
//   }
}
