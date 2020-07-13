[[
   #grammar cf

   #define start          ::= module $eof;
   #define start          ::= $eof;

   #define module         ::= 
<= 
  root ( 
    public_namespace (
      symbol (
        nameattr ( identifier = program )
        expression (
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
=>
                              "if" "(" expression ")" if_code_brackets
<=
             )
=>;

   #define if_code_brackets ::= <= operator = "?" => code_brackets;

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

   #define expression     ::= 
<=
             expression
             (
=>
                              object operation?
<=
	     )
=>;

   #define operation  ::= <= operator = "==" => "==" object;

   #define operation  ::= <= operator = "<" => "<" object;

   #define operation  ::= <= operator = ">" => ">" object;

   #define operation  ::= <= operator = "<=" => "<=" object;

   #define operation  ::= <= operator = "+" => "+" object;

   #define operation  ::= <= operator = "-" => "-" object;

   #define object         ::= literal;
   #define object         ::= integer;
   #define object         ::= identifier;
   #define object         ::= <= expression ( => m_call <= ) =>;
   #define object         ::= "(" expression ")";

   #define m_call         ::= 
<=
       identifier = self
=>
                              m_name "(" args? ")";

   #define args           ::= expression next_arg*;
   #define next_arg       ::= "," expression;

   #define print          ::= 
<=
    expression (
       reference = system'console
       message = writeLine

=>
                              expression
<=
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
         message = $current 
=>  
                              (!"print" | "if" | "var" | "while");

   #define f_arg          ::= <= parameter ( nameattr ( identifier = $identifier )) =>;

   #define literal        ::= <= literal = "$literal" =>;
   #define integer        ::= <= integer = $numeric =>;
   #define identifier     ::= <= identifier = $identifier =>;
]]
