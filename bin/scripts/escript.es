[[
   #grammar cf

   #define start      ::= <= root ( => class <= ) =>;
   #define start      ::= $eof;
   #define class      ::= <= class ( => "{" members <= ) =>;
   #define members    ::= method members;
   #define members    ::=  "}";
   #define method     ::= <= method ( => name parameters body <= ) =>;
   #define parameters ::= "(" param;
   #define param      ::= ")";
   #define body       ::= <= code ( => "{" statements <= ) => ;
   #define statements ::= statement next_stat;
   #define statements ::= "}";
   #define next_stat  ::= "}";
   #define statement  ::= expression;
   #define expression ::= <= expression ( => object operations <= ) =>; 
   #define operations ::= call;
   #define operations ::= $eps;
   #define call       ::= "." message msg_params;
   #define msg_params ::= "(" object next_m_prm;
   #define next_m_prm ::= ")";
   #define object     ::= reference;
   #define object     ::= literal;

   #define name       ::= <= message = $identifier =>;
   #define message    ::= <= message = $identifier =>;
   #define reference  ::= <= symbol = $reference =>;
   #define literal    ::= <= literal = "$literal" =>;
]]
