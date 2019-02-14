[[   
   #grammar transform
   #grammar cf

   #define start       ::= module $eof;
   #define start       ::= $eof;

   #define module      ::= <= ( > => 
                             "root" "(" "namespace" "(" include* symbol* ")" ")" 
                           <= "*system'dynamic'ClosureTape=" # ) =>;

   #define include     ::=   "include" "(" forward identifier_v ")"
                           <= += " extensions'dynamic'includeClosure ^ ""new[2]"" " =>;

   #define forward     ::= "forward" "(" reference_v ")";

   #define symbol      ::=   symbol_expr;
   #define symbol      ::=   singleton_expr;

   #define singleton_expr ::= <= += " extensions'dynamic'openSingletonClosure "  => 
                                "singleton" "(" method* ")" 
                              <= += " extensions'dynamic'closeClosure " =>;

   #define symbol_expr ::= <= += " extensions'dynamic'openSymbolClosure "  => 
                             "symbol" "(" s_identifier expression ")" 
                           <= += " extensions'dynamic'closeClosure " =>;

   #define symbol_expr ::= <= += " extensions'dynamic'openPreloadedSymbolClosure "  => 
                              "preloaded_symbol" "(" identifier? expression ")" 
                              <= += " extensions'dynamic'closeClosure " =>;

   #define expression  ::= <= += " extensions'dynamic'openExpressionClosure " =>
                             "expression" "(" expr_member+ ")"
                              <= += " extensions'dynamic'closeClosure " =>;

   #define expr_member ::=   nested;
   #define expr_member ::=   numeric;
   #define expr_member ::=   literal;
   #define expr_member ::=   identifier;
   #define expr_member ::=   reference;
   #define expr_member ::=   expression;
   #define expr_member ::=   message;
   #define expr_member ::=   assign;
   #define expr_member ::=   operator;
   #define expr_member ::= <= += " extensions'dynamic'nextClosure " =>
                             ";";

   #define expr_member ::= <= += " extensions'dynamic'openCodeClosure " =>
                             "code" "(" code ")"
                           <= += " extensions'dynamic'closeClosure " =>;

   #define nested      ::= <= += " extensions'dynamic'openSingletonClosure " =>
                             "nested" "(" identifier? method* ")"
                              <= += " extensions'dynamic'closeClosure " =>;

   #define method      ::= <= 
                              += " extensions'dynamic'openMethodClosure " 
                           =>
                             "method" "(" message_ident meth_params meth_body ")"
                              <= += " extensions'dynamic'closeClosure " =>;

   #define meth_params ::= <= += " ""__target"" extensions'dynamic'newParamTokenClosure ^ ""new[1]"" " => parameter*;

   #define parameter   ::=   "parameter" "(" "nameattr" "(" "identifier" "=" ident_quote ")" ")"
                           <= += " extensions'dynamic'newParamTokenClosure ^ ""new[1]"" " => ;

   #define meth_body   ::= ret_expr;
   #define meth_body   ::= <= += " extensions'dynamic'openCodeClosure " =>
                             "code" "(" code ")"
                           <= += " extensions'dynamic'closeClosure " =>;

   #define code        ::= statement* ret_expr?;

   #define statement   ::= expression;
   #define statement   ::= "variable" "(" variable ")";
   #define statement   ::= <= += " extensions'dynamic'openLoopClosure " =>
                             "loop" "(" expression ")"
                           <= += " extensions'dynamic'closeClosure " =>;

   #define variable    ::= <= += " extensions'dynamic'openVariableClosure " =>
                              identifier
                           <= += " extensions'dynamic'closeClosure " =>;

   #define ret_expr    ::= <= += " extensions'dynamic'openReturningClosure " =>
                             "returning" "(" expression ")"
                           <= += " extensions'dynamic'closeClosure " =>;

   #define message     ::=   "message" "=" ident_quote
                           <= += " extensions'dynamic'newMessageClosure ^ ""new[1]"" " => ;

   #define operator    ::=   "operator" "=" operator_quote
                           <= += " extensions'dynamic'newMessageClosure ^ ""new[1]"" " => ;

   #define operator    ::=   "operator" "=" "?"
                           <= += " extensions'dynamic'ifClosure " => ;

   #define message_ident ::= "nameattr" "(" "identifier" "=" ident_quote ")" 
                           <= += " extensions'dynamic'newMessageClosure ^ ""new[1]"" " => ;

   #define s_identifier ::= "nameattr" "(" identifier ")" ;

   #define identifier  ::=   "identifier" "=" ident_quote
                           <= += " extensions'dynamic'newIdentifierClosure ^ ""new[1]"" " => ;

   #define numeric     ::=   "numeric" "=" num_quote
                           <= += " extensions'dynamic'newNumericClosure ^ ""new[1]"" " => ;

   #define literal     ::=   "literal" "=" str_quote
                           <= += " extensions'dynamic'newStringClosure ^ ""new[1]"" " => ;

   #define reference   ::=   "reference" "=" ref_quote
                           <= += " extensions'dynamic'newReferenceClosure ^ ""new[1]"" " => ;

   #define assign      ::=   "assign" "=" num_quote
                           <= += " extensions'dynamic'assignClosureFactory ^ ""new[1]"" " => ;

   #define reference_v ::= "reference" "=" ref_quote;

   #define identifier_v::= "nameattr" "(" "identifier" "=" ident_quote ")";

   #define operator_quote ::= <= >> " ""equal"" " > =>
                             "==";

   #define operator_quote ::= <= >> " ""less"" " > =>
                             "<";

   #define operator_quote ::= <= >> " ""greater"" " > =>
                             ">";

   #define operator_quote ::= <= >> " ""notgreater"" " > =>
                             "<=";

   #define operator_quote ::= <= >> " ""add"" " > =>
                             "+";

   #define operator_quote ::= <= >> " ""subtract"" " > =>
                             "-";

   #define ident_quote ::= <= >> " """ => ident_token <= """ " > =>;

   #define str_quote   ::= <= >> " """ => str_token <= """ " > =>;

   #define ref_quote   ::= <= >> " """ => ref_token <= """ " > =>;

   #define num_quote   ::= <= >> " """ => num_token  <= """ " > =>;

   #define num_dummy   ::= $numeric;

   #define ident_token ::= <= "$identifier" =>;

   #define str_token   ::= <= "$literal" =>;

   #define ref_token   ::= <= "$reference" =>;

   #define num_token   ::= <= "$numeric" =>;
]]
