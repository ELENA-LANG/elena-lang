import system;
import extensions;

A;

B;

C
{
   A getVal()
      = new A();

   multiret B getVal()
      = new B();
}

//B : A
//{
//}


public program() 
{
   C c := new C();

   A a := c.getVal();
   B b := c.getVal();
}
