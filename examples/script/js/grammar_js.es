[[
   #grammar cf

   #define start     ::= $eof;

   #define start     ::= 
<=
root 
( 
  namespace
  (
=>
    scope $eof
<=
  )
)
=>;

   #define scope ::= 
<=
   preloaded_symbol
   (
       expression
       (
          expression
          (
             nested
             (
=>
                fun_decl* code
<=
             ) 
          )
          message = eval
       ) 
   )
=>;

   #define fun_decl  ::= 
<= 
      method
      (
=>
      "function" function_name  "(" function_params? ")" "{" function_body "}"
<=
      )
=>;

   #define code      ::= 
<= 
                method 
                (
                   nameattr ( identifier = eval )

                   code
                   (
=>
                   statement+
<= 
                   )
                )
=>;

   #define function_params ::= function_param next_function_param*;

   #define next_function_param ::= "," function_param;

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
             returning
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
=>
         "if" "(" expression ")" if_code_brackets
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

   #define if_code_brackets ::=
<=
                operator = "?"
=>
                code_brackets;

   #define code_brackets ::= 
<=

                 expression (
                    code (
=>
                  "{" statement* ret_statement? "}"
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

   #define message_name ::=
		<= message = $current =>  (!"print" | "if" | "loop" | "var" | "while");

   #define function_name ::=
		<= nameattr ( identifier = $current =>  (!"print" | "if" | "loop" | "var" | "while") <= ) =>;

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
                operator = ">"
=>
                ">" object;

   #define operation  ::= 
<=
                operator = "<="
=>
                "<=" object;

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

   #define object ::= 
<=
             expression
             (
                identifier = __target
=>                
                message_name "(" parameter? next_parameter* ")"
<=                                           
	     )
=>;

   #define object     ::= <= expression ( literal = "$literal" ) =>;
   #define object     ::= <= expression ( identifier = $identifier ) =>;
   #define object     ::= <= expression ( numeric = $numeric ) =>;

   #define function_param ::= <= parameter ( nameattr ( identifier = $identifier ) ) =>;
]]
