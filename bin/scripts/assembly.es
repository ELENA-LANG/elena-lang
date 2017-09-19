[[   
   #grammar transform
   #grammar cf

   #define start       ::= module $eof;
   #define start       ::= $eof;

   #define module      ::= <= ( > += "2" += "%""system'dynamic'tapeOp.tape_var[]""" => 
                             "root" "(" include* symbol* ")" 
                           <= "*system'dynamic'Tape=" # ) =>;

   #define include     ::=   "include" "(" forward identifier_v ")"
                           <= += "%""include[2]""" =>;

   #define forward     ::= "forward" "(" reference_v ")";

   #define symbol      ::=   symbol_expr;

   #define symbol_expr ::= <= += "%""openSymbol[0]"""  => 
                             "symbol" "(" identifier expression ")" 
                           <= += "%""close[0]""" =>;

   #define symbol_expr ::= <= += "%""openPreloadedSymbol[0]"""  => 
                              "preloaded_symbol" "(" identifier? expression ")" 
                           <= += "%""close[0]""" =>;

   #define expression  ::= <= += "%""openExpression[0]""" =>
                             "expression" "(" expr_member+ ")"
                           <= += "%""close[0]""" =>;

   #define expr_member ::=   nested;
   #define expr_member ::=   numeric;
   #define expr_member ::=   literal;
   #define expr_member ::=   identifier;
   #define expr_member ::=   reference;
   #define expr_member ::=   expression;
   #define expr_member ::=   message;
   #define expr_member ::=   assign;
   #define expr_member ::=   operator;
   #define expr_member ::= <= += "%""nextExpression[0]""" =>
                             ";";

   #define expr_member ::= <= += "%""openCode[0]""" =>
                             "code" "(" code ")"
                           <= += "%""close[0]""" =>;

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
   #define statement   ::= "variable" "(" variable ")";
   #define statement   ::= <= += "%""openLoop[0]""" =>
                             "loop" "(" expression ")"
                           <= += "%""close[0]""" =>;

   #define variable    ::= <= += "%""openVariable[0]""" =>
                              identifier
                           <= += "%""close[0]""" =>;

   #define ret_expr    ::= <= += "%""openReturning[0]""" =>
                             "returning" "(" expression ")"
                           <=  += "%""close[0]""" =>;

   #define message     ::=   "message" "=" ident_quote
                           <= += "%""system'dynamic'tapeOp.tape_swap_top[]""" += "%""newMessageToken[1]""" => ;

   #define operator    ::=   "operator" "=" operator_quote
                           <= += "%""system'dynamic'tapeOp.tape_swap_top[]""" += "%""newMessageToken[1]""" => ;

   #define operator    ::=   "operator" "=" "?"
                           <= += "%""if[0]""" => ;

   #define identifier  ::=   "identifier" "=" ident_quote
                           <= += "%""system'dynamic'tapeOp.tape_swap_top[]""" += "%""newIdentToken[1]""" => ;

   #define numeric     ::=   "numeric" "=" num_quote
                           <= += "%""system'dynamic'tapeOp.tape_swap_top[]""" += "%""newNumericToken[1]""" => ;

   #define literal     ::=   "literal" "=" str_quote
                           <= += "%""system'dynamic'tapeOp.tape_swap_top[]""" += "%""newLiteralToken[1]""" => ;

   #define reference   ::=   "reference" "=" ref_quote
                           <= += "%""system'dynamic'tapeOp.tape_swap_top[]""" += "%""newReference[1]""" => ;

   #define assign      ::=   "assign" "=" num_quote
                           <= += "%""system'dynamic'tapeOp.tape_swap_top[]""" += "%""newAssignToken[1]""" => ;

   #define reference_v ::= "reference" "=" ref_quote
                           <= += "%""system'dynamic'tapeOp.tape_swap_top[]""" => ;

   #define identifier_v::= "identifier" "=" ident_quote
                           <= += "%""system'dynamic'tapeOp.tape_swap_top[]""" => ;

   #define operator_quote ::= <= >> += " ""equal"" " > =>
                             "==";

   #define operator_quote ::= <= >> += " ""less"" " > =>
                             "<";

   #define operator_quote ::= <= >> += " ""greater"" " > =>
                             ">";

   #define operator_quote ::= <= >> += " ""notgreater"" " > =>
                             "<=";

   #define operator_quote ::= <= >> += " ""add"" " > =>
                             "+";

   #define operator_quote ::= <= >> += " ""subtract"" " > =>
                             "-";

   #define ident_quote ::= <= >> += " """ => ident_token <= """ " > =>;

   #define str_quote   ::= <= >> += " """ => str_token <= """ " > =>;

   #define ref_quote   ::= <= >> += " """ => ref_token <= """ " > =>;

   #define num_quote   ::= <= >> += " """ => num_token  <= """ " > =>;

   #define ident_token ::= <= "$identifier" =>;

   #define str_token   ::= <= "$literal" =>;

   #define ref_token   ::= <= "$reference" =>;

   #define num_token   ::= <= "$numeric" =>;
]]
