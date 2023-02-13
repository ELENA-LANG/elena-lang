import system;
import system'collections;

public extension stringListOp : String
{
   Array splitBy(char ch)
   {
      ArrayList list := new ArrayList();
      int       start := 0;
      int       index := 0;
      int       chLen := ch.Length;

      while (index >= 0)
      {
         index := self.indexOf(start, ch);
         if (-1 != index)
         {
            list.append(self.Substring(start,index - start));
            start := index + chLen
         }
         else
         {
            list.append(self.Substring(start))
         }
      };

      ^ list
   }
}