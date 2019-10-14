[[
   #grammar cf

   #define start      ::= <= root ( namespace ( => member+ $eof <= )) =>;
   #define start      ::= $eof;

   #define member     ::= symbol;

   #define symbol     ::= <= symbol ( => s_name "=" expression ";" <= ) =>;

   #define expression ::= object ;

   #define object     ::= singleton;

   #define singleton  ::= <= nested ( => "{" "}" <= ) =>;

   #define s_name     ::= <= nameattr ( identifier = $identifier ) =>;
]]
