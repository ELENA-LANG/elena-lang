[[
   #grammar cf

   #define start      ::= statement;
   #define start      ::= $eof;
   #define statement  ::= expression next_expr;
   #define expression ::= <= ( => object operation <= ) =>;   
   #define object     ::= <= $reference =>;
   #define object     ::= <= $literal =>;
   #define object     ::= <= $numeric =>;
   #define object     ::= "(" expression ")" ;
   #define operation  ::= message parameters <= ^ = =>;
   #define operation  ::= $eps;
   #define message    ::= <= + $identifier =>;
   #define parameters ::= parameter;
   #define parameters ::= $eps; 
   #define parameter  ::= ":" object;
   #define next_expr  ::= $eof;
]]
