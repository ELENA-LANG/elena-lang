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

#new __string testCaseCollection;

public inline testCase()
{
   #share super __string testCaseCollection;

   #let testCaseCollection += __decl;
}

myTest() : testCase()
{
   console.writeLine("myTest fired!");
}

//#let testCaseCollection += myTest;

public program()
{
   var list := distributed testCaseCollection;

   list[0]();
}
