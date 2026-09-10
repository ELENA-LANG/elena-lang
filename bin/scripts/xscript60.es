[[
   #grammar cf

   #define start          ::= <= root ( public_namespace ( public_singleton ( nameattr ( identifier = Program ) function ( code ( => statement+ $eof <= ))))) =>;
   #define start          ::= $eof;

   #define statement      ::= expression ";";
   #define statement      ::= ret_expr ";";

   #define ret_expr       ::= <= returning ( => "^" expression <= ) =>;

   #define expression     ::= <= expression ( => l5 <= ) =>;

   #define l5             ::= $ object;

   #define object         ::= <= object ( => terminal <= ) =>;

   #define terminal       ::= integer;

   #define integer        ::= <= integer = $numeric =>;
]]