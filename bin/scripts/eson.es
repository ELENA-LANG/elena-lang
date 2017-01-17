[[
   #grammar cf

   #define start      ::= <= root ( => members <= ) =>;
   #define start      ::= $eof;

   #define members    ::= member ";" members;
   #define members    ::= $eof;
   #define member     ::= <= symbol_decl ( => s_name "=" expression <= ) =>;

   #define expression ::= <= expression ( => object <= ) =>; 

   #define object     ::= numeric;
   #define object     ::= "{" singleton;

   #define singleton  ::= <= nested_decl ( => cls_mmbrs <= ) =>;
   #define cls_mmbrs  ::= cls_mmbr ";" cls_mmbrs;
   #define cls_mmbrs  ::= "}";

   #define cls_mmbr   ::= <= method ( => name ":" mmbr_expr <= ) =>;
   #define mmbr_expr  ::= <= returning ( => expression <= ) =>;
   #define mmbr_expr  ::= <= action ( => "function" "(" f_params f_body <= ) =>;

   #define f_params   ::= ")";
   #define f_body     ::= "{" f_line;

   #define f_line     ::= <= returning ( => "return" expression f_end <= ) =>;
   #define f_line     ::= f_eof;
   #define f_end      ::= ";" f_eof;
   #define f_eof      ::= "}";

   #define numeric    ::= <= numeric = $numeric =>;
   #define name       ::= <= message = $identifier =>;
   #define s_name     ::= <= identifier = $identifier =>;
]]
