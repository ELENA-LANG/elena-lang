[[
   #grammar cf

   #define start          ::= "root" "(" namespace ")" $eof;            
   #define start          ::= $eof;

   #define namespace      ::= 
<= 
   system'dynamic'expressions'SymbolCollection ( 
=>
                                     "namespace" "(" import* member* ")"
<= ) =>;

   #define namespace      ::= 
<= 
   system'dynamic'expressions'SymbolCollection ( 
=>
                                     "public_namespace" "(" import* member* ")"
<= ) =>;

   #define import         ::=
<=
   system'dynamic'expressions'ImportInfo (
=> 
                                    "import" "(" "identifier" "=" ident_quote ")"
<=
   ) 
=>; 

   #define member         ::=
<= 
     system'dynamic'expressions'SymbolInfo ( 
=>
                                   "singleton" "(" s_name class_expr ")"
<=   ) =>;


   #define member         ::=
<= 
     system'dynamic'expressions'SymbolInfo ( 
=>
                                   "public_singleton" "(" s_name class_expr ")"
<=   ) =>;

  #define class_expr      ::= 
<=
       system'dynamic'expressions'DynamicSingleton (
=>
                                   class_member*
<=
       )
=>;

  #define class_member    ::= function;

  #define function        ::= 
<=
       system'dynamic'expressions'ActionMethodExpression (
=>
                                   "function" "(" parameter* body ")"
<=
       )
=>;

  #define parameter       ::= 
<=
         system'dynamic'expressions'ScopeIdentifier (
=>
                                   "parameter" "(" p_name ")"
<=
         )
=>;

  #define body            ::=
<=
             system'dynamic'expressions'CodeblockExpression (
=>
                                   "code" "(" statement* ")"
<=
             )
=>;

  #define statement       ::= "expression" "(" root_expr ")"; 
  #define statement       ::= ret_expression; 

  #define root_expr       ::= expression;

  #define expression      ::= "expression" "(" expression ")";
  #define expression      ::= "message_operation" "(" call_expression ")";
  #define expression      ::= "add_operation" "(" add_expression ")";
  #define expression      ::= object_expr;

  #define ret_expression  ::= 
<=
               system'dynamic'expressions'ReturnExpression (
=>
                                   "returning" "(" expression ")"
<=
               )
=>;

  #define call_expression ::=
<=
               system'dynamic'expressions'MessageCallExpression (
=>
                              object_expr message expression*
<=
               )
=>;

  #define call_expression ::=
<=
               system'dynamic'expressions'FunctionCallExpression (
=>
                              object_expr expression*
<=
               )
=>;

  #define call_expression ::=
<=
               system'dynamic'expressions'MessageCallExpression (
=>
                              expression message expression*
<=
               )
=>;

  #define add_expression ::=
<=
               system'dynamic'expressions'MessageCallExpression (
=>
                              expression add_operation
<=
               )
=>;

  #define add_operation ::=
               <= "add" => expression;

  #define object_expr     ::= "object" "(" object ")";

  #define object          ::=
<=
                    system'dynamic'expressions'IdentifierExpression ( 
                       system'dynamic'expressions'ScopeIdentifier (
=>
                              "identifier" "=" ident_quote
<=
                       )
                    )
=>;
      
  #define object          ::=
<=
                    system'dynamic'expressions'IntConstantExpression ( 
=>
                              "integer" "=" int_quote
<=
                    )
=>;
      
  #define object          ::=
<=
                    system'dynamic'expressions'CharConstantExpression ( 
=>
                              "character" "=" char_quote
<=
                    )
=>;
      
  #define object          ::=
<=
                    system'dynamic'expressions'SymbolExpression ( 
=>
                              "reference" "=" ref_quote
<=
                    )
=>;

  #define object          ::=
<=
                    system'dynamic'expressions'ConstantExpression ( 
=>
                              "literal" "=" quote
<=
                    )
=>;                            

  #define message         ::= "message" "(" identifier ")";

  #define s_name          ::= "nameattr" "(" identifier ")" ;
  #define p_name          ::= "nameattr" "(" identifier ")" ;

  #define identifier      ::= "identifier" "=" ident_quote;

  #define ident_quote  ::= <= "$identifier" =>;
  #define ref_quote    ::= <= "$reference" =>;
  #define int_quote    ::= <= "$numeric" =>;
  #define char_quote   ::= <= "$character" =>;
  #define quote        ::= <= "$literal" =>;
]]