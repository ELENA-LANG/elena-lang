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

// --- FuncFunction ---

public struct FuncFunction : TapeFunction
{
   Message  message;
   int      targetIndex;
    
   constructor(string message, int targetIndex)
      <= load(new Message(message), targetIndex);
    
   constructor load(string message, int targetIndex)
      <= load(new Message(message), targetIndex);
    
   constructor load(Message message, int targetIndex)
   {
      this message := message;
      this targetIndex := targetIndex;
   }
    
   mixin function(params object[] args) : external(system'core_routines'__funcmssg_closure);
}

// --- ExtensionMessageFunction ---

public class ExtensionMessageFunction : TapeFunction
{
   Message message;
   Object  target;
    
   constructor(string message, object target)
      <= load(new Message(message), target);
    
   constructor load(string message, object target)
      <= load(new Message(message), target);
    
   constructor load(Message message, object target)
   {
      this message := message;
      this target := target
   }
    
   mixin function(params object[] args) : external(system'core_routines'__ext_closure);
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

public struct CopyToStackFunction : TapeFunction
{
   int index;
   int offs;
    
   constructor(int index, int offs)
   {
        this index := index;
        this offs := offs;
   }
    
   mixin function(params object[] args) : external(system'core_routines'__copytostack_closure);
}

// --- AllocFunction ---

public struct AllocFunction : TapeFunction
{
   int size;
    
   constructor(string strSize)
   {
      this size := intConvertor.convert(strSize)
   }
    
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

// --- LocalFunction ---

public struct LocalFunction : TapeFunction
{
   embeddable int index;
    
   constructor(int index)
   {
      this index := index
   }
    
   constructor(string strIndex)
   {
      this index := intConvertor.convert(strIndex)
   }
    
   mixin function(params object[] args) : external(system'core_routines'__local_closure);
}

// --- DynamicMessageFunction ---

public struct DynamicMessageFunction : TapeFunction
{
   embeddable int index;
    
   constructor(int index)
   {
      this index := index
   }    
    
   mixin function(params object[] args) : external(system'core_routines'__dynamicmssg_closure);
}

public singleton JumpToEnd : TapeFunction
{
    mixin function(params object[] args) : external(system'core_routines'__exit_closure);
}

// --- JumpIfNotFunction ---

public struct JumpIfNotFunction : TapeFunction
{
   int index;
    
   constructor(int index)
   {
      this index := index
   }
    
   mixin function(params object[] args) : external(system'core_routines'__jumpifnot_closure);
}

// --- GoToFunction ---

public struct GoToFunction : TapeFunction
{
   int index;
    
   constructor(int index)
   {
      this index := index
   }
    
   mixin function(params object[] args) : external(system'core_routines'__jump_closure);
}
