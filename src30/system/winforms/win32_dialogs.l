#define system.

#symbol(const,type:int)OFN_PATHMUSTEXIST = 00000800h.
#symbol(const,type:int)OFN_EXPLORER      = 00080000h.
#symbol(const,type:int)OFN_LONGNAMES     = 00200000h.
#symbol(const,type:int)OFN_HIDEREADONLY  = 00000004h.

#symbol(const,type:int)DS_CENTER         = 0800h.

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
     
    #constructor new &hwnd:aParent &filters:Filters &filter_index:aFilterIndex &length:aMaxFile &wide:aCaption &int_attributes:Flags
    [
        #var(type:array) arr := Filters array.
        #var(type:int)   i := 0.
        #var(type:vint)  length := 0.
        #var(type:vint)  s_length := 0.
        #var(type:vint)  buf_size := 0.
        
        theParent := aParent.
        theFilterIndex := aFilterIndex int.
        
        theFileName := ShortArray new &length:(aMaxFile + 1).
        theFileName write &index:0 &short:0.
        
        theMaxFile := aMaxFile.
        theCaption := aCaption.
        theFlags := Flags.
        
        // calculate the filter buffer size
        arr readLength &vint:length.
        #loop (i < length)?
        [
            #var(type:wide)aFilter := arr getAt &index:i wide.
            aFilter readLength &vint:s_length.
            buf_size += s_length.
            buf_size += 1.
            
            i := i + 1.
        ].
        buf_size += 2.

        // fill the filter buffer        
        theFilters := ShortArray new &length:buf_size.
        i := 0.
        buf_size := 0.
        #loop (i < length)?
        [
            #var(type:wide)aFilter := arr getAt &index:i wide.
            aFilter readLength &vint:s_length.
            
            aFilter $save &index:buf_size &shortarray:theFilters &length:s_length.
            buf_size += s_length.
            
            theFilters write &index:buf_size &short:0.
            
            buf_size += 1.
            
            i := i + 1.
        ].
        theFilters write &index:buf_size &short:0.
        theFilters write &index:(buf_size + 1) &short:0.
    ]    
    
    #method $prepare &bytearray:aStruct
    [
        #var(type:dirty_ptr)ptr.
        #var(type:vint)     tmp.
        
        aStruct fill &index:0 &length:76 &byte:0.
        
//      lStructSize;
        aStruct write &index:0 &int:76.        
          
//      hwndOwner;
        theParent $save &dirty_ptr:ptr.
        ptr read &vint:tmp.
        aStruct write &index:4 &int:tmp.
          
//      hInstance;
        CurrentInstance $read &vint:tmp.
        aStruct write &index:8 &int:tmp.

//        lpstrFilter;
        ptr write &shortarray:theFilters.
        ptr read &vint:tmp.
        aStruct write &index:12 &int:tmp.

//        nFilterIndex;
        aStruct write &index:24 &int:theFilterIndex.

//        lpstrFile;
        ptr write &shortarray:theFileName.
        ptr read &vint:tmp.
        aStruct write &index:28 &int:tmp.

//        nMaxFile;
        aStruct write &index:32 &int:theMaxFile.

//        lpstrTitle;
        ptr write &wide:theCaption.
        ptr read &vint:tmp.
        aStruct write &index:48 &int:tmp.

//        Flags;
        aStruct write &index:52 &int:theFlags.
    ]
}

// --- OpenFileDialog ---

#class(sealed) OpenFileDialog :: BaseFileDialog
{
    #constructor new &hwnd:aParent &filters:Filters &filter_index:aFilterIndex &length:aMaxFile &wide:aCaption
        <= new &hwnd:aParent &filters:Filters &filter_index:aFilterIndex &length:aMaxFile &wide:aCaption 
            &int_attributes:(OFN_PATHMUSTEXIST or:OFN_EXPLORER or:OFN_LONGNAMES or:DS_CENTER or:OFN_HIDEREADONLY int).
    
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
                
                ^ WideLiteralValue $new &length:aLength &index:0 &shortarray:theFileName.
            ].
    ]
}

// --- SaveFileDialog ---

#class(sealed) SaveFileDialog :: BaseFileDialog
{
    #constructor new &hwnd:aParent &filters:Filters &filter_index:aFilterIndex &length:aMaxFile &wide:aCaption
        <= new &hwnd:aParent &filters:Filters &filter_index:aFilterIndex &length:aMaxFile &wide:aCaption 
            &int_attributes:(OFN_PATHMUSTEXIST or:OFN_EXPLORER or:OFN_LONGNAMES or:DS_CENTER or:OFN_HIDEREADONLY int).
    
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
                
                ^ WideLiteralValue $new &length:aLength &index:0 &shortarray:theFileName.
            ].        
    ]
}
