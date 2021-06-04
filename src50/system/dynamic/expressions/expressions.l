namespace expressions
{
    import system'collections;
    import system'text;
    import system'routines;

//    const int mode_non   = 0;
    const int mode_lcl   = 1;
//    const int mode_asn   = 3;
//    const int mode_class = 4;

    preloaded used = new object[]{Expression,DynamicSingleton/*,DynamicExpressionSymbol,SymbolCollection,DynamicImport*/};

    public class ScopeIdentifier
    {
        prop string Name;
        prop int Mode;
        
        static ScopeIdentifier Variable(string name)
            = new ScopeIdentifier(name, mode_lcl);
        
        static ScopeIdentifier TempVariable(int index)
            = new ScopeIdentifier(index.toPrintable(), mode_lcl);
        
        bool equal(ScopeIdentifier var)
        {
            if (var.Name == Name && var.Mode == Mode)
            {
                ^ true
            };
            
            ^ false;
        }
        
        constructor(string name, int mode)
        {
            Name := name;
            Mode := mode
        }
    }

    public class MethodParameterList
    {
        ScopeIdentifier[] parameters;
        
        constructor(params string[] variables) 
        {
            int len := variables.Length;
            
            this parameters := new ScopeIdentifier[](len + 1);
            
            for (int i := 0, int j := len - 1, i < len, i += 1, j -= 1)
            {
                parameters[i] := ScopeIdentifier.Variable(variables[j])
            };
            parameters[len] := ScopeIdentifier.Variable("self");
        }
         
        get int Length()
            => parameters;
            
        ScopeIdentifier[] Value
            = parameters;
    }
            
    public interface ExpressionScope
    {
        abstract bool reserve(int level);

        get abstract int Reserved();
                        
        abstract int allocTempVariable();                        
                        
        get abstract int ScopeLevel();
        
//        get abstract ExpressionScope Previous();
        
        abstract TapeFunction resolve(ScopeIdentifier ident);

//        abstract TapeFunction resolveExtension(Message message);
//        
//        abstract declare(ScopeVariable variable);
    }

    public singleton IdleExpressionScope : ExpressionScope
    {
        bool reserve(int level)
            = false;
            
        int Reserved = 0;
        
        int allocTempVariable()
            = 0;
            
        int ScopeLevel
            = 0;
            
        TapeFunction resolve(ScopeIdentifier ident)
            = nil; 
    }

    public class RootExpressionScope : ExpressionScope
    {
        ref<int>        reserved := 0;
        ExpressionScope parent;
        
        constructor()
        {
        }
        
        constructor(ExpressionScope parent)
        {
            this parent := parent
        } 
        
        int Reserved = *reserved;
        
        bool reserve(int level)
        {
            int current := *reserved;
            if (current < level)
            {
                reserved.Value := level
            };
            
            ^ true;
        }        
        
        int allocTempVariable()
        {
            reserved.append(1);
            
            ^ *reserved;
        }
        
        int ScopeLevel = *reserved;
        
//        ExpressionScope Previous = nil;
        
        TapeFunction resolve(ScopeIdentifier variable) 
        {
            ^ parent.?resolve(variable)
        }
    
//        TapeFunction resolveExtension(Message message)
//            = nil;
//                
//        declare(ScopeVariable variable)
//        {
//            ^ nil
//        }
    }

    public interface Expression
    {    
        static Expression CodeBlock(params Expression[] statements)
            = CodeblockExpression.new(params statements);
        
//        static Expression CodeBlock(Expression[] statements)
//            = CodeblockExpression.new(statements);
//        
        static Expression Constant(object o)
            = new ConstantExpression(o);
            
//        static Expression IntConstant(string value)
//            = new IntConstantExpression(value);
//            
//        static Expression CharConstant(string value)
//            = new CharConstantExpression(value);
            
        static Expression Symbol(string s)
            = new SymbolExpression(s);
            
        static Expression ClassReference(string s)
            = new ConstantExpression(new ClassReference(s));
            
//        static Expression ClassIdentifier(string s)
//            = new ClassIdentifierExpression(s);
            
        static Expression New(Expression t)
            = new MessageCallExpression(t, "#constructor");
           
        static Expression New(Expression t, Expression p0)
            = new MessageCallExpression(t, "#constructor", p0);
           
//        static Expression FunctionCall(params Expression[] arguments)
//            = FunctionCallExpression.new(params arguments);
           
        static Expression MessageCall(Message message, Expression t, params Expression[] arguments)
            = MessageCallExpression.new(message,t,params arguments);
           
        static Expression MessageCall(Message message, Expression t)
            = MessageCallExpression.new(message,t);

//        static Expression ExtensionOrMessageCall(Message message, Expression t, params Expression[] arguments)
//            = ExtensionOrMessageCallExpression.new(message,t,params arguments);
//           
//        static Expression ExtensionOrMessageCall(Message message, Expression t)
//            = ExtensionOrMessageCallExpression.new(message,t);
//                        
//        static Expression DynamicMessageCall(Expression t, params Expression[] arguments)
//            = DynamicMessageCallExpression.new(t,params arguments);
//           
//        static Expression DynamicMessageCall(Expression t)
//            = DynamicMessageCallExpression.new(t);
            
        static Expression GetProperty(MessageName name, Expression t)
            = GetPropertyExpression.new(name, t);
            
        static Expression SetProperty(MessageName name, Expression t, Expression v)
            = SetPropertyExpression.new(name, t, v);
           
        static Expression GetDynamicProperty(Expression m, Expression t)
            = GetDynamicPropertyExpression.new(m, t);
            
        static Expression SetDynamicProperty(Expression m, Expression t, Expression v)
            = SetDynamicPropertyExpression.new(m, t, v);
           
//        static Declaration(ScopeVariable variable)
//            = new DeclarationExpression(variable);
//           
//        static Expression Assigning(ScopeVariable variable, Expression expr)
//            = new AssigningExpression(variable, expr);
//              
//        static Expression Assigning(string variable, Expression expr)
//            = new AssigningExpression(new ScopeVariable(variable), expr);
//              
//        static Expression DeclareAndAssigning(ScopeVariable variable, Expression expr)
//            = new DeclaringAndAssigningExpression(variable, expr);
//              
//        static Expression DeclareAndAssigning(string variable, Expression expr)
//            = new DeclaringAndAssigningExpression(new ScopeVariable(variable), expr);
//
        static Expression VariableByIndex(int index)
            = new VariableByIndexExpression(index);
                            
//        static Expression Variable(ScopeVariable variable)
//            = new VariableExpression(variable);
//              
//        static Expression Identifier(ScopeVariable variable)
//            = new IdentifierExpression(variable);
              
        static Expression Variable(string variable)
            = new VariableExpression(variable);

//        static Expression PreviousVariable(ScopeVariable variable)
//            = new PreviousVariableExpression(variable);
//              
//        static Expression PreviousVariable(string variable)
//            = new PreviousVariableExpression(new ScopeVariable(variable));
//
//        static Expression NestedCodeBlock(params Expression[] statements) 
//            = NestedBlockExpression.new(params statements);
//                                          
//        static Expression Statement(params Expression[] expressions)
//            = StatementExpression.new(params expressions);
//              
//        static Expression If(Expression cond, Expression body)
//            = new IfExpression(cond, body);
//              
//        static Expression If(Expression cond, Expression body, Expression elseBody)
//            = new IfExpression(cond, body, elseBody);
//              
//        static Expression Loop(Expression cond, Expression body)
//            = new LoopExpression(cond, body);
//            
//        static Expression GetMethod(string name, Expression body)
//            = new GetMethodExpression(name, body);        
//              
//        static Expression ActionMethod(Expression body)
//            = new ActionMethodExpression(body);        
//              
//        static Expression ActionMethod(ScopeVariable param1, Expression body)
//            = new ActionMethodExpression(param1, body);        
//              
//        static Expression Method(string name, Expression body)
//            = new MethodExpression(name, body);        
              
        static Expression Method(string name, Expression body, params ScopeIdentifier[] variables)
            = MethodExpression.new(name, body, params variables);        
              
        static Expression Closure(Expression body)
            = new ClosureExpression(body);        
              
        static Expression Closure(Expression body, params ScopeIdentifier[] variables)
            = ClosureExpression.new(body, params variables);        
              
        static Expression Return(Expression expr)
            = new ReturnExpression(expr);
    
//        static Expression Nested(DynamicSingleton expr)
//            = new NestedExpression(expr);          
//        

        internal static saveMessageCallTo(List<TapeFunction> list, ExpressionScope scope, Expression target, 
                                            Expression[] arguments, TapeFunction operation)
        {
            int len := arguments.Length;
            Queue<Expression> tempVariables := nil;
            
            // first pass - preallocating sub expr 
            if (target.IsOperation && len > 0) {
                int tempVar := scope.allocTempVariable();
                target.saveTo(list, scope, 0);
                list.append(new AssigningFunction(tempVar));
                
                tempVariables.!push(Expression.VariableByIndex(tempVar));                
            };
            
            for (int i := 0, i < len, i += 1) {
                if (arguments[i].IsOperation) {
                    int tempVar := scope.allocTempVariable();
                    
                    arguments[i].saveTo(list, scope, 0);
                    list.append(new AssigningFunction(tempVar));
                
                    tempVariables.!push(Expression.VariableByIndex(tempVar));
                }
            };
            
            for (int i := 0, i < len, i += 1) {
                if (arguments[i].IsOperation) {
                    tempVariables.pop().saveTo(list, scope, i + 1) 
                }
                else {
                    arguments[i].saveTo(list, scope, i + 1) 
                }
            };

            // second pass - the operation
            if (target.IsOperation && len > 0) {
                tempVariables.pop().saveTo(list, scope, 0) 
            }
            else {
                target.saveTo(list, scope, 0)
            };
                                    
            list.append(operation);
        }
        
        get abstract bool IsOperation();
                     
        [info:"Prepares the expression tree. Returns the maximal size of the preallocated stack"]                                                     
        abstract int prepare(ExpressionScope scope);
                                        
        abstract saveTo(List<TapeFunction> list, ExpressionScope scope, int index);
    
        saveTo(List<Tuple<Message,DynamicTape>> list, ExpressionScope scope)
        {
            NotSupportedException.raise()
        }
                            
        // temporal generic solution
        eval()
        {
            var t := self.compile(new RootExpressionScope());
            ^ t();
        }
            
        DynamicTape compile(ExpressionScope scope)
        {
            List<TapeFunction> list := new List<TapeFunction>(8);
            
            int stackSize := self.prepare(scope);
            stackSize += scope.Reserved;
            
            list.append(new AllocFunction(stackSize));
            
            self.saveTo(list, scope, 0);

            ^ DynamicTape.load(list)
        }
    }

//    public class StatementExpression : Expression
//    {
//        Expression[] statements;
//    
//        constructor(Expression p1)
//            <= new(p1);
//        
//        constructor(Expression p1, Expression p2)
//            <= new(p1, p2);
//        
//        constructor(Expression p1, Expression p2, Expression p3)
//            <= new(p1, p2, p3);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4)
//            <= new(p1, p2, p3, p4);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5)
//            <= new(p1, p2, p3, p4, p5);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6)
//            <= new(p1, p2, p3, p4, p5, p6);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7)
//            <= new(p1, p2, p3, p4, p5, p6, p7);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11, Expression p12)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11, Expression p12, Expression p13)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11, Expression p12, Expression p13, Expression p14)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11, Expression p12, Expression p13, Expression p14, Expression p15)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11, Expression p12, Expression p13, Expression p14, Expression p15, Expression p16)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11, Expression p12, Expression p13, Expression p14, Expression p15, Expression p16,
//                    Expression p17)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11, Expression p12, Expression p13, Expression p14, Expression p15, Expression p16,
//                    Expression p17, Expression p18)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11, Expression p12, Expression p13, Expression p14, Expression p15, Expression p16,
//                    Expression p17, Expression p18, Expression p19)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11, Expression p12, Expression p13, Expression p14, Expression p15, Expression p16,
//                    Expression p17, Expression p18, Expression p19, Expression p20)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20);
//            
//        constructor new(params Expression[] statements)
//        {
//            this statements := statements
//        }
//        
//        bool IsOperation = true;
//        
//        prepare(ExpressionScope scope)
//        {
//            int len := statements.Length;
//            for(int i := 0, i < len, i+=1) {
//                statements[i].prepare(scope)
//            };
//        }
//        
//        saveTo(List<TapeFunction> list, ExpressionScope scope)
//        {
//            int len := statements.Length;
//            for(int i := 0, i < len, i+=1) {
//                statements[i].saveTo(list, scope)
//            };
//        }
//    }
//
    public class CodeblockExpression : Expression
    {
        Expression[]          statements;
        List<ScopeIdentifier> variables;
        
        constructor new(params Expression[] statements)
        {
            this statements := statements
        }
        
        constructor new(Expression[] statements)
        {
            this statements := statements
        }
        
//        constructor(Expression p1)
//            <= new(p1);
        
        constructor(params Expression[] statements)
            <= new(params statements);
        
//        constructor(Expression p1, Expression p2)
//            <= new(p1, p2);
//        
//        constructor(Expression p1, Expression p2, Expression p3)
//            <= new(p1, p2, p3);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4)
//            <= new(p1, p2, p3, p4);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5)
//            <= new(p1, p2, p3, p4, p5);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6)
//            <= new(p1, p2, p3, p4, p5, p6);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7)
//            <= new(p1, p2, p3, p4, p5, p6, p7);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11, Expression p12)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11, Expression p12, Expression p13)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11, Expression p12, Expression p13, Expression p14)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11, Expression p12, Expression p13, Expression p14, Expression p15)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11, Expression p12, Expression p13, Expression p14, Expression p15, Expression p16)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11, Expression p12, Expression p13, Expression p14, Expression p15, Expression p16,
//                    Expression p17)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11, Expression p12, Expression p13, Expression p14, Expression p15, Expression p16,
//                    Expression p17, Expression p18)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11, Expression p12, Expression p13, Expression p14, Expression p15, Expression p16,
//                    Expression p17, Expression p18, Expression p19)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19);
//        
//        constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11, Expression p12, Expression p13, Expression p14, Expression p15, Expression p16,
//                    Expression p17, Expression p18, Expression p19, Expression p20)
//            <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20);
        
        bool IsOperation = true;
        
        int prepare(ExpressionScope scope)
        {
            variables := new List<ScopeIdentifier>();
            
            auto codeScope := new ExpressionScope
            {
                int Reserved = scope.Reserved;
                
                bool reserve(int level)
                    = scope.reserve(level);
                
                int allocTempVariable()
                {
                    int index := variables.Length + 1;
                    scope.reserve(index);
                                        
                    index := index + scope.ScopeLevel;
                    
                    variables.append(ScopeIdentifier.TempVariable(index));
                    
                    ^ index
                }
                
                TapeFunction resolve(ScopeIdentifier variable)
                {
                    int len := variables.Length;
                    for(int i := 0, i < len, i+=1) {
                        if (variable == variables[i])
                        {
                            int index := i + 1;
//                                
//                                if (mode == mode_asn) 
//                                {
//                                    NotSupportedException.raise()
//                                }
//                                else 
//                                {
                                    ^ new LocalFunction(index)
//                                };                                
                        }
                    };
                    
                    ^ scope.resolve(variable)
                }
                
                
//                ExpressionScope Previous = scope;
                
                int ScopeLevel = scope.ScopeLevel + variables.Length;
                
//                TapeFunction resolveExtension(Message message)
//                    => scope;
//                
//                TapeFunction resolve(ScopeVariable variable, int mode)
//                {
//                    int mode_mask := mode && mode_lcl;
//                    
//                    if (mode == 0 || mode_mask == mode_lcl)
//                    {
//                        int len := variables.Length;
//                        for(int i := 0, i < len, i+=1) {
//                            if (variables[i] == variable)
//                            {
//                                int parentLevel := scope.ScopeLevel;
//                                
//                                int index := parentLevel + i + 1;
//                                
//                                if (mode == mode_asn) 
//                                {
//                                    ^ new AssigningFunction(index)
//                                }
//                                else 
//                                {
//                                    ^ new LocalFunction(index)
//                                };                                
//                            }
//                        }
//                    };
//                    
//                    ^ scope.resolve(variable, mode)
//                }
//        
//                declare(ScopeVariable variable)
//                {
//                    variables.append(variable)
//                }            
            };

            int maxStackLevel := 0;
            int len := statements.Length;
            for(int i := 0, i < len, i+=1) {
                int current := statements[i].prepare(codeScope);
                if (current > maxStackLevel)
                    { maxStackLevel := current }
            };
            
            ^ maxStackLevel
        }
        
        saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
        {
            int len := statements.Length;
            for(int i := 0, i < len, i+=1) {
                statements[i].saveTo(list, scope, 0);
            };

            if (index > 0)
              { list.append(new SavingFunction(index)) };
        }
    }

    public class ConstantExpression : Expression
    {
        object object;
        
        constructor(o)
        {
            this object := o
        }
        
        bool IsOperation = false;
        
        eval()
            = object;
        
        int prepare(ExpressionScope scope)
            = 1;
        
        saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
        {
            list.append(ConstantFunction.load(object));
            if (index > 0)
              { list.append(new SavingFunction(index)) };
        }
    }
    
//    public class IntConstantExpression : ConstantExpression
//    {
//        constructor(o)
//        {
//            this object := intConvertor.convert(o)
//        }
//    }
//    
//    public class CharConstantExpression : ConstantExpression
//    {
//        constructor(string o)
//        {
//            var n := intConvertor.convert(o.Substring(1));
//            
//            this object := charConvertor.convert(n)
//        }
//    }
//    
//    public class ClassIdentifierExpression : Expression
//    {
//        ScopeVariable name;
//        TapeFunction  classIdentifier;
//                
//        constructor(string name)
//        {
//            this name := new ScopeVariable(name);
//        }
//        
//        bool IsOperation = false;
//        
//        prepare(ExpressionScope scope)
//        {
//            classIdentifier := scope.resolve(name, mode_class);
//        }
//        
//        saveTo(List<TapeFunction> list, ExpressionScope scope)
//        {
//            list.append(classIdentifier);
//        }
//    }

    public class SymbolExpression : Expression
    {
        Symbol symbol;
        
        constructor(string s)
        {
            symbol := new Symbol(s)
        }
        
        bool IsOperation = false;
        
        eval()
            = symbol;

        int prepare(ExpressionScope scope)
            = 1;
                        
        saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
        {
            list.append(ConstantFunction.load(symbol));
            if (index > 0) {
                list.append(new SavingFunction(index));
            }
        }                
    }

    public class MessageCallExpression : Expression
    {
        Message      _message;
        Expression   _target;
        Expression[] _arguments;
        
        constructor new(Message message, Expression t, params Expression[] arguments)
        {
            _message := message;
            _target := t;
            _arguments := arguments;
        }
        
        constructor new(Message message, Expression t)
        {
            _message := message;
            _target := t;
            _arguments := new Expression[](0);
        }
        
        constructor(Expression t, string message)
            <= new(new Message(message + "[1]"), t);
        
        constructor(Expression t, string message, Expression p1)
            <= new(new Message(message + "[2]"), t, p1);
        
        constructor(Expression t, string message, Expression p1, Expression p2)
            <= new(new Message(message + "[3]"), t, p1, p2);
        
        constructor(Expression t, string message, Expression p1, Expression p2, Expression p3)
            <= new(new Message(message + "[4]"), t, p1, p2, p3);
        
        constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4)
            <= new(new Message(message + "[5]"), t, p1, p2, p3, p4);
        
        constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5)
            <= new(new Message(message + "[6]"), t, p1, p2, p3, p4, p5);
        
        constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6)
            <= new(new Message(message + "[7]"), t, p1, p2, p3, p4, p5, p6);
        
        constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7)
            <= new(new Message(message + "[8]"), t, p1, p2, p3, p4, p5, p6, p7);
        
        constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8)
            <= new(new Message(message + "[9]"), t, p1, p2, p3, p4, p5, p6, p7, p8);
        
        constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9)
            <= new(new Message(message + "[10]"), t, p1, p2, p3, p4, p5, p6, p7, p8, p9);
        
        constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10)
            <= new(new Message(message + "[11]"), t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
        
        constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                    Expression p11)
            <= new(new Message(message + "[12]"), t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
        
        constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                    Expression p11, Expression p12)
            <= new(new Message(message + "[13]"), t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
        
        bool IsOperation = true;
        
        int prepare(ExpressionScope scope)
        {
            int len := _arguments.Length;
            int maxStackSize := len + 1;
            
            int argStackSize := _target.prepare(scope);
            if (argStackSize > maxStackSize)
                { maxStackSize := argStackSize; };
            
            for(int i := 0, i < len, i+=1) {
                argStackSize := _arguments[i].prepare(scope);
                if (argStackSize > maxStackSize)
                    { maxStackSize := argStackSize; }
            };

            ^ maxStackSize
        }
        
        saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
        {
            Expression.saveMessageCallTo(list, scope, _target, _arguments, MessageFunction.load(_message));                                                            
            if (index > 0) {
                list.append(new SavingFunction(index));
            }
        }
    }

//    public class ExtensionOrMessageCallExpression : Expression
//    {
//        Message      _message;
//        Expression   _target;
//        Expression[] _arguments;
//        
//        constructor new(Message message, Expression t, params Expression[] arguments)
//        {
//            _message := message;
//            _target := t;
//            _arguments := arguments;
//        }
//        
//        constructor new(Message message, Expression t)
//        {
//            _message := message;
//            _target := t;
//            _arguments := new Expression[](0);
//        }
//        
//        constructor(Expression t, string message)
//            <= new(new Message(message + "[1]"), t);
//        
//        constructor(Expression t, string message, Expression p1)
//            <= new(new Message(message + "[2]"), t, p1);
//        
//        constructor(Expression t, string message, Expression p1, Expression p2)
//            <= new(new Message(message + "[3]"), t, p1, p2);
//        
//        constructor(Expression t, string message, Expression p1, Expression p2, Expression p3)
//            <= new(new Message(message + "[4]"), t, p1, p2, p3);
//        
//        constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4)
//            <= new(new Message(message + "[5]"), t, p1, p2, p3, p4);
//        
//        constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5)
//            <= new(new Message(message + "[6]"), t, p1, p2, p3, p4, p5);
//        
//        constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6)
//            <= new(new Message(message + "[7]"), t, p1, p2, p3, p4, p5, p6);
//        
//        constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7)
//            <= new(new Message(message + "[8]"), t, p1, p2, p3, p4, p5, p6, p7);
//        
//        constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8)
//            <= new(new Message(message + "[9]"), t, p1, p2, p3, p4, p5, p6, p7, p8);
//        
//        constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9)
//            <= new(new Message(message + "[10]"), t, p1, p2, p3, p4, p5, p6, p7, p8, p9);
//        
//        constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10)
//            <= new(new Message(message + "[11]"), t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
//        
//        constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11)
//            <= new(new Message(message + "[12]"), t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
//        
//        constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11, Expression p12)
//            <= new(new Message(message + "[13]"), t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
//        
//        bool IsOperation = true;
//        
//        prepare(ExpressionScope scope)
//        {
//            _target.prepare(scope);
//            
//            int len := _arguments.Length;
//            for(int i := 0, i < len, i+=1) {
//                _arguments[i].prepare(scope);
//            }
//        }
//        
//        saveTo(List<TapeFunction> list, ExpressionScope scope)
//        {
//            TapeFunction messageExpr := scope.resolveExtension(_message);
//            if (messageExpr == nil) {
//                messageExpr := MessageFunction.load(_message)
//            };             
//            
//            Expression.saveMessageCallTo(list, scope, _target, _arguments, messageExpr)
//        }
//    }
//
//    public class FunctionCallExpression : Expression
//    {
//        Message      _message;
//        Expression   _target;
//        Expression[] _arguments;
//        
//        constructor new(Expression t, params Expression[] arguments)
//        {
//            int len := arguments.Length;
//            string messageStr := "#invoke[" + len.toPrintable() + "]";
//            
//            _message := new Message(messageStr);
//            _target := t;
//            _arguments := arguments;
//        }
//        
//        constructor new(Expression t)
//        {
//            _message := new Message("#invoke[0]");
//            _target := t;
//            _arguments := new Expression[](0);
//        }
//        
//        constructor(Expression t)
//            <= new(t);
//        
//        constructor(Expression t, Expression p1)
//            <= new(t, p1);
//        
//        constructor(Expression t, Expression p1, Expression p2)
//            <= new(t, p1, p2);
//        
//        constructor(Expression t, Expression p1, Expression p2, Expression p3)
//            <= new(t, p1, p2, p3);
//        
//        constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4)
//            <= new(t, p1, p2, p3, p4);
//        
//        constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5)
//            <= new(t, p1, p2, p3, p4, p5);
//        
//        constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6)
//            <= new(t, p1, p2, p3, p4, p5, p6);
//        
//        constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7)
//            <= new(t, p1, p2, p3, p4, p5, p6, p7);
//        
//        constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8)
//            <= new(t, p1, p2, p3, p4, p5, p6, p7, p8);
//        
//        constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9)
//            <= new(t, p1, p2, p3, p4, p5, p6, p7, p8, p9);
//        
//        constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10)
//            <= new(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
//        
//        constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11)
//            <= new(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
//        
//        constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11, Expression p12)
//            <= new(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
//
//        bool IsOperation = true;
//        
//        prepare(ExpressionScope scope)
//        {
//            _target.prepare(scope);
//            
//            int len := _arguments.Length;
//            for(int i := 0, i < len, i+=1) {
//                _arguments[i].prepare(scope);
//            }
//        }
//                
//        saveTo(List<TapeFunction> list, ExpressionScope scope)
//        {
//            Expression.saveMessageCallTo(list, scope, _target, _arguments, FuncFunction.load(_message))
//        }
//    }
//
//    public class DynamicMessageCallExpression : Expression
//    {
//        Expression[] _arguments;
//        
//        constructor new(params Expression[] arguments)
//        {
//            _arguments := arguments;
//        }
//        
//        constructor new()
//        {
//            _arguments := new Expression[](0);
//        }
//        
//        constructor(Expression t)
//            <= new(t);
//        
//        constructor(Expression t, Expression p1)
//            <= new(t, p1);
//        
//        constructor(Expression t, Expression p1, Expression p2)
//            <= new(t, p1, p2);
//        
//        constructor(Expression t, Expression p1, Expression p2, Expression p3)
//            <= new(t, p1, p2, p3);
//        
//        constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4)
//            <= new(t, p1, p2, p3, p4);
//        
//        constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5)
//            <= new(t, p1, p2, p3, p4, p5);
//        
//        constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6)
//            <= new(t, p1, p2, p3, p4, p5, p6);
//        
//        constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7)
//            <= new(t, p1, p2, p3, p4, p5, p6, p7);
//        
//        constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8)
//            <= new(t, p1, p2, p3, p4, p5, p6, p7, p8);
//        
//        constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9)
//            <= new(t, p1, p2, p3, p4, p5, p6, p7, p8, p9);
//        
//        constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10)
//            <= new(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
//        
//        constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11)
//            <= new(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
//        
//        constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
//                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
//                    Expression p11, Expression p12)
//            <= new(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
//
//        bool IsOperation = true;
//
//        prepare(ExpressionScope scope)
//        {
//            int len := _arguments.Length;
//            for(int i := 0, i < len, i+=1) {
//                _arguments[i].prepare(scope);
//            }
//        }
//                
//        saveTo(List<TapeFunction> list, ExpressionScope scope)
//        {
//            Expression.saveMessageCallTo(list, scope, _arguments, DynamicMessageFunction)
//        }
//    }

    public class GetPropertyExpression : Expression
    {
        MessageName _name;
        Expression  _target;
        
        constructor(Expression t, string message)
            <= new(new MessageName(message), t);
            
        constructor new(MessageName name, Expression t)
        {
            _name := name;
            _target := t
        }
                
        bool IsOperation = true;

        int prepare(ExpressionScope scope)
            = _target.prepare(scope);
        
        saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
        {
            _target.saveTo(list, scope, 0);
            
            auto mssg := _name.getPropertyMessage();
            list.append(MessageFunction.load(mssg));
            if (index > 0) {
                list.append(new SavingFunction(index));
            }
        }
    }

    public class GetDynamicPropertyExpression : Expression
    {
        Expression   _property;
        Expression   _target;
        int          _tempVariable;  
        
        constructor(Expression t, Expression p)
            <= new(p, t);
            
        constructor new(Expression p, Expression t)
        {
            _property := Expression.MessageCall( 
                            new Message("getPropertyMessage[1]"),
                            Expression.New(
                                Expression.ClassReference("system'MessageName"),
                                p
                            )
                         );
            
            _target := t
        }
        
        bool IsOperation = true;
        
        int prepare(ExpressionScope scope)
        {
            _tempVariable := scope.allocTempVariable();
            
            int maxStackLevel := _property.prepare(scope);
            int current := _target.prepare(scope);
            if (current > maxStackLevel)
                { maxStackLevel := current };
            
            ^ maxStackLevel;
        }

        saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
        {
            _property.saveTo(list, scope, 0);
            list.append(new AssigningFunction(_tempVariable));                       
            
            _target.saveTo(list, scope, 0);

            list.append(new DynamicMessageFunction(_tempVariable));
            
            if (index > 0) {
                list.append(new SavingFunction(index));
            }
        }
    }

    public class SetPropertyExpression : Expression
    {
        MessageName _name;
        Expression  _target;
        Expression  _value;
        
        constructor(Expression t, string message, Expression v)
            <= new(new MessageName(message), t, v);
            
        constructor new(MessageName name, Expression t, Expression v)
        {
            _name := name;
            _target := t;
            _value := v
        }
        
        bool IsOperation = true;
        
        int prepare(ExpressionScope scope)
        {
            int maxStackSize := 2;
            int argStackSize := _target.prepare(scope);
            if (argStackSize > maxStackSize)
                { maxStackSize := argStackSize; };
            
            argStackSize := _value.prepare(scope);            
            if (argStackSize > maxStackSize)
                { maxStackSize := argStackSize; };
            
            ^ maxStackSize;
        }
        
        saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
        {
            _value.saveTo(list, scope, 1);
            _target.saveTo(list, scope, 0);
            
            auto mssg := _name.setPropertyMessage();
            list.append(MessageFunction.load(mssg));
            
            if (index > 0) {
                list.append(new SavingFunction(index));
            }
        }
    }

    public class SetDynamicPropertyExpression : Expression
    {
        Expression _property;
        Expression  _target;
        Expression  _value;
        int         _tempVariable;  
        
        constructor(Expression t, Expression p, Expression v)
            <= new(p, t, v);
            
        constructor new(Expression p, Expression t, Expression v)
        {
            _property := Expression.MessageCall( 
                            new Message("setPropertyMessage[1]"),
                            Expression.New(
                                Expression.ClassReference("system'MessageName"),
                                p
                            )
                         );
            
            _target := t;
            _value := v
        }
        
        bool IsOperation = true;
        
        int prepare(ExpressionScope scope)
        {
            _tempVariable := scope.allocTempVariable();
            
            int maxStackLevel := _property.prepare(scope);

            int current := _target.prepare(scope);
            if (current > maxStackLevel)
                { maxStackLevel := current };

            current := _value.prepare(scope);
            if (current > maxStackLevel)
                { maxStackLevel := current };
                                    
            ^ maxStackLevel;
        }
        
        saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
        {
            _property.saveTo(list, scope, 0);
            list.append(new AssigningFunction(_tempVariable));                       
            
            _value.saveTo(list, scope, 1);
            _target.saveTo(list, scope, 0);

            list.append(new DynamicMessageFunction(_tempVariable));
            
            if (index > 0) {
                list.append(new SavingFunction(index));
            }
        }
    }

//    public class DeclarationExpression : Expression
//    {
//        ScopeVariable  variable;
//        TapeFunction local;
//        
//        constructor(ScopeVariable variable)
//        {
//            this variable := variable;
//        }
//        
//        bool IsOperation = true;
//
//        prepare(ExpressionScope scope)
//        {
//            scope.declare(variable);
//            
//            local := scope.resolve(variable, mode_lcl);
//        }
//        
//        saveTo(List<TapeFunction> list, ExpressionScope scope)
//        {
//            list.append(local);        
//        }    
//    }
//
//    public class AssigningExpression : Expression
//    {
//        ScopeVariable  variable;
//        Expression     expression;  
//        TapeFunction assigning;
//        
//        constructor(ScopeVariable variable, Expression expression)
//        {
//            this variable := variable;
//            this expression := expression
//        }
//
//        constructor(string variable, Expression expression)
//        {
//            this variable := new ScopeVariable(variable);
//            this expression := expression
//        }
//
//        bool IsOperation = false;
//
//        prepare(ExpressionScope scope)
//        {
//            assigning := scope.resolve(variable, mode_asn);
//            
//            expression.prepare(scope)
//        }
//                
//        saveTo(List<TapeFunction> list, ExpressionScope scope)
//        {
//            expression.saveTo(list, scope);
//            list.append(assigning);
//        }
//    }
//
//    public class DeclaringAndAssigningExpression : AssigningExpression
//    {
//        constructor(ScopeVariable variable, Expression expression)
//        {
//            this variable := variable;
//            this expression := expression
//        }
//
//        constructor(string variable, Expression expression)
//        {
//            this variable := new ScopeVariable(variable);
//            this expression := expression
//        }
//
//        prepare(ExpressionScope scope)
//        {
//            scope.declare(variable);
//            
//            super.prepare(scope);
//        }
//                
//        saveTo(List<TapeFunction> list, ExpressionScope scope)
//        {
//            new ConstantExpression(nilValue).saveTo(list, scope);
//            
//            super.saveTo(list, scope);
//        }
//    }
//
//    public class IdentifierExpression : Expression
//    {
//        ScopeVariable  name;
//        TapeFunction identifier;
//        
//        constructor(ScopeVariable name)
//        {
//            this name := name
//        }
//
//        bool IsOperation = false;
//
//        prepare(ExpressionScope scope)
//        {
//            identifier := scope.resolve(name, mode_non);
//        }
//                
//        saveTo(List<TapeFunction> list, ExpressionScope scope)
//        {
//            list.append(identifier);
//        }
//    }

    public struct VariableByIndexExpression : Expression
    {
        int varIndex;
        
        constructor(int index)
        {
            this varIndex := index
        }
        
        bool IsOperation = false;
        
        int prepare(ExpressionScope scope) 
            = 1;
            
        saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
        {
            if (index == 0) {
                list.append(new LocalFunction(varIndex));
            }
            else {
                list.append(new CopyToStackFunction(varIndex, index));
            }
        }
        
    }

    public class IdentifierExpression : Expression
    {
        ScopeIdentifier ident;
        TapeFunction    local;
        
        constructor(ScopeIdentifier ident)
        {
            this ident := ident;
        }

        bool IsOperation = false;

        int prepare(ExpressionScope scope)
        {
            local := scope.resolve(ident);
            
            ^ 1
        }
                
        saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
        {
            list.append(local);
            if (index > 0) {
                list.append(new SavingFunction(index));
            }
        }
    }

    public class VariableExpression : IdentifierExpression
    {
        constructor(string name)
        {
            this ident := ScopeIdentifier.Variable(name)
        }
    }

//    public class PreviousVariableExpression : VariableExpression
//    {
//        constructor(ScopeVariable variable)
//        {
//            this variable := variable
//        }
//
//        prepare(ExpressionScope scope)
//        {            
//            local := scope.Previous.resolve(variable, mode_lcl)
//        }
//    }
//
//    public class NestedBlockExpression : CodeblockExpression
//    {
//        constructor new(params Expression[] statements)
//        {
//            this statements := statements;
//        }
//
//        constructor(Expression s1)
//            <= new(s1); 
//                        
//        constructor(Expression s1, Expression s2)
//            <= new(s1, s2); 
//                        
//        constructor(Expression s1, Expression s2, Expression s3)
//            <= new(s1, s2, s3); 
//                        
//        constructor(Expression s1, Expression s2, Expression s3, Expression s4)
//            <= new(s1, s2, s3, s4); 
//                        
//        constructor(Expression s1, Expression s2, Expression s3, Expression s4, Expression s5)
//            <= new(s1, s2, s3, s4, s5); 
//                        
//        constructor(Expression s1, Expression s2, Expression s3, Expression s4, Expression s5, 
//                    Expression s6)
//            <= new(s1, s2, s3, s4, s5, s6); 
//                        
//        constructor(Expression s1, Expression s2, Expression s3, Expression s4, Expression s5, 
//                    Expression s6, Expression s7)
//            <= new(s1, s2, s3, s4, s5, s6, s7); 
//                        
//        constructor(Expression s1, Expression s2, Expression s3, Expression s4, Expression s5, 
//                    Expression s6, Expression s7, Expression s8)
//            <= new(s1, s2, s3, s4, s5, s6, s7, s8); 
//                        
//        constructor(Expression s1, Expression s2, Expression s3, Expression s4, Expression s5, 
//                    Expression s6, Expression s7, Expression s8, Expression s9)
//            <= new(s1, s2, s3, s4, s5, s6, s7, s8, s9); 
//                        
//        constructor(Expression s1, Expression s2, Expression s3, Expression s4, Expression s5, 
//                    Expression s6, Expression s7, Expression s8, Expression s9, Expression s10)
//            <= new(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10); 
//                        
//        constructor(Expression s1, Expression s2, Expression s3, Expression s4, Expression s5, 
//                    Expression s6, Expression s7, Expression s8, Expression s9, Expression s10, 
//                    Expression s11)
//            <= new(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11); 
//                        
//        constructor(Expression s1, Expression s2, Expression s3, Expression s4, Expression s5, 
//                    Expression s6, Expression s7, Expression s8, Expression s9, Expression s10, 
//                    Expression s11, Expression s12)
//            <= new(s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12); 
//                        
//        prepare(ExpressionScope scope)
//        {
//            variables := new List<ScopeVariable>();
//            
//            int parentLevel := scope.ScopeLevel;
//            auto codeScope := new ExpressionScope
//            {
//                get int ScopeLevel() => scope;
//                
//                ExpressionScope Previous = scope;
//                
//                TapeFunction resolveExtension(Message message)
//                    => scope;
//                
//                TapeFunction resolve(ScopeVariable variable, int mode)
//                {
//                    int mode_mask := mode && mode_lcl;
//                    
//                    if (mode == 0 || mode_mask == mode_lcl)
//                    {
//                        int len := variables.Length;
//                        for(int i := 0, i < len, i+=1) {
//                            if (variables[i] == variable)
//                            {
//                                int index := parentLevel + i + 1;
//                                
//                                if (mode == mode_asn) 
//                                {
//                                    ^ new AssigningFunction(index)
//                                }
//                                else 
//                                {
//                                    ^ new LocalFunction(index)
//                                };                                
//                            }
//                        }
//                    };
//                    
//                    ^ scope.resolve(variable, mode)
//                }
//        
//                declare(ScopeVariable variable)
//                {
//                    variables.append(variable);
//                    
//                    var tempLocal := new ScopeVariable("$$" + scope.ScopeLevel.toPrintable());
//                    scope.declare(tempLocal);                    
//                }            
//            };
//            
//            int len := statements.Length;
//            for(int i := 0, i < len, i+=1) {
//                statements[i].prepare(codeScope);
//            }
//        }
//                        
//        saveTo(List<TapeFunction> list, ExpressionScope scope)
//        {
//            var eosClosure := new ReleaseFunction(1);
//            int len := statements.Length;
//            for(int i := 0, i < len, i+=1) {
//                statements[i].saveTo(list, scope);
//                list.append(eosClosure)
//            };
//        }
//    }

    public class ReturnExpression : Expression
    {
        Expression expr;
        
        constructor(Expression expr)
        {
            this expr := expr
        }
        
        bool IsOperation = false;

        int prepare(ExpressionScope scope)
            = expr.prepare(scope);

        saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
        {
            expr.saveTo(list, scope, 0);
            list.append(JumpToEnd)
        }
    }

//    public class IfExpression : Expression
//    {
//        Expression condition;
//        Expression body;
//        Expression elseBody;
//        
//        constructor(Expression condition, Expression body)
//        {
//            this condition := condition;
//            this body := body;
//        }
//
//        constructor(Expression condition, Expression body, Expression elseBody)
//        {
//            this condition := condition;
//            this body := body;
//            this elseBody := elseBody;
//        }
//
//        prepare(ExpressionScope scope)
//        {
//            condition.prepare(scope);
//            body.prepare(scope);            
//            elseBody.?prepare(scope);
//        }
//                
//        bool IsOperation = true;
//
//        saveTo(List<TapeFunction> list, ExpressionScope scope)
//        {
//            condition.saveTo(list, scope);
//            int bookmark := list.Length;
//            list.append(cast TapeFunction(nilValue));
//            body.saveTo(list, scope);
//            if (elseBody != nil) {
//                int bookmark2 := list.Length;
//                list.append(cast TapeFunction(nilValue));
//                
//                //int offs := list.Length - bookmark - 1;
//                int offs := list.Length - bookmark;
//                offs := offs - 1;
//                list[bookmark] := new JumpIfNotFunction(offs);
//                
//                elseBody.saveTo(list, scope);
//                
//                offs := list.Length - bookmark2;
//                offs := offs - 1;                
//                list[bookmark2] := new GoToFunction(offs);
//            }
//            else {
//                //int offs := list.Length - bookmark - 1;
//                int offs := list.Length - bookmark;
//                offs := offs - 1;
//                
//                list[bookmark] := new JumpIfNotFunction(offs);
//                list.append(ConstantFunction.load(nilValue)); // !! should be redesigned
//            }            
//        }
//    }
//
//    public class LoopExpression : Expression
//    {
//        Expression condition;
//        Expression body;
//        
//        constructor(Expression condition, Expression body)
//        {
//            this condition := condition;
//            this body := body
//        }
//
//        prepare(ExpressionScope scope)
//        {
//            condition.prepare(scope);
//            body.prepare(scope);
//        }
//                
//        bool IsOperation = true;
//                
//        saveTo(List<TapeFunction> list, ExpressionScope scope)
//        {
//            int top := list.Length;
//            
//            condition.saveTo(list, scope);
//            int bookmark := list.Length;
//            list.append(cast TapeFunction(nilValue));
//            body.saveTo(list, scope);
//            
//            int offs := top - list.Length - 1;
//            
//            list.append(new GoToFunction(offs));
//            
//            //offs := list.Length - bookmark - 1;
//            offs := list.Length - bookmark;
//            offs := offs - 1;
//            
//            list[bookmark] := new JumpIfNotFunction(offs);
//            
//            list.append(ConstantFunction.load(nilValue)); // !! should be redesigned
//        }
//    }

    public class MethodExpression : Expression
    {
        Message           message;
        ScopeIdentifier[] parameters;
        Expression        body;

        constructor new(string name, Expression body, params ScopeIdentifier[] params)
        {
            int len := params.Length;
            
            auto text := TextBuilder.load(name);
            text.append("[");
            text.append(len + 1);
            text.append("]");
            
            this message := new Message(text.Value);
            
            // revese the order
            this parameters := new ScopeIdentifier[](len + 1);
            
            for (int i := 0, int j := len - 1, i < len, i += 1, j -= 1)
            {
                parameters[i] := params[j]
            };
            parameters[len] := ScopeIdentifier.Variable("self");
            
            this body := body;
        }         
                        
        constructor(string name, MethodParameterList parameters, Expression body)
        {
            int len := parameters.Length;
            
            auto text := TextBuilder.load(name);
            text.append("[");
            text.append(len);
            text.append("]");
            
            this body := body;
            this message := new Message(*text);
            this parameters := *parameters;
        }
                        
        constructor(string name, Expression body)
        {
            this parameters := new ScopeIdentifier[]{ ScopeIdentifier.Variable("self") };
            
            auto text := TextBuilder.load(name);
            text.append("[1]");
            
            this body := body;
            this message := new Message(*text);
        }
     
        bool IsOperation = true;

        saveTo(List<Tuple<Message,DynamicTape>> list, ExpressionScope scope)
        {
            list.append(new Tuple<Message,DynamicTape>(message, self.compile(scope)));
        }

        saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
        {    
            body.saveTo(list, scope, index);
        }
        
        int prepare(ExpressionScope scope)
        {
            auto methodScope := new ExpressionScope
            {
                int ScopeLevel = parameters.Length;
                
//                ExpressionScope Previous = scope;
//                
//                TapeFunction resolveExtension(Message message)
//                    => scope;

                bool reserve(int index)
                    = scope.reserve(index);
                
                int allocTempVariable()
                    = scope.allocTempVariable();
                
                int Reserved 
                    = scope.Reserved;

                TapeFunction resolve(ScopeIdentifier variable)
                {
                    int len := parameters.Length;
                    for(int i := 0, i < len, i+=1) {
                        if (variable == parameters[i])
                        {
                            int index := i + 1;
//                                
//                                if (mode == mode_asn) 
//                                {
//                                    NotSupportedException.raise()
//                                }
//                                else 
//                                {
                                    ^ new LocalFunction(index)
//                                };                                
                        }
                    };
                    
                    ^ scope.resolve(variable)
                }
        
//                declare(ScopeVariable variable)
//                {
//                }            
            };
    
            ^ body.prepare(methodScope);        
        }

        DynamicTape compile(ExpressionScope scope)
            = super.compile(new RootExpressionScope(scope));

    }

//    public class GetMethodExpression : MethodExpression
//    {
//        constructor(string name, Expression body)
//        //    <= (new MessageName(name).getPropertyMessage(), body);   // !! temporally commented out
//        {
//            this parameters := new ScopeVariable[]{ new ScopeVariable("self") };
//            this message := new MessageName(name).getPropertyMessage();
//            
//            this body := body;
//        }
//    
//    }
//
//    public class ActionMethodExpression : MethodExpression
//    {
//        constructor(Expression body)
//        //    <= (new Message("[0]"), body);   // !! temporally commented out
//        {
//            this parameters := new ScopeVariable[]{ new ScopeVariable("self") };
//            this message := new Message("[0]");
//            
//            this body := body;
//        }
//        
//        constructor(ScopeVariable param1, Expression body)
//        //    <= (new Message("[0]"), body);   // !! temporally commented out
//        {
//            this parameters := new ScopeVariable[]{ param1, new ScopeVariable("self") };
//            this message := new Message("[1]");
//            
//            this body := body;
//        }
//        
//        constructor(ScopeVariable param1, ScopeVariable param2, Expression body)
//        //    <= (new Message("[0]"), body);   // !! temporally commented out
//        {
//            this parameters := new ScopeVariable[]{ param2, param1, new ScopeVariable("self") };
//            this message := new Message("[2]");
//            
//            this body := body;
//        }
//        
//        constructor(ScopeVariable param1, ScopeVariable param2, ScopeVariable param3, Expression body)
//        //    <= (new Message("[0]"), body);   // !! temporally commented out
//        {
//            this parameters := new ScopeVariable[]{ param3, param2,param1, new ScopeVariable("self") };
//            this message := new Message("[3]");
//            
//            this body := body;
//        }
//        
//        constructor(ScopeVariable param1, ScopeVariable param2, ScopeVariable param3, ScopeVariable param4, Expression body)
//        //    <= (new Message("[0]"), body);   // !! temporally commented out
//        {
//            this parameters := new ScopeVariable[]{ param4,param3,param2,param1, new ScopeVariable("self") };
//            this message := new Message("[4]");
//            
//            this body := body;
//        }
//        
//        constructor(ScopeVariable param1, ScopeVariable param2, ScopeVariable param3, ScopeVariable param4, 
//                        ScopeVariable param5, Expression body)
//        //    <= (new Message("[0]"), body);   // !! temporally commented out
//        {
//            this parameters := new ScopeVariable[]{ param5,param4,param3,param2,param1, new ScopeVariable("self") };
//            this message := new Message("[5]");
//            
//            this body := body;
//        }
//        
//        constructor(ScopeVariable param1, ScopeVariable param2, ScopeVariable param3, ScopeVariable param4, 
//                        ScopeVariable param5, ScopeVariable param6, Expression body)
//        //    <= (new Message("[0]"), body);   // !! temporally commented out
//        {
//            this parameters := new ScopeVariable[]{ param6,param5,param4,param3,param2,param1, new ScopeVariable("self") };
//            this message := new Message("[6]");
//            
//            this body := body;
//        }
//    }

    public class ClosureExpression : Expression
    {
        ScopeIdentifier[] parameters;
        Expression        body;
        
        constructor(Expression body)
        {
            this parameters := new ScopeIdentifier[](0);
            
            this body := body;
        } 
        
        constructor new(Expression body, params ScopeIdentifier[] params)
        {
            // revese the order
            int len := params.Length;
            this parameters := new ScopeIdentifier[](len);
            for (int i := 0, int j := len - 1, i < len, i += 1, j -= 1)
            {
                parameters[i] := params[j]
            };
            
            this body := body;
        } 
        
//        constructor(ScopeVariable param1, Expression body)
//        {
//            this parameters := new ScopeVariable[]{ param2 }; // NOTE : it should be in reversed order
//            
//            this body := body;
//        }
//        
//        constructor(ScopeVariable param1, ScopeVariable param2, Expression body)
//        {
//            this parameters := new ScopeVariable[]{ param2, param1 }; // NOTE : it should be in reversed order
//            
//            this body := body;
//        }
//        
//        constructor(ScopeVariable param1, ScopeVariable param2, ScopeVariable param3, Expression body)
//        {
//            this parameters := new ScopeVariable[]{ param3, param2, param1 }; // NOTE : it should be in reversed order
//            
//            this body := body;
//        }        
     
        bool IsOperation = true;
     
        int prepare(ExpressionScope scope)
        {
            auto closureScope := new ExpressionScope
            {
                int ScopeLevel = parameters.Length;
                
//                ExpressionScope Previous = scope;
//                
//                TapeFunction resolveExtension(Message message)
//                    => scope;
                
                bool reserve(int index)
                    = scope.reserve(index);
                
                int allocTempVariable()
                    = scope.allocTempVariable();
                
                int Reserved 
                    = scope.Reserved;
                
                TapeFunction resolve(ScopeIdentifier variable)
                {
                    int len := parameters.Length;
                    for(int i := 0, i < len, i+=1) {
                        if (variable == parameters[i])
                        {
                            int index := i + 1;
//                                
//                                if (mode == mode_asn) 
//                                {
//                                    NotSupportedException.raise()
//                                }
//                                else 
//                                {
                                    ^ new LocalFunction(index)
//                                };                                
                        }
                    };
                    
                    ^ scope.resolve(variable)
                }
        
//                declare(ScopeVariable variable)
//                {
//                }            
            };

            ^ body.prepare(closureScope);
        }
     
        saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
        {    
            body.saveTo(list, scope, index);
        }    
    }

//    public class NestedExpression : Expression
//    {
//        DynamicSingleton singleton;
//        
//        constructor(DynamicSingleton singleton)
//        {
//            this singleton := singleton;
//        }
//        
//        bool IsOperation = true;
//
//        prepare(ExpressionScope scope)
//        {
//        }
//        
//        saveTo(List<TapeFunction> list, ExpressionScope scope)
//        {
//            var tape := singleton.compile(nil, scope);
//            
//            list.append(ConstantFunction.load(tape));
//        }      
//    }

    public abstract DynamicSymbol
    {
        abstract compile(module, ExpressionScope scope);
    }
 
//    public class DynamicImport : DynamicSymbol
//    {
//        ScopeVariable  import;
//        
//        constructor(ScopeVariable import)
//        {
//            this import := import;
//        }
//        
//        compile(module, ExpressionScope scope)
//        {
//            scope.declare(import);
//            
//            ^ nil
//        }
//    }
//
//    public sealed class DynamicExpressionSymbol : DynamicSymbol
//    {
//        Expression expr;
//        
//        constructor(Expression expr)
//        {
//            this expr := expr
//        }
//        
//        compile()
//            <= compile(nil, IdleExpressionScope);
//
//        compile(module, ExpressionScope scope)
//        {
//            ^ expr.compile(scope)
//        }        
//    }
          
    public sealed class DynamicSingleton : DynamicSymbol
    {
        Expression[] methods;
    
        constructor()
        {
            this methods := new Expression[](0);
        }
            
        constructor(params Expression[] methods)
        {
            this methods := methods;
        }
                
        constructor new(params Expression[] methods)
        {
            this methods := methods;                
        }
                    
        compile()
            <= compile(nil, IdleExpressionScope);

        compile(module, ExpressionScope scope)
        {
            auto list := new List<Tuple<Message,DynamicTape>>();
            methods.forEach:(m){m.saveTo(list, scope)};
            
            ^ DynamicClass.create(list);
        }
    }
    
//    public class SymbolInfo
//    {
//        prop string Name;
//        prop DynamicSymbol Symbol;
//        
//        constructor(string name, DynamicSymbol symbol)
//        {
//            Name := name;
//            Symbol := symbol
//        } 
//        constructor(DynamicSymbol symbol)
//        {
//            Symbol := symbol
//        } 
//    }
//    
//    [info:"a dynamic namespace, containing list of symbols"]
//    public class SymbolCollection
//    {
//        SymbolInfo[] symbols;
//        string[]     imports := new string[] { "system" };
//
//        constructor(SymbolInfo symbol1)
//            <= load(symbol1);
//                
//        constructor(SymbolInfo symbol1, SymbolInfo symbol2)
//            <= load(symbol1, symbol2);
//                
//        constructor(SymbolInfo symbol1, SymbolInfo symbol2, SymbolInfo symbol3)
//            <= load(symbol1, symbol2, symbol3);
//                
//        constructor(SymbolInfo symbol1, SymbolInfo symbol2, SymbolInfo symbol3, SymbolInfo symbol4)
//            <= load(symbol1, symbol2, symbol3, symbol4);
//
//        constructor(params SymbolInfo[] symbols)
//            <= load(params symbols);
//                                
//        constructor load(params SymbolInfo[] symbols)
//        {
//            this symbols := symbols;        
//        }
// 
//        compile()
//            <= compile(new DynamicStruct());
// 
//        compile(set)
//            <= compile(set, new ExpressionScope
//            {
//                get int ScopeLevel() 
//                {
//                    NotSupportedException.raise();
//                    
//                    ^ nil // !! to supress warning
//                }
//                
//                declare(ScopeVariable variable) 
//                {
//                    auto subarr := new string[] { variable.Name };
//                    
//                    if (imports == nil)
//                    {
//                        imports := subarr
//                    }
//                    else
//                    {
//                        imports := imports + subarr
//                    }
//                }
//                
//                ExpressionScope Previous
//                    = nil;
//                
//                TapeFunction resolveExtension(Message message)
//                {
//                    auto varMessage := message.MessageName.VariadicMessage;
//                    
//                    var op := ExtensionDispatcher.load(message, imports);
//                    if (op != nil)
//                    {
//                        ^ ExtensionMessageFunction.load(op, message)
//                    }
//                    else {
//                        var op2 := VariadicExtensionDispatcher.load(varMessage, imports);
//                        if (op2 != nil) {
//                            ^ VariadicExtensionMessageFunction.load(op2, message)
//                        }
//                    };
//                    
//                    ^ nil
//                }
//                
//                TapeFunction resolve(ScopeVariable variable, int mode) 
//                {
//                    var refName := variable.Name;
//                    
//                    if (mode == 0 || mode == mode_class)
//                    {
//                        // look for external references
//                        if(imports != nil) {
//                            int len := imports.Length;
//                            for(int i := 0, i < len, i+=1) {
//                                var symbol := Symbol.tryLoad(imports[i], refName);
//                                if (symbol != nil)
//                                {
//                                    ^ ConstantFunction.load(symbol)
//                                }
//                            }
//                        };
//                        
//                        var prop := new MessageName(refName).getPropertyMessage();
//                        
//                        var symbol := prop(set);
//                        if (symbol != nil)
//                        {
//                            ^ ConstantFunction.load(symbol);
//                        }
//                    };
//                    
//                    ^ nil
//               }                
//            });
// 
//        compile(set, ExpressionScope scope)
//        {
//            int len := symbols.Length;
//            for (int i := 0, i < len, i+=1) {
//                auto info := symbols[i];
//                
//                var symbolName := info.Name;
//                if (symbolName == nil) {
//                    info.Symbol.compile(nil, scope);
//                }
//                else {
//                    MessageName name := new MessageName(symbolName);
//                    Message prop := name.setPropertyMessage(); 
//        
//                    var tape := info.Symbol.compile(set, scope);
//         
//                    prop(set, tape);
//                }                
//            };
//            
//            ^ set
//        }
//        
//        [info:"Exectutes the program entry point"]
//        eval()
//        {
//            var set := new DynamicStruct();
//            
//            var tape := self.compile(set);
//            
//            var program := tape.program;
//            ^ program();
//        }
//    }
}
