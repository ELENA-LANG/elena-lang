[[
   #grammar cf

   #define start      ::= <= root ( => members <= ) =>;
   #define start      ::= $eof;

   #define members    ::= member ";" members;
   #define members    ::= directive ";" members;
   #define members    ::= $eof;
   #define member     ::= <= symbol ( => s_name "=" expression <= ) =>;

   #define directive  ::= <= include ( => "#import" forward alias <= ) =>;
   #define alias      ::= "as" s_name;

   #define expression ::= <= expression ( => object operations <= ) =>; 
   #define operations ::= operation operations;
   #define operations ::= $eps;

   #define operation  ::= "." message ext_op op_params;
   #define op_params  ::= "(" op_params_r;
   #define op_params_r::= expression op_nparam;
   #define op_params_r::= ")";

   #define op_nparam  ::= "," expression op_nparam;
   #define op_nparam  ::= ")";

   #define ext_op     ::= "<" extension ">";
   #define ext_op     ::= $eps;

   #define extension  ::= "%" message;

   #define object     ::= numeric;
   #define object     ::= identifier;
   #define object     ::= "{" singleton;

   #define singleton  ::= <= nested ( => cls_mmbrs <= ) =>;
   #define cls_mmbrs  ::= cls_mmbr ";" cls_mmbrs;
   #define cls_mmbrs  ::= "}";

   #define cls_mmbr   ::= <= method ( => name ":" mmbr_expr <= ) =>;
   #define mmbr_expr  ::= <= returning ( => expression <= ) =>;
   #define mmbr_expr  ::= "function" "(" f_params f_body;

   #define f_params   ::= f_param f_nparam;
   #define f_params   ::= ")";
   #define f_param    ::= parameter;
   #define f_nparam   ::= "," f_param f_nparam;
   #define f_nparam   ::= ")";

   #define f_body     ::= <= code ( => "{" f_line <= ) =>;

   #define f_line     ::= <= returning ( => "return" expression f_end <= ) =>;
   #define f_line     ::= expression f_next;
   #define f_line     ::= f_eof;
   #define f_next     ::= ";" f_line;
   #define f_end      ::= ";" f_eof;
   #define f_eof      ::= "}";

   #define numeric    ::= <= numeric = $numeric =>;
   #define name       ::= <= message = $identifier =>;
   #define s_name     ::= <= identifier = $identifier =>;
   #define parameter  ::= <= parameter = $identifier =>;
   #define identifier ::= <= identifier = $identifier =>;
   #define message    ::= <= message = $identifier =>;

   #define forward    ::= <= forward ( reference = $literal ) =>;
]]
