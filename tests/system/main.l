
// --- Main ---

#symbol program =
[   
    XTest eval.

    system'console writeLine:"Starting...".

    ControlTest eval.
    ObjectTest eval.
    BooleanTest eval.
    IntegerTest eval.
    LongTest eval.
    RealTest eval.
    LiteralTest eval.

    BreakTest eval.
    MultiDispatchTest eval.

    MultySelectTest eval.
    EnumTest eval.    
    TextBufferTest eval.
    LoopTest eval.
    ActionTest eval.
    RoleTest eval.
    ConvertorTest eval.
    ArrayTest eval.     
    MethodNotFoundTest eval.
    ExceptionTest eval.
    DateTest eval.
    ListTest eval.    
    DictionaryTest eval.
    GetPropertyTest eval.
    BoxingTest eval.
    OuterFieldTest eval.
    LocalTest eval.
    OpenArgTest eval.
    StructTest eval.
    GroupTest eval.
    GenericMethodTest eval.
].
