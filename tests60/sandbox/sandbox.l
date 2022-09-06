import system;
import extensions;

//A
//{
//   int getVal()
//      = 2;
//}

//B : A
//{
//}


//C
//{
//   read(ref int retVal)
//   {
//      retVal := 3;
//   }
//}

struct BoxingStruct
{
   int x;
   int y;

   constructor()
   {
      x := 1;
      y := 2;
   }

//   set(int x, int y)
//   {
//      this x := x;
//      this y := y;
//   }

//   test(int x, int y)
//   {
//      Assert.ifTrue(this x == x);
//      Assert.ifTrue(this y == y);
//   }
}

//singleton BoxingProber
//{
//}

public program() 
{
   var s := new BoxingStruct();

  // BoxingStruct st;


//   C c := new C();
//   c.read(ref int retVal);

//   console.writeLine(retVal);

//   /*auto*/A a := new A();
//   var b := new B();

//   int v := a.getVal();
//   int v2 := b.getVal();
}
