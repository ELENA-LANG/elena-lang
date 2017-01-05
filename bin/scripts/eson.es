[[
   #grammar cf

   #define start      ::= <= root ( => members <= ) =>;
   #define start      ::= $eof;

   #define members    ::= member ";" members;
   #define members    ::= $eof;
   #define member     ::= <= symbol_decl ( => s_name "=" expression <= ) =>;

   #define expression ::= <= expression ( => object <= ) =>; 

   #define object     ::= numeric;

   #define numeric    ::= <= numeric = $numeric =>;
   #define s_name     ::= <= identifier = $identifier =>;
]]
