[[
   #grammar cf

   #define start          ::= <= root ( public_namespace ( => member+ $eof <= )) =>;
   #define start          ::= $eof;

   #define member         ::= import;
   #define member         ::= function;

   #define import         ::= 
<=
    import (
=>
                             "import" identifier ";"
<=
    )
=>;

   #define function       ::= 
<=
   singleton (
=>
                              public_prefix? name f_decl
<=
   )
=>;

   #define f_decl         ::=
<=
     function (
=>
                              f_args body 
<=
     )
=>;

   #define f_args         ::= "(" ")";

   #define body           ::= 
<=
     code (
=>
                              "{" statement* l_statement? "}"
<=
     )
=>;

   #define statement      ::= l_statement ";";

   #define l_statement    ::= expression;
   #define l_statement    ::= "var" decl_variable;
   #define l_statement    ::= a_expression ;

   #define decl_variable  ::= 
<=
        expression 
        (
=>
                              a_new_variable ":=" expression
<=
        )
=>;

   #define a_new_variable ::= new_variable
<=
           assign = 0
=>;

   #define a_expression     ::= 
<=
        expression (
=>
                              object ":=" assigning
<=
        )
=>;

   #define assigning         ::=
<=
           assign = 0
=>
                              expression;

   #define s_expression     ::= 
<=
        expression (
=>
                              s_object
<=
        )
=>;

   #define expression     ::= 
<=
        expression (
=>
                              l4_expression
<=
        )
=>;

   #define l4_expression  ::= l3_expression;

   #define l4_expression  ::= 
<=
        expression (
=>
                           l3_expression l4_operation l4_operations*
<=
        )
=>;

   #define l3_expression  ::= l2_expression;

   #define l3_expression  ::= 
<=
        expression (
=>
                           l2_expression l3_operation l3_operations*
<=
        )
=>;

   #define l2_expression  ::= l1_expression;

   #define l2_expression  ::= 
<=
        expression (
=>
                           l1_expression l2_operation l2_operations*
<=
        )
=>;

   #define l1_expression  ::= object;

   #define l2_expression  ::= 
<=
        expression (
=>
                           object l1_operation l1_operations*
<=
        )
=>;

   #define l1_operation   ::= <= message = 0 => function_args;
   #define l2_operations  ::= <= ; message = 0 => function_args;

   #define l2_operation   ::= "." message m_args;
   #define l2_operations  ::= <= ; => l2_operation;

   #define l3_operation   ::= <= operator = "*" => "*" l2_expression;
   #define l3_operation   ::= <= operator = "/" => "/" l2_expression;
   #define l3_operations  ::= <= , => l3_operation;

   #define l4_operation   ::= <= operator = "+" => "+" l3_expression;
   #define l4_operation   ::= <= operator = "-" => "-" l3_expression;
   #define l4_operations  ::= <= , => l4_operation;

   #define object         ::= s_object;
   #define object         ::= "new" new_identifier;

   #define s_object       ::= integer;
   #define s_object       ::= literal;
   #define s_object       ::= reference;
   #define s_object       ::= identifier;

   #define function_args  ::= "(" ")";
   #define function_args  ::= "(" expression { "," expression }* ")";

   #define m_args         ::= "(" ")";
   #define m_args         ::= "(" expression { "," expression }* ")";
   #define m_args         ::= ":" s_expression;

   #define name           ::= <= nameattr ( identifier = $identifier ) =>; 

   #define new_variable   ::= <= variable_identifier = $identifier =>; 
   #define new_identifier ::= <= new_identifier = $identifier =>;

   #define message        ::= <= message = $identifier =>;
   #define identifier     ::= <= identifier = $identifier =>;
   #define integer        ::= <= integer = $numeric =>;
   #define literal        ::= <= literal = "$literal" =>;
   #define reference      ::= <= reference = $reference =>;

   #define public_prefix  ::= "public";
]]