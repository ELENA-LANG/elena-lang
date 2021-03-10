import extensions;

// --- Main ---

public program()
{
    console.writeLine:"--- ELENA API Tests ---";

    controlTest();
    objectTest();
    booleanTest();
    integerTest();
    longTest();
    realTest();
    literalTest();
    wideTest();

    mssgDispatchTest();
    multySelectTest();
    enumTest();
    textBufferTest();
    loopTest();
    actionTest();
    convertorTest();
    arrayTest();
    exceptionTest();
////    OuterFieldTest eval.
////    GenericMethodTest eval.
////    ByRefTest eval.
////    SuperTest eval.

    nestedFieldsTest();
    shortCircuitBooleanOperation();
    variadicTest();
////    proberTest().
    nestedTest();
    nestetNestedTest();
//    duplicateStructBoxing();
    duplicateBoxing();

    stackSafeExtensions();

    nilDispatcherTest();
    templateExtensionTest();

    console.writeLine:"--- Passed ---"
}
