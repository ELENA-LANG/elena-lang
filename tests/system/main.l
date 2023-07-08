import extensions;

// --- Main ---

public program()
{
    console.writeLine:"--- ELENA API Tests ---";
    
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
