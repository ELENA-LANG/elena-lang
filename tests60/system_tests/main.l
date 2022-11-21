public program()
{
   console.writeLine("--- ELENA 6 API Unit Tests ---");

   intTests();
   longTests();
   gcTests();
   assignTests();
   boxingTests();
   byRefTests();
   stringTests();
   shortTests();
   conversionTests();
   //gcTests()

   console.writeLine("--- Passed ---")
}