import system'winforms;

// --- messageDialog ---

public singleton messageDialog
{
   showInformation(parent, caption, text)
   {
      messageBox.open(cast WindowHandle(parent), cast string(caption), cast string(text), MB_ICONINFORMATION)
   }
    
   showInformation(caption, text)
   {
      messageBox.open(cast string(caption), cast string(text), MB_ICONINFORMATION)
   }
    
   showExclamation(parent, caption, text)
   {
      messageBox.open(cast WindowHandle(parent), cast string(caption), cast string(text), MB_ICONEXCLAMATION)
   }
    
   showExclamation(caption, text)
   {
      messageBox.open(cast string(caption), cast string(text), MB_ICONEXCLAMATION)
   }
    
   showWarning(parent, caption, text)
   {
      messageBox.open(cast WindowHandle(parent), cast string(caption), cast string(text), MB_ICONWARNING)
   }
    
   showWarning(caption, text)
   {
      messageBox.open(cast string(caption), cast string(text), MB_ICONWARNING)
   }
    
   showQuestion(parent, caption, text)
   {
      var result := messageBox.open(cast WindowHandle(parent), cast string(caption), cast string(text), MB_ICONQUESTION | MB_YESNO);
        
      ^ result == IDYES
   }
    
   showQuestion(string caption, string text)
   {
      int result := messageBox.open(caption, text, MB_ICONQUESTION | MB_YESNO);
        
      ^ result == IDYES
   }
}
