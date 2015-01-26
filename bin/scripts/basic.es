[[
#define start ::= expression;
#define expression ::= object "." operation;
#define object ::= $reference;
#define object ::= $literal;
#define operation ::= message "(" parameters;
#define message ::= %operation;
#define parameters ::= <= ( => expression next_parameter <= ) =>;
#define next_parameter ::= "," expression next_parameter;
#define next_parameter ::= ")";
]]