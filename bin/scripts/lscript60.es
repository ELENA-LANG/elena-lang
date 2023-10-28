[[
   #grammar cf

   #define start          ::= <= root ( public_namespace ( => import* member+ $eof <= )) =>;
   #define start          ::= $eof;

   #define member         ::= function;

   #define import         ::= 
<=
    import (
=>
                             "import" identifier ";"
<=
    )
=>;

   #define import         ::= 
<=
    import (
=>
                             "import" reference ";"
<=
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

   #define ret_expr       ::= <= returning ( => "^" expression <= ) =>;

   #define expression     ::= <= expression ( => l4 <= ) =>;

   #define l3_expression  ::= <= expression ( => l4 <= ) =>;

   #define l4             ::= $ object l1_operation* l2_operation* l4_operation*;

   #define l3             ::= $ object l1_operation* l2_operation*;

   #define l2             ::= $ object l1_operation* l2_operation*;

   #define l1_operation   ::= function_call;

   #define l2_operation   ::= "." message mssg_call;

   #define l4_operation   ::= ^ <= add_operation ( => "+" l3_expression <= ) =>;

   #define mssg_call      ::= ^ <= message_operation ( =>  args <= ) =>;

   #define function_call  ::= ^ <= message_operation ( =>  args <= ) =>;

   #define args           ::= "(" ")";
   #define args           ::= "(" arg next_arg* ")";

   #define arg            ::= expression;
   #define next_arg       ::= "," arg;

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