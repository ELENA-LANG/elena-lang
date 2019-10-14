import system'collections;
import system'routines;

preloaded used = new object[]::(Expression,DynamicSingleton,SymbolCollection);

public class ScopeVariable
{
    prop string Name;
    
    bool equal(ScopeVariable var)
    {
        ^ var.Name == Name
    }
    
    constructor(string name)
    {
        Name := name
    }
}

public interface ExpressionScope
{
    abstract get int ScopeLevel() {}
    
    abstract Expression resolve(ScopeVariable variable, ref int index) {}
    
    abstract declare(ScopeVariable variable) {}
}

public singleton IdleExpressionScope : ExpressionScope
{
    int ScopeLevel = 0;
    
    Expression resolve(ScopeVariable variable, ref int index) 
    {
        ^ nil
    }
    
    declare(ScopeVariable variable)
    {
        ^ nil
    }
}

public interface Expression
{    
    static Expression CodeBlock(params Expression[] statements)
        = CodeblockExpression.new(params statements);
    
    static Expression CodeBlock(Expression[] statements)
        = CodeblockExpression.new(statements);
    
    static Expression Constant(object o)
        = new ConstantExpression(o);
        
    static Expression NumericConstant(object o)
        = new NumericConstantExpression(o);
        
    static Expression Symbol(string s)
        = new SymbolExpression(s);
        
    static Expression ClassReference(string s)
        = new ConstantExpression(new ClassReference(s));
        
    static Expression New(Expression t)
        = new MessageCallExpression(t, "#constructor");
       
    static Expression New(Expression t, Expression p0)
        = new MessageCallExpression(t, "#constructor", p0);
       
    static Expression MessageCall(mssg message, Expression t, params Expression[] arguments)
        = MessageCallExpression.new(message,t,params arguments);
       
    static Expression MessageCall(mssg message, Expression t)
        = MessageCallExpression.new(message,t);
        
    static Expression DynamicMessageCall(Expression t, params Expression[] arguments)
        = DynamicMessageCallExpression.new(t,params arguments);
       
    static Expression DynamicMessageCall(Expression t)
        = DynamicMessageCallExpression.new(t);
        
    static Expression GetProperty(MessageName name, Expression t)
        = GetPropertyExpression.new(name, t);
        
    static Expression SetProperty(MessageName name, Expression t, Expression v)
        = SetPropertyExpression.new(name, t, v);
       
    static Expression GetDynamicProperty(Expression m, Expression t)
        = GetDynamicPropertyExpression.new(m, t);
        
    static Expression SetDynamicProperty(Expression m, Expression t, Expression v)
        = SetDynamicPropertyExpression.new(m, t, v);
       
    static Declaration(ScopeVariable variable)
        = new DeclarationExpression(variable);
       
    static Expression Assigning(ScopeVariable variable, Expression expr)
        = new AssigningExpression(variable, expr);
          
    static Expression Assigning(string variable, Expression expr)
        = new AssigningExpression(new ScopeVariable(variable), expr);
          
    static Expression DeclareAndAssigning(ScopeVariable variable, Expression expr)
        = new DeclaringAndAssigningExpression(variable, expr);
          
    static Expression Variable(ScopeVariable variable)
        = new VariableExpression(variable);
          
    static Expression Variable(string variable)
        = new VariableExpression(new ScopeVariable(variable));
          
    static Expression Statement(params Expression[] expressions)
        = StatementExpression.new(params expressions);
          
    static Expression If(Expression cond, Expression body)
        = new IfExpression(cond, body);
          
    static Expression Loop(Expression cond, Expression body)
        = new LoopExpression(cond, body);
        
    static Expression Method(string name, Expression body)
        = new MethodExpression(name, body);        
          
    static Expression Method(string name, ScopeVariable param1, Expression body)
        = new MethodExpression(name, param1, body);        
          
    static Expression Method(string name, ScopeVariable param1, ScopeVariable param2, Expression body)
        = new MethodExpression(name, param1, param2, body);        
          
    static Expression Method(string name, ScopeVariable param1, ScopeVariable param2, ScopeVariable param3, Expression body)
        = new MethodExpression(name, param1, param2, param3, body);        
          
    static Expression Closure(Expression body)
        = new ClosureExpression(body);        
          
    static Expression Closure(ScopeVariable param1, Expression body)
        = new ClosureExpression(param1, body);        
          
    static Expression Closure(ScopeVariable param1, ScopeVariable param2, Expression body)
        = new ClosureExpression(param1, param2, body);        
          
    static Expression Closure(ScopeVariable param1, ScopeVariable param2, ScopeVariable param3, Expression body)
        = new ClosureExpression(param1, param2, param3, body);        
          
    static Expression Return(Expression expr)
        = new ReturnExpression(expr);
          
    abstract saveTo(List<DynamicClosure> list, ExpressionScope scope) {}

    saveTo(List<Tuple<Message,ClosureTape>> list)
    {
        NotSupportedException.raise()
    }
                        
    // temporal generic solution
    eval()
    {
        ^ (self.compiled())();
    }
        
    ClosureTape compiled()
    {
        List<DynamicClosure> list := new List<DynamicClosure>(8);
        
        self.saveTo(list, IdleExpressionScope);
        
        ^ ClosureTape.new:list
    }
}

public class StatementExpression : Expression
{
    Expression[] statements;

    constructor(Expression p1)
        <= new(p1);
    
    constructor(Expression p1, Expression p2)
        <= new(p1, p2);
    
    constructor(Expression p1, Expression p2, Expression p3)
        <= new(p1, p2, p3);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4)
        <= new(p1, p2, p3, p4);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5)
        <= new(p1, p2, p3, p4, p5);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6)
        <= new(p1, p2, p3, p4, p5, p6);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7)
        <= new(p1, p2, p3, p4, p5, p6, p7);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                Expression p11)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                Expression p11, Expression p12)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                Expression p11, Expression p12, Expression p13)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                Expression p11, Expression p12, Expression p13, Expression p14)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                Expression p11, Expression p12, Expression p13, Expression p14, Expression p15)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                Expression p11, Expression p12, Expression p13, Expression p14, Expression p15, Expression p16)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                Expression p11, Expression p12, Expression p13, Expression p14, Expression p15, Expression p16,
                Expression p17)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                Expression p11, Expression p12, Expression p13, Expression p14, Expression p15, Expression p16,
                Expression p17, Expression p18)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                Expression p11, Expression p12, Expression p13, Expression p14, Expression p15, Expression p16,
                Expression p17, Expression p18, Expression p19)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                Expression p11, Expression p12, Expression p13, Expression p14, Expression p15, Expression p16,
                Expression p17, Expression p18, Expression p19, Expression p20)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20);
        
    constructor new(params Expression[] statements)
    {
        this statements := statements
    }
    
    saveTo(List<DynamicClosure> list, ExpressionScope scope)
    {
        int len := statements.Length;
        for(int i := 0, i < len, i+=1) {
            statements[i].saveTo(list, scope)
        };
    }
}

public class CodeblockExpression : Expression
{
    Expression[] statements;
    
    constructor new(params Expression[] statements)
    {
        this statements := statements
    }
    
    constructor new(Expression[] statements)
    {
        this statements := statements
    }
    
    constructor(Expression p1)
        <= new(p1);
    
    constructor(Expression p1, Expression p2)
        <= new(p1, p2);
    
    constructor(Expression p1, Expression p2, Expression p3)
        <= new(p1, p2, p3);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4)
        <= new(p1, p2, p3, p4);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5)
        <= new(p1, p2, p3, p4, p5);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6)
        <= new(p1, p2, p3, p4, p5, p6);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7)
        <= new(p1, p2, p3, p4, p5, p6, p7);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                Expression p11)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                Expression p11, Expression p12)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                Expression p11, Expression p12, Expression p13)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                Expression p11, Expression p12, Expression p13, Expression p14)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                Expression p11, Expression p12, Expression p13, Expression p14, Expression p15)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                Expression p11, Expression p12, Expression p13, Expression p14, Expression p15, Expression p16)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                Expression p11, Expression p12, Expression p13, Expression p14, Expression p15, Expression p16,
                Expression p17)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                Expression p11, Expression p12, Expression p13, Expression p14, Expression p15, Expression p16,
                Expression p17, Expression p18)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                Expression p11, Expression p12, Expression p13, Expression p14, Expression p15, Expression p16,
                Expression p17, Expression p18, Expression p19)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19);
    
    constructor(Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                Expression p11, Expression p12, Expression p13, Expression p14, Expression p15, Expression p16,
                Expression p17, Expression p18, Expression p19, Expression p20)
        <= new(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20);
    
    saveTo(List<DynamicClosure> list, ExpressionScope scope)
    {
        auto variables := new List<ScopeVariable>();
        
        auto codeScope := new ExpressionScope::
        {
            get int ScopeLevel = scope.ScopeLevel + variables.Length;
            
            Expression resolve(ScopeVariable variable, ref int index)
            {
                int len := variables.Length;
                for(int i := 0, i < len, i+=1) {
                    if (variables[i] == variable)
                    {
                        int parentLevel := scope.ScopeLevel;
                        
                        index := parentLevel + i + 1;
                        
                        ^ self
                    }
                };
                
                ^ scope.resolve(variable, ref index)
            }
    
            declare(ScopeVariable variable)
            {
                variables.append(variable)
            }            
        };
        
        var eosClosure := new ReleaseClosure(1);
        int len := statements.Length;
        for(int i := 0, i < len, i+=1) {
            statements[i].saveTo(list, codeScope);
            list.append(eosClosure)
        };
        
        int l := variables.Length;
        if (l != 0) {
            list.append(new ReleaseClosure(variables.Length))
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
    
    eval()
        = object;
    
    saveTo(List<DynamicClosure> list, ExpressionScope scope)
    {
        list.append(new ConstantClosure(object));
    }
}

public class NumericConstantExpression : ConstantExpression
{
    constructor(o)
    {
        this object := intConvertor.convert(o)
    }
}

public class SymbolExpression : Expression
{
    Symbol symbol;
    
    constructor(string s)
    {
        symbol := new Symbol(s)
    }
    
    eval()
        = symbol;
        
    saveTo(List<DynamicClosure> list, ExpressionScope scope)
    {
        list.append(new ConstantClosure(symbol));        
    }                
}

public class MessageCallExpression : Expression
{
    mssg         _message;
    Expression   _target;
    Expression[] _arguments;
    
    constructor new(mssg message, Expression t, params Expression[] arguments)
    {
        _message := message;
        _target := t;
        _arguments := arguments;
    }
    
    constructor new(mssg message, Expression t)
    {
        _message := message;
        _target := t;
        _arguments := new Expression[](0);
    }
    
    constructor(Expression t, string message)
        <= new(new Message(message + "[0]"), t);
    
    constructor(Expression t, string message, Expression p1)
        <= new(new Message(message + "[1]"), t, p1);
    
    constructor(Expression t, string message, Expression p1, Expression p2)
        <= new(new Message(message + "[2]"), t, p1, p2);
    
    constructor(Expression t, string message, Expression p1, Expression p2, Expression p3)
        <= new(new Message(message + "[3]"), t, p1, p2, p3);
    
    constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4)
        <= new(new Message(message + "[4]"), t, p1, p2, p3, p4);
    
    constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5)
        <= new(new Message(message + "[5]"), t, p1, p2, p3, p4, p5);
    
    constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6)
        <= new(new Message(message + "[6]"), t, p1, p2, p3, p4, p5, p6);
    
    constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7)
        <= new(new Message(message + "[7]"), t, p1, p2, p3, p4, p5, p6, p7);
    
    constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8)
        <= new(new Message(message + "[8]"), t, p1, p2, p3, p4, p5, p6, p7, p8);
    
    constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9)
        <= new(new Message(message + "[9]"), t, p1, p2, p3, p4, p5, p6, p7, p8, p9);
    
    constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10)
        <= new(new Message(message + "[10]"), t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
    
    constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                Expression p11)
        <= new(new Message(message + "[11]"), t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
    
    constructor(Expression t, string message, Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                Expression p11, Expression p12)
        <= new(new Message(message + "[12]"), t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
    
    saveTo(List<DynamicClosure> list, ExpressionScope scope)
    {
        int len := _arguments.Length;
        int i := len;
        while(i > 0)
        {
            i -= 1;
            
            _arguments[i].saveTo(list, scope);
        };
        
        _target.saveTo(list, scope);
        
        list.append(new MessageClosure(_message))
    }
}

public class DynamicMessageCallExpression : Expression
{
    Expression[] _arguments;
    
    constructor new(params Expression[] arguments)
    {
        _arguments := arguments;
    }
    
    constructor new()
    {
        _arguments := new Expression[](0);
    }
    
    constructor(Expression t)
        <= new(t);
    
    constructor(Expression t, Expression p1)
        <= new(t, p1);
    
    constructor(Expression t, Expression p1, Expression p2)
        <= new(t, p1, p2);
    
    constructor(Expression t, Expression p1, Expression p2, Expression p3)
        <= new(t, p1, p2, p3);
    
    constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4)
        <= new(t, p1, p2, p3, p4);
    
    constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5)
        <= new(t, p1, p2, p3, p4, p5);
    
    constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6)
        <= new(t, p1, p2, p3, p4, p5, p6);
    
    constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7)
        <= new(t, p1, p2, p3, p4, p5, p6, p7);
    
    constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8)
        <= new(t, p1, p2, p3, p4, p5, p6, p7, p8);
    
    constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9)
        <= new(t, p1, p2, p3, p4, p5, p6, p7, p8, p9);
    
    constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10)
        <= new(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
    
    constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                Expression p11)
        <= new(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
    
    constructor(Expression t, Expression p1, Expression p2, Expression p3, Expression p4, 
                Expression p5, Expression p6, Expression p7, Expression p8, Expression p9, Expression p10, 
                Expression p11, Expression p12)
        <= new(t, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
    
    saveTo(List<DynamicClosure> list, ExpressionScope scope)
    {
        int len := _arguments.Length;
        int i := len;
        while(i > 0)
        {
            i -= 1;
            
            _arguments[i].saveTo(list, scope);
        };
        
        list.append(DynamicMessageClosure)
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
    
    saveTo(List<DynamicClosure> list, ExpressionScope scope)
    {
        _target.saveTo(list, scope);
        
        auto mssg := _name.getPropertyMessage();
        list.append(new MessageClosure(mssg))
    }
}

public class GetDynamicPropertyExpression : Expression
{
    Expression _property;
    Expression _target;
    
    constructor(Expression t, Expression p)
        <= new(p, t);
        
    constructor new(Expression p, Expression t)
    {
        _property := Expression.MessageCall( 
                        new Message("getPropertyMessage[0]"),
                        Expression.New(
                            Expression.ClassReference("system'MessageName"),
                            p
                        )
                     );
        
        _target := t
    }
    
    saveTo(List<DynamicClosure> list, ExpressionScope scope)
    {
        _target.saveTo(list, scope);
        _property.saveTo(list, scope);
        
        list.append(DynamicMessageClosure)
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
    
    saveTo(List<DynamicClosure> list, ExpressionScope scope)
    {
        _value.saveTo(list, scope);
        _target.saveTo(list, scope);
        
        auto mssg := _name.setPropertyMessage();
        list.append(new MessageClosure(mssg))        
    }
}

public class SetDynamicPropertyExpression : Expression
{
    Expression _property;
    Expression  _target;
    Expression  _value;
    
    constructor(Expression t, Expression p, Expression v)
        <= new(p, t, v);
        
    constructor new(Expression p, Expression t, Expression v)
    {
        _property := Expression.MessageCall( 
                        new Message("setPropertyMessage[0]"),
                        Expression.New(
                            Expression.ClassReference("system'MessageName"),
                            p
                        )
                     );
        
        _target := t;
        _value := v
    }
    
    saveTo(List<DynamicClosure> list, ExpressionScope scope)
    {
        _value.saveTo(list, scope);
        _target.saveTo(list, scope);
        _property.saveTo(list, scope);
        
        list.append(DynamicMessageClosure)
    }
}

public class DeclarationExpression : Expression
{
    ScopeVariable variable;
    Expression    defaultValue;
    
    constructor(ScopeVariable variable)
    {
        this variable := variable;
        
        defaultValue := new ConstantExpression(nilValue)
    }
    
    saveTo(List<DynamicClosure> list, ExpressionScope scope)
    {
        scope.declare(variable);
        
        defaultValue.saveTo(list, scope);
        
        scope.resolve(variable, ref int index);        
        list.append(new LocalClosure(index));        
    }    
}

public class AssigningExpression : Expression
{
    ScopeVariable variable;
    Expression    expression;  
    
    constructor(ScopeVariable variable, Expression expression)
    {
        this variable := variable;
        this expression := expression
    }
    
    saveTo(List<DynamicClosure> list, ExpressionScope scope)
    {
        scope.resolve(variable, ref int index);
        
        expression.saveTo(list, scope);
        list.append(new AssigningClosure(index));
    }
}

public class DeclaringAndAssigningExpression : AssigningExpression
{
    constructor(ScopeVariable variable, Expression expression)
        <= (variable, expression)
    {
    }
    
    saveTo(List<DynamicClosure> list, ExpressionScope scope)
    {
        scope.declare(variable);
        
        new ConstantExpression(nilValue).saveTo(list, scope);
        
        super.saveTo(list, scope);
    }
}

public class VariableExpression : Expression
{
    ScopeVariable variable;
    
    constructor(ScopeVariable variable)
    {
        this variable := variable
    }
    
    saveTo(List<DynamicClosure> list, ExpressionScope scope)
    {
        scope.resolve(variable, ref int index);
        
        list.append(new LocalClosure(index));
    }
}

public class ReturnExpression : Expression
{
    Expression expr;
    
    constructor(Expression expr)
    {
        this expr := expr
    }
    
    saveTo(List<DynamicClosure> list, ExpressionScope scope)
    {
        expr.saveTo(list, scope);
        list.append(JumpToEnd)
    }
}

public class IfExpression : Expression
{
    Expression condition;
    Expression body;
    
    constructor(Expression condition, Expression body)
    {
        this condition := condition;
        this body := body;
    }
    
    saveTo(List<DynamicClosure> list, ExpressionScope scope)
    {
        condition.saveTo(list, scope);
        int bookmark := list.Length;
        list.append(cast DynamicClosure(nilValue));
        body.saveTo(list, scope);
        
        //int offs := list.Length - bookmark - 1;
        int offs := list.Length - bookmark;
        offs := offs - 1;
        
        list[bookmark] := new JumpIfNotClosure(offs);
        
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
    
    saveTo(List<DynamicClosure> list, ExpressionScope scope)
    {
        int top := list.Length;
        
        condition.saveTo(list, scope);
        int bookmark := list.Length;
        list.append(cast DynamicClosure(nilValue));
        body.saveTo(list, scope);
        
        int offs := top - list.Length - 1;
        
        list.append(new GoToClosure(offs));
        
        //offs := list.Length - bookmark - 1;
        offs := list.Length - bookmark;
        offs := offs - 1;
        
        list[bookmark] := new JumpIfNotClosure(offs);
        
        list.append(new ConstantClosure(nilValue));
    }
}

public class MethodExpression : Expression
{
    Message         message;
    ScopeVariable[] parameters;
    Expression      body;
    
    constructor(string name, Expression body)
    {
        this parameters := new ScopeVariable[]::( new ScopeVariable("self") );
        this message := new Message(name + "[0]");
        
        this body := body;
    } 
    
    constructor(string name, ScopeVariable param1, Expression body)
    {
        this parameters := new ScopeVariable[]::( param1, new ScopeVariable("self") );
        this message := new Message(name + "[1]");
        
        this body := body;
    } 
    
    constructor(string name, ScopeVariable param1, ScopeVariable param2, Expression body)
    {
        this parameters := new ScopeVariable[]::( param2, param1, new ScopeVariable("self") );
        this message := new Message(name + "[2]");
        
        this body := body;
    }
    
    constructor(string name, ScopeVariable param1, ScopeVariable param2, ScopeVariable param3, Expression body)
    {
        this parameters := new ScopeVariable[]::( param3, param2, param1, new ScopeVariable("self") );
        this message := new Message(name + "[3]");
        
        this body := body;
    }        
 
    saveTo(List<Tuple<Message,ClosureTape>> list)
    {
        list.append(new Tuple<Message,ClosureTape>(message, self.compiled()));
    }
           
    saveTo(List<DynamicClosure> list, ExpressionScope scope)
    {
        auto methodScope := new ExpressionScope::
        {
            get int ScopeLevel = parameters.Length;
            
            Expression resolve(ScopeVariable variable, ref int index)
            {
                int len := parameters.Length;
                for(int i := 0, i < len, i+=1) {
                    if (parameters[i] == variable)
                    {
                        index := i + 1;
                        
                        ^ self
                    }
                };
                
                ^ scope.resolve(variable, ref index)
            }
    
            declare(ScopeVariable variable)
            {
            }            
        };

        body.saveTo(list, methodScope);        
    }    
}


public class ClosureExpression : Expression
{
    ScopeVariable[] parameters;
    Expression      body;
    
    constructor(Expression body)
    {
        this parameters := new ScopeVariable[](0);
        
        this body := body;
    } 
    
    constructor(ScopeVariable param1, Expression body)
    {
        this parameters := new ScopeVariable[]::( param1 );
        
        this body := body;
    } 
    
    constructor(ScopeVariable param1, ScopeVariable param2, Expression body)
    {
        this parameters := new ScopeVariable[]::( param2, param1 ); // NOTE : it should be in reversed order
        
        this body := body;
    }
    
    constructor(ScopeVariable param1, ScopeVariable param2, ScopeVariable param3, Expression body)
    {
        this parameters := new ScopeVariable[]::( param3, param2, param1 ); // NOTE : it should be in reversed order
        
        this body := body;
    }        
 
    saveTo(List<DynamicClosure> list, ExpressionScope scope)
    {
        auto closureScope := new ExpressionScope::
        {
            get int ScopeLevel = parameters.Length;
            
            Expression resolve(ScopeVariable variable, ref int index)
            {
                int len := parameters.Length;
                for(int i := 0, i < len, i+=1) {
                    if (parameters[i] == variable)
                    {
                        index := i + 1;
                        
                        ^ self
                    }
                };
                
                ^ scope.resolve(variable, ref index)
            }
    
            declare(ScopeVariable variable)
            {
            }            
        };

        body.saveTo(list, closureScope);        
    }    
}

public abstract DynamicSymbol
{
    abstract compiled() {}
}

public class DynamicSingleton : DynamicSymbol
{
    Expression[] methods;

    constructor()
    {
        this methods := new Expression[](0);
    }
        
    constructor(Expression method)
    {
        this methods := new Expression[]::(method);
    }
    
    constructor(Expression method1, Expression method2)
    {
        this methods := new Expression[]::(method1, method2);                
    }
    
    constructor(Expression method1, Expression method2, Expression method3)
    {
        this methods := new Expression[]::(method1, method2, method3);                
    }
    
    constructor new(params Expression[] methods)
    {
        this methods := methods;                
    }
    
    compiled()
    {
        auto list := new List<Tuple<Message,ClosureTape>>();
        methods.forEach:(m){m.saveTo(list)};
        
        ^ ClosureClass.create(list);
    }
}

public class SymbolInfo
{
    prop string Name;
    prop DynamicSymbol Symbol;
    
    constructor(string name, DynamicSymbol symbol)
    {
        Name := name;
        Symbol := symbol
    } 
}

public class SymbolCollection
{
    SymbolInfo[] symbols;
    
    constructor(params SymbolInfo[] symbols)
    {
        this symbols := symbols;                
    }
    
    compiled()
    {
        auto set := new DynamicStruct();
        
        int len := symbols.Length;
        for (int i := 0, i < len, i+=1) {
            auto info := symbols[i];
            
            MessageName name := new MessageName(info.Name);
            Message prop := name.setPropertyMessage(); 

            var tape := info.Symbol.compiled();
 
            prop(set, tape);
        };
        
        ^ set
    }
}