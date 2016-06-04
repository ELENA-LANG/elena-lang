[[
   #grammar cf

   #define start      ::= <= [ *system'dynamic'Struct => struct $eof <= ] =>;   
   #define start      ::= $eof;   
   #define struct     ::= "{"  fields "}";
   #define fields     ::= field next_field;
   #define next_field ::= "," fields;
   #define next_field ::= $eps;
   #define field      ::= <= $identifier => initializer;
   #define initializer::= ":" expression;
   #define initializer::= ":" func;
   #define initializer::= $eps;
   #define expression ::= <= ( => object operation <= ) =>;   
   #define object     ::= <= $reference =>;
   #define object     ::= <= $literal =>;
   #define object     ::= <= $numeric =>;
   #define object     ::= "(" expression ")" ;
   #define object     ::= <= [ *system'dynamic'Struct => struct <= ] =>;
   #define operation  ::= "." message parameters next_op;
   #define operation  ::= $eps;
   #define next_op    ::= <= ; => message parameters next_op;
   #define next_op    ::= $eps;
   #define message    ::= <= ^ $identifier =>;
   #define parameters ::= "(" object next_param;
   #define parameters ::= $eps;
   #define next_param ::= "," object next_param;
   #define next_param ::= ")";

   #define func       ::= <= [ *system'dynamic'Tape => "function" func_expr <= ] =>;
   #define func_expr  ::= <= $scope => "(" func_expr2;
   #define func_expr2  ::= <= $idlevar => args func_body;
   #define args       ::= arg next_arg;
   #define args       ::= ")";
   #define next_arg   ::= "," arg next_arg;
   #define next_arg   ::= ")";
   #define arg        ::= <= $newvar =>;
   #define func_body  ::= "{" fstatements;
   #define fstatements::= fexpression next_fstatement;
   #define fstatements::= <= $end => "}";
   #define next_fstatement ::= ";" fstatements;
   #define next_fstatement ::= <= $end => "}";

   #define fexpression ::= <= ( => fobject foperation <= ) =>;   
   #define fobject     ::= <= $reference =>;
   #define fobject     ::= <= $literal =>;
   #define fobject     ::= <= $numeric =>;
   #define fobject     ::= <= ( % "system'dynamic'tapeOp.<#%&var&args$" $var ) =>;
   #define fobject     ::= "(" fexpression ")" ;
   #define fobject     ::= <= ( system'dynamic'Dynamic new ) => "{" ffields;
   #define ffields     ::= ffield ffield_exp next_ffield ;
   #define ffield      ::= <= set $identifier =>;
   #define ffield_exp  ::= <= ( => ":" fexpression <= ) =>;   
   #define next_ffield ::= "," ffields;
   #define next_ffield ::= "}";   

   #define foperation  ::= "." fmessage fparameters fnext_op;
   #define foperation  ::= $eps;
   #define fnext_op    ::= <= ; => "." fmessage fparameters;
   #define fnext_op    ::= $eps;
   #define fmessage    ::= <= $identifier =>;
   #define fparameters ::= "(" fexpression fnext_param;
   #define fparameters ::= $eps;
   #define fnext_param ::= "," fexpression fnext_param;
   #define fnext_param ::= ")";
]]
