namespace collections
{
   // --- SortedList ---
   sealed class SortedListItem
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
   }

   public sealed class SortedList
   {
      SortedListItem _top;
   
//      Func<object,object,int> _comparator;
//   
//      constructor new()
//   //      <= new((object l,object r => l < r ? -1 : (l == r ? 0 : 1)))
//      {
//      }
//   
//      constructor new(Func<object,object,int> comparator)
//      {
//         _comparator := comparator
//      }   
//   
//      add(object key, object item)
//      {
//         auto item := new SortedListItem(key, item);
//   
//         SortedListItem prev := nil;
//         SortedListItem current := _top;
//         while (current != nil && _comparator(current.Key, key) < 0) {
//            prev := current;
//            current := current.Next;
//         };
//   
//         if (prev == nil) {
//            item.Next := _top;
//            _top := item;
//         }
//         else {
//            item.Next := prev;
//            prev.Next := item;
//         }
//      }
//   
//      private SortedListItem retrieve(int index)
//      {
//         int n := index;
//         SortedListItem current := _top;
//         while (n > 1) {
//            current := current.Next;
//            n--;
//         };
//   
//         ^ current
//      }
//   
//      object at(int index)
//      {
//         SortedListItem current := retrieve(index);
//   
//         ^ current?.Item;
//      }   
//   
//      setAt(int index, object item)
//      {
//         SortedListItem current := retrieve(index);
//         
//         current.Item := item;
//      }
   }

//   // --- SortedList<TKey,TValue> ---
//
//   sealed class SortedListItem<TKey,TValue>
//   {
//      TKey                         Key  : rprop;
//      TValue                       Item : prop;
//      SortedListItem<TKey, TValue> Next : prop;
//   
//      constructor(TKey key, TValue item)
//      {
//         Key := key;
//         Item := item;
//         Next := nil;
//      }
//   }
   
//   public sealed class SortedList<TKey,TValue>
//   {
//      SortedListItem<TKey, TValue> _top;
//   
//      Func<TKey,TKey,int> _comparator;
//   
//      constructor new()
//   //      <= new((object l,object r => l < r ? -1 : (l == r ? 0 : 1)))
//      {
//      }
//   
//      constructor new(Func<TKey,TKey,int> comparator)
//      {
//         _comparator := comparator
//      }   
//   
//      add(TKey key, TValue item)
//      {
////         auto item := new SortedListItem<TKey,TValue>(key, item);
////   
////         SortedListItem<TKey,TValue> prev := nil;
////         SortedListItem<TKey,TValue> current := _top;
////         while (current != nil && _comparator(current.Key, key) < 0) {
////            prev := current;
////            current := current.Next;
////         };
////   
////         if (prev == nil) {
////            item.Next := _top;
////            _top := item;
////         }
////         else {
////            item.Next := prev;
////            prev.Next := item;
////         }
//      }
//   
//      private /*SortedListItem<TKey,TValue>*/object retrieve(int index)
//      {
////         int n := index;
////         SortedListItem<TKey,TValue> current := _top;
////         while (n > 1) {
////            current := current.Next;
////            n--;
////         };
////   
////         ^ current
//      }
//   
//      TValue at(int index)
//      {
////         SortedListItem<TKey,TValue> current := retrieve(index);
////   
////         ^ current?.Item;
//      }   
//   
//      setAt(int index, TValue item)
//      {
////         SortedListItem current := retrieve(index);
////         
////         current.Item := item;
//      }
//   }
}