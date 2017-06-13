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

   #define code      ::= 
<= 
root
(
   nested
   (
      method
      (
=>
      "function" function_name "(" function_params? ")" function_body
<=
      )
   )
=>;

   #define function_params ::= function_param next_function_param*;

   #define next_function_param ::= "," function_param

   #define function_body  ::=
<=
                   code
                   (
=>
                   statement* ret_statement?
<= 
                   )
=>;

   #define ret_statement  ::= 
<= 
             ret_expression
             (
=>
                 "return" expression ";"
<=
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

   #define expression ::= 
<=
             expression
             (
=>                
                (!"print" | "if" | "loop") "(" function_name parameter? next_parameter* ")"
<=
	     )
=>;

   #define function_name ::=
		<= message = $current =>;

   #define parameter  ::=
                expression;

   #define next_parameter  ::=
                "," expression;

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

   #define function_param ::= <= identifier = $identifier =>;
]]
