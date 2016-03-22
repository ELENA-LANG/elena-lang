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
   #define initializer::= $eps;
   #define expression ::= <= ( => object operation <= ) =>;   
   #define object     ::= <= $reference =>;
   #define object     ::= <= $literal =>;
   #define object     ::= <= $numeric =>;
   #define object     ::= "(" expression ")" ;
   #define operation  ::= "." message parameters;
   #define operation  ::= $eps;
   #define message    ::= <= ^ $identifier =>;
   #define parameters ::= "(" parameter next_param;
   #define parameter  ::= ":" object;
   #define next_param ::= "," parameter next_param;
   #define next_param ::= ")";
]]
