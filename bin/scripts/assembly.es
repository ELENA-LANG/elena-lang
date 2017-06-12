[[   
   #grammar transform
   #grammar cf

   #define start       ::= module $eof;
   #define start       ::= $eof;

   #define module      ::= <= ( => 
                             "root" "(" include* symbol* ")" 
                           <= > += "%""init[0]""" "*system'dynamic'Tape=" # ) =>;

   #define include     ::= <= ( < += "2 " > += "%""system'dynamic'tapeOp.tape_var[]""" += "%""include[2]""" => 
                             "include" "(" forward identifier_v ")" 
                           <= ) ~ =>;

   #define forward     ::= "forward" "(" reference_v ")";

   #define symbol      ::= symbol_expr;
   #define symbol      ::= <= ( < += "2 " > += "%""system'dynamic'tapeOp.tape_var[]"""=>
                              singleton
                            <= ) =>;

   #define symbol_expr ::= <= ( < += "2 " > += "%""system'dynamic'tapeOp.tape_var[]""" += "%""open&Symbol[0]"""  => 
                              "symbol" "(" identifier expression ")" 
                           <=  > += "%""close[0]""" ) =>;

   #define symbol_expr ::= <= ( < += "2 " > += "%""system'dynamic'tapeOp.tape_var[]""" += "%""open&Preloaded_symbol[0]"""  => 
                              "preloaded_symbol" "(" identifier expression ")" 
                           <=  > += "%""close[0]""" ) =>;

   #define expression  ::= <= > += "%""open&Expression[0]""" =>
                             "expression" "(" expr_member+ ")"
                           <= > += "%""close[0]""" =>;

   #define expr_member ::= nested;
   #define expr_member ::= numeric;
   #define expr_member ::= identifier;
   #define expr_member ::= message;
   #define expr_member ::= reference;
   #define expr_member ::= literal;
   #define expr_member ::= expression;
   #define expr_member ::= assign;

   #define singleton   ::= <= > += "%""open&Singleton[0]""" =>
                             "singleton" "(" identifier? method* ")"
                            <= > += "%""close[0]""" =>;

   #define nested      ::= <= > += "%""open&Singleton[0]""" =>
                             "nested" "(" identifier? method* ")"
                            <= > += "%""close[0]""" =>;

   #define method      ::= <= > += "%""open&Method[0]""" += "%""new&ParamToken[1]""" < += """self""" =>
                             "method" "(" message parameter* meth_body ")"
                           <= > += "%""close[0]""" =>;

   #define meth_body   ::= ret_expr;
   #define meth_body   ::= <= > += "%""open&Code[0]""" =>
                             "code" "(" code ")"
                           <=  > += "%""close[0]""" =>;

   #define code        ::= statement* ret_expr?;

   #define statement   ::= expression;
   #define statement   ::= "variable" "(" variable ")";

   #define variable    ::= <= > += "%""open&Variable[0]""" =>
                              identifier
                           <=  > += "%""close[0]""" =>;
             
   #define ret_expr    ::= <= > += "%""open&Ret_expr[0]""" =>
                             "returning" "(" expression ")"
                           <=  > += "%""close[0]""" =>;

   #define numeric     ::= <= > += "%""new&NumericToken[1]""" =>
                             "numeric" "=" num_quote;

   #define literal     ::= <= > += "%""new&LiteralToken[1]""" =>
                             "literal" "=" str_quote;

   #define identifier  ::= <= > += "%""new&IdentToken[1]""" =>
                             "identifier" "=" ident_quote;

   #define message     ::= <= > += "%""new&MessageToken[1]""" =>
                             "message" "=" ident_quote;

   #define parameter   ::= <= > += "%""new&ParamToken[1]""" =>
                             "parameter" "=" ident_quote;

   #define reference   ::= <= > += "%""new&ReferenceToken[1]""" =>
                             "reference" "=" ref_quote;

   #define assign      ::= <= > += "%""new&AssignToken[1]""" =>
                             "assign" "=" num_quote;

   #define reference_v ::= "reference" "=" ref_quote;

   #define identifier_v::= "identifier" "=" ident_quote;

   #define num_quote   ::= <= << += """ " => num_token  <= " """ =>;

   #define str_quote   ::= <= << += """ " => str_token <= " """ =>;

   #define ident_quote ::= <= << += """ " => ident_token <= " """ =>;

   #define ref_quote   ::= <= << += """ " => ref_token <= " """ =>;

   #define num_token   ::= <= "$numeric" =>;

   #define str_token   ::= <= "$literal" =>;

   #define ident_token ::= <= "$identifier" =>;

   #define ref_token   ::= <= "$reference" =>;

]]
