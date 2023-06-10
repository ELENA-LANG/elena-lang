import system;
import system'collections;
import system'routines;
import system'text;

namespace expressions
{
   const int mode_lcl   = 1;

   symbol used : preloaded = new object[]{Expression, DynamicSingleton/*,DynamicExpressionSymbol,SymbolCollection,DynamicImport*/};

   public class ScopeIdentifier
   {
      string Name : prop;
      int Mode : prop;
        
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

   public interface ExpressionScope
   {
      abstract bool reserve(int level);

      get abstract int Reserved();

      abstract int allocTempVariable();

      get abstract int ScopeLevel();

      abstract TapeFunction resolve(ScopeIdentifier ident, bool assignMode);

      abstract declare(ScopeIdentifier variable);
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
            
      declare(ScopeIdentifier variable)
      {
         ^ nil
      }
        
      TapeFunction resolve(ScopeIdentifier ident, bool assignMode)
         = nil; 
   }

   public class RootExpressionScope : ExpressionScope
   {
      Reference<int>  reserved := 0;
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
        
      TapeFunction resolve(ScopeIdentifier variable, bool assignMode) 
      {
         ^ parent.?resolve(variable, assignMode)
      }
    
      declare(ScopeIdentifier variable)
      {
         ^ nil
      }
   }

   public interface Expression
   {
      static Expression Symbol(string s)
         = new SymbolExpression(s);

      static Expression MessageCall(Message message, Expression t)
         = MessageCallExpression.new(message,t);      

      static Expression Constant(object o)
         = new ConstantExpression(o);

      static Expression GetProperty(MessageName name, Expression t)
         = GetPropertyExpression.new(name, t);

      static Expression SetProperty(MessageName name, Expression t, Expression v)
         = SetPropertyExpression.new(name, t, v);

      static Expression Closure(Expression body)
         = new ClosureExpression(body);        
              
      static Expression Closure(Expression body, params ScopeIdentifier[] variables)
         = ClosureExpression.new(body, params variables);        

      static Expression CodeBlock(params Expression[] statements)
         = CodeblockExpression.new(params statements);

      static Expression SetDynamicProperty(Expression m, Expression t, Expression v)
         = SetDynamicPropertyExpression.new(m, t, v);

      static Expression GetDynamicProperty(Expression m, Expression t)
         = GetDynamicPropertyExpression.new(m, t);

      static Expression Return(Expression expr)
         = new ReturnExpression(expr);

      static Expression Variable(string variable)
         = new VariableExpression(variable);

      static Expression ClassReference(string s)
         = new ConstantExpression(new ClassReference(s));

      static Expression New(object t)
         = new NewExpression(t);
           
      static Expression New(object t, Expression p0)
         = new NewExpression(t, p0);

      static Expression Method(string name, Expression body, params ScopeIdentifier[] variables)
         = MethodExpression.new(name, body, params variables);

      get abstract bool IsOperation();

      abstract saveTo(List<TapeFunction> list, ExpressionScope scope, int index);

      saveTo(List<Tuple<Message,DynamicTape>> list, ExpressionScope scope)
      {
         NotSupportedException.raise()
      }

      abstract int prepare(ExpressionScope scope) 
            : info("Prepares the expression tree. Returns the maximal size of the preallocated stack");

      internal static saveMessageCallTo(List<TapeFunction> list, ExpressionScope scope, Expression target, 
                                            Expression[] arguments, TapeFunction operation)
      {
         int len := arguments.Length;
         Queue<Expression> tempVariables := nil;
            
         // first pass - preallocating sub expr 
         if (target.IsOperation && len > 0) {
            int tempVar := scope.allocTempVariable();
            tempVar += scope.ScopeLevel;
            target.saveTo(list, scope, 0);
            list.append(new AssigningFunction(tempVar));
                
            tempVariables.!push(Expression.VariableByIndex(tempVar));                
         };
            
         for (int i := 0, i < len, i += 1) {
            if (arguments[i].IsOperation) {
               int tempVar := scope.allocTempVariable();
               tempVar += scope.ScopeLevel;
                                    
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

      internal static saveFunctionCallTo(List<TapeFunction> list, ExpressionScope scope, 
                                            Expression[] arguments, TapeFunction operation)
      {
         int len := arguments.Length;
         Queue<Expression> tempVariables := nil;
            
         for (int i := 0, i < len, i += 1) {
            if (arguments[i].IsOperation) {
               int tempVar := scope.allocTempVariable();
               tempVar += scope.ScopeLevel;
                                    
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
               arguments[i].saveTo(list, scope, i) 
            }
         };
                                    
         list.append(operation);
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

   public class SymbolExpression : Expression
   {
      Symbol symbol;
        
      constructor(string s)
      {
         symbol := new Symbol(s)
      }

      bool IsOperation = false;

      int prepare(ExpressionScope scope)
         = 1;

      eval()
         = symbol;

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

      constructor new(Message message, Expression t)
      {
         _message := message;
         _target := t;
         _arguments := new Expression[](0);
      }

      constructor new(Message message, Expression t, params Expression[] arguments)
      {
         _message := message;
         _target := t;
         _arguments := arguments;
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

   public class MethodParameterList
   {
      ScopeIdentifier[] parameters;
        
      constructor(params string[] variables) 
      {
         int len := variables.Length;
            
         this parameters := new ScopeIdentifier[](len + 2);
         parameters[0] := ScopeIdentifier.Variable("$tape");
         parameters[1] := ScopeIdentifier.Variable("self");

         for (int i := 0, i < len, i += 1)
         {
            parameters[i] := ScopeIdentifier.Variable(variables[i])
         };
      }
         
      get int Length()
         => parameters;
            
      ScopeIdentifier[] Value
         = parameters;
   }

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
            
         this parameters := new ScopeIdentifier[](len + 2);
            
         parameters[0] := new ScopeIdentifier("$tape", 0);
         parameters[1] := ScopeIdentifier.Variable("self");

         for (int i := 0, i < len, i += 1)
         {
            parameters[i + 2] := params[i]
         };
            
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
         this parameters := new ScopeIdentifier[]
         { 
            ScopeIdentifier.Variable("$tape"), 
            ScopeIdentifier.Variable("self")
         };
            
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
                
            bool reserve(int index)
               = scope.reserve(index);
                
            int allocTempVariable()
               = scope.allocTempVariable();
                
            int Reserved 
               = scope.Reserved;

            TapeFunction resolve(ScopeIdentifier variable, bool assignMode)
            {
               int len := parameters.Length;
               for(int i := 0, i < len, i+=1) {
                  if (variable == parameters[i])
                  {
                     int index := i + 1;
                                
                     if (assignMode) 
                     {
                        NotSupportedException.raise()
                     }
                     else 
                     {
                        ^ new LocalFunction(index)
                     };                                
                  }
               };
                    
               ^ scope.resolve(variable, assignMode)
            }
        
            declare(ScopeIdentifier variable)
            {
            }            
         };
    
         ^ body.prepare(methodScope);        
      }

      DynamicTape compile(ExpressionScope scope)
         = super.compile(new RootExpressionScope(scope));

   }

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
         this parameters := new ScopeIdentifier[](len + 1);
         parameters[0] := new ScopeIdentifier("$tape", 0);
         for (int i := 0, i < len, i += 1)
         {
            parameters[i + 1] := params[i]
         };
            
         this body := body;
      } 

      bool IsOperation = true;
     
      int prepare(ExpressionScope scope)
      {
         auto closureScope := new ExpressionScope
         {
            int ScopeLevel = parameters.Length;
                
            bool reserve(int index)
               = scope.reserve(index);
                
            int allocTempVariable()
               = scope.allocTempVariable();
                
            int Reserved 
               = scope.Reserved;
                
            TapeFunction resolve(ScopeIdentifier variable, bool assignMode)
            {
               int len := parameters.Length;
               for(int i := 0, i < len, i+=1) {
                  if (variable == parameters[i])
                  {
                     int index := i + 1;
                            
                     if (assignMode) 
                     {
                        NotSupportedException.raise()
                     }
                     else 
                     {
                        ^ new LocalFunction(index)
                     };                                
                  }
               };
                    
               ^ scope.resolve(variable, assignMode)
            }
        
            declare(ScopeIdentifier variable)
            {
            }            
         };

         ^ body.prepare(closureScope);
      }
     
      saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
      {    
         body.saveTo(list, scope, index);
      }    
   }      

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
                
            TapeFunction resolve(ScopeIdentifier variable, bool assignMode)
            {
               int prevLevel := scope.ScopeLevel;
               int len := variables.Length;
               for(int i := 0, i < len, i+=1) {
                  if (variable == variables[i])
                  {
                     int index := i + 1;
                     index += prevLevel;

                     if (assignMode) 
                     {
                        ^ new AssigningFunction(index)
                     }
                     else 
                     {
                        ^ new LocalFunction(index)
                     };                                
                  }
               };
                    
               ^ scope.resolve(variable, assignMode)
            }                
               
            int ScopeLevel = scope.ScopeLevel + variables.Length;
        
            declare(ScopeIdentifier variable)
            {
               int index := variables.Length + 1;
               scope.reserve(index);
                    
               variables.append(variable)
            }            
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
                           system'MessageName,
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
                              system'MessageName,
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
         local := scope.resolve(ident, false);
            
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

   // --- NewExpression

   public class NewExpression : Expression
   {
      Message      _message;
      object       _target;
      Expression[] _arguments;

      constructor new(object t)
      {
         _message := new Message("#constructor[0]");
         _target := t;
         _arguments := new Expression[](0);
      }

      constructor new(object t, params Expression[] arguments)
      {
         int len := arguments.Length;

         _message := new Message("#constructor[" + len.toPrintable() + "]");
         _target := t;
         _arguments := arguments;
      }

      constructor(object t)
         <= new(t);
        
      constructor(object t, Expression p1)
         <= new(t, p1);
        
      constructor(object t, Expression p1, Expression p2)
         <= new(t, p1, p2);
        
      constructor(object t, string message, Expression p1, Expression p2, Expression p3)
         <= new(t, p1, p2, p3);
        
      constructor(object t, string message, Expression p1, Expression p2, Expression p3, Expression p4)
         <= new(t, p1, p2, p3, p4);
        
      constructor(object t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5)
         <= new(t, p1, p2, p3, p4, p5);
        
      constructor(object t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6)
         <= new(t, p1, p2, p3, p4, p5, p6);
        
      constructor(object t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7)
         <= new(t, p1, p2, p3, p4, p5, p6, p7);
        
      constructor(object t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8)
         <= new(t, p1, p2, p3, p4, p5, p6, p7, p8);
        
      constructor(object t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9)
         <= new(t, p1, p2, p3, p4, p5, p6, p7, p8, p9);
        
      constructor(object t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10)
         <= new(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
        
      constructor(object t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                    Expression p11)
         <= new(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
        
      constructor(object t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                    Expression p11, Expression p12)
         <= new(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);

      bool IsOperation = true;

      int prepare(ExpressionScope scope)
      {
         int len := _arguments.Length;
         int maxStackSize := len + 1;
            
         for(int i := 0, i < len, i+=1) {
            int argStackSize := _arguments[i].prepare(scope);
            if (argStackSize > maxStackSize)
               { maxStackSize := argStackSize; }
         };

         ^ maxStackSize
      }

      saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
      {
         Expression.saveFunctionCallTo(list, scope, _arguments, ExtensionMessageFunction.load(_message, _target));                                                            
         if (index > 0) {
            list.append(new SavingFunction(index));
         }
      }
   }

   // --- DynamicSymbol ---
   public abstract DynamicSymbol
   {
      abstract compile(module, ExpressionScope scope);
   }

   // --- DynamicSingleton ---
   public sealed class DynamicSingleton : DynamicSymbol
   {
      Expression[] methods;
    
      constructor()
      {
         this methods := new Expression[](0);
      }
            
      constructor(Expression[] methods)
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
}

