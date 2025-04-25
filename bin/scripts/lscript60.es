[[
   #grammar cf

   #define start          ::= <= root ( public_namespace ( => import* member+ $eof <= )) =>;
   #define start          ::= $eof;

   #define member         ::= function;
   #define member         ::= symbol;

   #define import         ::= 
<=
    import (
       nameattr (
=>
                             "import" identifier ";"
<=
       )
    )
=>;

   #define import         ::= 
<=
    import (
       nameattr (
=>
                             "import" reference ";"
<=
       )
    )
=>;

   #define function       ::= 
<=
   singleton (
=>
                              name f_decl
<=
   )
=>;

   #define function       ::= 
<=
   public_singleton (
=>
                              "public" name f_decl
<=
   )
=>;

   #define f_decl         ::=
<=
     function (
=>
                              f_parameters body 
<=
     )
=>;

   #define body           ::= 
<=
     code (
=>
                              "{" statement next_statement
<=
     )
=>;

   #define symbol         ::= <= public_symbol ( => s_name "=" get_expression ";" <= ) =>;

   #define method         ::= <= get_method ( => name ret_statement ";" <= ) =>;
   #define method         ::= <= script_method ( => name f_parameters body <= ) =>;

   #define var_statement  ::= "var" decl_variable;

   #define statement      ::= expression;
   #define statement      ::= ret_expr;
   #define statement      ::= var_statement;
   #define statement      ::= branching;
   #define statement      ::= looping;
   #define statement      ::= for_looping;
   #define statement      ::= assign_expr;

   #define decl_variable  ::= <= expression ( assign_operation ( => new_variable ":=" expression <= ) ) =>;

   #define ret_expr       ::= <= returning ( => "^" expression <= ) =>;
   #define ret_statement  ::= <= returning ( => "=" expression <= ) =>;

   #define branching      ::= 
<= 
             expression
             (
                branch_operation
                (
=>

                              "if" "(" expression ")" code_brackets else_code_brackets?
<=
                )
             )
=>;
   #define looping      ::= 
<= 
             expression
             (
                loop_expression (
                   if_operation (
=>

                              "while" "(" expression ")" code_brackets
<=
                   )
                )
             )
=>;

   #define for_looping      ::= 
<= 
             virtual_for_loop
             (
=>
                              "for" "(" var_statement ";" expression ";" step_expr ")" code_brackets
<=
             )
=>;

   #define step_expr ::= <= expression ( => l5 <= ) =>;
   #define step_expr ::=
<=
             expression
             (
                assign_operation ( 
=>
                              variable ":=" expression
<=
                )
             )
=>;

   #define assign_expr    ::=
<=
             expression
             (
                assign_operation ( 
=>
                              variable ":=" expression
<=
                )
             )
=>;

   #define get_expression ::= <= get_expression ( => l5 <= ) =>;
   #define expression     ::= <= expression ( => l5 <= ) =>;
   #define expression     ::= <= expression ( => new_operation <= ) =>;

   #define s_expression     ::= 
<=
        expression (
=>
                              s_object
<=
        )
=>;

   #define l2_expression  ::= <= expression ( => l2 <= ) =>;

   #define l3_expression  ::= <= expression ( => l3 <= ) =>;

   #define l4_expression  ::= <= expression ( => l4 <= ) =>;

   #define l5             ::= $ object l0_operation* l1_operation* l2_operation* l3_operation* l4_operation* l5_operation?;

   #define l4             ::= $ object l0_operation* l1_operation* l2_operation* l3_operation* l4_operation*;

   #define l3             ::= $ object l0_operation* l1_operation* l2_operation* l3_operation*;

   #define l2             ::= $ object l0_operation* l1_operation* l2_operation*;

   #define new_operation  ::= <= message_operation ( => "new" new_terminal args <= ) =>;

   #define l0_operation   ::= ^ <= message_operation ( => "[" expression "]" <= ) =>;

   #define l1_operation   ::= function_call;

   #define l2_operation   ::= "." message mssg_call;
   #define l2_operation   ::= "." message prop_call;

   #define l3_operation   ::= ^ <= mul_operation ( => "*" l2_expression <= ) =>;
   #define l3_operation   ::= ^ <= div_operation ( => "/" l2_expression <= ) =>;

   #define l4_operation   ::= ^ <= add_operation ( => "+" l3_expression <= ) =>;
   #define l4_operation   ::= ^ <= sub_operation ( => "-" l3_expression <= ) =>;

   #define l5_operation   ::= ^ <= equal_operation ( => "==" l4_expression <= ) =>;
   #define l5_operation   ::= ^ <= less_operation ( => "<" l4_expression <= ) =>;

   #define mssg_call      ::= ^ <= message_operation ( =>  args <= ) =>;
   #define prop_call      ::= ^ <= property_operation ( => not_bracket <= ) =>;

   #define function_call  ::= ^ <= message_operation ( => args <= ) =>;

   #define args           ::= "(" ")";
   #define args           ::= "(" arg next_arg* ")";

   #define arg            ::= expression;
   #define next_arg       ::= "," arg;

   #define new_variable   ::= <= new_variable ( => identifier <= ) =>;
   #define new_terminal   ::= <= new_identifier ( => identifier <= ) =>;

   #define else_code_brackets ::= "else" code_brackets;

   #define code_brackets ::= 
<=

                 expression (
                    closure (
                       code (
=>
                                "{" statement next_statement
<=
                       )
                    )
                 )
=>;

   #define singleton      ::= <= nested ( => "{" method* "}" <= ) =>;

   #define object         ::= "(" expression ")";
   #define object         ::= singleton;
   #define object         ::= <= object ( => terminal <= ) =>;
   #define s_object       ::= <= object ( => terminal <= ) =>;
   #define variable       ::= <= object ( => identifier <= ) =>;

   #define terminal       ::= identifier;
   #define terminal       ::= reference;
   #define terminal       ::= integer;
   #define terminal       ::= literal;
   #define terminal       ::= character;

   #define next_statement ::= ";" statement next_statement;
   #define next_statement ::= ";" "}";
   #define next_statement ::= "}";

   #define f_parameters   ::= "(" parameters;

   #define parameters     ::= parameter { "," parameter }* ")";
   #define parameters     ::= ")";

   #define parameter      ::= <= parameter ( nameattr ( identifier = $identifier )) =>;

   #define message        ::= <= message ( identifier = $identifier ) =>;

   #define name           ::= <= nameattr ( identifier = $identifier ) =>; 
   #define s_name         ::= <= nameattr ( identifier = $identifier ) =>;

   #define not_bracket    ::= $if (!"(");

   #define identifier     ::= <= identifier = $identifier =>;
   #define reference      ::= <= reference = $reference =>;
   #define integer        ::= <= integer = $numeric =>;
   #define literal        ::= <= literal = "$literal" =>;
   #define character      ::= <= character = $character =>;
]]