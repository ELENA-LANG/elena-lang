[[   
   #grammar cf

   #define start      ::= module;
   #define start      ::= $eof;

   #define module     ::= <= [ ( 2 %"system'dynamic'tapeOp.var&args$[]" => "root" "(" items <= ) * system'dynamic'Tape ] =>;
   #define items      ::= "symbol_decl" symbol items;
   #define items      ::= ")" eof;

   #define symbol     ::= <=  %"open&symbol[0]" => "(" symbol_bdy ")" <= %"close[0]" =>;
   #define symbol_bdy ::= identifier expr;

   #define identifier ::= <= %"new&identToken[1]" => "identifier" name;
   #define expr       ::= <= %"open&expression[0]" => "expression" "(" tokens <= %"close[0]" =>;
   #define ret_expr   ::= <= %"open&ret_expr[0]" => "returning" "(" expr ")" <= %"close[0]" =>;

   #define singleton  ::= <= %"open&singleton[0]" => "(" sing_items <= %"close[0]" =>;

   #define action     ::= a_line;
   #define a_line     ::= ret_expr a_line;
   #define a_line     ::= ")";

   #define sing_items ::= sing_item sing_items; 
   #define sing_items ::= ")";

   #define sing_item  ::= "method" method;
   #define method     ::= <= %"open&method[0]" => "(" mth_body <= %"close[0]" =>;
   #define mth_body   ::= mth_name mth_expr; 
   #define mth_name   ::= <= %"new&messageToken[1]" => "message" name;
   #define mth_expr   ::= ret_expr ")";
   #define mth_expr   ::= mth_body ")";

   #define mth_body   ::= <= %"open&action[0]" => "action" "(" action <= %"close[0]" =>;

   #define tokens     ::= "nested_decl" singleton tokens;
   #define tokens     ::= "numeric" numeric tokens;
   #define tokens     ::= ")";
   #define name       ::= "=" ident_token;
   #define numeric    ::= <= %"new&numericToken[1]" => "=" num_token;

   #define ident_token::= <= "$identifier" =>; 
   #define num_token  ::= <= $numeric =>; 

   #define eof        ::= $eof; 
]]
