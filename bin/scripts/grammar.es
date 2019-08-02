[[
   #grammar build
   #grammar cf

   #define start ::= statements $eof;            
   #define start ::= $eof;

   #define statements     ::= <= system'dynamic'expressions'CodeblockExpression ( => statement next_statement* <= ) =>;
   #define next_statement ::= ";" statement;

   #define statement  ::= <= system'dynamic'expressions'StatementExpression ( => expression <= ) =>;
   #define statement  ::= <= system'dynamic'expressions'StatementExpression ( => if_expr <= ) =>;
   #define statement  ::= <= system'dynamic'expressions'StatementExpression ( => while_expr <= ) =>;

   #define if_expr    ::= <= system'dynamic'expressions'IfExpression ( => "if" "(" expression ")" "{" statements "}" <= ) =>; 
   #define while_expr ::= <= system'dynamic'expressions'LoopExpression ( => "while" "(" expression ")" "{" statements "}" <= ) =>; 

   #define expression ::= object;  
   #define expression ::= <= system'dynamic'expressions'MessageCallExpression ( => object "." message "(" parameter parameters* ")" <= ) =>;  
   #define expression ::= <= system'dynamic'expressions'MessageCallExpression ( => object operator parameter  <= ) =>;  
   #define expression ::= <= system'dynamic'expressions'AssigningExpression ( => variable ":=" expression <= ) =>;
   #define expression ::= <= system'dynamic'expressions'DeclaringAndAssigningExpression ( => "var" variable ":=" expression <= ) =>;

   #define parameter  ::= expression;
   #define parameters ::= "," parameter;
   #define object     ::= <= system'dynamic'expressions'SymbolExpression ( => reference <= ) =>;
   #define object     ::= <= system'dynamic'expressions'ConstantExpression ( => literal <= ) =>;
   #define object     ::= <= system'dynamic'expressions'NumericConstantExpression ( => number <= ) =>;
   #define object     ::= <= system'dynamic'expressions'VariableExpression ( => variable  <= ) =>;
   #define variable   ::= <= system'dynamic'expressions'ScopeVariable ( => identifier <= ) =>;

   #define identifier ::= <= "$identifier" =>;
   #define message    ::= <= "$identifier" =>;
   #define operator   ::= <= "less" => "<";
   #define operator   ::= <= "add"  => "+";
   #define reference  ::= <= "$reference" =>;
   #define literal    ::= <= "$literal" =>;

   #define number    ::= <= "$numeric" =>;

]]