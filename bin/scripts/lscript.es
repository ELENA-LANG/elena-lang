[[
   #grammar cf

   #define start          ::= <= root ( namespace ( => member+ $eof <= )) =>;
   #define start          ::= $eof;

   #define member         ::= symbol;

   #define symbol         ::= <= symbol ( => s_name "=" expression ";" <= ) =>;

   #define expression     ::= object ;

   #define object         ::= singleton;

   #define singleton      ::= <= nested ( => "{" method* "}" <= ) =>;

   #define method         ::= <= method ( => m_name "(" parameters? ")" body <= ) =>;  
   #define method         ::= <= get_method ( => m_name "=" ret_body <= ) =>;  

   #define parameters     ::= parameter next_parameter*;
   #define next_parameter ::= "," parameter;

   #define body           ::= <= code ( => "{" statement* "}" <= ) =>;
   #define ret_body       ::= <= code ( ret_expression ( => "=" expression ";" <= ) ) =>;
   #define statement      ::= expression ";";
   #define expression     ::= <= expression ( => object operations? <= ) =>; 
   #define expression     ::= <= expression ( => singleton <= ) =>; 

   #define object         ::= identifier;

   #define operations     ::= "." property;
   #define property       ::= message prop_parameter;

   #define s_name         ::= <= nameattr ( identifier = $identifier ) =>;
   #define m_name         ::= <= nameattr ( identifier = $identifier ) =>;
   #define parameter      ::= <= parameter ( nameattr ( identifier = $identifier )) =>;

   #define message        ::= <= message = $identifier =>;
   #define identifier     ::= <= identifier = $identifier =>;
   #define prop_parameter ::= <= property_parameter =>;
]]
