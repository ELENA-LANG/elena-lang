import system;

// --- TapeFunction ---

public interface TapeFunction
{
   abstract mixin function(params object[] args);
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
   embeddable Message message;
    
   constructor(string message)
      <= load(new Message(message));
    
   constructor load(string message)
      <= load(new Message(message));
    
   constructor load(Message message)
   {
      this message := message
   }
    
   mixin function(params object[] args) : external(system'core_routines'__mssg_closure);
}

// --- SavingFunction ---

public struct SavingFunction : TapeFunction
{
    embeddable int index;
    
    constructor(int index)
    {
        this index := index
    }
    
    mixin function(params object[] args) : external(system'core_routines'__saving_closure);
}

// --- AllocFunction ---

public struct AllocFunction : TapeFunction
{
    int size;
    
    constructor(int size)
    {
        this size := size
    }
    
    mixin function(params object[] args) : external(system'core_routines'__alloc_closure);
}

// --- AssigningFunction ---

public struct AssigningFunction : TapeFunction
{
    embeddable int index;
    
    constructor(int index)
    {
        this index := index
    }
    
    mixin function(params object[] args) : external(system'core_routines'__assign_closure);
}
