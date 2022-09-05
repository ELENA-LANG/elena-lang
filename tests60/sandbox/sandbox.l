import system;

//A
//{
//   int getVal()
//      = 2;
//}

//B : A
//{
//}


C
{
   read(ref int retVal)
   {
      retVal := 3;
   }
}

public program() 
{
   C c := new C();
   c.read(ref int retVal);

//   console.writeLine(retVal);

//   /*auto*/A a := new A();
//   var b := new B();

//   int v := a.getVal();
//   int v2 := b.getVal();
}
