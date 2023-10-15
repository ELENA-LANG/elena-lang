[[
   #grammar cf

   #define start          ::= <= root ( namespace ( => member+ $eof <= ) ) =>;
   #define start          ::= $eof;

   #define member         ::= symbol;
   
   #define symbol         ::= <= public_symbol ( => s_name "=" get_expression ";" <= ) =>;

   #define statement      ::= expression ";";
   #define get_expression ::= <= get_expression ( => expr_operation <= ) =>;

   #define expression     ::= <= expression ( => expr_operation <= ) =>;
   #define ret_expr       ::= <= returning ( => "^" expression ";" <= ) =>;

   #define expr_operation ::= $ object operation*;

   #define object         ::= <= object ( => terminal <= ) =>;
   #define object         ::= singleton;

   #define terminal       ::= identifier;
   #define terminal       ::= reference;
   #define terminal       ::= integer;
   #define terminal       ::= literal;

   #define operation      ::= "." message mssg_call;
   #define operation      ::= "." message prop_call;
   #define operation      ::= function_call;

   #define mssg_call      ::= ^ <= message_operation ( =>  m_args <= ) =>;
   #define prop_call      ::= ^ <= property_operation ( => not_bracket <= ) => ;
   #define function_call  ::= ^ <= message_operation ( => m_args <= ) =>;

   #define not_bracket    ::= $if (!"(");

   #define m_args         ::= "(" ")";
   #define m_args         ::= "(" m_arg next_arg* ")";

   #define m_arg          ::= expression;
   #define next_arg       ::= "," m_arg;

   #define singleton      ::= <= nested ( => "{" method* "}" <= ) =>;  

   #define method         ::= <= get_method ( => m_name ret_body <= ) =>;
   #define method         ::= <= script_method ( => m_name "(" parameters? ")" body <= ) =>;  

   #define ret_body       ::= <= returning ( => "=" expression ";" <= ) =>;
   #define body           ::= <= code ( => "{" statement* ret_expr? "}" <= ) =>;

   #define parameters     ::= parameter next_parameter*;
   #define next_parameter ::= "," parameter;

   #define parameter      ::= <= parameter ( nameattr ( identifier = $identifier )) =>;

   #define message        ::= <= message ( identifier = $identifier ) =>;

   #define s_name         ::= <= nameattr ( identifier = $identifier ) =>;
   #define m_name         ::= <= nameattr ( identifier = $identifier ) =>;

   #define identifier     ::= <= identifier = $identifier =>;
   #define reference      ::= <= reference = $reference =>;
   #define integer        ::= <= integer = $numeric =>;
   #define literal        ::= <= literal = "$literal" =>;
]]