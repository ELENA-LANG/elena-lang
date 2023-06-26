public program()
{
   console.writeLine("--- ELENA 6 API Unit Tests ---");

   intTests();
   uintTests();
   longTests();
   realTests();
   gcTests();
   assignTests();
   boxingTests();
   byRefTests();
   stringTests();
   wideTests();
   shortTests();
   conversionTests();
   inlineAssignmentTests();
   singleDispatchTest();
   variadicTest();
   dynamicLoadTests();
   interfaceTests();
   vardispatcherTest();

   console.writeLine("--- Passed ---")
}