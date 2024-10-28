import extensions;
import system'threading;

public const struct XCharValue
{
   embeddable __word _value[4];

   bool equal(XString s)
   {
//      int sLen := s.Length;
//      int chLen := self.Length;
//      
//      if (sLen == chLen)
//      {
         XCharValue ch := s[0];

//         ^ self == ch    
//      };
//
//      ^ false
   }
}

public sealed class XString
{
   XCharValue at(int n)
   {
   }
}

public program()
{
    Assert.ifTrue("abc"[1] == "b");
    console.write(".");
}