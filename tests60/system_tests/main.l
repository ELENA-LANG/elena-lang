public program()
{
   console.writeLine("--- ELENA 6 API Unit Tests ---");

   gcTests();
   assignTests();
   boxingTests();
   byRefTests();
   shortTests();
   conversionTests();
   //gcTests()

   console.writeLine("--- Passed ---")
}