[[
   #grammar cf
   #define start ::= variable eol start;
   #define start ::= set_variable eol start;
   #define start ::= statement eol start;
   #define start ::= $eof;
   #define statement ::=  <= [ => expression <= ] . =>;   
   #define expression ::= object operations;
   #define operations ::= call operations;
   #define operations ::= $eps;
   #define call ::= <= ^ => "." message parameters;
   #define message ::= $identifier;
   #define parameters ::= <= ( => "(" expression next_parameter <= ) =>;
   #define parameters ::= $eps;
   #define next_parameter ::= "," expression next_parameter;
   #define next_parameter ::= ")";
   #define object ::= $reference <= + =>;
   #define object ::= $literal <= + =>;
   #define object ::= $numeric <= + =>;
   #define object ::= <= < => $identifier <= + =>;
   #define identifier ::= $identifier;
   #define variable ::= <= [ : => "var" identifier assigning <= ]; =>;
   #define set_variable ::= <= [ > => identifier assigning <= ]- =>;
   #define assigning ::= "=" expression;

   #define eol ::= ";";
   
   #config vm_console 
   #start;
]]
