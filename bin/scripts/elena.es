[[
   #grammar transform
   #grammar cf

   #define start      ::= statement;
   #define start      ::= $eof;
   #define statement  ::= expression next_expr;
   #define statement  ::= assigning next_expr;
   #define expression ::= <= ( < => object operations <= ) =>;

   #define assigning  ::= <= ( < "extensions'scripting'globals" => var ":=" expression <= > "^ ""setAt[2]""" ) => ;

   #define object     ::= <= "$reference" =>;
   #define object     ::= <= """" => literal <= """" =>;
   #define literal    ::= <= "$literal" =>;
   #define object     ::= <= "$numeric" =>;
   #define object     ::= <= ( < "extensions'scripting'globals" => var <= > " ^ ""getAt[1]""" ) < =>;
   #define object     ::= "(" expression ")" ;

   #define identifier ::= <= "$identifier" =>;
   #define var        ::= <= << """" => identifier <= """" < =>;

   #define operations ::= operation;
   #define operations ::= $eps;

   #define operation  ::= message "(" parameters ")" <= >> "[" = "]""" =>;
   #define message    ::= <= >> " ^ """ "$identifier" =>;

   #define parameters ::= parameter next_param;
   #define parameters ::= $eps; 

   #define next_param ::= "," parameter next_param;
   #define next_param ::= $eps; 
   #define parameter  ::=  <= + => expression;
   #define next_expr  ::= $eof;
]]
