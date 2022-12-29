import extensions;

interface B
{
   abstract callMe();
}

A
{
   callMe()
   {
      console.writeLine("Call Me")
   }   

   B getB() = B { embeddable dispatch() => self; };
}
   
public program()
{
   A a := new A();

   B b := a.getB();

   b.callMe()

//   console.writeLine("Hello");
}
