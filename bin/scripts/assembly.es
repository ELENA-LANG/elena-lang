[[   
   #grammar cf

   #define start      ::= <= [ ( 2 %"system'dynamic'tapeOp.var&args$[]" => module <=  ) * system'dynamic'Tape ] =>;
   #define module     ::= "root" "(" items module;
   #define module     ::= $eof;
   #define items      ::= "class" class items;
   #define items      ::= ")";
   #define class      ::= <= %"open&class[0]" => "(" members <= %"close[0]" =>;
   #define members    ::= method members;
   #define members    ::= ")";
   #define method     ::= <= %"open&method[0]" => "method" "(" mth_body ")" <= %"close[0]" =>;
   #define mth_body   ::= name code;
   #define name       ::= <= %"new&messageToken[1]" => "message" message;
   #define code       ::= <= %"open&code[0]" => "code" "(" exprs <= %"close[0]" =>;
   #define exprs      ::= expr exprs;
   #define exprs      ::= ")";

   #define expr       ::= <= %"open&expression[0]" => "expression" "(" tokens <= %"close[0]" =>;
   #define tokens     ::= <= %"new&symbolToken[1]" => "symbol" symbol tokens;
   #define tokens     ::= <= %"new&messageToken[1]" => "message" message tokens;
   #define tokens     ::= <= %"new&literalToken[1]" => "literal" literal tokens;
   #define tokens     ::= ")";

   #define symbol     ::= "=" ref_token;
   #define literal    ::= "=" str_token;
   #define message    ::= "=" ident_token;

   #define ref_token  ::= <= "$reference" =>; 
   #define str_token  ::= <= $literal =>; 
   #define ident_token::= <= "$identifier" =>; 
]]
