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
