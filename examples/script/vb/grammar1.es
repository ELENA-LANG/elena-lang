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
                   statement+
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

   #define statement  ::= expression ";" ;
   #define statement  ::= "var" variable ";" ;

   #define expression ::= "print" "(" print ")";
   #define expression ::= object;

   #define print      ::= 
<= 
             expression
             (
                reference = system'console 
                message = writeLine
=>
                object
<=
             )
=>;

   #define variable   ::= 
                            var_ident "=" expression ";"
<=
             )
=>;

   #define var_ident  ::= 
<=
             variable
             (
                identifier = $identifier
             )
             expression 
             (
                assign = 0
                identifier = $identifier
=>;

   #define object     ::= <= literal = "$literal" =>;
   #define object     ::= <= identifier = "$identifier" =>;
]]
