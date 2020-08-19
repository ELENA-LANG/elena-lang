[[
   #grammar cf

   #define start          ::= "root" "(" namespace ")" $eof;            
   #define start          ::= $eof;

   #define namespace      ::= 
<= 
   system'dynamic'expressions'SymbolCollection ( 
=>
                                     "namespace" "(" member* ")"
<= ) =>;

   #define namespace      ::= 
<= 
   system'dynamic'expressions'SymbolCollection ( 
=>
                                     "public_namespace" "(" member* ")"
<= ) =>;

   #define member         ::=
<=
   system'dynamic'expressions'SymbolInfo (
     system'dynamic'expressions'DynamicImport (
       system'dynamic'expressions'ScopeVariable (
=>
                                     "import" "(" "identifier" "=" ident_quote ")"
<=
       )
     )
   )
=>;

   #define member         ::=
<= 
     system'dynamic'expressions'SymbolInfo ( 
=>
                                   "symbol" "(" s_name symbol_expr ")"
<=   ) =>;

   #define member         ::=
<= 
     system'dynamic'expressions'SymbolInfo ( 
=>
                                   "singleton" "(" s_name class_expr ")"
<=   ) =>;

  #define symbol_expr     ::= "expression" "(" nested_symbol ")"; 
  #define symbol_expr     ::= "expression" "(" expr_symbol ")"; 
                              
  #define class_expr      ::= 
<=
       system'dynamic'expressions'DynamicSingleton (
=>
                                   member*
<=
       )
=>;

  #define nested_symbol   ::= 
<=
       system'dynamic'expressions'DynamicSingleton (
=>
                                   "nested" "(" member* ")"
<=     ) =>;

  #define expr_symbol     ::= 
<=
       system'dynamic'expressions'DynamicExpressionSymbol (
=>
                                   expr_operation
<=     ) =>;

  #define expr_symbol     ::= 
<=
       system'dynamic'expressions'DynamicExpressionSymbol (
=>
                                   object
<=     ) =>;

  #define member          ::= method;
  #define member          ::= function;

  #define function        ::= 
<=
       system'dynamic'expressions'ActionMethodExpression (
=>
                                   "function" "(" parameter* body ")"
<=
       )
=>;

  #define function        ::= 
<=
       system'dynamic'expressions'ActionMethodExpression (
=>
                                   "script_function" "(" parameter* body ")"
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

  #define nested_body     ::=
<=
             system'dynamic'expressions'NestedBlockExpression (
=>
                                   "code" "(" statement* ")"
<=
             )
=>;

  #define statement       ::= expression; 
  #define statement       ::= ret_expression; 
  #define statement       ::= nested_body; 

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
  #define expression      ::= "expression" "(" variable ")";

  #define variable        ::= 
<=
               system'dynamic'expressions'DeclaringAndAssigningExpression (
=>
                                "variable_identifier" "=" ident_quote assigning
<=
               )
=>;

  #define assigning       ::= "assign" "=" "0"  expression;
                                             
  #define operation        ::= 
<=
               system'dynamic'expressions'AssigningExpression (
=>
                                "identifier" "=" ident_quote assigning
<=
               )
=>;

  #define operation       ::=
<=
               system'dynamic'expressions'NestedExpression (
=>
                                 nested_symbol
<=
               )
=>;

  #define operation       ::= expr_operation;

  #define expr_operation  ::=
<=
               system'dynamic'expressions'GetPropertyExpression (
=>
                                 object message "property_parameter" "=" "0"
<=
               )
=>; 

  #define expr_operation  ::=
<=
               system'dynamic'expressions'SetPropertyExpression (
=>
                                "expression" "(" object message "property_parameter" "=" "0" ")" "assign" "=" "0"  object
<=
               )
=>; 

  #define expr_operation  ::=
<=
               system'dynamic'expressions'ExtensionOrMessageCallExpression (
=>
                                 object message expression*
<=
               )
=>; 

  #define expr_operation  ::=
<=
               system'dynamic'expressions'FunctionCallExpression (
=>
                                 object idle_mssg expression*
<=
               )
=>; 

  #define operation       ::=
<=
               system'dynamic'expressions'MessageCallExpression (
=>
                                 object "operator" "=" operator object
<=
               )
=>; 

  #define operation       ::=
<=
               system'dynamic'expressions'IfExpression (
=>
                                 object "operator" "=" "?" body_expr
<=
               )
=>; 

  #define body_expr       ::= "expression" "(" body ")";

  #define operator        ::= <= "add" => "+";
  #define operator        ::= <= "subtract" => "-";
  #define operator        ::= <= "multiply" => "*";
  #define operator        ::= <= "divide" => "/";
  #define operator        ::= <= "notgreater" => "<=";
  #define operator        ::= <= "equal" => "==";
  #define operator        ::= <= "less" => "<";
  #define operator        ::= <= "greater" => ">";

  #define object          ::= expression;

  #define object          ::=
<=
                    system'dynamic'expressions'IdentifierExpression ( 
                       system'dynamic'expressions'ScopeVariable (
=>
                           "identifier" "=" ident_quote
<=
                       )
                    )
=>;
      
  #define object          ::=
<=
                    system'dynamic'expressions'PreviousVariableExpression ( 
                       system'dynamic'expressions'ScopeVariable (
=>
                           "prev_identifier" "=" ident_quote
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

  #define new_expr          ::=
<=
                       system'dynamic'expressions'ConstantExpression ( 
                          system'ClassReference ( 
=>
                               "new_reference" "=" ref_quote "message" "=" "0"
<=
                          )
                       )
                       "#constructor"
=>;
      
  #define object          ::=
<=
                    system'dynamic'expressions'MessageCallExpression ( 
=>
                               new_expr object*
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
      
  #define idle_mssg       ::= "message" "(" ")";

  #define s_name          ::= "nameattr" "(" identifier ")" ;
  #define m_name          ::= "nameattr" "(" identifier ")" ;
  #define p_name          ::= "nameattr" "(" identifier ")" ;

  #define message         ::= "message" "=" ident_quote;
  #define message         ::= "message" "(" identifier ")";
  #define identifier      ::= "identifier" "=" ident_quote;
  #define reference       ::= "reference" "=" ref_quote;

  #define ident_quote  ::= <= "$identifier" =>;
  #define ref_quote    ::= <= "$reference" =>;
  #define int_quote    ::= <= "$numeric" =>;
  #define quote        ::= <= "$literal" =>;
]]