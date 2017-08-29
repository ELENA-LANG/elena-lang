[[   
   #grammar transform
   #grammar cf

   #define start       ::= module $eof;
   #define start       ::= $eof;

   #define module      ::= <= ( > += "2" += "%""system'dynamic'tapeOp.tape_var[]""" => 
                             "root" "(" symbol* ")" 
                           <= "*system'dynamic'Tape=" # ) =>;

   #define symbol      ::=   symbol_expr;

   #define symbol_expr ::= <= += "%""openSymbol[0]"""  => 
                             "symbol" "(" identifier expression ")" 
                           <= += "%""close[0]""" =>;

   #define expression  ::= <= += "%""openExpression[0]""" =>
                             "expression" "(" expr_member+ ")"
                           <= += "%""close[0]""" =>;

   #define expr_member ::=   nested;

   #define nested      ::= <= += "%""openSingleton[0]""" =>
                             "nested" "(" ")"
                            <= += "%""close[0]""" =>;

   #define identifier  ::=   "identifier" "=" ident_quote
                           <= += "%""system'dynamic'tapeOp.tape_swap_top[]""" += "%""newIdentToken[1]""" => ;

   #define ident_quote ::= <= >> += " """ => ident_token <= """ " > =>;

   #define ident_token ::= <= "$identifier" =>;
]]
