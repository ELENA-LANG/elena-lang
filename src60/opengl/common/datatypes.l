// --- OVec4 ---

public const struct OVec4
{
   float x : rprop(_x);
   float y : rprop(_y);
   float z : rprop(_z);
   float w : rprop(_w);
   
   constructor(float x, float y, float z, float w)
   {
      _x := x;
      _y := y;
      _z := z;
      _w := w;
   }
   constructor()
   {
      
   }   
}
