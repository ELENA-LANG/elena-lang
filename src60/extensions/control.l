// --- flowOp ---

public extension flowOp
{
   writeTo(target)
   {
      target.write(self);
   }
}

// --- loopOp --

public extension loopOp
{
   doWhile()
   {
      while (cast bool(self))
      {
      }
   }
}

// --- invoker ---

public extension invokeOp : MessageName
{
    getPropertyValue(target)
    {
        Message mssg := self.getPropertyMessage();
        
        ^ mssg(target)
    
    }
    setPropertyValue(target,value)
    {
        Message mssg := self.setPropertyMessage();
        
        mssg(target, value)
    }
}

// --- exchange ---

public exchange(ref object l, ref object r)
{
    var tmp := l;
    l := r;
    r := tmp
}
