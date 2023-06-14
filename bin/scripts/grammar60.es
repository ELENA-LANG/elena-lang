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
     system'dynamic'expressions'DynamicImport (
       system'dynamic'expressions'ScopeVariable (
=>
                                     "import" "(" "reference" "=" ref_quote ")"
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
                                   operation
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

  #define statement       ::= "expression" "(" root_expr ")"; 
  #define statement       ::= ret_expression; 
  #define statement       ::= nested_body; 
  #define statement       ::= looping; 

  #define root_expr       ::= variable;
  #define root_expr       ::= expression;
  #define root_expr       ::= prop_assign_expr;

  #define ret_expression  ::= 
<=
               system'dynamic'expressions'ReturnExpression (
=>
                                   "returning" "(" expression ")"
<=
               )
=>;

  #define expression      ::= $ object operation?;
  #define expression      ::= new_call;
  #define expression      ::= var_assign_expr;

  #define loop_expr       ::= object "operator" "=" "?" body_expr;

  #define operation       ::= message_call;
  #define operation       ::= function_call;
  #define operation       ::= get_property;
  #define operation       ::= operator_call;
  #define operation       ::= if_operator;

  #define message_call    ::= ^
<= 
               system'dynamic'expressions'ExtensionOrMessageCallExpression (
=> 
                              message expression* next_message* 
<=
               )
=>; 

  #define next_message    ::= <= ; => ";" message expression*;

  #define function_call   ::= ^
<=
               system'dynamic'expressions'FunctionCallExpression (
=>
                              idle_mssg expression*
<=
               )
=>; 

  #define get_property    ::= ^
<=
               system'dynamic'expressions'GetPropertyExpression (
=>                               
                              message "property_parameter" "=" "0"
<=
               )
=>; 

  #define operator_call   ::= ^
<=
               system'dynamic'expressions'MessageCallExpression (
=>
                              "operator" "=" operator expression next_operation*
<=
               )
=>; 

  #define next_operation  ::= <= ; => ";" "operator" "=" operator expression;

  #define if_operator     ::= ^  
<=
               system'dynamic'expressions'IfExpression (
=>
                                 "operator" "=" "?" body_expr body_expr?
<=
               )
=>; 

  #define prop_assign_expr::=
<=
               system'dynamic'expressions'SetPropertyExpression (
=>                               
                                 "expression" "(" object message "property_parameter" "=" "0" ")" assigning
<=
               )
=>;

  #define var_assign_expr ::=
<=
               system'dynamic'expressions'AssigningExpression (
=>
                              var_name assigning
<=
               )
=>;

  #define var_name          ::=
<=
                       system'dynamic'expressions'ScopeVariable (
=>
                              "identifier" "=" ident_quote
<=
                       )
=>;
      

  #define looping         ::= 
<=
               system'dynamic'expressions'LoopExpression (
=>
                              "loop_expression" "(" "expression" "(" loop_expr ")" ")" 
<=
               )
=>;

  #define variable        ::= 
<=
               system'dynamic'expressions'DeclaringAndAssigningExpression (
=>
                              "variable_identifier" "=" ident_quote assigning
<=
               )
=>;

  #define assigning       ::= "assign" "=" "0" expression;

  #define new_call        ::=
<=
                    system'dynamic'expressions'MessageCallExpression ( 
=>
                               new_object expression*
<=
                    )
=>;
      
  #define body_expr       ::= "expression" "(" body ")";

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

  #define object          ::=
<=
               system'dynamic'expressions'NestedExpression (
=>
                              nested_symbol
<=
               )
=>;

  #define object          ::= "expression" "(" expression ")";

  #define new_object      ::=
<=
                       system'dynamic'expressions'ClassIdentifierExpression ( 
=>
                              "new_identifier" "=" ident_quote idle_mssg
<=
                       )
                       "#constructor"
=>;

  #define new_object      ::=
<=
                       system'dynamic'expressions'ConstantExpression ( 
                          system'ClassReference ( 
=>
                              "new_reference" "=" ref_quote idle_mssg
<=
                          )
                       )
                       "#constructor"
=>;

  #define operator        ::= <= "add" => "+";
  #define operator        ::= <= "subtract" => "-";
  #define operator        ::= <= "multiply" => "*";
  #define operator        ::= <= "divide" => "/";
  #define operator        ::= <= "notgreater" => "<=";
  #define operator        ::= <= "equal" => "==";
  #define operator        ::= <= "notequal" => "!=";
  #define operator        ::= <= "less" => "<";
  #define operator        ::= <= "greater" => ">";

  #define idle_mssg       ::= "message" "(" ")";
  #define idle_mssg       ::= "message" "=" "0";

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
  #define char_quote   ::= <= "$character" =>;
  #define quote        ::= <= "$literal" =>;
]]