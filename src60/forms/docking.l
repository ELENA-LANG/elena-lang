import system'drawing;

public sealed class DockingBox : BaseNonVisibleControl, interface<IControl>
{
   Dimension    _size;
   
   BaseControl Left : prop(_left);
   BaseControl Right : prop(_right);
   BaseControl Top : prop(_top);
   BaseControl Bottom : prop(_bottom);
   BaseControl Client : prop(_client);

   constructor new()
   {
      _size :=  new Dimension(50, 50)
   }
    
   get Control() = self;

   IControl? retrieve(Handle handle)
   {
      ^ nil
   }
    
   click() {}
   
   change() {}   
   
   open()
   {
      refresh();
   }

   close() {}

   onParentResize(BaseControl parentControl)
   {
      setRegion(parentControl.X, parentControl.Y, parentControl.Width, parentControl.Height);
   }

   setRegion(int x, int y, int width, int height)
   {
      _size.Width := width;
      _size.Height := height;
      
      refresh();
   }

   refresh()
   {
      int height := 0;
      int width := 0;
      int x := 0;
      int y := 0;
      int totalWidth := _size.Width;
      int totalHeight := _size.Height;
        
      // Bottom
      if:not:nil(_bottom) {
         height := _bottom.Height;
         
         _bottom.Width := totalWidth;
         
         totalHeight -= height;
      };
        
      // Top
      if:not:nil(_top) {
         height := _top.Height;
         
         _bottom.Width := totalWidth;
         
         totalHeight -= height;         
         y := height;
      };
        
      // Left
      if:not:nil(_left) {
         width := _left.Width;
         
         _left.Height := totalHeight;
         
         totalWidth -= width;
         x := width;
      };
        
      // Right
      if:not:nil(_right) {
         width := _right.Width;
         
         _right.Height := totalHeight;
         
         totalWidth -= width;
      };

      // Client
      if:not:nil(_client) {
         _client.setRegion(x, y, totalWidth, totalHeight);
      };
      
      x += totalWidth;
      y += totalHeight;

      if:not:nil(_right) {
         _right.X := x;
         _right.Y := height;
      };
      
      if:not:nil(_bottom) {
         _bottom.X := 0;
         _right.Y := y;
      }
   }
}
