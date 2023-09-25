import system;

public sealed class SDIApplication
{
   object _mainWindow;

   internal constructor()
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

   error(string caption, Exception e)
   {
      messageBox.open("ELENA", e.Message, MB_ICONEXCLAMATION);
   }   
}

public static SDIApplication program = new SDIApplication();