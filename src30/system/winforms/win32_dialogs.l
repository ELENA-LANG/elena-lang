#import system.

#symbol(const,type:int)OFN_PATHMUSTEXIST = 00000800h.
#symbol(const,type:int)OFN_EXPLORER      = 00080000h.
#symbol(const,type:int)OFN_LONGNAMES     = 00200000h.
#symbol(const,type:int)OFN_HIDEREADONLY  = 00000004h.

#symbol(const,type:int)DS_CENTER         = 0800h.

// --- messageBox ---

#symbol messageBox =
{
    #method(stacksafe) open &hwnd:parent &caption:caption &value:message &int:flags
    [
        #var(type:int)retVal := system'external'USER32 MessageBoxW
            &handle:parent
            &wide:(message wide)
            &wide:(caption wide)
            &int:flags.
            
        ^ IntNumber new &int:retVal.
    ]
    
    #method(stacksafe) open &caption:caption &value:message &int:flags
    [
        #var(type:int)retVal := system'external'USER32 MessageBoxW
            &handle:('program mainWindow hwnd)
            &wide:(message wide)
            &wide:(caption wide)
            &int:flags.
            
        ^ IntNumber new &int:retVal.
    ]
}.

// --- BaseFileDialog ---

#class BaseFileDialog
{
    #field(type:hwnd)theParent.
    #field(type:shortarray)theFilters.
    #field(type:shortarray)theFileName.
    #field(type:int)theFilterIndex.
    #field(type:int)theMaxFile.
    #field(type:wide)theCaption.
    #field(type:int)theFlags.
     
    #constructor new &hwnd:aParent &filters:Filters &int:aFilterIndex &int:aMaxFile &wide:aCaption &int:Flags
    [
        #var(type:array) arr := Filters array.
        #var(type:int)   i := 0.
        #var(type: int)  length := 0.
        #var(type: int)  s_length := 0.
        #var(type: int)  buf_size := 0.
        
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
            #var(type:wide)aFilter := arr getAt &int:i wide.
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
            #var(type:wide)aFilter := arr getAt &int:i wide.
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
        #var(type:dirty_ptr)ptr.
        
        aStruct fill &int:0 &int:76 &byte:0.
        
//      lStructSize;
        aStruct write &int:0 &int:76.        
          
//      hwndOwner;
        ptr write &handle:theParent.
        aStruct write &int:4 &int:ptr.
          
//      hInstance;
        ptr write &handle:CurrentInstance.
        aStruct write &int:8 &int:ptr.

//        lpstrFilter;
        ptr write &shortarray:theFilters.
        aStruct write &int:12 &int:ptr.

//        nFilterIndex;
        aStruct write &int:24 &int:theFilterIndex.

//        lpstrFile;
        ptr write &shortarray:theFileName.
        aStruct write &int:28 &int:ptr.

//        nMaxFile;
        aStruct write &int:32 &int:theMaxFile.

//        lpstrTitle;
        ptr write &wide:theCaption.
        aStruct write &int:48 &int:ptr.

//        Flags;
        aStruct write &int:52 &int:theFlags.
    ]
}

// --- OpenFileDialog ---

#class(sealed) OpenFileDialog :: BaseFileDialog
{
    #constructor new &hwnd:aParent &filters:Filters &int:aFilterIndex &int:aMaxFile &wide:aCaption
        <= new &hwnd:aParent &filters:Filters &int:aFilterIndex &int:aMaxFile &wide:aCaption 
            &int:(OFN_PATHMUSTEXIST or:OFN_EXPLORER or:OFN_LONGNAMES or:DS_CENTER or:OFN_HIDEREADONLY int).
    
    #method select
    [
        #var(type:bytearray,size:76)aStruct.
        
        $self $prepare &bytearray:aStruct.
        // NOTE: !! it should be any object creation between two lines
        #var(type:int)aRetVal := system'external'Comdlg32 GetOpenFileNameW &bytearray:aStruct.
        
        (aRetVal == 0)
            ? [ ^ nil. ]
            ! [
                #var(type:int)aLength := system'external'KERNEL32 lstrlenW &shortarray:theFileName.
                
                ^ WideLiteralValue $new &int:0 &int:aLength &shortarray:theFileName.
            ].
    ]
}

// --- SaveFileDialog ---

#class(sealed) SaveFileDialog :: BaseFileDialog
{
    #constructor new &hwnd:aParent &filters:Filters &int:aFilterIndex &int:aMaxFile &wide:aCaption
        <= new &hwnd:aParent &filters:Filters &int:aFilterIndex &int:aMaxFile &wide:aCaption 
            &int:(OFN_PATHMUSTEXIST or:OFN_EXPLORER or:OFN_LONGNAMES or:DS_CENTER or:OFN_HIDEREADONLY int).
    
    #method select
    [
        #var(type:bytearray,size:76)aStruct.
        
        $self $prepare &bytearray:aStruct.
        // NOTE: !! it should be any object creation between two lines
        #var(type:int)aRetVal := system'external'Comdlg32 GetSaveFileNameW &bytearray:aStruct.
        
        (aRetVal == 0)
            ? [ ^ nil. ]
            ! [
                #var(type:int)aLength := system'external'KERNEL32 lstrlenW &shortarray:theFileName.
                
                ^ WideLiteralValue $new &int:0 &int:aLength &shortarray:theFileName.
            ].        
    ]
}
