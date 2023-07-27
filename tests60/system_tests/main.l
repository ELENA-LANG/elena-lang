public program()
{
   console.writeLine("--- ELENA 6 API Unit Tests ---");

   controlTest();
   objectTest();
   booleanTest();
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
   mssgDispatchTest();
   multySelectTest();
   enumTest();
   textBufferTest();
   loopTest();
   actionTest();
   arrayTest();
   exceptionTest();
   nestedFieldsTest();
   shortCircuitBooleanOperation();
   nestedTest();
   nestetNestedTest();
   ifExpressionTest();
   loopExpressionTest();
   nilDispatcherTest();
   duplicateBoxing();
   templateExtensionTest();
   stackSafeExtensions();
   genericTest();
   outerFieldAssigningTest();
   methodNotFoundTest();
   nilReferenceTest();

   console.writeLine("--- Passed ---")
}