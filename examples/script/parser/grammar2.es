[[                                                     
   #grammar build

   #grammar cf

   #define start          ::= node $eof;
   #define start          ::= $eof;

   #define node          ::= <= parser'Node ( => "(" leaf ")" <= ) =>;
   #define node          ::= <= parser'Node ( => "(" node operation operations* ")"  <= ) =>;

   #define operation     ::= node "+" <= "+" =>;
   #define operation     ::= node "-" <= "-" =>;
   #define operation     ::= node "*" <= "*" =>;
   #define operation     ::= node "/" <= "/" =>;
   #define leaf          ::= <= "$numeric" =>;

   #define operations    ::= <= , => "," operation;

   #grammar cf

   #define start          ::= l0_expression $eof;
   #define start          ::= l1_expression $eof;
   #define start          ::= $eof;

   #define l0_expression  ::= <= ( => l1_expression l0_operation l0_operations* <= ) =>;
   #define l0_operation   ::= "+" l1_expression <= + =>;
   #define l0_operation   ::= "-" l1_expression <= - =>;
   #define l0_operations  ::= <= , => l0_operation;

   #define l1_expression  ::= number;
   #define l1_expression  ::= <= ( => number l1_operation l1_operations* <= ) =>;
   #define l1_operation   ::= "*" number <= * =>;
   #define l1_operation   ::= "/" number <= / =>;
   #define l1_operations  ::= <= , => l1_operation;

   #define number         ::= <= ( $numeric ) =>;
]]
