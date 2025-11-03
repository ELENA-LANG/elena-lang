// scenario default namespaces
constexpr auto S_DefaultNamespace_1 = "namespace (class (nameattr (identifier \"Object\" ())) class (nameattr (identifier \"B\" ())) $1)";
constexpr auto S_DefaultNamespace_2 = "namespace (class (nameattr (identifier \"Object\" ()) method (nameattr (identifier \"dispatch\" ())redirect (expression (object (identifier \"nil\" ()))))) $1)";
constexpr auto S_DefaultNamespace_4 = "namespace (class (nameattr (identifier \"Object\" ()) method (nameattr (identifier \"dispatch\" ())redirect (expression (object (identifier \"nil\" ()))))) $1)";
constexpr auto S_DefaultNamespace_3 = "namespace (class (nameattr (identifier \"Object\" ())) $1)";

// scenario basic types
constexpr auto S_IntNumber = "class (attribute -2147467263 () attribute -2147475455 () attribute -2147479550 () nameattr (identifier \"IntNumber\" ()) field (attribute -2147475454 () attribute -2147481597 () nameattr (identifier \"_value\" ())dimension (integer \"4\" ())))";
constexpr auto S_ByteNumber = "class (attribute -2147467263 () attribute -2147475455 () attribute -2147479550 () nameattr (identifier \"ByteNumber\" ()) field (attribute -2147475454 () attribute -2147481596 () nameattr (identifier \"_value\" ())dimension (integer \"1\" ())))";
constexpr auto S_IntRefeference = "class ( attribute -2147471359 () nameattr (identifier \"IntReference\" ()) field (attribute -2147475454 () type (identifier \"IntNumber\" ()) nameattr (identifier \"_value\" ())) )";

// scenario symbols
constexpr auto S2_Scenario_Sync = "public_symbol (attribute -2147479537 () nameattr (identifier \"sync\" ())get_expression (expression (message_operation (object (attribute -2147479534 () identifier \"object\" ())))))";

// main program body
constexpr auto S_NillableIntAssigning = "class (attribute -2147467263 ()nameattr (identifier \"program\" ())attribute -2147479546 ()method (attribute -2147479540 ()code (expression (assign_operation (object (nullable (type (identifier \"IntNumber\" ()))identifier \"n\" ())expression (object (identifier \"nil\" ()))))expression (assign_operation (object (nullable (type (identifier \"IntNumber\" ()))identifier \"m\" ())expression (object (integer \"2\" ())))))))";
constexpr auto S_IntAssigningNil = "class (attribute -2147467263 ()nameattr (identifier \"program\" ())attribute -2147479546 ()method (attribute -2147479540 ()code (expression (assign_operation (object (type (identifier \"IntNumber\" ())identifier \"m\" ())expression (object (identifier \"nil\" ())))))))";

constexpr auto S1_VariadicTemplates = " class (attribute -2147467263 ()attribute -2147471359 ()attribute -2147479542 ()nameattr (identifier \"VariadicArray\" ())field (attribute -2147475454 ()attribute -2147481599 ()array_type (type (identifier \"Object\" ()))nameattr (identifier \"array\" ())))  class (attribute -2147467263 ()attribute -2147471359 ()attribute -2147479542 ()nameattr (identifier \"VariadicBArray\" ())field (attribute -2147475454 ()attribute -2147481599 ()array_type (type (identifier \"B\" ()))nameattr (identifier \"array\" ())))";
constexpr auto S_DummyByteArray = " class (attribute -2147467263 ()attribute -2147471359 ()nameattr (identifier \"ByteArray\" ())field (attribute -2147475454 ()attribute -2147481599 ()array_type (type (identifier \"ByteNumber\" ()))nameattr (identifier \"array\" ())))";

constexpr auto Tester_2Args = "class (attribute -2147467263 ()attribute -2147479546 () nameattr (identifier \"Tester\" ()) method (nameattr (identifier \"testArg2\" ()) parameter (nameattr (identifier \"arg1\" ())) parameter (nameattr (identifier \"arg2\" ())) code ()))";

constexpr auto IndexedClass_Scenario1 = "class (attribute -2147479545 ()nameattr 67 (identifier \"X\" ())method (type (identifier \"IntNumber\" ())nameattr (identifier \"calc\" ())parameter (type (identifier \"IntNumber\" ())nameattr (identifier \"arg\" ()))returning ())) class (attribute -2147479546 ()nameattr (identifier \"Y\" ())parent (type (identifier \"X\" ())))";

constexpr auto S_ByRefHandlerTest_Class = "class (attribute -2147479551 (identifier \"class\" ())nameattr (identifier \"A\" ()) method (type (identifier \"IntNumber\" ()) nameattr (identifier \"incSum\" ()) parameter (type (identifier \"IntNumber\" ()) nameattr (identifier \"arg1\" ()))returning (expression (add_operation (object (identifier \"arg1\" ())expression (object (integer \"1\" ())))))))";

constexpr auto S2_Scenario_IntermediateVar = "class (attribute -2147467263 ()nameattr (identifier \"program\" ())attribute -2147479546 ()method (attribute -2147479540 ()code (code (expression (assign_operation (object (attribute -2147479506 ()identifier \"v\" ())expression (expression (attribute -2147479505 () object (identifier \"sync\" ())))))expression (message_operation (object (identifier \"v\" ())message (identifier \"enterCriticalSection\" ()))) expression (final_operation (code () final_block (expression (message_operation (object (identifier \"v\" ()) message (identifier \"leaveCriticalSection\" ()))))))) EOP ()))))";