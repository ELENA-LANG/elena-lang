//B;
//
//C
//{
//    B cast()
//        = new B();
//}
//
//singleton E
//{
//    load(params B[] o)
//    {
//        for(int i := 0; i < o.Length; i += 1)
//        {
//            console.writeLine(o[i])
//        }
//    }
//}
//
//public program()
//{
//   var b := new B();
//   var c := new C();
//
//   E.load(b,c);
//
////   var e := E;    
////   e.load(b,c);
//}

import system'text;
import extensions;

VT_B;

VT_C
{
    VT_B cast()
        = new VT_B();
}

singleton VT_E
{
   load(params VT_B[] o)
   {
      ^ new TextBuilder().print(o.asEnumerable()).Value;
   }

   loadAndTypecast(params object[] args)
   {
      ^ self.load(cast VT_B(params args));
   }
}

public program()
{
   var b := new VT_B();
   var c := new VT_C();
   
   Assert.ifEqual(VT_E.loadAndTypecast(b,c), "system_tests'$private'VT_B,system_tests'$private'VT_B");

   console.write(".");
}
