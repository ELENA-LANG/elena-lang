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
   Reference<int> Used : rprop;

   PermVectorArray Array : rprop;

   PermVectorChain Next : rprop;
   
   private PermVectorArray createArray() : external(system'core_routines'__perm_vector_create);

   get bool isAvailable()
   {
      int length := Array.Length;
      int used := *Used;

      ^ length > used;
   }

   pointer allocate(object)
   {
      int     index := *Used;

      Array[index] := object;

      Used.append(1);

      Array.refer(index, ref pointer ptr);

      ^ ptr;
   }

   constructor(PermVectorChain next)
   {
      Array := self.createArray();
      Used  := 0;
      Next  := next;
   }
}

// --- PermVectorTable ---

public singleton PermVectorTable
{
   static PermVectorChain _chainItem;

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

   pointer allocate(object obj)
   {
      if (!self.isAvailable) {
         _chainItem := new PermVectorChain(_chainItem);
      };

      pointer ptr := _chainItem.allocate(obj);
   }
}