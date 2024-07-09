// scenario default namespaces
constexpr auto S_DefaultNamespace_1 = "namespace (class (nameattr (identifier \"Object\" ())) class (nameattr (identifier \"B\" ())) $1)";
constexpr auto S_DefaultNamespace_2 = "namespace (class (nameattr (identifier \"Object\" ()) method (nameattr (identifier \"dispatch\" ())redirect (expression (object (identifier \"nil\" ()))))) $1)";
constexpr auto S_DefaultNamespace_4 = "namespace (class (nameattr (identifier \"Object\" ()) method (nameattr (identifier \"dispatch\" ())redirect (expression (object (identifier \"nil\" ()))))) $1)";
constexpr auto S_DefaultNamespace_3 = "namespace (class (nameattr (identifier \"Object\" ())) $1)";

// scenario basic types
constexpr auto S_IntNumber = "class (attribute -2147467263 () attribute -2147475455 () attribute -2147479550 () nameattr (identifier \"IntNumber\" ()) field (attribute -2147475454 () attribute -2147481597 () nameattr (identifier \"_value\" ())dimension (integer \"4\" ())))";

// main program body
constexpr auto S_NillableIntAssigning = "class (attribute -2147467263 ()nameattr (identifier \"program\" ())attribute -2147479546 ()method (attribute -2147479540 ()code (expression (assign_operation (object (nullable (type (identifier \"IntNumber\" ()))identifier \"n\" ())expression (object (identifier \"nil\" ()))))expression (assign_operation (object (nullable (type (identifier \"IntNumber\" ()))identifier \"m\" ())expression (object (integer \"2\" ())))))))";
constexpr auto S_IntAssigningNil = "class (attribute -2147467263 ()nameattr (identifier \"program\" ())attribute -2147479546 ()method (attribute -2147479540 ()code (expression (assign_operation (object (type (identifier \"IntNumber\" ())identifier \"m\" ())expression (object (identifier \"nil\" ())))))))";

constexpr auto S1_VariadicTemplates = " class (attribute -2147467263 ()attribute -2147471359 ()attribute -2147479542 ()nameattr (identifier \"VariadicArray\" ())field (attribute -2147475454 ()attribute -2147481599 ()array_type (type (identifier \"Object\" ()))nameattr (identifier \"array\" ())))  class (attribute -2147467263 ()attribute -2147471359 ()attribute -2147479542 ()nameattr (identifier \"VariadicBArray\" ())field (attribute -2147475454 ()attribute -2147481599 ()array_type (type (identifier \"B\" ()))nameattr (identifier \"array\" ())))";


