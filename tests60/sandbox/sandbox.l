import extensions;

//class Y
//{
//   Reference</*int*/string> index /*:= 2*/;

//   foo()
//   {
//      /*int*/string n := *index;
//   }
//}

class IntWrapper 
{
   int _value;

   constructor(int v) { _value := v }

   sealed int Value // NOTE : the method or a class must be sealed
      = _value;
}

public Program()
{
//   new Y().foo();

   auto o := new IntWrapper(3);

   int r := *o;

//   int n := *o + 2;
  
//   Console.printLine("o.Value=", n)
}
