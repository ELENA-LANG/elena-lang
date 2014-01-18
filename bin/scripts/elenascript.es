[[
   #config vm_console #start;
   #define start    ::= "?" system'console .eval&writeLine(expr);   
   #define expr     ::= constant operation; 
   #define constant ::= $literal;
   #define constant ::= $numeric;
   #define operation ::= "+" .add( constant );
   #define operation ::= $eps;
]]
