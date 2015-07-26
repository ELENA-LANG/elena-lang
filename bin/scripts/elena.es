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
   #define object ::= <= * system'dynamic'Struct { => "{" struct <= } + =>;
   #define object ::= <= * system'dynamic'Tape { => "function" arguments tape <= }+ =>;
   #define object ::= $reference <= + =>;
   #define object ::= $literal <= + =>;
   #define object ::= $numeric <= + =>;
   #define object ::= <= < => $identifier <= + =>;
   #define identifier ::= $identifier;
   #define subject ::= <= % => $identifier <= + =>;
   #define variable ::= <= [ : => "var" identifier assigning <= ]; =>;
   #define set_variable ::= <= [ > => identifier assigning <= ]-; =>;
   #define assigning ::= "=" expression;
   #define struct ::= member next_member;
   #define member ::= subject member_value;
   #define member_value ::= <= [ => ":" expression <= ] =>;
   #define next_member ::= "," member next_member;
   #define next_member ::= "}";

   #define arguments ::= "(" next_argument;
   #define next_argument ::= ")";
   #define tape ::= "{" tape_statement next_tape_statement;
   #define tape_statement ::= <= [ => tape_expression <= ] => ;
   #define tape_expression ::= object tape_operations;
   #define tape_operations ::= tape_call tape_operations;
   #define tape_operations ::= $eps;

   #define tape_call ::= <= & => "." message tape_parameters;
   #define tape_parameters ::= <= ( => "(" tape_expression next_tape_parameter <= ) =>;
   #define tape_parameters ::= $eps;
   #define next_tape_parameter ::= "," tape_expression next_tape_parameter;
   #define next_tape_parameter ::= ")";   
   #define next_tape_statement ::= "}";
   
   #define eol ::= ";";
   
   #config vm_console 
   #start;
]]
