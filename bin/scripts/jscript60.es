[[
   #grammar cf

   #define start          ::= module $eof;
   #define start          ::= $eof;

   #define module         ::= 
<= 
  root ( 
    public_namespace (
      public_symbol (
        nameattr ( identifier = program )
        get_expression (
          nested (
              
=> 
                               function* code 
<= 
          )
        )
      )
    )
  ) 
=>;

   #define function       ::=
<=
       script_method (
=>
                              "function" f_name "(" f_args? ")" f_code
<=
       )
=>;

   #define f_args         ::= f_arg next_f_arg*;
   #define next_f_arg     ::= "," f_arg;
                             
   #define f_code         ::=
<=
         code (
=>
                              "{" statement* "}"
<=
         )
=>;

   #define code           ::= 
<=
       script_method (
         nameattr ( identifier = main )
         code (
=>
                              statement+
<=
         )
       )
=>;

   #define statement      ::= "print" "(" print ")" ";";
   #define statement      ::= expression ";";
   #define statement      ::= 
<= 
             returning
             (
=>
                              "return" expression ";"
<=
             )
=>;

   #define statement  ::= 
<= 
             expression
             (
               if_operation (
=>
                              "if" "(" expression ")" if_code_brackets
<=
               )
             )
=>;

   #define if_code_brackets ::= code_brackets;

   #define code_brackets ::= 
<=

                 expression (
                   closure (
                     code (
=>
                                "{" statement* "}"
<=
                     )
                   )
                 )
=>;

   #define expression ::= 
<=
             expression
             (
=>
                              expr_operation
<=
	     )
=>;

   #define expr_operation  ::= $ object operation? ;

   #define operation  ::= ^ <= equal_operation ( => "==" object <= ) =>;

   #define operation  ::= ^ <= less_operation ( => "<" object <= ) =>;

   #define operation  ::= ^ <= greater_operation ( => ">" object <= ) =>;

   #define operation  ::= ^ <= notgreater_operation ( => "<=" object <= ) =>;

   #define operation  ::= ^ <= add_operation ( => "+" object <= ) =>;

   #define operation  ::= ^ <= sub_operation ( => "-" object <= ) =>;

   #define object         ::= <= object ( => literal <= ) =>;
   #define object         ::= <= object ( => integer <= ) =>;
   #define object         ::= <= object ( => identifier <= ) =>;
   #define object         ::= <= expression ( => m_call_expr <= ) =>;
   #define object         ::= "(" expression ")";

   #define m_call_expr    ::= $ m_call;

   #define m_call         ::= ^
<=
       message_operation (
         object ( identifier = self )
=>
                              m_name "(" args? ")"
<=
       )
=>;

   #define args           ::= expression next_arg*;
   #define next_arg       ::= "," expression;

   #define print          ::= 
<=
    expression (
     message_operation (
       object ( reference = system'console )
       message ( identifier = writeLine )

=>
                              expression
<=
     )
    )
=>;

   #define f_name         ::=
<= 
         nameattr ( identifier = $current 
=>  
                              (!"print" | "if" | "var" | "while") 
<=       ) =>;

   #define m_name         ::=
<= 
         message ( identifier = $current )

=>  
                              (!"print" | "if" | "var" | "while");

   #define f_arg          ::= <= parameter ( nameattr ( identifier = $identifier )) =>;

   #define literal        ::= <= literal = "$literal" =>;
   #define integer        ::= <= integer = $numeric =>;
   #define identifier     ::= <= identifier = $identifier =>;
]]
