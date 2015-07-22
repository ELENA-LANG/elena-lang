[[
   #grammar cf
   #define start ::= variable eol start;
   #define start ::= expression eol start;
   #define start ::= $eof;
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
   #define eol ::= ";";
   #define object ::= <= ^ => $identifier;
   #define variable ::= <= #set => "var" identifier assigning;
   #define identifier ::= $identifier;
   #define assigning ::= "=" expression;
   
   #config vm_console 
   #start;
]]

system'console.writeLine("abc".length);
