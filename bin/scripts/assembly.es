[[   
   #grammar transform
   #grammar cf

   #define start      ::= module;
   #define start      ::= $eof;

   #define module     ::= <= ( => "root" "(" items <= > " *" "system'dynamic'Tape" "=" == ) =>;
   #define items      ::= "symbol_decl" symbol items;
   #define items      ::= "include" include items;
   #define items      ::= ")" eof;

   #define include    ::= <= ( < += "2" > += " %""system'dynamic'tapeOp.var&args$[]""" += " %""include[2]""" => "(" forward alias <= ) " " >> > " . " =>;

   #define symbol     ::= <= ( < += "2" > += " %""system'dynamic'tapeOp.var&args$[]""" => "(" symbol_bdy ")" <= ) " " >> > " . " =>;
   #define symbol_bdy ::= <= > += " %""open&symbol[0]""" => identifier expr <= > += " %""close[0]""" =>;

   #define identifier ::= <= > += " %""new&identToken[1]""" => "identifier" name;
   #define expr       ::= <= > += " %""open&expression[0]""" => "expression" "(" tokens <= > += " %""close[0]""" =>;
   #define ret_expr   ::= <= > += " %""open&ret_expr[0]""" => "returning" "(" expr ")" <= > += " %""close[0]""" =>;

   #define singleton  ::= <= > += " %""open&singleton[0]""" => "(" sing_items <= > += " %""close[0]""" =>;

   #define code       ::= a_line;
   #define a_line     ::= ret_expr a_line;
   #define a_line     ::= expr a_line;
   #define a_line     ::= ")";

   #define sing_items ::= sing_item sing_items; 
   #define sing_items ::= ")";

   #define sing_item  ::= "method" method;
   #define method     ::= <= > += " %""open&method[0]""" => "(" mth_body <= > += " %""close[0]""" =>;
   #define mth_body   ::= <= > += " %""new&paramToken[1]""" < += " " """this"""  => mth_name mth_params mth_expr; 
   #define mth_name   ::= <= > += " %""new&messageToken[1]""" => "message" name;
   #define mth_params ::= mth_param mth_params;
   #define mth_params ::= $eps;
   #define mth_expr   ::= ret_expr ")";
   #define mth_expr   ::= mth_body ")";

   #define mth_param  ::= <= > += " %""new&paramToken[1]""" => "method_param" name;
   #define mth_body   ::= <= > += " %""open&code[0]""" => "code" "(" code <= > += " %""close[0]""" =>;

   #define tokens     ::= "nested_decl" singleton tokens;
   #define tokens     ::= "numeric" numeric tokens;
   #define tokens     ::= "identifier" identifier tokens;
   #define tokens     ::= <= > += " %""new&messageToken[1]""" => "message" name tokens;
   #define tokens     ::= expr tokens;
   #define tokens     ::= ")";
   #define name       ::= <= < += " " => "=" ident_token;
   #define reference  ::= <= < += " " => "=" ref_token;
   #define numeric    ::= <= > += " %""new&numericToken[1]""" < += " " => "=" num_token;
   #define identifier ::= <= > += " %""new&identToken[1]""" < += " " => "=" ident_token;
   #define ref_expr   ::= "reference" reference;

   #define forward    ::= "forward" f_expr;
   #define f_expr     ::= "(" ref_expr ")";
   #define alias      ::= "identifier" name ")";

   #define ident_token::= <= """" => ident <= """" =>; 
   #define ident      ::= <= "$identifier" =>;
   #define ref_token  ::= <= """" => ref <= """" =>;
   #define ref        ::= <= "$reference" =>;
   #define num_token  ::= <= "$numeric" =>; 

   #define eof        ::= $eof; 
]]
