[[   
   #grammar transform
   #grammar cf

   #define start       ::= module $eof;
   #define start       ::= $eof;

   #define module      ::= <= ( => 
                             "root" "(" include* symbol* ")" 
                           <= > "*system'dynamic'Tape=" # ) =>;

   #define include     ::= <= ( < += "2" > += "%""system'dynamic'tapeOp.var[]""" += "%""include[2]""" => 
                             "include" "(" reference identifier ")" 
                           <= ) > * =>;

   #define symbol      ::= symbol_expr;
   #define symbol      ::= singleton;

   #define symbol_expr ::= <= ( < += "2" > += "%""system'dynamic'tapeOp.var[]""" => 
                              "symbol" "(" identifier expression ")" 
                           <= ) > * =>;

   #define expression  ::= <= > += "%""open&expression[0]""" =>
                             "expression" "(" expr_member+ ")"
                           <= > += "%""close[0]""" =>;

   #define expr_member ::= singleton;
   #define expr_member ::= numeric;
   #define expr_member ::= identifier;
   #define expr_member ::= message;
   #define expr_member ::= reference;
   #define expr_member ::= literal;

   #define singleton   ::= <=  += "2" > += "%""system'dynamic'tapeOp.var[]""" += "%""open&singleton[0]""" =>
                             "singleton" "(" identifier method* ")"
                            <= > += "%""close[0]""" =>;

   #define method      ::= <= > += "%""open&method[0]""" =>
                             "method" "(" message parameter* meth_body ")"
                           <= > += "%""close[0]""" =>;

   #define meth_body   ::= ret_expr;
   #define meth_body   ::= <= > += "%""open&code[0]""" =>
                             "code" "(" code ")"
                           <=  > += "%""close[0]""" =>;

   #define code        ::= expression*;
             
   #define ret_expr    ::= <= > += "%""open&ret_expr[0]""" =>
                             "returning" "(" expression ")"
                           <=  > += "%""close[0]""" =>;

   #define numeric     ::= <= > += "%""new&numericToken[1]""" =>
                             "numeric" "=" $numeric  
                           <= < += $terminal =>;

   #define literal     ::= <= > += "%""new&literalToken[1]""" =>
                             "literal" "=" $literal
                           <= < += $terminal =>;

   #define identifier  ::= <= > += "%""new&numericToken[1]""" =>
                             "identifier" "=" $identifier
                           <= < += $terminal =>;

   #define message     ::= <= > += "%""new&messageToken[1]""" =>
                             "message" "=" $identifier
                           <= < += $terminal =>;

   #define parameter   ::= <= > += "%""new&paramToken[1]""" =>
                             "parameter" "=" $identifier
                           <= < += $terminal =>;

   #define reference   ::= <= > += "%""new&referenceToken[1]""" =>
                             "reference" "=" $reference
                           <= < += $terminal =>;
]]
