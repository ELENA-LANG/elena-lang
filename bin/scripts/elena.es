[[
   #grammar cf

   #define start      ::= expression;
   #define start      ::= $eof;
   #define expression ::= object;
   #define object     ::= $reference;

   #config vm_console 
   #start;
]]
