import system;
import system'collections;

namespace expressions
{
   public interface ExpressionScope
   {
      get abstract int Reserved();
   }

   public class RootExpressionScope : ExpressionScope
   {
      Reference<int>  reserved := 0;
      ExpressionScope parent;

      int Reserved = *reserved;

      constructor()
      {
      }
        
      constructor(ExpressionScope parent)
      {
         this parent := parent
      } 
   }

   public interface Expression
   {
      static Expression Symbol(string s)
         = new SymbolExpression(s);

      static Expression MessageCall(Message message, Expression t)
         = MessageCallExpression.new(message,t);      

      abstract saveTo(List<TapeFunction> list, ExpressionScope scope, int index);

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
}