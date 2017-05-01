[[
   #grammar cf

   #define start     ::= code? $eof;

   #define code      ::= 
<= 
root
(
   preloaded_symbol
   (
       identifier = printing

       expression
       (
          expression
          (
             nested
             (
                method
                (
                   message = eval

                   code
                   (
=>
                   command+
<= 
                   )
                )
             ) 
          )
          message = eval
       ) 
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
