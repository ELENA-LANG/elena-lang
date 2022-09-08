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


singleton ByRefProber
{
//   set(ref int retVal)
//   {
//   }

   set(ref string retVal)
   {
      retVal := "hello";
   }
}

public program() 
{
//   int n := 0;
//   ByRefProber.set(ref n);

//   Assert.ifTrue(n == 123);


//   string s := "none";
//   ByRefProber.set(ref s);

//   Assert.ifTrue(s == "hello");



//   C c := new C();
//   c.read(ref int retVal);

//   console.writeLine(retVal);

//   /*auto*/A a := new A();
//   var b := new B();

//   int v := a.getVal();
//   int v2 := b.getVal();
}
