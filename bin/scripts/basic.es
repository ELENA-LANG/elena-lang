[[
#define start ::= expression;
#define expression ::= object "." operation;
#define operation ::= message "(" parameters;
#define parameters ::= expression next_parameter;
#define next_parameter ::= "," expression next_parameter;
#define next_parameter ::= ")";
]]