// --- Assert ---

public class AssertException : Exception
{
   constructor new(string message)
      <= super new(message);
}

public singleton Assert
{
   ifEqual(object expr, object val)
   {
      ifnot(val == expr)
      {
         AssertException.raise("Assertion failed: expected {0}
was {1}".interpolate(val, expr))
      }
   }

   ifFailed(Func f)
   {
      var failed := false;
      f() \\ 
         on::(e)
         {
            failed := true;
         };
            
      Assert.ifTrue(failed);
    }

   ifTrue(b)
   {
      ifnot(b)
      {
         AssertException.raise("Assertion failed")
      }
   }
    
   ifFalse(b)
   {
      if(b)
      {
         AssertException.raise("Assertion failed")
      }
   }

   ifNil(b)
   {
      ifnot(nil == b)
      {
         AssertException.raise("Assertion failed")
      }
   }

   ifInstanceOf(object, class)
   {
      Assert.ifTrue(object.instanceOf(class));
   }
}

// --- testOp ---

public extension testOp
{
    assertTrue()
        = Assert.ifTrue(self);
    
    assertFalse()
        = Assert.ifFalse(self);
}
