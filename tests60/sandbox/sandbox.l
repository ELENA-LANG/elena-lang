//import extensions;
//import tests;
//
//class Store
//{
//   field value;
//
//   Value
//   {
//      set(o)
//      {
//         value := o
//      }
//
//      get()
//         = value;
//   }
//}
//
//Test1()
//   : testCase
//{
//   var s := new Store();
//
//   s.Value := 2;
//
//   Assert.ifTrue(s.Value == 2)
//}

//myTest() : testCase()
//{
//   console.writeLine("myTest fired!");
//}

#new __string testCaseCollection;

public inline injectCode()
{
//   #share super __string testCaseCollection;

   #let testCaseCollection += 
   ::{
      function()
      {
         console.writeLine(/*__decl*/$name __decl)
      }
   };
}

class MyClass : injectCode()
{
}

public program()
{
   auto collection := distributed testCaseCollection;
   
   collection[0]();

//   Engine.run();
}
