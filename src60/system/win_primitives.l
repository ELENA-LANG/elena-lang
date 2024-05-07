// --- PrimitivePointerOperations ---
internal singleton PrimitivePointerOperations
{
   string readString(pointer ptr)
   {
      int  len := extern KERNEL32.lstrlenA(ptr);
      string s := String.allocate(len);
      extern KERNEL32.lstrcpyA(s, ptr);
        
      ^ s
   }

   wide readWideString(pointer ptr)
   {
      int  len := extern KERNEL32.lstrlenW(ptr);
      wide s := WideString.allocate(len);
      extern KERNEL32.lstrcpyW(s, ptr);
        
      ^ s
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
