//import system'text.

// --- File ---

public sealed /*nonstructural*/ class File
{
    string thePath;
    
    constructor assign(string path)
    {
        thePath := path
    }

//    cast T<literal> = thePath.
//
//    cast T<wide> = thePath.
//
//    T<literal> rprop path :: thePath.
//
//    T<literal> literal = thePath.
//    
//    T<TextReader> textreader = fileControl reader:thePath.
//    
//    T<TextWriter> textwriter = fileControl writer:thePath.
//    
//    T<TextWriter> logger = fileControl logger:thePath.
//    
//    bool isAvailable
//        = fileControl isAvailable:thePath.
//    
//    literal content
//    [
//        auto anOutput := TextBuilder new.
//        
//        self read(anOutput).
//        
//        ^ T<literal>(anOutput)
//    ]
//    
//    wide wideContent
//    [
//        auto anOutput := TextBuilder new.
//        
//        self read(anOutput).
//        
//        ^ T<wide>(anOutput).
//    ]
//    
//    int length
//    [
//        T<Stream> stream := FileStream openForRead:thePath.
//        int       fileLen := stream length.        
//        
//        stream close.
//        
//        ^ fileLen
//    ]
//
//    read(TextBuilder anOutput)
//    [
//        T<TextReader> aReader := fileControl reader:thePath.
//        V<char,128>   buffer.
//        int           read := 0.
//        while (aReader available)
//        [
//            read := 128.
//            aReader read(T<CharArray>(buffer), &read).
//            
//            anOutput write(T<CharArray>(buffer), read)
//        ].
//        
//        aReader close
//    ]
//    
//    saveContent(literal content)
//    [
//        T<TextWriter> aWriter := fileControl writer:thePath.
//        
//        aWriter write(content).
//        
//        aWriter close
//    ]    
//    
//    saveContent(wide content)
//    [
//        T<TextWriter> aWriter := fileControl writer:thePath.
//        
//        aWriter write(content).
//        
//        aWriter close
//    ]    
//    
//    delete
//        = fileControl delete:thePath.
}

//// --- Directory ---
//
//public sealed /*nonstructural */class Directory
//{
//    literal thePath.
//    
//    constructor new:aPath
//    [
//        thePath := aPath.
//    ]
//
//    T<literal> literal = thePath.
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