import system;
import extensions;

AssignTestClass
{
   field x;
   field y;

   constructor(x, y)
   {
      this x := x;
      this y := y;
   }

   test(x, y)
   {
      console.writeLine(this x == x);
      Assert.ifTrue(this x == x);
      console.writeLine(this y == y);
      Assert.ifTrue(this y == y);
   }
}

assignTests()
{
   console.write("assignTests");
   var o := new AssignTestClass(2,"test");
   o.test(2,"test");

   console.writeLine(".");
}

public program() 
{
   assignTests()
}
