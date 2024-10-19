import system;
import system'collections;
//import system'routines;
import system'text;

namespace expressions
{
   const int mode_lcl   = 1;
   const int mode_class = 4;

   symbol used : preloaded = new object[]{Expression, DynamicSingleton,DynamicExpressionSymbol,SymbolCollection,LazySymbolExpression};

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
         int varMode := var.Mode;

         if (var.Name == Name && (varMode == Mode || Mode == 0))
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
      constructor(string name)
      {
         Name := name;
         Mode := 0
      }
   }

   public interface ExpressionScope
   {
      get abstract int Reserved();

      get abstract int ScopeLevel();

      abstract allocVariable(ScopeIdentifier variable);

      abstract int allocTempVariable();

      abstract reserveStack(int level);

      abstract TapeFunction resolve(ScopeIdentifier ident, bool assignMode);

      abstract TapeFunction resolveExtension(Message message);
   }

   public singleton IdleExpressionScope : ExpressionScope
   {
      int Reserved = 0;
        
      int allocTempVariable()
         = 0;
            
      int ScopeLevel
         = 0;

      reserveStack(int level) {}
            
      allocVariable(ScopeIdentifier variable)
      {
         NotSupportedException.raise();
      }

      TapeFunction resolveExtension(Message message)
         = nil;    

      TapeFunction resolve(ScopeIdentifier ident, bool assignMode)
         = nil; 
   }

   public sealed class RootExpressionScope : interface<ExpressionScope>
   {
      Reference<int>  stackSize := 0;
      Reference<int>  declared := 0;
      Reference<int>  reserved := 0;
      ExpressionScope parent;

      constructor()
      {
      }
        
      constructor(ExpressionScope parent)
      {
         this parent := parent
      } 

      int StackSize = *stackSize;

      int ScopeLevel = *declared;

      int Declared = *declared;

      int Reserved = *reserved;

      reserveStack(int level)
      {
         if (*stackSize < level) {
            stackSize.Value := level;
         }
      }

      int allocTempVariable()
      {
         reserved.append(1);
            
         ^ *reserved;
      }
        
      TapeFunction resolve(ScopeIdentifier variable, bool assignMode) 
      {
         ^ parent?.resolve(variable, assignMode)
      }
    
      allocVariable(ScopeIdentifier variable)
      {
         declared.append(1);
      }

      TapeFunction resolveExtension(Message message)
         => parent;    
   }

   public interface Expression
   {
      static Expression Symbol(string s)
         = new SymbolExpression(s);

      static Expression MessageCall(Message message, Expression t, params Expression[] arguments)
         = MessageCallExpression.load(message,t,params arguments);

      static Expression ExtensionOrMessageCall(Message message, Expression t, params Expression[] arguments)
         = ExtensionOrMessageCallExpression.load(message,t,params arguments);

      static Expression Constant(object o)
         = new ConstantExpression(o);

      static Expression IntConstant(string value)
         = new IntConstantExpression(value);

      static Expression CharConstant(string value)
         = new CharConstantExpression(value);

      static Expression GetProperty(MessageName name, Expression t)
         = GetPropertyExpression.new(name, t);

      static Expression SetProperty(MessageName name, Expression t, Expression v)
         = SetPropertyExpression.new(name, t, v);

      static Expression Closure(Expression body, params ScopeIdentifier[] variables)
         = ClosureExpression.load(body, params variables);        

      static Expression CodeBlock(params Expression[] statements)
         = CodeblockExpression.load(params statements);

      static Expression SetDynamicProperty(Expression m, Expression t, Expression v)
         = SetDynamicPropertyExpression.new(m, t, v);

      static Expression GetDynamicProperty(Expression m, Expression t)
         = GetDynamicPropertyExpression.new(m, t);

      static Expression Nested(DynamicSingleton expr)
         = new NestedExpression(expr);          

      static Expression Return(Expression expr)
         = new ReturnExpression(expr);

      static Expression If(Expression cond, Expression body, Expression elseBody)
         = new IfExpression(cond, body, elseBody);

      static Expression Loop(Expression cond, Expression body)
         = new LoopExpression(cond, body);

      static Expression VariableByIndex(int index)
         = new VariableByIndexExpression(index);

      static Expression Variable(string variable)
         = new VariableExpression(variable);

      static Expression ClassReference(string s)
         = new ConstantExpression(new ClassReference(s));

      static Expression ClassIdentifier(string s)
         = new ClassIdentifierExpression(s);

      static Expression New(Expression t)
         = new NewExpression(t);
           
      static Expression New(Expression t, Expression p0)
         = NewExpression.load(t, p0);

      static Expression FunctionCall(params Expression[] arguments)
         = FunctionCallExpression.load(params arguments);

      static Expression Assigning(string variable, Expression expr)
         = new AssigningExpression(variable, expr);

      static Expression DeclareAndAssigning(string variable, Expression expr)
         = new DeclaringAndAssigningExpression(variable, expr);

      static Expression Method(string name, Expression body, params ScopeIdentifier[] variables)
         = MethodExpression.new(name, body, params variables);

      static Expression ActionMethod(Expression body)
         = new ActionMethodExpression(body);        

      object Key = nil;

      get abstract bool IsOperation();

      abstract declare(ExpressionScope parentScope);

      abstract saveTo(List<TapeFunction> list, ExpressionScope scope, int index);

      internal static saveMessageCallTo(List<TapeFunction> list, ExpressionScope scope, Expression target, 
                                            Expression[] arguments, TapeFunction operation)        
      {
         int len := arguments.Length;
         Queue<Expression> tempVariables := nil;

         // first pass - preallocating sub expr 
         if (target.IsOperation && len > 0) {
            int tempVar := scope.allocTempVariable() + scope.ScopeLevel;

            target.saveTo(list, scope, 0);
            list.append(new AssigningFunction(tempVar));
                
            tempVariables!.push(Expression.VariableByIndex(tempVar));                
         };

         for (int i := 0; i < len; i += 1) {
            if (arguments[i].IsOperation) {
               int tempVar := scope.allocTempVariable() + scope.ScopeLevel;

               arguments[i].saveTo(list, scope, 0);
               list.append(new AssigningFunction(tempVar));
                
               tempVariables!.push(Expression.VariableByIndex(tempVar));
            }
         };
            
         // second pass - the operation
         Expression firstArg := target;
         if (target.IsOperation && len > 0) {
            firstArg := tempVariables.pop() 
         };

         for (int i := 0; i < len; i += 1) {
            if (arguments[i].IsOperation) {
               tempVariables.pop().saveTo(list, scope, i + 1) 
            }
            else {
               arguments[i].saveTo(list, scope, i + 1) 
            }
         };

         firstArg.saveTo(list, scope, 0);
                                    
         list.append(operation);                  
      }

      internal static saveMessageCallTo(List<TapeFunction> list, ExpressionScope scope,
                                            Expression[] arguments, TapeFunction operation)        
      {
         int len := arguments.Length;
         Queue<Expression> tempVariables := nil;

         // first pass - preallocating sub expr 
         for (int i := 0; i < len; i += 1) {
            if (arguments[i].IsOperation) {
               int tempVar := scope.allocTempVariable() + scope.ScopeLevel;

               arguments[i].saveTo(list, scope, 0);
               list.append(new AssigningFunction(tempVar));
                
               tempVariables!.push(Expression.VariableByIndex(tempVar));
            }
         };
            
         // second pass - the operation
         for (int i := len - 1; i >= 0; i -= 1) {
            if (arguments[i].IsOperation) {
               tempVariables.pop().saveTo(list, scope, i) 
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

      DynamicTape compile(ExpressionScope parentScope)
      {
         List<TapeFunction> list := new List<TapeFunction>(8);
   
         // reserve the alloc place
         list.append(nilValue);

         auto scope := new RootExpressionScope(parentScope);

         self.declare(scope);         
         self.saveTo(list, scope, 0);
         
         int allocated := scope.StackSize;
         int declared := scope.Declared;
         int reserved := scope.Reserved;

         allocated += declared;
         allocated += reserved;

         list[0] := new AllocFunction(allocated);

         ^ DynamicTape.new(list)
      }
   }

   public class ClassIdentifierExpression : Expression
   {
      ScopeIdentifier name;
                
      constructor(string name)
      {
         this name := new ScopeIdentifier(name, mode_class);
      }
        
      bool IsOperation = false;
        
      declare(ExpressionScope scope)
      {
         scope.reserveStack(1);
      }
        
      saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
      {
         TapeFunction  classIdentifier := scope.resolve(name, false);

         list.append(classIdentifier);
         if (index > 0) {
            list.append(new SavingFunction(index));
         }
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

      eval()
         = symbol;

      declare(ExpressionScope parentScope)
      {
      }

      saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
      {
         list.append(ConstantFunction.load(symbol));
         if (index > 0) {
            list.append(new SavingFunction(index));
         };

         scope.reserveStack(1);
      }                
   }

   public class LazySymbolExpression : Expression
   {
      string symbolReference;
        
      constructor(string s)
      {
         symbolReference := s
      }

      bool IsOperation = false;

      eval()      
         = new Symbol(symbolReference);

      declare(ExpressionScope parentScope)
      {
      }

      saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
      {
         list.append(ConstantFunction.load(new Symbol(symbolReference)));
         if (index > 0) {
            list.append(new SavingFunction(index));
         };

         scope.reserveStack(1);
      }                
   }

   public class FunctionCallExpression : Expression
   {
      Message      _message;
      Expression   _target;
      Expression[] _arguments;

      constructor new(Expression t)
      {
         _message := new Message("function:#invoke[0]");
         _target := t;
         _arguments := new Expression[](0);
      }

      constructor load(Expression t, params Expression[] arguments)
      {
         int len := arguments.Length;

         _message := new Message("function:#invoke[" + len.toPrintable() + "]");
         _target := t;
         _arguments := arguments;
      }

      constructor(Expression t)
         <= new(t);
        
      constructor(Expression t, Expression p1)
         <= load(t, p1);
        
      constructor(Expression t, Expression p1, Expression p2)
         <= load(t, p1, p2);
        
      constructor(Expression t, Expression p1, Expression p2, Expression p3)
         <= load(t, p1, p2, p3);
        
      constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4)
         <= load(t, p1, p2, p3, p4);
        
      constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5)
         <= load(t, p1, p2, p3, p4, p5);
        
      constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6)
         <= load(t, p1, p2, p3, p4, p5, p6);
        
      constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7)
         <= load(t, p1, p2, p3, p4, p5, p6, p7);
        
      constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8)
         <= load(t, p1, p2, p3, p4, p5, p6, p7, p8);
        
      constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9)
         <= load(t, p1, p2, p3, p4, p5, p6, p7, p8, p9);
        
      constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10)
         <= load(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
        
      constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                    Expression p11)
         <= load(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
        
      constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                    Expression p11, Expression p12)
         <= load(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);

      bool IsOperation = true;

      declare(ExpressionScope scope)
      {
         _target.declare(scope);
         int len := _arguments.Length;
         for(int i := 0; i < len; i+=1) {
            _arguments[i].declare(scope);
         };

         scope.reserveStack(len);
      }

      saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
      {
         int tempVar := scope.allocTempVariable() + scope.ScopeLevel;
         _target.saveTo(list, scope, 0);
         list.append(new AssigningFunction(tempVar));

         Expression.saveMessageCallTo(list, scope, _arguments, FuncFunction.load(_message, tempVar));                                                            
         if (index > 0) {
            list.append(new SavingFunction(index));
         }
      }
   }
                	
   public class NewExpression : FunctionCallExpression
   {
      constructor new(Expression t)
      {
         _message := new Message("function:#constructor[0]");
         _target := t;
         _arguments := new Expression[](0);
      }

      constructor load(Expression t, params Expression[] arguments)
      {
         int len := arguments.Length;

         _message := new Message("function:#constructor[" + len.toPrintable() + "]");
         _target := t;
         _arguments := arguments;
      }

      constructor(Expression t)
         <= new(t);
        
      constructor(Expression t, Expression p1)
         <= load(t, p1);
        
      constructor(Expression t, Expression p1, Expression p2)
         <= load(t, p1, p2);
        
      constructor(Expression t, Expression p1, Expression p2, Expression p3)
         <= load(t, p1, p2, p3);
        
      constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4)
         <= load(t, p1, p2, p3, p4);
        
      constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5)
         <= load(t, p1, p2, p3, p4, p5);
        
      constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6)
         <= load(t, p1, p2, p3, p4, p5, p6);
        
      constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7)
         <= load(t, p1, p2, p3, p4, p5, p6, p7);
        
      constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8)
         <= load(t, p1, p2, p3, p4, p5, p6, p7, p8);
        
      constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9)
         <= load(t, p1, p2, p3, p4, p5, p6, p7, p8, p9);
        
      constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10)
         <= load(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
        
      constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                    Expression p11)
         <= load(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
        
      constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                    Expression p11, Expression p12)
         <= load(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
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

      constructor load(Message message, Expression t, params Expression[] arguments)
      {
         _message := message;
         _target := t;
         _arguments := arguments;
      }

      constructor(Expression t, string message)
         <= new(new Message(message + "[1]"), t);
        
      constructor(Expression t, string message, Expression p1)
         <= load(new Message(message + "[2]"), t, p1);
        
      constructor(Expression t, string message, Expression p1, Expression p2)
         <= load(new Message(message + "[3]"), t, p1, p2);
        
      constructor(Expression t, string message, Expression p1, Expression p2, Expression p3)
         <= load(new Message(message + "[4]"), t, p1, p2, p3);
        
      constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4)
         <= load(new Message(message + "[5]"), t, p1, p2, p3, p4);
        
      constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5)
         <= load(new Message(message + "[6]"), t, p1, p2, p3, p4, p5);
        
      constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6)
         <= load(new Message(message + "[7]"), t, p1, p2, p3, p4, p5, p6);
        
      constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7)
         <= load(new Message(message + "[8]"), t, p1, p2, p3, p4, p5, p6, p7);
        
      constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8)
         <= load(new Message(message + "[9]"), t, p1, p2, p3, p4, p5, p6, p7, p8);
        
      constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9)
         <= load(new Message(message + "[10]"), t, p1, p2, p3, p4, p5, p6, p7, p8, p9);
        
      constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10)
         <= load(new Message(message + "[11]"), t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
        
      constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                    Expression p11)
         <= load(new Message(message + "[12]"), t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
        
      constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                    Expression p11, Expression p12)
         <= load(new Message(message + "[13]"), t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);

      bool IsOperation = true;

      declare(ExpressionScope scope)
      {
         _target.declare(scope);
         int len := _arguments.Length;
         for(int i := 0; i < len; i+=1) {
            _arguments[i].declare(scope);
         };

         scope.reserveStack(len + 1);
      }

      saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
      {
         Expression.saveMessageCallTo(list, scope, _target, _arguments, MessageFunction.load(_message));                                                            
         if (index > 0) {
            list.append(new SavingFunction(index));
         }
      }
   }

   public class ExtensionOrMessageCallExpression : Expression
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

      constructor load(Message message, Expression t, params Expression[] arguments)
      {
         _message := message;
         _target := t;
         _arguments := arguments;
      }

      constructor(Expression t, string message)
         <= new(new Message(message + "[1]"), t);
        
      constructor(Expression t, string message, Expression p1)
         <= load(new Message(message + "[2]"), t, p1);
        
      constructor(Expression t, string message, Expression p1, Expression p2)
         <= load(new Message(message + "[3]"), t, p1, p2);
        
      constructor(Expression t, string message, Expression p1, Expression p2, Expression p3)
         <= load(new Message(message + "[4]"), t, p1, p2, p3);
        
      constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4)
         <= load(new Message(message + "[5]"), t, p1, p2, p3, p4);
        
      constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5)
         <= load(new Message(message + "[6]"), t, p1, p2, p3, p4, p5);
        
      constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6)
         <= load(new Message(message + "[7]"), t, p1, p2, p3, p4, p5, p6);
        
      constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7)
         <= load(new Message(message + "[8]"), t, p1, p2, p3, p4, p5, p6, p7);
        
      constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8)
         <= load(new Message(message + "[9]"), t, p1, p2, p3, p4, p5, p6, p7, p8);
        
      constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9)
         <= load(new Message(message + "[10]"), t, p1, p2, p3, p4, p5, p6, p7, p8, p9);
        
      constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10)
         <= load(new Message(message + "[11]"), t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
        
      constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                    Expression p11)
         <= load(new Message(message + "[12]"), t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
        
      constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                    Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                    Expression p11, Expression p12)
         <= load(new Message(message + "[13]"), t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
        
      bool IsOperation = true;
        
      declare(ExpressionScope scope)
      {
         _target.declare(scope);
         int len := _arguments.Length;
         for(int i := 0; i < len; i+=1) {
            _arguments[i].declare(scope);
         };

         scope.reserveStack(len + 1);
      }

      saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
      {
         TapeFunction messageExpr := scope.resolveExtension(_message);
         if (messageExpr == nil) {
            messageExpr := MessageFunction.load(_message)
         };

         Expression.saveMessageCallTo(list, scope, _target, _arguments, messageExpr);
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

      declare(ExpressionScope scope)
      {
         _target.declare(scope);
         scope.reserveStack(1);
      }

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
        
      declare(ExpressionScope scope)
      {
         _target.declare(scope);
         _value.declare(scope);      

         scope.reserveStack(2);
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
        
      declare(ExpressionScope parentScope)
      {
      }

      saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
      {
         list.append(ConstantFunction.load(object));
         if (index > 0)
            { list.append(new SavingFunction(index)) };

         scope.reserveStack(1);
      }
   }

   public class IntConstantExpression : ConstantExpression
   {
      constructor(o)
      {
         this object := IntConvertor.convert(o)
      }
   }

   public class CharConstantExpression : ConstantExpression
   {
      constructor(string o)
      {
         int n := IntConvertor.convert(o.Substring(1));

         this object := CharConvertor.convert(n)
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

         for (int i := 0; i < len; i += 1)
         {
            parameters[i + 2] := ScopeIdentifier.Variable(variables[i])
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

         for (int i := 0; i < len; i += 1)
         {
            parameters[i + 2] := params[i]
         };
            
         this body := body;
      }         
                        
      constructor(string name, MethodParameterList parameters, Expression body)
      {
         // excluding $tape variable
         int len := parameters.Length - 1;
            
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
        
      object Key = message;

      bool IsOperation = false;

//      DynamicTape compile(ExpressionScope scope)
//         = super.compile(new RootExpressionScope(scope));

      declare(ExpressionScope scope)
      {
      }

      saveTo(List<TapeFunction> list, ExpressionScope parentScope, int index)
      {
         int level := parameters.Length;

         auto scope := ExpressionScope
         {
            int Reserved
               = parentScope.Reserved;

            int ScopeLevel
               = parentScope.ScopeLevel + level;

            reserveStack(int level)
               => parentScope;

            allocVariable(ScopeIdentifier variable)
               => parentScope;

            int allocTempVariable()
               = parentScope.allocTempVariable();

            TapeFunction resolve(ScopeIdentifier variable, bool assignMode)
            {
               int len := parameters.Length;
               for(int i := 0; i < len; i+=1) {
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
                    
               ^ parentScope.resolve(variable, assignMode)
            }

            TapeFunction resolveExtension(Message message)
               => parentScope;
         };

         body.saveTo(list, scope, index);
      }

      DynamicTape compile(ExpressionScope parentScope)
      {
         List<TapeFunction> list := new List<TapeFunction>(8);
   
         // reserve the alloc place
         list.append(nilValue);

         auto scope := new RootExpressionScope(parentScope);

         self.saveTo(list, scope, 0);
         
         int allocated := scope.StackSize;
         int declared := scope.Declared;
         int reserved := scope.Reserved;

         allocated += declared;
         allocated += reserved;

         list[0] := new AllocFunction(allocated);

         ^ DynamicTape.new(list)         
      }
   }

   public class ActionMethodExpression : MethodExpression
   {
      constructor(Expression body)
      {
         this parameters := new ScopeIdentifier[]
         { 
            ScopeIdentifier.Variable("$tape")
         };

         this message := new Message("function:#invoke[0]");

         this body := body;
      }

      constructor(ScopeIdentifier param1, Expression body)
      {
         this parameters := new ScopeIdentifier[]
         { 
            ScopeIdentifier.Variable("$tape"), 
            param1
         };

         this message := new Message("function:#invoke[1]");

         this body := body;
      }

      constructor(ScopeIdentifier param1, ScopeIdentifier param2, Expression body)
      {
         this parameters := new ScopeIdentifier[]
         { 
            ScopeIdentifier.Variable("$tape"), 
            param1,
            param2
         };

         this message := new Message("function:#invoke[2]");

         this body := body;
      }

      constructor(ScopeIdentifier param1, ScopeIdentifier param2, ScopeIdentifier param3, Expression body)
      {
         this parameters := new ScopeIdentifier[]
         { 
            ScopeIdentifier.Variable("$tape"), 
            param1,
            param2,
            param3
         };

         this message := new Message("function:#invoke[3]");

         this body := body;
      }

      constructor(ScopeIdentifier param1, ScopeIdentifier param2, ScopeIdentifier param3, 
         ScopeIdentifier param4, Expression body)
      {
         this parameters := new ScopeIdentifier[]
         { 
            ScopeIdentifier.Variable("$tape"), 
            param1,
            param2,
            param3,
            param4
         };

         this message := new Message("function:#invoke[4]");

         this body := body;
      }

      constructor(ScopeIdentifier param1, ScopeIdentifier param2, ScopeIdentifier param3, 
         ScopeIdentifier param4, ScopeIdentifier param5, Expression body)
      {
         this parameters := new ScopeIdentifier[]
         { 
            ScopeIdentifier.Variable("$tape"), 
            param1,
            param2,
            param3,
            param4,
            param5
         };

         this message := new Message("function:#invoke[5]");

         this body := body;
      }
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
        
      constructor load(Expression body, params ScopeIdentifier[] params)
      {
         // revese the order
         int len := params.Length;
         this parameters := new ScopeIdentifier[](len + 1);
         parameters[0] := new ScopeIdentifier("$tape", 0);
         for (int i := 0; i < len; i += 1)
         {
            parameters[i + 1] := params[i]
         };
            
         this body := body;
      } 

      bool IsOperation = true;
     
      declare(ExpressionScope scope)
      {
      }

      saveTo(List<TapeFunction> list, ExpressionScope parentScope, int index)
      {
         int level := parameters.Length;

         auto scope := ExpressionScope
         {
            int Reserved
               = parentScope.Reserved;

            int ScopeLevel
               = parentScope.ScopeLevel + level;

            reserveStack(int level)
               => parentScope;

            allocVariable(ScopeIdentifier variable)
               => parentScope;

            int allocTempVariable()
               = parentScope.allocTempVariable();

            TapeFunction resolve(ScopeIdentifier variable, bool assignMode)
            {
               int len := parameters.Length;
               for(int i := 0; i < len; i+=1) {
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
                    
               ^ parentScope.resolve(variable, assignMode)
            }

            TapeFunction resolveExtension(Message message)
               => parentScope;
         };

         body.saveTo(list, scope, index);
      }
   }      

   public class CodeblockExpression : Expression
   {
      Expression[]          statements;
        
      constructor(params Expression[] statements)
      {
         this statements := statements
      }

      constructor load(params Expression[] statements)
      {
         this statements := statements
      }

      constructor new(Expression[] statements)
      {
         this statements := statements
      }

      bool IsOperation = true;
        
      declare(ExpressionScope parentScope)
      {
      }

      saveTo(List<TapeFunction> list, ExpressionScope parentScope, int index)
      {
         auto variables := new List<Tuple<ScopeIdentifier, int>>();

         Reference<int> reserved := 0;
         auto scope := ExpressionScope
         {
            int Reserved = *reserved;

            int ScopeLevel = parentScope.ScopeLevel;

            TapeFunction resolve(ScopeIdentifier variable, bool assignMode)
            {
               int len := variables.Length;
               for(int i := 0; i < len; i+=1) {
                  if (variable == variables[i].Item1)
                  {
                     int index := variables[i].Item2;

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
                    
               ^ parentScope.resolve(variable, assignMode)               
            }

            reserveStack(int level)
               => parentScope;

            TapeFunction resolveExtension(Message message)
               => parentScope;

            int allocTempVariable()
            {
               reserved.append(1);
               while (parentScope.Reserved < *reserved)
                  parentScope.allocTempVariable();

               ^ *reserved;
            }

            allocVariable(ScopeIdentifier variable)
            {
               parentScope.allocVariable(variable);
                    
               int index := parentScope.ScopeLevel;               

               variables.append(new Tuple<ScopeIdentifier, int>(variable, index))
            }
         };

         int len := statements.Length;
         for(int i := 0; i < len; i+=1) {
            statements[i].declare(scope);
         };

         for(int i := 0; i < len; i+=1) {
            reserved.Value := 0;

            statements[i].saveTo(list, scope, 0);
         };         

         if (index > 0)
            { list.append(new SavingFunction(index)) };
      }
   }

   public class SetDynamicPropertyExpression : Expression
   {
      Expression _property;
      Expression _target;
      Expression _value;
        
      constructor(Expression t, Expression p, Expression v)
            <= new(p, t, v);
            
      constructor new(Expression p, Expression t, Expression v)
      {
         _property := Expression.MessageCall( 
                        new Message("setPropertyMessage[1]"),
                        Expression.New(
                           Expression.Constant(system'MessageName),
                           p
                        )
                     );
            
         _target := t;
         _value := v
      }
        
      bool IsOperation = true;
        
      declare(ExpressionScope scope)
      {
         _property.declare(scope);
         _target.declare(scope);
         _value.declare(scope);      

         scope.reserveStack(2);
      }

      saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
      {
         int tempVar := scope.allocTempVariable() + scope.ScopeLevel;
         _property.saveTo(list, scope, 0);
         list.append(new AssigningFunction(tempVar));                       

         _value.saveTo(list, scope, 1);
         _target.saveTo(list, scope, 0);

         list.append(new DynamicMessageFunction(tempVar));

         if (index > 0)
            { list.append(new SavingFunction(index)) };
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
                              Expression.Constant(system'MessageName),
                              p
                           )
                        );
            
         _target := t
      }
        
      bool IsOperation = true;
        
      declare(ExpressionScope scope)
      {
         _property.declare(scope);
         _target.declare(scope);

         scope.reserveStack(1);
      }

      saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
      {
         int tempVar := scope.allocTempVariable() + scope.ScopeLevel;
         _property.saveTo(list, scope, 0);
         list.append(new AssigningFunction(tempVar));                       

         _target.saveTo(list, scope, 0);

         list.append(new DynamicMessageFunction(tempVar));

         if (index > 0)
            { list.append(new SavingFunction(index)) };
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

      declare(ExpressionScope scope)
      {
         expr.declare(scope);
                
         scope.reserveStack(1);
      }

      saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
      {
         expr.saveTo(list, scope, 0);
         list.append(JumpToEnd)
      }
   }

   public class IfExpression : Expression
   {
      Expression condition;
      Expression body;
      Expression elseBody;
        
      constructor(Expression condition, Expression body)
      {
         this condition := condition;
         this body := body;
      }

      constructor(Expression condition, Expression body, Expression elseBody)
      {
         this condition := condition;
         this body := body;
         this elseBody := elseBody;
      }

      declare(ExpressionScope scope)
      {
         condition.declare(scope);
         body.declare(scope);            
                
         elseBody?.declare(scope);
                
         scope.reserveStack(2);
      }
                
      bool IsOperation = true;

      saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
      {
         condition.saveTo(list, scope, 0);
         int bookmark := list.Length;
         list.append(cast TapeFunction(nilValue));
         body.saveTo(list, scope, 0);
         if (elseBody != nil) {
            int bookmark2 := list.Length;
            list.append(cast TapeFunction(nilValue));
                
            //int offs := list.Length - bookmark - 1;
            int offs := list.Length - bookmark;
            offs := offs - 1;
            list[bookmark] := new JumpIfNotFunction(offs);
                
            elseBody.saveTo(list, scope, 0);
                
            offs := list.Length - bookmark2;
            offs := offs - 1;                
            list[bookmark2] := new GoToFunction(offs);
         }
         else {
            //int offs := list.Length - bookmark - 1;
            int offs := list.Length - bookmark;
            offs := offs - 1;
                
            list[bookmark] := new JumpIfNotFunction(offs);
            list.append(ConstantFunction.load(nilValue)); // !! should be redesigned
         }
      }
   }

   public class LoopExpression : Expression
   {
      Expression condition;
      Expression body;
        
      constructor(Expression condition, Expression body)
      {
         this condition := condition;
         this body := body
      }

      declare(ExpressionScope scope)
      {
         condition.declare(scope);
         body.declare(scope);

         scope.reserveStack(2);
      }
                
      bool IsOperation = true;
                
      saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
      {
         int top := list.Length;
            
         condition.saveTo(list, scope, 0);
         int bookmark := list.Length;
         list.append(cast TapeFunction(nilValue));
         body.saveTo(list, scope, 0);
            
         int offs := top - list.Length - 1;
            
         list.append(new GoToFunction(offs));
            
         //offs := list.Length - bookmark - 1;
         offs := list.Length - bookmark;
         offs := offs - 1;
            
         list[bookmark] := new JumpIfNotFunction(offs);
            
         list.append(ConstantFunction.load(nilValue)); // !! should be redesigned
      }
   }

   public struct VariableByIndexExpression : Expression
   {
      int varIndex;
        
      constructor(int index)
      {
         this varIndex := index
      }
        
      bool IsOperation = false;
        
      declare(ExpressionScope scope) 
      {
         scope.reserveStack(1);
      }   
            
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
        
      constructor(ScopeIdentifier ident)
      {
         this ident := ident;
      }

      bool IsOperation = false;

      declare(ExpressionScope parentScope)
      {
      }

      saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
      {
         TapeFunction local := scope.resolve(ident, false);

         list.append(local);
         if (index > 0) {
            list.append(new SavingFunction(index));
         };

         scope.reserveStack(1);
      }
   }

   public class VariableExpression : IdentifierExpression
   {
      constructor(string name)
      {
         this ident := ScopeIdentifier.Variable(name)
      }
   }

   public class AssigningExpression : Expression
   {
      ScopeIdentifier variable;
      Expression      expression;  
        
      constructor(ScopeIdentifier variable, Expression expression)
      {
         this variable := variable;
         this expression := expression
      }

      constructor(string variable, Expression expression)
      {
         this variable := ScopeIdentifier.Variable(variable);
         this expression := expression
      }

      bool IsOperation = false;

      declare(ExpressionScope scope)
      {
         expression.declare(scope);

         scope.reserveStack(2);
      }
                
      saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
      {
         auto assigning := scope.resolve(variable, true);

         expression.saveTo(list, scope, 0);
         list.append(assigning);
         if (index > 0) {
            list.append(new SavingFunction(index));
         }
      }
   }

   public class DeclaringAndAssigningExpression : AssigningExpression
   {
      constructor(string variable, Expression expression)
      {
         this variable := ScopeIdentifier.Variable(variable);
         this expression := expression
      }

      declare(ExpressionScope scope)
      {
         scope.allocVariable(variable);
            
         super.declare(scope);
      }
   }

   // --- DynamicSymbol ---
   public abstract DynamicSymbol
   {
      abstract compile(set, ExpressionScope scope);
   }

   // --- DynamicExpressionSymbol ---
   public sealed class DynamicExpressionSymbol : DynamicSymbol
   {
      Expression expr;
        
      constructor(Expression expr)
      {
         this expr := expr
      }
        
      compile()
         <= compile(nil, IdleExpressionScope);

      compile(set, ExpressionScope parentScope)
      {
         ^ expr.compile(parentScope)
      }        
   }

   // --- DynamicSingleton ---
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
                
      constructor load(params Expression[] methods)
      {
         this methods := methods;                
      }
                    
      compile()
         <= compile(nil, IdleExpressionScope);

      compile(set, ExpressionScope scope)
      {
         auto list := new List<Tuple<Message,DynamicTape>>();
         int len := methods.Length;
         for (int i := 0; i < len; i += 1) {
            Message message := methods[i].Key;
            auto tape := methods[i].compile(scope);

            list.append(new Tuple<Message,DynamicTape>(message, tape));
         };
            
         ^ DynamicClass.create(list);
      }
   }

   public class NestedExpression : Expression
   {
      DynamicSingleton singleton;

      constructor(DynamicSingleton singleton)
      {
         this singleton := singleton;
      }

      bool IsOperation = true;

      declare(ExpressionScope parentScope)
      {
         parentScope.reserveStack(1);
      }

      saveTo(List<TapeFunction> list, ExpressionScope scope, int index)
      {
         var tape := singleton.compile(nil, scope);

         list.append(ConstantFunction.load(tape));
         if (index > 0) {
            list.append(new SavingFunction(index));
         }
      }      
   }

   public class SymbolInfo
   {
      string Name : prop;
      DynamicSymbol Symbol : prop;
        
      constructor(string name, DynamicSymbol symbol)
      {
         Name := name;
         Symbol := symbol
      } 
      constructor(DynamicSymbol symbol)
      {
         Symbol := symbol
      } 
   }
    
   public sealed class ImportInfo
   {
      string[]     imports;

      constructor(params string[] import_strings)
      {
         imports := import_strings;
      }

      string[] declare(string[] declared)
      {
         ^ declared + imports
      }
   }

   public sealed class SymbolCollection
         : info("a dynamic namespace, containing list of symbols")
   {
      SymbolInfo[] symbols;
      string[]     imports := new string[] { "system" };

      constructor(SymbolInfo symbol1)
         <= load(symbol1);
                
      constructor(SymbolInfo symbol1, SymbolInfo symbol2)
         <= load(symbol1, symbol2);
                
      constructor(SymbolInfo symbol1, SymbolInfo symbol2, SymbolInfo symbol3)
         <= load(symbol1, symbol2, symbol3);
                
      constructor(SymbolInfo symbol1, SymbolInfo symbol2, SymbolInfo symbol3, SymbolInfo symbol4)
         <= load(symbol1, symbol2, symbol3, symbol4);

      constructor(params SymbolInfo[] symbols)
         <= load(params symbols);

      constructor(ImportInfo imports, SymbolInfo symbol1)
         <= load(imports, symbol1);
                                                
      constructor load(params SymbolInfo[] symbols)
      {
         this symbols := symbols;        
      }
                                                
      constructor load(ImportInfo importInfo, params SymbolInfo[] symbols)
      {
         this symbols := symbols;
         
         imports := importInfo.declare(imports);
      }
 
      compile()
         <= compile(new DynamicStruct());
 
      compile(set)
         <= compile(set, new ExpressionScope
      {
         int Reserved = 0;
           
         int allocTempVariable()
            = 0;
               
         int ScopeLevel
            = 0;
   
         reserveStack(int level) {}
               
         allocVariable(ScopeIdentifier variable)
         {
            auto subarr := new string[] { variable.Name };
                    
            imports := imports + subarr
         }
                
         TapeFunction resolveExtension(Message message)
         {
            var op := ExtensionDispatcher.load(message, imports);
            if (op != nil)
            {
               ^ ExtensionMessageFunction.load(message, op)
            }
            else {
               auto varMessage := message.MessageName.VariadicMessage;
                    
               var op2 := VariadicExtensionDispatcher.load(varMessage, imports);
               if (op2 != nil) {
                  ^ ExtensionMessageFunction.load(message, op2)
               }
            };
                    
            ^ nil
         }

         TapeFunction resolve(ScopeIdentifier variable, bool assignModemode) 
         {
            string refName := variable.Name;
            int mode := variable.Mode;
                    
            if (mode == 0 || mode == mode_class)
            {
               // look for external references
               if(imports != nil) {
                  int len := imports.Length;
                  for(int i := 0; i < len; i++) {
                     Symbol symbol := Symbol.tryLoad(imports[i], refName);
                     if (symbol != Symbol.Default)
                     {
                        ^ ConstantFunction.load(symbol)
                     }
                  }
               };

               var prop := new MessageName(refName).getPropertyMessage();
                        
               var symbol := prop(set);
               if (symbol != nil)
               {
                  ^ ConstantFunction.load(symbol);
               }
            };
                    
            ^ nil
         }                
      });

      compile(set, ExpressionScope scope)
      {
         int len := symbols.Length;
         for (int i := 0; i < len; i+=1) {
            auto info := symbols[i];
                
            var symbolName := info.Name;
            if (symbolName == nil) {
               info.Symbol.compile(set, scope);
            }
            else {
               MessageName name := new MessageName(symbolName);
               Message prop := name.setPropertyMessage(); 
        
               var tape := info.Symbol.compile(set, scope);
         
               prop(set, tape);
            }                
         };
            
         ^ set
      }
        
      eval()
         : info("Exectutes the program entry point")
      {
         var set := new DynamicStruct();
            
         var tape := self.compile(set);
            
         var program := tape.program;
         ^ program();
      }

      safeEval()
      {
         try
         {
            self.eval();
         }
         catch::
         {
            function(AbortException e)
            {
            }
          
            function(Exception err)
            {
               console.writeLine(err);
            }
         }
      }
   }
}

