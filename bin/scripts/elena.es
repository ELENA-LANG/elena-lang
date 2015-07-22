[[
   #grammar cf
   #define start ::= expression;
   #define expression ::= object operations;
   #define operations ::= call operations;
   #define operations ::= $eps;
   #define call ::= <= #send ( => "." message parameters <= ) =>;
   #define message ::= <= % => $identifier;
   #define parameters ::= "(" expression next_parameter;
   #define parameters ::= $eps;
   #define next_parameter ::= "," expression next_parameter;
   #define next_parameter ::= ")";
   #define object ::= $reference;
   #define object ::= $literal;
   #define object ::= $numeric;

   #config vm_console 
   #start;
]]

system'console.writeLine("abc".length)
