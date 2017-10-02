[[
   #grammar cf

   #define start      ::= <= root ( => class <= ) =>;
   #define start      ::= $eof;
   #define class      ::= <= singleton ( => "{" members <= ) =>;
   #define members    ::= method members;
   #define members    ::=  "}";
   #define method     ::= <= method ( => name parameters body <= ) =>;
   #define parameters ::= "(" param;
   #define param      ::= parameter next_param;
   #define param      ::= ")";
   #define next_param ::= "," param;
   #define next_param ::= ")";
   #define body       ::= <= code ( => "{" ret_statement <= ) => ;
   #define statements ::= statement next_stat;
   #define statements ::= "}";
   #define next_stat  ::= "}";
   #define ret_statement ::= <= returning ( => "^" expression "}" <= ) =>;
   #define statement  ::= expression;
   #define expression ::= <= expression ( => object operations <= ) =>; 
   #define operations ::= call;
   #define operations ::= $eps;
   #define call       ::= message msg_params;
   #define msg_params ::= "(" object next_m_prm;
   #define next_m_prm ::= "," object next_m_prm;
   #define next_m_prm ::= ")";
   #define object     ::= reference;
   #define object     ::= literal;
   #define object     ::= integer;
   #define object     ::= identifier;

   #define name       ::= <= message = $identifier =>;
   #define message    ::= <= message = $identifier =>;
   #define reference  ::= <= reference = $reference =>;
   #define literal    ::= <= literal = "$literal" =>;
   #define parameter  ::= <= parameter = $identifier =>;
   #define integer    ::= <= numeric = $numeric =>;
   #define identifier ::= <= identifier = $identifier =>;
]]
