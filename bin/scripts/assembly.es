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
   #define expr_member ::=   literal;
   #define expr_member ::=   identifier;

   #define nested      ::= <= += "%""openSingleton[0]""" =>
                             "nested" "(" identifier? method* ")"
                            <= += "%""close[0]""" =>;

   #define method      ::= <= 
                              += "%""openMethod[0]""" 
                              += """self""" += "%""system'dynamic'tapeOp.tape_swap_top[]""" += "%""newParamToken[1]""" 
                           =>
                             "method" "(" message parameter* meth_body ")"
                           <= += "%""close[0]""" =>;

   #define parameter   ::=   "parameter" "=" ident_quote
                           <= += "%""system'dynamic'tapeOp.tape_swap_top[]""" += "%""newParamToken[1]""" => ;

   #define meth_body   ::= ret_expr;
   #define meth_body   ::= <= += "%""openCode[0]""" =>
                             "code" "(" code ")"
                           <=  += "%""close[0]""" =>;

   #define code        ::= statement* ret_expr?;

   #define statement   ::= expression;

   #define ret_expr    ::= <= += "%""openReturning[0]""" =>
                             "returning" "(" expression ")"
                           <=  += "%""close[0]""" =>;

   #define message     ::=   "message" "=" ident_quote
                           <= += "%""system'dynamic'tapeOp.tape_swap_top[]""" += "%""newMessageToken[1]""" => ;

   #define identifier  ::=   "identifier" "=" ident_quote
                           <= += "%""system'dynamic'tapeOp.tape_swap_top[]""" += "%""newIdentToken[1]""" => ;

   #define literal     ::=   "literal" "=" str_quote
                           <= += "%""system'dynamic'tapeOp.tape_swap_top[]""" += "%""newLiteralToken[1]""" => ;

   #define ident_quote ::= <= >> += " """ => ident_token <= """ " > =>;

   #define str_quote   ::= <= >> += " """ => str_token <= """ " =>;

   #define ident_token ::= <= "$identifier" =>;

   #define str_token   ::= <= "$literal" =>;

]]
