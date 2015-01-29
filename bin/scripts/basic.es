[[
#define start ::= expression;
#define expression ::= <= ( => object operation;
#define operation ::= "." message parameters <= ) #reverse #send =>;
#define operation ::= $eps <= ) =>;
#define message ::= <= #message => $identifier;
#define parameters ::= "(" expression next_parameter;
#define next_parameter ::= "," expression next_parameter;
#define next_parameter ::= ")";
#define object ::= $reference;
#define object ::= $literal;
#define object ::= $numeric;
]]                     