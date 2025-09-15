public sealed class SDIApplication
{
   object _mainWindow;

   constructor new()
   {
      _mainWindow := forward MainWindow.new();
   }

   function()
   {
      _mainWindow.open();

      MSG          msg := new MSG();
      int          proceeded := 0;
      int          result := 0;
      WindowHandle handleCopy := _mainWindow;
        
      extern
      {
         result := extern USER32.GetMessageW(msg, 0, 0, 0);
         while (result != 0)
         {
            proceeded := extern USER32.IsDialogMessageW(handleCopy, msg);
            if (proceeded == 0)
            {
               extern USER32.TranslateMessage(msg);
               extern USER32.DispatchMessageW(msg)
            };
            result := extern USER32.GetMessageW(msg, 0, 0, 0)
         }            
      }      
   }

   run(window)
   {
      MSG          msg := new MSG();
      int          proceeded := 0;
      int          result := 0;
      WindowHandle handleCopy := window;

      extern
      {
         while (handleCopy.isVisible())
         {
            result := extern USER32.PeekMessageW(msg, 0, 0, 0, 1);
            if (result != 0)
            {
               proceeded := extern USER32.IsDialogMessageW(handleCopy, msg);
               if (proceeded == 0)
               {
                  extern USER32.TranslateMessage(msg);
                  extern USER32.DispatchMessageW(msg)
               }
            }
            else 
            {
                    extern USER32.WaitMessage()
            }
         }
      }
   }

   error(string caption, Exception e)
   {
      messageBox.open("ELENA", /*e.Message*/e.toPrintable(), MB_ICONEXCLAMATION);
   }   

   stop()
   {
      extern USER32.PostQuitMessage(0)
   }
}

public static SDIApplication program = SDIApplication.new(); 