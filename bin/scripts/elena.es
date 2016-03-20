[[
   #grammar cf

   #define start      ::= expression;
   #define start      ::= $eof;
   #define expression ::= <= ( => object operation <= ) =>;
   #define object     ::= <= $reference =>;
   #define object     ::= <= $literal =>;
   #define operation  ::= message parameters;
   #define message    ::= <= ^ $identifier =>;
   #define parameters ::= parameter;
   #define parameters ::= $eps;
   #define parameter  ::= ":" object;

   #config vm_console 
   #start;
]]
