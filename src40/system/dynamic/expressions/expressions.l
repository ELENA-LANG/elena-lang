import system'collections;

public interface class Expression
{
    static Expression Constant(object o)
        = new ConstantExpression(o);
        
    static Expression MessageCall(mssg message, Expression t, params Expression[] arguments)
        = MessageCallExpression.new(message,t,params arguments);
        
//    abstract saveTo(List<DynamicClosure> list)
//    {
//    }
//    
//    ClosureTape compiled()
//    {
//        List<DynamicClosure> list := new List<DynamicClosure>(8);
//        
//        self.saveTo(list);
//        
//        ^ ClosureTape.new:list
//    }
}

public class ConstantExpression : Expression
{
    object object;
    
    constructor(o)
    {
        this object := o
    }
    
//    saveTo(List<DynamicClosure> list)
//    {
//        list.append(new ConstantClosure(object));
//    }
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
    
//    saveTo(List<DynamicClosure> list)
//    {
//        int len := _arguments.Length;
//        int i := len;
//        while(i > 0)
//        {
//            i -= 1;
//            
//            _arguments[i].saveTo(list);
//        };
//        
//        _target.saveTo(list);
//        
//        list.append(new MessageClosure(_message))
//    }
}
