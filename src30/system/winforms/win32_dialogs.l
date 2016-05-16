#import system.

#symbol(const,int)OFN_PATHMUSTEXIST = 00000800h.
#symbol(const,int)OFN_EXPLORER      = 00080000h.
#symbol(const,int)OFN_LONGNAMES     = 00200000h.
#symbol(const,int)OFN_HIDEREADONLY  = 00000004h.

#symbol(const,int)DS_CENTER         = 0800h.

// --- messageBox ---

#symbol messageBox =
{
    #method(stacksafe) open &hwnd:parent &caption:caption &value:message &int:flags
    [
        #var(int)retVal := system'external'USER32 MessageBoxW
            &int:parent
            &wide:(message wide)
            &wide:(caption wide)
            &int:flags.
            
        ^ IntNumber new &int:retVal.
    ]
    
    #method(stacksafe) open &caption:caption &value:message &int:flags
    [
        #var aHandle := 'program mainWindow hwnd.
        
        ($nil == aHandle)
            ? [ aHandle := WindowHandle new. ].
                
        ^ messageBox open 
            &hwnd:(aHandle hwnd)
            &caption:caption
            &value:message
            &int:flags.
    ]
}.

// --- BaseFileDialog ---

#class BaseFileDialog
{
    #field(hwnd)theParent.
    #field(shortarray)theFilters.
    #field(shortarray)theFileName.
    #field(int)theFilterIndex.
    #field(int)theMaxFile.
    #field(wide)theCaption.
    #field(int)theFlags.
     
    #constructor new &hwnd:aParent &filters:Filters &int:aFilterIndex &int:aMaxFile &wide:aCaption &int:Flags
    [
        #var(array) arr := Filters array.
        #var(int)   i := 0.
        #var(int)  length := 0.
        #var(int)  s_length := 0.
        #var(int)  buf_size := 0.
        
        theParent := aParent.
        theFilterIndex := aFilterIndex.
        
        theFileName := ShortArray new &int:(aMaxFile + 1).
        theFileName write &int:0 &short:0.
        
        theMaxFile := aMaxFile.
        theCaption := aCaption.
        theFlags := Flags.
        
        // calculate the filter buffer size
        arr readLength &vint:length.
        #loop (i < length)?
        [
            #var(wide)aFilter := arr getAt &int:i wide.
            aFilter readLength &vint:s_length.
            buf_size := buf_size + s_length.
            buf_size := buf_size + 1.
            
            i := i + 1.
        ].
        buf_size := buf_size + 2.

        // fill the filter buffer        
        theFilters := ShortArray new &int:buf_size.
        i := 0.
        buf_size := 0.
        #loop (i < length)?
        [
            #var(wide)aFilter := arr getAt &int:i wide.
            aFilter readLength &vint:s_length.
            
            aFilter $save &int:buf_size &shortarray:theFilters &int:s_length.
            buf_size := buf_size + s_length.
            
            theFilters write &int:buf_size &short:0.
            
            buf_size := buf_size + 1.
            
            i := i + 1.
        ].
        theFilters write &int:buf_size &short:0.
        theFilters write &int:(buf_size + 1) &short:0.
    ]    
    
    #method(stacksafe) $prepare &bytearray:aStruct
    [
        #var(dirty_ptr)ptr.
        
        aStruct fill &int:0 &int:76 &byte:0.
        
//      lStructSize;
        aStruct write &int:0 &int:76.        
          
//      hwndOwner;
        ptr set &int:theParent.
        ptr saveTo &bytearray:aStruct &int:4.
          
//      hInstance;
        ptr set &int:CurrentInstance.
        ptr saveTo &bytearray:aStruct &int:8.

//        lpstrFilter;
        ptr set:theFilters.
        ptr saveTo &bytearray:aStruct &int:12.

//        nFilterIndex;
        aStruct write &int:24 &int:theFilterIndex.

//        lpstrFile;
        ptr set:theFileName.
        ptr saveTo &bytearray:aStruct &int:28.

//        nMaxFile;
        aStruct write &int:32 &int:theMaxFile.

//        lpstrTitle;
        ptr set:theCaption.
        ptr saveTo &bytearray:aStruct &int:48.

//        Flags;
        aStruct write &int:52 &int:theFlags.
    ]
}

//// --- OpenFileDialog ---
//
//#class(sealed) OpenFileDialog :: BaseFileDialog
//{
//    #constructor new &hwnd:aParent &filters:Filters &int:aFilterIndex &int:aMaxFile &wide:aCaption
//        <= new &hwnd:aParent &filters:Filters &int:aFilterIndex &int:aMaxFile &wide:aCaption 
//            &int:(OFN_PATHMUSTEXIST or:OFN_EXPLORER or:OFN_LONGNAMES or:DS_CENTER or:OFN_HIDEREADONLY int).
//    
//    #method select
//    [
//        #var(type:bytearray,size:76)aStruct.
//        
//        $self $prepare &bytearray:aStruct.
//        // NOTE: !! it should be any object creation between two lines
//        #var(type:int)aRetVal := system'external'Comdlg32 GetOpenFileNameW &bytearray:aStruct.
//        
//        (aRetVal == 0)
//            ? [ ^ nil. ]
//            ! [
//                #var(type:int)aLength := system'external'KERNEL32 lstrlenW &shortarray:theFileName.
//                
//                ^ WideLiteralValue $new &int:0 &int:aLength &shortarray:theFileName.
//            ].
//    ]
//}
//
//// --- SaveFileDialog ---
//
//#class(sealed) SaveFileDialog :: BaseFileDialog
//{
//    #constructor new &hwnd:aParent &filters:Filters &int:aFilterIndex &int:aMaxFile &wide:aCaption
//        <= new &hwnd:aParent &filters:Filters &int:aFilterIndex &int:aMaxFile &wide:aCaption 
//            &int:(OFN_PATHMUSTEXIST or:OFN_EXPLORER or:OFN_LONGNAMES or:DS_CENTER or:OFN_HIDEREADONLY int).
//    
//    #method select
//    [
//        #var(type:bytearray,size:76)aStruct.
//        
//        $self $prepare &bytearray:aStruct.
//        // NOTE: !! it should be any object creation between two lines
//        #var(type:int)aRetVal := system'external'Comdlg32 GetSaveFileNameW &bytearray:aStruct.
//        
//        (aRetVal == 0)
//            ? [ ^ nil. ]
//            ! [
//                #var(type:int)aLength := system'external'KERNEL32 lstrlenW &shortarray:theFileName.
//                
//                ^ WideLiteralValue $new &int:0 &int:aLength &shortarray:theFileName.
//            ].        
//    ]
//}
