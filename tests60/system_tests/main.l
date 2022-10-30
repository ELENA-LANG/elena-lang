public program()
{
   console.writeLine("--- ELENA 6 API Unit Tests ---");

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