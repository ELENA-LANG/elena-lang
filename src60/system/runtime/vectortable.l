import system;

// --- PermVectorArray ---

class PermVectorArray
{
   __string object[] array;

   // cannot be created directly
   protected constructor(int size)
      = nil;

   at(int index)
      = array[index];

   setAt(int index, object obj)
      : external(system'core_routines'__perm_array_set);

   removeAt(int index)
      : external(system'core_routines'__perm_array_clear);

   int Length
   {
      get()
      {
         int len := $len array;
            
         ^ len
      }
   }

   refer(int index, ref pointer ptr)
      : external(system'core_routines'__perm_array_refer);
}

class PermVectorChain
{
   Reference<int> Free    : rprop;
   Reference<int> Current : rprop;

   PermVectorArray Array : rprop;

   PermVectorChain Next : rprop;
   
   private PermVectorArray createArray() : external(system'core_routines'__perm_vector_create);

   get bool isAvailable()
   {
      int free := *Free;

      ^ free > 0;
   }

   pointer allocate(object)
   {
      int     index := *Current;
      while (Array[index] != nil)
         index++;

      Array[index] := object;

      Current.Value := index + 1;

      Free.reduce(1);

      Array.refer(index, out pointer ptr);

      ^ ptr;
   }

   int indexOf(object)
   {
      int len := Array.Length;
      for (int i := 0; i < len; i++) {
         if (Array[i] != nil && object == Array[i]) {
            ^ i
         }
      };

      ^ -1
   }

   remove(int index)
   {
      Array.removeAt(index);
   }

   bool rearrange()
   {
      int len := Array.Length;
      int free := 0;
      for (int i := 0; i < len; i++) {
         if (Array[i] == nil)
            free++;
      };

      Current.Value := 0;
      Free.Value := free;

      ^ self.isAvailable;
   }

   constructor(PermVectorChain next)
   {
      Array   := self.createArray();
      Current := 0;
      Free    := Array.Length;
      Next    := next;
   }
}

// --- PermVectorTable ---

public singleton PermVectorTable
{
   static PermVectorChain? _firstChainItem;
   static PermVectorChain? _chainItem;

   get private bool isAvailable()
   {
      if (_chainItem == nil) {
         ^ false
      }
      else if(_chainItem.isAvailable) {
         ^ true
      };

      ^  false;
   }

   bool rearrange()
   {
      PermVectorChain? previous := nil;
      PermVectorChain? current := _firstChainItem;
      while (current != nil) {
         if (current.rearrange()) {
            _chainItem := current;

            ^ true
         };

         previous := current;
         current := current.Next;
      };

      if (previous == nil) {
         _firstChainItem := new PermVectorChain(previous);
         _chainItem := _firstChainItem;

         ^ true
      };
      _chainItem := previous;

      ^ false;
   }

   pointer allocate(object obj)
   {
      if (!self.isAvailable) {
         if:not (rearrange()) {
            _chainItem := new PermVectorChain(_chainItem);
         }
      };

      pointer ptr := _chainItem!.allocate(obj);

      ^ ptr;
   }

   release(object obj)
   {
      PermVectorChain? current := _firstChainItem;
      while (current != nil) {
         int index := current.indexOf(obj);
         if (index >= 0) {
            current.remove(index);

            :break;
         };

         current := current.Next;
      };
   }
}