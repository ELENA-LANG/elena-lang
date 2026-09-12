[[
   #grammar cf

   #define start          ::= <= root ( public_namespace ( public_singleton ( nameattr ( identifier = Program ) function ( code ( => statement+ $eof <= ))))) =>;
   #define start          ::= $eof;

   #define method         ::= <= get_method ( => name ret_statement ";" <= ) =>;
   #define method         ::= <= script_method ( => name f_parameters body <= ) =>;

   #define body           ::= 
<=
     code (
=>
                              "{" statement* "}"
<=
     )
=>;

   #define ret_statement  ::= <= returning ( => "=" expression <= ) =>;

   #define statement      ::= expression ";";
   #define statement      ::= ret_expr ";";
   #define statement      ::= var_statement ";";
   #define statement      ::= "let" import_statement ";";

   #define ret_expr       ::= <= returning ( => "^" expression <= ) =>;

   #define var_statement  ::= "var" decl_variable;

   #define decl_variable  ::= <= expression ( assign_operation ( => new_variable ":=" expression <= ) ) =>;

   #define import_statement ::= <= expression ( assign_operation ( => new_variable ":=" importer <= ) ) =>;
   #define importer       ::= "require" "(" import_link ")";

   #define expression     ::= <= expression ( => l5 <= ) =>;

   #define l2_expression  ::= <= expression ( => l2 <= ) =>;

   #define l3_expression  ::= <= expression ( => l3 <= ) =>;

   #define l5             ::= $ object l2_operation* l3_operation* l4_operation*;

   #define l3             ::= $ object l2_operation* l3_operation*;

   #define l2             ::= $ object l2_operation*;

   #define l2_operation   ::= "." message mssg_call;
   #define l2_operation   ::= "." message prop_call;

   #define l3_operation   ::= ^ <= mul_operation ( => "*" l2_expression <= ) =>;
   #define l3_operation   ::= ^ <= div_operation ( => "/" l2_expression <= ) =>;

   #define l4_operation   ::= ^ <= add_operation ( => "+" l3_expression <= ) =>;
   #define l4_operation   ::= ^ <= sub_operation ( => "-" l3_expression <= ) =>;

   #define mssg_call      ::= ^ <= message_operation ( =>  args <= ) =>;
   #define prop_call      ::= ^ <= property_operation ( => not_bracket <= ) =>;

   #define args           ::= "(" ")";
   #define args           ::= "(" arg next_arg* ")";

   #define arg            ::= expression;
   #define next_arg       ::= "," arg;

   #define singleton      ::= <= nested ( => "{" method* "}" <= ) =>;

   #define object         ::= <= object ( => terminal <= ) =>;
   #define object         ::= singleton;

   #define new_variable   ::= <= new_variable ( => identifier <= ) =>;

   #define message        ::= <= message ( identifier = $identifier ) =>;

   #define f_parameters   ::= "(" parameters;

   #define parameters     ::= parameter { "," parameter }* ")";
   #define parameters     ::= ")";

   #define parameter      ::= <= parameter ( nameattr ( identifier = $identifier )) =>;

   #define terminal       ::= integer;
   #define terminal       ::= identifier;
   #define terminal       ::= literal;

   #define name           ::= <= nameattr ( identifier = $identifier ) =>; 

   #define identifier     ::= <= identifier = $identifier =>;
   #define integer        ::= <= integer = $numeric =>;
   #define literal        ::= <= literal = "$literal" =>;

   #define not_bracket    ::= $if (!"(");

   #define import_link    ::= <= expression ( message_operation ( object ( reference = extensions'scripting'Exports ) message ( identifier = at ) expression ( object ( => literal <= ) ) ) ) =>;
]]