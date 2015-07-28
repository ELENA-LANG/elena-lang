[[
   #grammar cf

   #define start      ::= $scope statements;
   #define statements ::= statement statements;
   #define statements ::= $eof;
   #define statement  ::= <= [ => "var" new_var assigning eol <= ] , =>;
   #define statement  ::= <= [ > => var assigning eol <= ] , =>;
   #define statement  ::= <= [ => expr eol <= ] ; =>;
   #define expr       ::= obj ops;
   #define obj        ::= $reference;
   #define obj        ::= $literal;
   #define obj        ::= $numeric;
   #define obj        ::= <= < => $var;
   #define obj        ::= <= *system'dynamic'Struct { => "{" fields <= } =>;
   #define ops        ::= "." call ops;
   #define ops        ::= $eps; 
   #define call       ::= <= ^ => msg args;
   #define msg        ::= $identifier;
   #define args       ::= <= ( => "(" expr next_arg <= ) =>;
   #define args       ::= $eps;
   #define next_arg   ::= "," expr next_arg;
   #define next_arg   ::= ")";
   #define eol        ::= ";";
   #define new_var    ::= $newvar;
   #define var        ::= $var;
   #define assigning  ::= "=" expr;
   #define fields     ::= field field_exp next_field;
   #define field      ::= <= % => $identifier;
   #define field_exp  ::= <= [ => ":" expr <= ] =>;
   #define next_field ::= "," fields;
   #define next_field ::= "}";

   #config vm_console 
   #start;
]]
