import extensions;

struct X
{
   int field;

   int Value = field;

   foo()
   {
      int n := *self;

      Console.writeLine(n);
   }

   constructor() { field := 2 }
}

/*class IntWrapper 
{
   int _value;

   constructor(int v) { _value := v }

   sealed int Value // NOTE : the method or a class must be sealed
      = _value;
}*/

public Program()
{
   X x := default;
   x.foo();

//   auto o := new IntWrapper(3);

//   int r := *o;

//   int n := *o + 2;
  
//   Console.printLine("o.Value=", n)
}
