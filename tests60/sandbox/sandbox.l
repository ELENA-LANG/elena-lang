import extensions;
import ltests;

namespace subns
{
   public class B : info("hidden class B")
   {
      test() : info("Test Method") {}
   }
   
   public testSubNs() : testCase()
   {
      Console.write("Working")
   }
}

public class A : info("super puper class A");

public testRoot() : testCase()
{
   Console.write("Working Too")
}

public Program()
{
   Engine.run();
}
