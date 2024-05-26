import extensions;

/*   sealed class SortedListItem
   {
      object         Key  : rprop;
      object         Item : prop;
      SortedListItem Next : prop;
   
      constructor(object key, object item)
      {
         Key := key;
         Item := item;
         Next := nil;
      }
   }*/
/*
   public sealed class SortedList
   {
      SortedListItem _top;
      Reference<int> _length;
   
      Func<object,object,int> _comparator;
   
      constructor new()
         <= new((object l,object r => l < r ? -1 : (l == r ? 0 : 1)))
      {
      }
   
      constructor new(Func<object,object,int> comparator)
      {
         _comparator := comparator;
         _length := 0;
      }   

      int Length
         = *_length;

      internal SortedListItem FirstItem
         = _top;

      append(object key, object item)
      {
         auto item := new SortedListItem(key, item);
   
         SortedListItem prev := nil;
         SortedListItem current := _top;
         while (current != nil && _comparator(current.Key, key) < 0) {
            prev := current;
            current := current.Next;
         };
   
         if (prev == nil) {
            item.Next := _top;
            _top := item;
         }
         else {
            item.Next := prev;
            prev.Next := item;
         };

         _length++
      }
   
      private SortedListItem retrieve(int index)
      {
         int n := index;
         SortedListItem current := _top;
         while (n > 1) {
            current := current.Next;
            n--;
         };
   
         ^ current
      }
   
      object at(int index)
      {
         SortedListItem current := retrieve(index);
   
         ^ current?.Item;
      }   
   }
*/
class ifNilOperatorHelper
{
   get()
     = 2;

   get Value()
     = 3;
}

public program()
{
   var a := new ifNilOperatorHelper();

   var b := nil;
   Assert.ifEqual(a?.get(), 2);
   Assert.ifEqual(a?.Value, 3);
   console.write(".");
   Assert.ifNil(b?.get());
   Assert.ifNil(b?.Value);
   console.write(".");
}
