// --- Image ---

public sealed class Image
{
   wide      _path;
   HDC       _DC;
   HBITMAP   _bitmap;
   Dimension _size;
    
   constructor load(string path, Dimension size)
   {
      _path := path;
      _size := size
   }
    
   open(HDC parentDC)
   {
      int width := _size.Width;
      int height := _size.Height;
 
      _bitmap := HBITMAP.load(_path, width, height);
        
      _DC := HDC.newCompatible(parentDC);
      _bitmap.select(_DC) 
   }
    
   HDC cast() = _DC;

   free()
   {
      _DC.free();
      _bitmap.free();
   }
}