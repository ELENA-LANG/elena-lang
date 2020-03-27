// --- DynamicClosure ---

public interface DynamicClosure
{
    abstract mixin function(params object[] args) {}
}

// --- DynamicMessageClosure ---

public singleton DynamicMessageClosure : DynamicClosure
{
    mixin function(params object[] args)
        => intern system'core_routines'__dynamicmssg_closure;
}

// --- MessageClosure ---

public struct MessageClosure : DynamicClosure
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

// --- FunctionClosure ---

public struct FunctionClosure : DynamicClosure
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
        => intern system'core_routines'__fun_closure;
}

// --- ConstantClosure --- 

public class ConstantClosure : DynamicClosure
{
    object constant;
    
    constructor load(constant)
    {
        this constant := constant;
    }
    
    mixin function(params object[] args)
        = constant;
}

// --- AssigningClosure ---

public struct AssigningClosure : DynamicClosure
{
    embeddable int index;
    
    constructor(int index)
    {
        this index := index
    }
    
    mixin function(params object[] args)
        => intern system'core_routines'__assign_closure;    
}

// --- LocalClosure ---

public struct LocalClosure : DynamicClosure
{
    embeddable int index;
    
    constructor(int index)
    {
        this index := index
    }
    
    mixin function(params object[] args)
        => intern system'core_routines'__local_closure;    
}

// --- ReleaseClosure ---

public struct ReleaseClosure : DynamicClosure
{
    embeddable int counter;
    
    constructor(int counter)
    {
        this counter := counter
    }
    
    mixin function(params object[] args)
        => intern system'core_routines'__free_closure;    
}

// --- GoToClosure ---

public struct GoToClosure : DynamicClosure
{
    int index;
    
    constructor(int index)
    {
        this index := index
    }
    
    mixin function(params object[] args)
        => intern system'core_routines'__jump_closure;
}

//public struct JumpIfClosure :: DynamicClosure
//{
//    int index.
//    
//    constructor new(int index)
//    [
//        @index := index.
//    ]
//    
//    generic closure(V<object> args)
//        => system'core_routines'__jumpif_closure.    
//}

// --- JumpIfNotClosure ---

public struct JumpIfNotClosure : DynamicClosure
{
    int index;
    
    constructor(int index)
    {
        this index := index
    }
    
    mixin function(params object[] args)
        => intern system'core_routines'__jumpifnot_closure;
}

public singleton JumpToEnd : DynamicClosure
{
    mixin function(params object[] args)
        => intern system'core_routines'__exit_closure;
}

