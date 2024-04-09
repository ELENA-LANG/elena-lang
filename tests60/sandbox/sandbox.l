import extensions;
import ltests;

class Store
{
   field value;

   Value
   {
      set(o)
      {
         value := o
      }

      get()
         = value;
   }
}

Test1()
   : testCase()
{
   var s := new Store();

   s.Value := 2;

   Assert.ifTrue(s.Value == 2);

   console.write(".")
}

Test2()
   : testCase()
{
   Assert.ifTrue(false);
}

public program()
{
   Engine.run();
}
