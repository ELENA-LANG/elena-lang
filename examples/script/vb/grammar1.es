[[
   #grammar cf

   #define start     ::= code;
   #define start     ::= $eof;

   #define code      ::= 
<= 
root
(
   preloaded_symbol
   (
       identifier = printing

=>
             command
<= 
   )
)   
=>;

   #define command    ::= "print" "(" print ")" ";" ;

   #define print      ::= 
<= 
             expression
             (
                reference = system'console 
                message = writeLine
=>
                argument
<=
             )
=>;

   #define argument   ::= <= literal = "$literal" =>;
]]
