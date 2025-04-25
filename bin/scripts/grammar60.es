[[
   #grammar cf

   #define start          ::= "root" "(" namespace ")" $eof;            
   #define start          ::= $eof;

   #define namespace      ::= 
<= 
   system'dynamic'expressions'SymbolCollection ( 
=>
                                     "namespace" "(" imports? member* ")"
<= ) =>;

   #define namespace      ::= 
<= 
   system'dynamic'expressions'SymbolCollection ( 
=>
                                     "public_namespace" "(" imports? member* ")"
<= ) =>;

   #define imports        ::=
<=
   system'dynamic'expressions'ImportInfo (
=>
                                      import import*
<=
   ) 
=>;

   #define import         ::=
                                    "import" "(" "nameattr" "(" "identifier" "=" ident_quote ")" ")"; 

   #define import         ::=
                                    "import" "(" "nameattr" "(" "reference" "=" ref_quote ")" ")"; 

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
                                   "public_symbol" "(" s_name symbol_expr ")"
<=   ) =>;

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

  #define symbol_expr     ::= 
<=
       system'dynamic'expressions'DynamicExpressionSymbol (
=>
                                   "get_expression" "(" expression ")"
<=     ) =>;

  #define class_member    ::= function;
  #define class_member    ::= method;

  #define function        ::= 
<=
       system'dynamic'expressions'ActionMethodExpression (
=>
                                   "function" "(" parameter* body ")"
<=
       )
=>;

  #define method          ::= 
<=
       system'dynamic'expressions'MethodExpression (
=>
                                   "script_method" "(" m_name parameter_block? body ")"
<=
       )
=>;

  #define parameter_block ::=
<=
         system'dynamic'expressions'MethodParameterList (
=>
                                   param_str param_str*
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
  #define param_str       ::= 
                                   "parameter" "(" p_name ")";

  #define body            ::=
<=
             system'dynamic'expressions'CodeblockExpression (
=>
                                   "code" "(" statement* ")"
<=
             )
=>;

  #define statement       ::= statement_expr; 
  #define statement       ::= for_looping; 
  #define statement       ::= ret_expression; 

  #define statement_expr  ::= "expression" "(" root_expr ")"; 

  #define root_expr       ::= expression;
  #define root_expr       ::= "assign_operation" "(" variable ")";
  #define root_expr       ::= "assign_operation" "(" assigning ")";
  #define root_expr       ::= branching; 
  #define root_expr       ::= looping; 

  #define branching       ::= "branch_operation" "(" branch_op ")";
  #define looping         ::= "loop_expression" "(" "if_operation" "(" loop_op ")" ")";
  #define for_looping     ::= "virtual_for_loop" "(" for_loop_op ")";

  #define expression      ::= "expression" "(" expression ")";
  #define expression      ::= "message_operation" "(" call_expression ")";
  #define expression      ::= "property_operation" "(" prop_expression ")";
  #define expression      ::= "add_operation" "(" add_expression ")";
  #define expression      ::= "sub_operation" "(" sub_expression ")";
  #define expression      ::= "mul_operation" "(" mul_expression ")";
  #define expression      ::= "div_operation" "(" div_expression ")";
  #define expression      ::= "equal_operation" "(" equal_expression ")";
  #define expression      ::= "less_operation" "(" less_expression ")";
  #define expression      ::= "at_operation" "(" at_expression ")";
  #define expression      ::= object_expr;
 
  #define branch_op       ::= 
<=
               system'dynamic'expressions'IfExpression (
=>
                              expression closure_body closure_body?
<=
               )
=>;

  #define loop_op       ::= 
<=
               system'dynamic'expressions'LoopExpression (
=>
                              expression closure_body
<=
               )
=>;

  #define for_loop_op       ::= 
<=
               system'dynamic'expressions'ForLoopExpression (
=>
                              statement_expr expression statement_expr closure_body
<=
               )
=>;

  #define closure_body       ::= "expression" "(" "closure" "(" body ")" ")";

  #define variable        ::= 
<=
               system'dynamic'expressions'DeclaringAndAssigningExpression (
=>
                              "new_variable" "(" identifier ")" expression
<=
               )
=>;

  #define assigning        ::= 
<=
               system'dynamic'expressions'AssigningExpression (
=>
                              "object" "(" identifier ")" expression
<=
               )
=>;

  #define ret_expression  ::= 
<=
               system'dynamic'expressions'ReturnExpression (
=>
                                   "returning" "(" expression ")"
<=
               )
=>;

  #define prop_expression ::=
<=
               system'dynamic'expressions'GetPropertyExpression (
=>
                              expression message
<=
               )
=>;

  #define call_expression ::=
<=
               system'dynamic'expressions'NewExpression (
=>
                              new_object expression*
<=
               )
=>;

  #define call_expression ::=
<=
               system'dynamic'expressions'ExtensionOrMessageCallExpression (
=>
                              expression message expression*
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

  #define at_expression ::=
<=
               system'dynamic'expressions'MessageCallExpression (
=>
                              expression at_operation
<=
               )
=>;

  #define at_operation ::=
               <= "at" => expression;

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

  #define sub_expression ::=
<=
               system'dynamic'expressions'MessageCallExpression (
=>
                              expression sub_operation
<=
               )
=>;

  #define sub_operation ::=
               <= "subtract" => expression;

  #define mul_expression ::=
<=
               system'dynamic'expressions'MessageCallExpression (
=>
                              expression mul_operation
<=
               )
=>;

  #define mul_operation ::=
               <= "multiply" => expression;

  #define div_expression ::=
<=
               system'dynamic'expressions'MessageCallExpression (
=>
                              expression div_operation
<=
               )
=>;

  #define div_operation ::=
               <= "divide" => expression;

  #define equal_expression ::=
<=
               system'dynamic'expressions'MessageCallExpression (
=>
                              expression equal_operation
<=
               )
=>;

  #define equal_operation ::=
               <= "equal" => expression;

  #define less_expression ::=
<=
               system'dynamic'expressions'MessageCallExpression (
=>
                              expression less_operation
<=
               )
=>;

  #define less_operation ::=
               <= "less" => expression;

  #define object_expr     ::= "object" "(" object ")";

  #define object_expr   ::= 
<=
     system'dynamic'expressions'NestedExpression (
       system'dynamic'expressions'DynamicSingleton (
=>
                                   "nested" "(" class_member* ")"
<=     ) 
     )=>;

  #define new_object      ::=
<=
                       system'dynamic'expressions'ClassIdentifierExpression ( 
=>
                              "new_identifier" "(" "identifier" "=" ident_quote ")"
<=
                       )
=>;

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
                    system'dynamic'expressions'LazySymbolExpression ( 
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
  #define m_name          ::= "nameattr" "(" identifier ")" ;
  #define p_name          ::= "nameattr" "(" identifier ")" ;

  #define identifier      ::= "identifier" "=" ident_quote;

  #define ident_quote  ::= <= "$identifier" =>;
  #define ref_quote    ::= <= "$reference" =>;
  #define int_quote    ::= <= "$numeric" =>;
  #define char_quote   ::= <= "$character" =>;
  #define quote        ::= <= "$literal" =>;
]]