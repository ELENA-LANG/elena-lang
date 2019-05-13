[[                                                     

   #grammar text
   #grammar cf

   #define start          ::= l0_expression $eof;
   #define start          ::= $eof;

   #define l0_expression  ::= <= ( => l1_expression l0_operation l0_next_operation* <= ) =>;
   #define l0_expression  ::= l1_expression ;

   #define l0_operation   ::= "*" l1_expression  <= * =>;
   #define l0_operation   ::= "/" l1_expression  <= / =>;

   #define l0_next_operation ::= <= , => l0_operation;

   #define l1_expression  ::= <= ( => number l1_operation l1_next_operation* <= ) =>;
   #define l1_expression  ::= number;

   #define l1_operation   ::= "+" number  <= + =>;
   #define l1_operation   ::= "-" number  <= - =>;

   #define l1_next_operation ::= <= , => l1_operation;

   #define number         ::= <= ( $numeric ) =>;
]]
