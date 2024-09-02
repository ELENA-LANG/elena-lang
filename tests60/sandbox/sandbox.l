import extensions;

//textblock AImpl
//{
//   get() {}
//}

public class A //: using(AImpl)
{
   static int MyX := 2;

   static constructor()
   {
      //console.writeLine($"A is called with {MyX}");
      console.writeLine("Hello from A")
   }
}

public program()
{
   A a := new A();
}
