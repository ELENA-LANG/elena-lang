[[
   #grammar transform
   #grammar cf

   #define start      ::= statement;
   #define start      ::= $eof;
   #define statement  ::= expression next_expr;
   #define expression ::= <= ( < => object operations <= ) =>;

   #define object     ::= <= "$reference" =>;
   #define object     ::= <= """" => literal <= """" =>;
   #define literal    ::= <= "$literal" =>;
   #define object     ::= <= "$numeric" =>;
   #define object     ::= "(" expression ")" ;

   #define operations ::= operation operations;
   #define operations ::= $eps;
   #define operation  ::= message parameters <= >> "[" = "]""" =>;
   #define message    ::= <= >> " ^ """ "$identifier" =>;

   #define parameters ::= parameter parameters;
   #define parameters ::= $eps; 
   #define parameter  ::=  <= + << " " => ":" object;
   #define next_expr  ::= $eof;
]]
