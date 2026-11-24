import system'winforms;

public sealed class OpenGLApplication
{
   Func OnUpdate : prop(_onUpdate);
   Func OnQuit   : prop(_onQuit);
   
   object       _mainWindow;

   constructor new()
   {
      _mainWindow := forward MainWindow.new();
      _onUpdate :=  {};
      _onQuit :=  {};
   }
   
   function()
   {
      _mainWindow.open();

      MSG          msg := new MSG();
      int          proceeded := 0;
      int          result := 0;
      bool         running := true;    
      
      WindowHandle handleCopy := _mainWindow;

      while (running) {
         result := extern USER32.PeekMessageW(msg, 0, 0, 0, PM_REMOVE);
         if (result != 0)
         {
            int cmd := msg.Code;
            if (cmd == WM_QUIT) {
               running := false;
            };
            
            excluded {
               extern USER32.TranslateMessage(msg);
               extern USER32.DispatchMessageW(msg)               
            }
         };
         _onUpdate();
      };
      
      _onQuit();
   }
   
   error(string caption, Exception e)
   {
      messageBox.open("ELENA", e.Message, MB_ICONEXCLAMATION);
   }   
   
   stop()
   {
      extern USER32.PostQuitMessage(0)
   }   
}

public static OpenGLApplication OpenGLProgram = OpenGLApplication.new(); 