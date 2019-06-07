import system'collections;

public interface Expression
{
    static Expression Constant(object o)
        = new ConstantExpression(o);
        
    static Expression Symbol(string s)
        = new SymbolExpression(s);
        
    static Expression MessageCall(mssg message, Expression t, params Expression[] arguments)
        = MessageCallExpression.new(message,t,params arguments);
        
    abstract saveTo(List<DynamicClosure> list)
    {
    }
    
    abstract eval() {}
    
    ClosureTape compiled()
    {
        List<DynamicClosure> list := new List<DynamicClosure>(8);
        
        self.saveTo(list);
        
        ^ ClosureTape.new:list
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
    
    saveTo(List<DynamicClosure> list)
    {
        list.append(new ConstantClosure(object));
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
        
    saveTo(List<DynamicClosure> list)
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
    
    // temporal solution
    eval()
    {
        self.compiled().eval();
    }
    
    saveTo(List<DynamicClosure> list)
    {
        int len := _arguments.Length;
        int i := len;
        while(i > 0)
        {
            i -= 1;
            
            _arguments[i].saveTo(list);
        };
        
        _target.saveTo(list);
        
        list.append(new MessageClosure(_message))
    }
}
