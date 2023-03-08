import system'text;
import extensions;

class A
{
   sealed static test(string s)
   {
      console.writeLine("Test.test");
   }
}

class B : A
{
}

public program()
{
//   A.test("hello");
   B.test("hello");

//   long l := 2l;
//   long l2 := -2l;
//   int n := 2;
//   int n2 := -2;

//   Assert.ifTrue(l == n);   // ; true
//   Assert.ifTrue(l2 == n2); // ; true
//   Assert.ifFalse(l == n2);  // ; false
//   Assert.ifFalse(l2 == n); // ; false

//   Assert.ifFalse(l < n2);  // ; false
//   Assert.ifFalse(l < n);  // ; false
//   Assert.ifTrue(l2 < n);  // ; true
//   Assert.ifFalse(l2 < n2);  // ; false


//    auto engine := new extensions'scripting'ScriptEngine();
//    engine.loadPath("grammar1.es");
}