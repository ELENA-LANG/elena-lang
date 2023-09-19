import extensions;

//class Base
//{
//    // declaring accumulator attribute
//    const object[] theFamily;
//    
//    // adding a reference to the class
//    this theFamily += Base;
//    
//    // declaring inheritable static method (it will be accessible in all derived classes)
//    sealed static printFamily()
//    {
//        console.printLine(theFamily.asEnumerable())
//    }
//}
//
//class A : Base
//{
//    // adding a reference to the derived class
//    this theFamily += A;
//}    
//
//class B : Base
//{
//    // adding a reference to the derived class
//    this theFamily += B;
//}    
//
//public program()
//{
//    console.print("A:");
//    A.printFamily();
//    
//    console.print("B:");
//    B.printFamily();
//}

class A
{
   const string nickName := "first";
}

class B : A
{
   this nickName := "second";

   printNickname()
   {
      console.printLine(nickName);
   }
}

public program()
{
   var a := new B();
   a.printNickname();

//   var a := new A();
//   var b := new B();
//
//   a.printNickname();
//   b.printNickname();
}