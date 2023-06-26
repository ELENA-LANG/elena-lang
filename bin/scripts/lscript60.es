[[
   #grammar cf

   #define start          ::= <= root ( => member+ $eof <= ) =>;
   #define start          ::= $eof;

   #define member         ::= symbol;
   
   #define symbol         ::= <= public_symbol ( => s_name "=" get_expression ";" <= ) =>;
   #define get_expression ::= <= get_expression ( => expression <= ) =>;

   #define statement      ::= expression ";";
   #define expression     ::= <= expression ( => expr_operation <= ) =>;
   #define ret_expr       ::= <= returning ( => "^" expression ";" <= ) =>;

   #define expr_operation ::= $ object operation* ;

   #define object         ::= <= object ( => identifier <= ) =>;
   #define object         ::= <= object ( => integer <= ) =>;
   #define object         ::= singleton;

   #define operation      ::= ^ <= message_operation ( => "." message m_args <= ) =>;
   #define operation      ::= ^ <= property_operation ( => "." message <= ) =>;

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

   #define message        ::= <= message = $identifier =>;

   #define s_name         ::= <= nameattr ( identifier = $identifier ) =>;
   #define m_name         ::= <= nameattr ( identifier = $identifier ) =>;

   #define identifier     ::= <= identifier = $identifier =>;
   #define integer        ::= <= integer = $numeric =>;
]]