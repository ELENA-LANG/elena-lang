//import system'text.
//
// --- File ---

public sealed /*nonstructural*/ class File
{
    literal thePath.
    
    constructor new:aPath
    [
        thePath := aPath.
    ]

    T<literal> literal = thePath.
    
//    wide = thePath wide.
    
    T<TextReader> textreader = fileControl reader:thePath.
    
    T<TextWriter> textwriter = fileControl writer:thePath.
    
    T<TextWriter> logger = fileControl logger:thePath.
    
//    bool isAvailable
//        = fileControl isAvailable:thePath.
//    
//    literal content
//    [
//        textbuffer anOutput := TextBuilder new.
//        
//        $self read textbuffer:anOutput.
//        
//        ^ anOutput literal
//    ]
//    
//    wide wideContent
//    [
//        textbuffer anOutput := TextBuilder new.
//        
//        $self read textbuffer:anOutput.
//        
//        ^ anOutput wide.
//    ]
//    
//    int length
//    [
//        stream stream := FileStream openForRead:thePath.
//        int    fileLen := stream length.        
//        
//        stream close.
//        
//        ^ fileLen
//    ]
//
//    read textbuffer:anOutput
//    [
//        textreader aReader := fileControl reader:thePath.
//        char<128>  buffer.
//        int        read := 0.
//        while (aReader available)
//        [
//            read := 128.
//            aReader read chararray:buffer vint:read.
//            
//            anOutput write chararray:buffer int:read
//        ].
//        
//        aReader close
//    ]
//    
//    saveContent literal:content
//    [
//        textwriter aWriter := fileControl writer:thePath.
//        
//        aWriter write literal:content.
//        
//        aWriter close
//    ]    
//    
//    saveContent wide:content
//    [
//        textwriter aWriter := fileControl writer:thePath.
//        
//        aWriter write wide:content.
//        
//        aWriter close
//    ]    
//    
//    delete
//        = fileControl delete:thePath.
}

//// --- Directory ---
//
//sealed nonstructural class Directory
//{
//    literal thePath.
//    
//    constructor new:aPath
//    [
//        thePath := aPath.
//    ]
//
//    literal = thePath.
//    
//    wide = thePath wide.
//    
//    bool isAvailable
//        = directoryControl isAvailable:thePath.    
//        
//    create
//    [
//        directoryControl create:thePath.
//    ]     
//       
//    delete
//        = directoryControl delete:thePath.
//}