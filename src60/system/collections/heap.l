namespace collections
{
   // --- BinaryHeap ---
   public class BinaryHeap<T> : info("A binary heap")
   {
      T                _default;
      T[]              _btree;
      Func<T, T, bool> _comparator;
      Reference<int>   _length;
      
      constructor() : info("Creates a min binary heap")
      {
         _comparator := (l, r => l < r);
         _btree := new T[](32);
         _length := 0;
         _default := default;
      }

      constructor newMaxHeap() : info("Creates a max binary heap")
      {
         _comparator := (l, r => l > r);
      }
            
      constructor new(Func<T, T, bool> comparator) : info("Creates a binary heap with a custom comparator")
      {
         _comparator := comparator
      }

      constructor heapify(Enumerable<T> list, Func<T, T, bool> comparator) : info("Creates a binary heap from the provided list with a custom comparator")
         <= new(comparator)
      {
         pushRange(list);
      }

      constructor heapifyMinHeap(Enumerable<T> list) : info("Creates a binary min heap from the provided list")
      {
         pushRange(list);
      }

      constructor heapifyMaxHeap(Enumerable<T> list) : info("Creates a binary max heap from the provided list")
         <= newMaxHeap()
      {
         pushRange(list);         
      }

      private pushRange(Enumerable<T> list)
      {
         foreach(T item; in list) {
            push(item);
         }
      }

      private expand(int delta)
      {
         int len := *_length;
         int newSize := $len _btree;
         
         newSize += delta;
         
         T[] newArray := new T[](newSize);
         for(int i := 0; i < len; i++)
            newArray[i] := _btree[i];
            
         _btree := newArray;
      }
      
      private T getParent(int index)      
         = _btree[(index - 1) shr:1];
         
      private int getParentIndex(int index)      
         = (index - 1) shr:1;
         
      private T getLeftChild(int index)         
         = _btree[(index shl:2) + 1];
         
      private int getLeftChildIndex(int index)         
         = (index shl:2) + 1;
         
      private T getRightChild(int index)         
         = _btree[(index shl:2) + 2];

      private int getRightChildIndex(int index)         
         = (index shl:2) + 2;

      private bool shiftUp(int index)
      {
         int parentIndex := getParentIndex(index);
         
         T current := _btree[index];
         T parent := _btree[parentIndex];

         if:not (_comparator(parent, current)) {
            _btree[index] := parent;
            _btree[parentIndex] := current;
            
            ^ true
         };
         
         ^ false
      }

      private heapify(int index)
      {
         int current := index;
         int len := *_length;
         
         while (current < len) {
            int l := getLeftChildIndex(current);
            int r := getRightChildIndex(current);
            
            int smallest := current;
            if (l < len && _comparator(_btree[l], _btree[smallest])) {
               smallest := l;
            };
            if (r < len && _comparator(_btree[r], _btree[smallest])) {
               smallest := r;
            };
            
            if (smallest != current) {
               T tmp := _btree[smallest];
               _btree[smallest] := _btree[current];
               _btree[current] := tmp;
               
               current := smallest
            }
            else current := len
         };
      }

      int Length : info("Returns the number of elements in the binary heap")
         = _length;

      push(T item) : info("Appends an element into the binary heap")
      {
         int len := *_length;
         if (len == (:len _btree))
            expand(32);
         
         _btree[len] := item;
         
         int i := len;
         while(i > 0 && shiftUp(i)) {
            i := getParentIndex(i);
         };
         
         len++;
         _length.Value := len
      }
      
      private removeAt(int index)
      {
         int len := *_length;
         _btree[index] := _btree[len - 1];
         len--;
         _length.Value := len;
         
         heapify(index);
      }
      
      bool remove(T item) : info("Removes a first occurrence of <b>item</b> element if found and returns true or false if no element was found")
      {
         int len := *_length;
         for (int i := 0; i < len; i++) {
            if (item == _btree[i]) {
               removeAt(i);
               
               ^ true
            }
         };
         
         ^ false
      }
      
      bool remove(item) : info("Removes a first occurrence of <b>item</b> element if found and returns true or false if no element was found")
      {
         int len := *_length;
         for (int i := 0; i < len; i++) {
            if (item == _btree[i]) {
               removeAt(i);
               
               ^ true
            }
         };
         
         ^ false         
      }
      
      T pop() : info("Removes a top element from the binary heap and returns it.<br/>Depending on the type of the heap it can be a minimal or maximal element")
      {
         int len := *_length;
         if (len == 0) {
            ^ _default
         };
                  
         T retVal := _btree[0];
         if (len > 1) {
            _btree[0] := _btree[len - 1];

            len--;
            _length.Value := len;
                        
            heapify(0);
         }
         else _length.Value := 0;
                  
         ^ retVal;
      }
      
      T peek() : info("Returns a top element from the binary heap<br/>Depending on the type of the heap it can be a minimal or maximal element.<br/>The heap content is not modified")
         = _btree[0];

      T pushPop(T item) : info("Appends an element into the binary heap, rebuilds it and removes a top element from the binary heap returning it.<br/>Depending on the type of the heap it can be a minimal or maximal element")
      {
         push(item);
         
         ^ pop()
      }

      T popPush(T item) : info("Removes a top element from the binary heap, appends a new one and returns the former top one.<br/>Depending on the type of the heap it can be a minimal or maximal element")
      {
         T retVal := pop();
         push(item);
         
         ^ retVal
      }

      clear() : info("Removes all elements from the binary heap")
      {
         _length.Value := 0
      }
   }
}