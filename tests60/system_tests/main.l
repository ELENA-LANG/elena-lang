public program()
{
   console.writeLine("--- ELENA 6 API Unit Tests ---");

   intTests();
   longTests();
   realTests();
   gcTests();
   assignTests();
   boxingTests();
   byRefTests();
   stringTests();
   shortTests();
   conversionTests();
   inlineAssignmentTests();

   console.writeLine("--- Passed ---")
}