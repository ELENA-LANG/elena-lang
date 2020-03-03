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
//    literalTest();

//    MultiDispatchTest eval.
////    MultySelectTest  eval.
////    EnumTest eval.    
////    TextBufferTest eval.
////    LoopTest eval.
////    ActionTest eval.
////    RoleTest eval.
////    ConvertorTest eval.
////    ArrayTest eval.
////    OpenArgTest eval.
////    MethodNotFoundTest eval.
//    ExceptionTest();
////    DateTest eval.
////    ListTest eval.
////    DictionaryTest eval.
////    BoxingTest eval.
////    OuterFieldTest eval.
////    LocalTest eval.
////    StructTest eval.
////    GroupTest eval.
////    GenericMethodTest eval.
////    ConstructorTest eval.
////    DTOToStructTest eval.
////    UnboxTest eval.
////    ByRefTest eval.
////    SuperTest eval.
////    //EmbedConstTest eval.
////    jsonTest eval.
////    subExpressionTest eval.

//    nestedFieldsTest();
//    shortCircuitBooleanOperation();
//    variadicTest();
////    proberTest().
//    nestedTest();
//    nestetNestedTest();
//    duplicateStructBoxing();
//    duplicateBoxing();

    stackSafeExtensions();

    console.writeLine:"--- Passed ---"
}
