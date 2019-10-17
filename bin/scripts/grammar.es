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

  #define symbol_expr     ::= "expression" "(" nested_symbol ")"; 
                              
  #define nested_symbol   ::= 
<=
       system'dynamic'expressions'DynamicSingleton (
=>
                                   "nested" "(" member* ")"
<=     ) =>;

  #define member          ::= method;
  #define member          ::= action;

  #define action          ::= 
<=
       system'dynamic'expressions'ActionMethodExpression (
=>
                                   "action" "(" parameter* body ")"
<=
       )
=>;


  #define method          ::= 
<=
       system'dynamic'expressions'MethodExpression (
=>
                                   "method" "(" m_name parameter* body ")"
<=
       )
=>;

  #define method          ::= 
<=
       system'dynamic'expressions'MethodExpression (
=>
                                   "script_method" "(" m_name parameter* body ")"
<=
       )
=>;

  #define method          ::= 
<=
       system'dynamic'expressions'GetMethodExpression (
=>
                                   "get_method" "(" m_name body ")"
<=
       )
=>;

  #define parameter       ::= 
<=
         system'dynamic'expressions'ScopeVariable (
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

  #define statement       ::= expression; 
  #define statement       ::= ret_expression; 

  #define ret_expression  ::= 
<=
               system'dynamic'expressions'ReturnExpression (
=>
                                   "returning" "(" expression ")"
<=
               )
=>;

  #define expression      ::= "expression" "(" operation ")";
  #define expression      ::= "expression" "(" object ")";

  #define operation       ::=
<=
               system'dynamic'expressions'NestedExpression (
=>
                                 nested_symbol
<=
               )
=>;

  #define operation       ::=
<=
               system'dynamic'expressions'GetPropertyExpression (
=>
                                 object message "property_parameter" "(" ")"
<=
               )
=>; 

  #define operation       ::=
<=
               system'dynamic'expressions'MessageCallExpression (
=>
                                 object message expression*
<=
               )
=>; 

  #define object          ::=
<=
                    system'dynamic'expressions'VariableExpression ( 
                       system'dynamic'expressions'ScopeVariable (
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
      
  #define s_name          ::= "nameattr" "(" identifier ")" ;
  #define m_name          ::= "nameattr" "(" identifier ")" ;
  #define p_name          ::= "nameattr" "(" identifier ")" ;

  #define message         ::= "message" "=" ident_quote;
  #define identifier      ::= "identifier" "=" ident_quote;

  #define ident_quote  ::= <= "$identifier" =>;
  #define ref_quote    ::= <= "$reference" =>;
  #define int_quote    ::= <= "$numeric" =>;
  #define quote        ::= <= "$literal" =>;
]]