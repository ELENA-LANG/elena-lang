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
   #define operation  ::= "." message parameters;
   #define operation  ::= $eps;
   #define message    ::= <= ^ $identifier =>;
   #define parameters ::= "(" object next_param;
   #define parameters ::= $eps;
   #define next_param ::= "," object next_param;
   #define next_param ::= ")";

   #define func       ::= <= [ *system'dynamic'Tape => "function" func_expr <= ] =>;
   #define func_expr  ::= <= $scope => "(" args func_body;
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
   #define foperation  ::= "." fmessage fparameters;
   #define foperation  ::= $eps;
   #define fmessage    ::= <= $identifier =>;
   #define fparameters ::= "(" fobject fnext_param;
   #define fparameters ::= $eps;
   #define fnext_param ::= "," fobject fnext_param;
   #define fnext_param ::= ")";
]]
