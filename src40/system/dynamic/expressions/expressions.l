//import system'collections.

public interface class Expression
{
    //static Expression Constant(object o)
      //  = new ConstantExpression(o);
        
//    T<Expression> generic static MessageCall(mssg message, Expression t, V<Expression> arguments)
//        = MessageCallExpression new(message,t,arguments).
//        
//    abstract saveTo(List<DynamicClosure> list)
//    [
//    ]
//    
//    T<ClosureTape> compiled
//    [
//        List<DynamicClosure> list := List<DynamicClosure>(8).
//        
//        self saveTo(list).
//        
//        ^ ClosureTape new:list.
//    ]
}

public class ConstantExpression : Expression
{
    object object;
    
    constructor(object o)
    {
        this object := o
    }
    
//    saveTo(List<DynamicClosure> list)
//    [
//        list append(ConstantClosure new(object)).
//    ]
}

//public class MessageCallExpression :: Expression
//{
//    mssg              _message.
//    T<Expression>     _target.
//    Array<Expression> _arguments.
//    
//    generic constructor new(mssg message, Expression t, V<Expression> arguments)
//    [
//        _message := message.
//        _target := t.
//        _arguments := arguments.
//    ]
//    
//    saveTo(List<DynamicClosure> list)
//    [
//        int len := _arguments length.
//        int i := len.
//        while(i > 0)
//        [
//            i -= 1.
//            
//            _arguments[i] saveTo(list).
//        ].
//        
//        _target saveTo(list).
//        
//        list append(MessageClosure new:_message)
//    ]
//}
