[[
   #grammar cf

   #define start          ::= <= root ( public_namespace ( => import* member+ $eof <= )) =>;
   #define start          ::= $eof;

   #define member         ::= function;

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

   #define statement      ::= expression;
   #define statement      ::= ret_expr;
   #define statement      ::= "var" decl_variable;
   #define statement      ::= branching ;

   #define decl_variable  ::= <= expression ( assign_operation ( => new_variable ":=" expression <= ) ) =>;

   #define ret_expr       ::= <= returning ( => "^" expression <= ) =>;

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

   #define expression     ::= <= expression ( => l5 <= ) =>;

   #define l3_expression  ::= <= expression ( => l3 <= ) =>;

   #define l4_expression  ::= <= expression ( => l4 <= ) =>;

   #define l5             ::= $ object l1_operation* l2_operation* l4_operation* l5_operation?;

   #define l4             ::= $ object l1_operation* l2_operation* l4_operation*;

   #define l3             ::= $ object l1_operation* l2_operation*;

   #define l2             ::= $ object l1_operation* l2_operation*;

   #define l1_operation   ::= function_call;

   #define l2_operation   ::= "." message mssg_call;

   #define l4_operation   ::= ^ <= add_operation ( => "+" l3_expression <= ) =>;

   #define l5_operation   ::= ^ <= equal_operation ( => "==" l4_expression <= ) =>;

   #define mssg_call      ::= ^ <= message_operation ( =>  args <= ) =>;

   #define function_call  ::= ^ <= message_operation ( =>  args <= ) =>;

   #define args           ::= "(" ")";
   #define args           ::= "(" arg next_arg* ")";

   #define arg            ::= expression;
   #define next_arg       ::= "," arg;

   #define new_variable   ::= <= new_variable ( => identifier <= ) =>;

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

   #define object         ::= <= object ( => terminal <= ) =>;

   #define terminal       ::= identifier;
   #define terminal       ::= reference;
   #define terminal       ::= integer;
   #define terminal       ::= literal;

   #define next_statement ::= ";" statement next_statement;
   #define next_statement ::= ";" "}";
   #define next_statement ::= "}";

   #define f_parameters   ::= "(" parameters;

   #define parameters     ::= parameter { "," parameter }* ")";
   #define parameters     ::= ")";

   #define parameter      ::= <= parameter ( nameattr ( identifier = $identifier )) =>;

   #define message        ::= <= message ( identifier = $identifier ) =>;

   #define name           ::= <= nameattr ( identifier = $identifier ) =>; 

   #define identifier     ::= <= identifier = $identifier =>;
   #define reference      ::= <= reference = $reference =>;
   #define integer        ::= <= integer = $numeric =>;
   #define literal        ::= <= literal = "$literal" =>;
]]