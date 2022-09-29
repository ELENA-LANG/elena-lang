import system;
import extensions;

inline external<routine>()
{
   #include intern routine;
}

public const struct String //: BaseValue
{
    __string byte[] theArray;

    bool equal(string s) : external<system'core_routines'__str_equal>;
}


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
