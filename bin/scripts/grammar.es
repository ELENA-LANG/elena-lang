[[
   #grammar build
   #grammar cf

   #define start          ::= "root" "(" namespace ")" $eof;            
   #define start          ::= $eof;

   #define namespace      ::= 
<= 
   system'dynamic'expressions'SymbolCollection ( 
=>
                                     "namespace" "(" member* ")"
<= ) =>;

   #define member         ::=
<= 
     system'dynamic'expressions'SymbolInfo ( 
=>
                                   "symbol" "(" s_name symbol_expr ")"
<=   ) =>;

  #define symbol_expr     ::= nested_symbol; 
                              
  #define nested_symbol   ::= 
<=
       system'dynamic'expressions'DynamicSingleton (
=>
                                   "nested" "(" ")"
<=     ) =>;
      
  #define s_name          ::= "nameattr" "(" identifier ")" ;

  #define identifier      ::= "identifier" "=" ident_quote;

  #define ident_quote  ::= <= "$identifier" =>;
]]