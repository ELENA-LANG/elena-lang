import extensions;
import ltests;

singleton StaticFieldAssigningOpHelper
{
   static int i := 0;

   inc()
   {
      i += 1;
   }

   get()
      = i;
}

public staticFieldAssigningOpTests() : testCase()
{
   StaticFieldAssigningOpHelper.inc();
   StaticFieldAssigningOpHelper.inc();

   Assert.ifEqual(StaticFieldAssigningOpHelper.get(), 2);
   console.write(".");
}

public program()
{
//   staticFieldAssigningOpTests()

//   StaticFieldAssigningOpHelper;

   Engine.run();
}