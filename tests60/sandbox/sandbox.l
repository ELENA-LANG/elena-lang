class B
{
   protected constructor new()
   {
      console.writeLine(self)
   }
}

class C : B
{
   _arg1;

   constructor(arg1)
      <= super new() 
   {
      _arg1 := arg1
   }
}

class D : B
{
   constructor(int n)
      <= super new() 
   {
   }
}

public program()
{
   var c := new C("arg");
   var d := new D(2);
}