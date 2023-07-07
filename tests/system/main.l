import extensions;

// --- Main ---

public program()
{
    console.writeLine:"--- ELENA API Tests ---";

    mssgDispatchTest();
    multySelectTest();
    enumTest();
    textBufferTest();
    loopTest();
    actionTest();
    convertorTest();
    arrayTest();
    exceptionTest();

    nestedFieldsTest();
    shortCircuitBooleanOperation();
    variadicTest();
    nestedTest();
    nestetNestedTest();
//    duplicateStructBoxing();
//    duplicateBoxing();

//    stackSafeExtensions();

    //nilDispatcherTest();
    //templateExtensionTest();

    ifExpressionTest();
    loopExpressionTest();

    console.writeLine:"--- Passed ---"
}
