
// --- UnsafePointer ---

public sealed const struct UnsafePointer
{
   embeddable __ptr _pointer;

   private clear() : external(system'core_routines'__clearPtr);

   constructor()
   {
      self.clear();
   }

   constructor(pointer ptr)
   {
      _pointer := ptr
   }

   get constructor Default()
      = new UnsafePointer();   

   private readIntPtr(ref int retVal) : external(system'core_routines'__readIntPtr);

   assignPtr(wide str) : external(system'core_routines'__assignPtr); 

   bool isUnassigned()
   {
      ^ _pointer == 0;
   }

   // unsafe code
   string cast()
   {
      int  len := extern KERNEL32.lstrlenA(self);
      string s := String.allocate(len);
      extern KERNEL32.lstrcpyA(s, self);
        
      ^ s
   }

   wide cast()
   {
      int  len := extern KERNEL32.lstrlenW(self);
      wide s := WideString.allocate(len);
      extern KERNEL32.lstrcpyW(s, self);
        
      ^ s
   }

   int cast()
   {
      self.readIntPtr(ref int retVal);

      ^ retVal
   }

   copyTo(byte[] target, int len) : external(system'core_routines'__ptrCopyTo);
}

// --- CallStack ---

public sealed class CallStack
   : info("A call stack")
{
   pointer[] stack;

   private load(pointer[] stack, int length, ref int retVal) : external(system'core_routines'__loadCallStack);

   constructor()
   {
      pointer stack[64];
      self.load(stack, 64, ref int length);

      this stack := class Array<pointer>.copy(stack, 0, length);
   }

   constructor(int trimingLevel)
   {
      pointer stack[64];
      self.load(stack, 64, ref int length);

      this stack := class Array<pointer>.copy(stack, trimingLevel, length - trimingLevel);
   }

   string toPrintable()
      : info("Generates the call stack literal representation")
   {
      byte    buffer[512];
      int     bufferLength := 0; 
      pointer address;
      string  callStackText := emptyString;
      int     length := stack.Length;
      for (int i := 1, i < length, i += 1) {
         address := stack[i];

         bufferLength := extern LoadAddressInfoLM(address, buffer, 512);
         if (bufferLength > 0) {
            callStackText := callStackText + String.fromByteArray(0, bufferLength, buffer).add(newLineConstant)
         }
      };

      ^ callStackText
   }
}

// --- COORD ---

public sealed struct COORD
{
   short _x;
   short _y;

   constructor()
   {
      _x := 0;
      _y := 0;
   }

   write(short x, short y)
   {
      _x := x;
      _y := y
   }

   write(COORD r)
   {
      short tmp := r.X;
      _x := tmp;

      tmp := r.Y;
      _y := tmp
   }
    
   short X
   {
      get()
      {
         ^ _x
      }
   }
    
   short Y
   {    
      get()
      {
         ^ _y
      }
   }
}

// --- SMALL_RECT --

public sealed struct SMALL_RECT
{
   short _left;
   short _top;
   short _right;
   short _bottom;
    
   short Left
   {
      get()
      {
         ^ _left
      }
   }
    
   short Top
   {
      get()
      {
         ^ _top
      }
   }
    
   short Right
   {
      get()
      {
         ^ _right
      }        
   }
    
   short Bottom
   {
      get()
      {
         ^ _bottom
      }
   }
    
   write(SMALL_RECT r)
   {
      short tmp := r.Left;
      _left := tmp;
        
      tmp := r.Top;
      _top := tmp;
        
      tmp := r.Right;
      _right := tmp;
        
      tmp := r.Bottom;
      _bottom := tmp;
   }
}    
