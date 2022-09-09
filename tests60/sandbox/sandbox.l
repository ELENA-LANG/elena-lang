import system;
import extensions;


A
{
   int getVal()
      = 2;
}

//B : A
//{
//}


public program() 
{
   /*auto*/A a := new A();
//   var b := new B();

   int v := a.getVal();
//   int v2 := b.getVal();

   console.writeLine(v);
}
