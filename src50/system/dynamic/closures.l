// --- TapeFunction ---

public interface TapeFunction
{
    abstract mixin function(params object[] args);
}

// --- AllocFunction ---

public struct AllocFunction : TapeFunction
{
    int size;
    
    constructor(int size)
    {
        this size := size
    }
    
    mixin function(params object[] args)
        => intern system'core_routines'__alloc_closure;    
}

// --- ConstantFunction --- 

public class ConstantFunction : TapeFunction
{
    object constant;
    
    constructor load(constant)
    {
        this constant := constant;
    }
    
    mixin function(params object[] args)
        = constant;
}

// --- MessageFunction ---

public struct MessageFunction : TapeFunction
{
    embeddable Message theMessage;
    
    constructor(string message)
        <= load(new Message(message));
    
    constructor load(string message)
        <= load(new Message(message));
    
    constructor load(Message message)
    {
        theMessage := message
    }
    
    mixin function(params object[] args)
        => intern system'core_routines'__mssg_closure;
}

// --- DynamicMessageFunction ---

public struct DynamicMessageFunction : TapeFunction
{
    embeddable int index;
    
    constructor(int index)
    {
        this index := index
    }    
    
    mixin function(params object[] args)
        => intern system'core_routines'__dynamicmssg_closure;
}

//// --- FuncFunction ---
//
//public struct FuncFunction : TapeFunction
//{
//    embeddable Message theMessage;
//    
//    constructor(string message)
//        <= load(new Message(message));
//    
//    constructor load(string message)
//        <= load(new Message(message));
//    
//    constructor load(Message message)
//    {
//        theMessage := message
//    }
//    
//    mixin function(params object[] args)
//        => intern system'core_routines'__fun_closure;
//}

// --- AssigningFunction ---

public struct AssigningFunction : TapeFunction
{
    embeddable int index;
    
    constructor(int index)
    {
        this index := index
    }
    
    mixin function(params object[] args)
        => intern system'core_routines'__assign_closure;    
}

// --- LocalFunction ---

public struct LocalFunction : TapeFunction
{
    embeddable int index;
    
    constructor(int index)
    {
        this index := index
    }
    
    mixin function(params object[] args)
        => intern system'core_routines'__local_closure;    
}

// --- SavingFunction ---

public struct SavingFunction : TapeFunction
{
    embeddable int index;
    
    constructor(int index)
    {
        this index := index
    }
    
    mixin function(params object[] args)
        => intern system'core_routines'__saving_closure;    
}

// --- LocalFunction ---

public struct CopyToStackFunction : TapeFunction
{
    int index;
    int offs;
    
    constructor(int index, int offs)
    {
        this index := index;
        this offs := offs;
    }
    
    mixin function(params object[] args)
        => intern system'core_routines'__copytostack_closure;    
}

//// --- ReleaseFunction ---
//
//public struct ReleaseFunction : TapeFunction
//{
//    embeddable int counter;
//    
//    constructor(int counter)
//    {
//        this counter := counter
//    }
//    
//    constructor()
//    {
//        this counter := 1
//    }
//    
//    mixin function(params object[] args)
//        => intern system'core_routines'__free_closure;    
//}
//
//// --- GoToFunction ---
//
//public struct GoToFunction : TapeFunction
//{
//    int index;
//    
//    constructor(int index)
//    {
//        this index := index
//    }
//    
//    mixin function(params object[] args)
//        => intern system'core_routines'__jump_closure;
//}
//
//public struct JumpIfFunction : TapeFunction
//{
//    int index;
//    
//    constructor new(int index)
//    {
//        this index := index
//    }
//    
//    mixin function(params object[] args)
//        => intern system'core_routines'__jumpif_closure;
//}
//
//// --- JumpIfNotFunction ---
//
//public struct JumpIfNotFunction : TapeFunction
//{
//    int index;
//    
//    constructor(int index)
//    {
//        this index := index
//    }
//    
//    mixin function(params object[] args)
//        => intern system'core_routines'__jumpifnot_closure;
//}

public singleton JumpToEnd : TapeFunction
{
    mixin function(params object[] args)
        => intern system'core_routines'__exit_closure;
}

//// --- ExtensionMessageFunction ---
//
//public class ExtensionMessageFunction : TapeFunction
//{
//    ExtensionDispatcher dispatcher;
//    Message             message;
//    
//    constructor load(ExtensionDispatcher dispatcher, Message message)
//    {
//        this dispatcher := dispatcher;
//        this message := message
//    }
//    
//    mixin function(params object[] args)
//        => intern system'core_routines'__ext_closure;    
//}
// 
//public class VariadicExtensionMessageFunction : TapeFunction
//{
//    VariadicExtensionDispatcher dispatcher;
//    Message                     message;
//    
//    constructor load(VariadicExtensionDispatcher dispatcher, Message message)
//    {
//        this dispatcher := dispatcher;
//        this message := message
//    }
//    
//    mixin function(params object[] args)
//        => intern system'core_routines'__ext_closure;    
//}
// 
