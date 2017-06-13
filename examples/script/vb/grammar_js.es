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
   #define statement  ::= 
<= 
             expression
             (
                 operator = "?"
=>
         "if" "(" expression ")" code_brackets
<=
             )
=>;

   #define statement  ::= 
<= 
          loop
          (
             expression
             (
                 operator = "?"
=>
         "while" "(" expression ")" code_brackets
<=
             )
          )
=>;

   #define code_brackets ::= 
<=

                 expression (
                    code (
=>
                  "{" statement* "}"
<=
                    )
                 )
=>;

   #define expression ::= "print" "(" print ")";
   #define expression ::= 
<=
             expression
             (
=>
                 object operation?
<=
	     )
=>;

   #define operation  ::= 
<=
                operator = "=="
=>
                "==" object;

   #define operation  ::= 
<=
                operator = "<"
=>
                "<" object;

   #define operation  ::= 
<=
                operator = "+"
=>
                "+" object;

   #define operation  ::= 
<=
                operator = "-"
=>
                "-" object;

   #define operation  ::= 
<=
                assign = 0
=>
                "=" expression;

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
                            var_decl "=" expression
<=
             )
=>;

   #define var_decl  ::= 
<=
             variable
             (
                identifier = $current
=>
             assign_var;
      
  #define assign_var ::=
<=  
             )
             expression 
             (
                assign = 0
                identifier = $identifier
=>;

   #define object     ::= <= literal = "$literal" =>;
   #define object     ::= <= identifier = $identifier =>;
   #define object     ::= <= numeric = $numeric =>;
]]
