import system;

// --- Struct ---

public mixin Struct
{
   embeddable __string object[] items;
    
   constructor allocate(int n)
      = new object[](n);

   constructor new(params object[] list)
      <= allocate(list.Length)
   {
      int len := list.Length;
      for(int i := 0, i < len, i := i + 1)
      {
         items[i] := list[i]
      }
   }
    
   constructor load(object[] list)
      <= allocate(list.Length)
   {
      int len := list.Length;
      for(int i := 0, i < len, i := i + 1)
      {
         items[i] := list[i]
      }
   }

   internal int Length
   {
      get()
      {
         int len := $len items;
            
         ^ len
      }
   }

   internal Struct expand(int gain)
   {
      int len := self.Length;
      int newLength := len + gain;
        
      auto newArr := Struct.allocate(newLength);
        
      for(int i := 0, i < len, i += 1)
      {
         newArr[i] := items[i];
      };
        
      ^ newArr
   }
                
   internal at(int index)
      = items[index];

   internal setAt(int index, object o)
   {
      items[index] := o
   }
    
   dispatch() : external(system'core_routines'__struct);
}

// --- DynamicStruct ---

public class DynamicStruct
{
   Struct _struct;
    
   constructor()
   {
      _struct := Struct.allocate(0)
   }
    
   set generic(o)
   {
      MessageName subj := __received.MessageName;
        
      bool notFound := true;
      int len := _struct.Length;
      int i := 0;
      while ((i < len) && notFound)
      {
         var item := _struct.at(i);
         if (cast MessageName(item) == subj)
         {
            notFound := false
         }
         else
         {
            i := i + 2
         }
      };
                
      // if not found extend the structure
      if(i == len)
      {                          
         _struct := _struct.expand(2);

         _struct.setAt(i, subj)
      };

      // set the content for the appropriate field
      _struct.setAt(i + 1, o);
   }
    
   containsProperty(MessageName subj)
   {
      int index := 0;        
      int length := _struct.Length;
        
      while (index < length)
      {
         var current := _struct[index];
            
         if(subj.equal(cast MessageName(current)))
            { ^ true };
                
         index := index + 2
      };
        
      ^ false
   }
    
   dispatch() => _struct;
}
