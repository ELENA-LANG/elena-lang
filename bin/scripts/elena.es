[[
   #grammar cf
   #define start ::= expression;
   #define expression ::= object operation;
   #define operation ::= <= ^ ( => "." message parameters <= ) =>;
   #define operation ::= $eps
   #define message ::= <= % => $identifier;
   #define parameters ::= "(" expression next_parameter;
   #define next_parameter ::= "," expression next_parameter;
   #define next_parameter ::= ")";
   #define object ::= $reference;
   #define object ::= $literal;
   #define object ::= $numeric;

   #config vm_console 
   #start;
]]

system'console.writeLine("Hello")