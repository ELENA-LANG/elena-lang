import extensions;
import system'text;

//   VT_B;
//   
//   VT_C
//   {
//       VT_B cast()
//           = new VT_B();
//   }
//   
//   singleton VT_E
//   {
//      load(params VT_B[] o)
//      {
//         ^ new TextBuilder().print(o.asEnumerable());
//       }
//   
//      loadAndTypecast(params object[] args)
//      {
//         ^ self.load(cast VT_B(params args));
//      }
//   }
//   
//   variadicTypecastingTests()
//   {
//      var b := new VT_B();
//      var c := new VT_C();
//      
//      Assert.ifEqual(VT_E.loadAndTypecast(b,c), "sandbox'$private'VT_B,sandbox'$private'VT_B");
//   
//      console.write(".");
//   }
//
//public program()
//{
//   variadicTypecastingTests();
//
////   var b := new B();
////   var c := new C();
////   
////   var s := E.loadAndTypecast(b,c);    
////
////   console.writeLine(s);
//
////    var e := E.load(b,c);
//}

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

public variadicTypecastingTests() //: testCase()
{
   var b := new VT_B();
   var c := new VT_C();
   
   console.writeLine(VT_E.loadAndTypecast(b,c));

   //Assert.ifEqual(VT_E.loadAndTypecast(b,c), "system_tests'$private'VT_B,system_tests'$private'VT_B");

   console.write(".");
}

public program()
{
   variadicTypecastingTests()
}
