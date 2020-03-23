[[
   #grammar cf

   #define start          ::= <= root ( namespace ( => member+ $eof <= )) =>;
   #define start          ::= $eof;

   #define member         ::= symbol;

   #define symbol         ::= <= symbol ( => s_name "=" expression ";" <= ) =>;

   #define expression     ::= <= expression ( => object operations? <= ) =>; 

   #define object         ::= singleton;
   #define object         ::= identifier;
   #define object         ::= integer;
   #define object         ::= literal;

   #define singleton      ::= <= nested ( => "{" method* "}" <= ) =>;

   #define method         ::= <= script_method ( => m_name "(" parameters? ")" body <= ) =>;  
   #define method         ::= <= get_method ( => m_name ret_body <= ) =>;  

   #define parameters     ::= parameter next_parameter*;
   #define next_parameter ::= "," parameter;

   #define body           ::= <= code ( => "{" statement* "}" <= ) =>;
   #define ret_body       ::= <= code ( returning ( => "=" expression ";" <= ) ) =>;
   #define statement      ::= expression ";";

   #define operations     ::= "." property;
   #define operations     ::= "." message m_args;

   #define m_args         ::= "(" ")";
   #define m_args         ::= "(" m_arg next_arg* ")";

   #define m_arg          ::= expression;
   #define next_arg       ::= "," m_arg;

   #define property       ::= message <= property_parameter ( ) =>;

   #define s_name         ::= <= nameattr ( identifier = $identifier ) =>;
   #define m_name         ::= <= nameattr ( identifier = $identifier ) =>;
   #define parameter      ::= <= parameter ( nameattr ( identifier = $identifier )) =>;

   #define message        ::= <= message = $identifier =>;
   #define identifier     ::= <= identifier = $identifier =>;
   #define integer        ::= <= integer = $numeric =>;
   #define literal        ::= <= literal = "$literal" =>;
]]
