import system'winforms;

// --- File Dialog ---

class FileDialog
{
   WindowHandle _parent;
   wide         _title;
   wide[]       _filters;
   object       _selectedPath;

   string Caption
   {
      get()
         = _title;
            
      set(s)
      {
         _title := s
      }
        
//      set(wide s)
//      {
//         _title := s
//      }
   }
            
   addFilter(string caption, string mask)
   {
      wide[] subarr := new wide[]{ cast wide(caption), cast wide(mask) };
        
      if (_filters == nil)
      {
         _filters := subarr
      }
      else
      {
         _filters := _filters + subarr
      }
   }
    
   string Selected
      = _selectedPath;
    
   constructor new(parent)
   {
      _parent := parent
   }
}

// --- OpenFileDialog ---

public sealed class OpenFileDialog : FileDialog
{
   constructor new(parent)
      <= super new(parent);
    
   bool run()
   {
//      var dialog := new system'winforms'OpenFileDialog(theParent, theFilters, 0, 255, theTitle);
//   
//      theSelectedPath := dialog.select();
//        
//      ^ (nil != theSelectedPath)
   }    
}

// --- SaveFileDialog ---

public sealed class SaveFileDialog : FileDialog
{
   constructor new(parent)
      <= super new(parent);
    
   bool run()
   {
//      var dialog := new system'winforms'SaveFileDialog(theParent, theFilters, 0, 255, theTitle);
//   
//      theSelectedPath := dialog.select();
//        
//      ^ (nil != theSelectedPath)
   }    
}

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
