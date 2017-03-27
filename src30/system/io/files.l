#import system.
#import system'text.

// --- File ---

sealed class File
{
    file_path thePath.
    
    constructor new &file_path:aPath
    [
        thePath := aPath.
    ]
    
    constructor new &path:aPath
        <= new &file_path:aPath.

    file_path = thePath.

    literal = thePath literal.
    
    wide = thePath wide.
    
    textreader = thePath::fileExtOp textreader.
    
    textwriter = thePath::fileExtOp textwriter.
    
    bool is &available
        = thePath::fileExtOp is &available.
    
    literal literalContent
        = thePath::fileOp literalContent.
    
    wide wideContent
        = thePath::fileOp wideContent.
    
    int length
        = thePath::fileOp length.
}

// --- fileOp ---

extension(file_path) fileOp
{
    read &textbuffer:anOutput
    [
        textreader aReader := self::fileExtOp textreader.
        char(128)  buffer := 0.
        int        read := 0.
        while (aReader available)
        [
            read := 128.
            aReader read &chararray:buffer &vint:read.
            
            anOutput write &chararray:buffer &int:read.
        ].
        
        aReader close.
    ]
 
    saveContent:content
    [
        textwriter aWriter := self::fileExtOp textwriter.
        
        aWriter write &literal:(content literal).
        
        aWriter close.
    ]
 
    saveWideContent:content
    [
        textwriter aWriter := self::fileExtOp textwriter.
        
        aWriter write &wide:(content wide).
        
        aWriter close.
    ]
          
    literal literalContent
    [
        textbuffer anOutput := TextBuilder new.
        
        self::fileOp read &textbuffer:anOutput.
        
        ^ anOutput literal.
    ]
    
    wide wideContent
    [
        textbuffer anOutput := TextBuilder new.
        
        self::fileOp read &textbuffer:anOutput.
        
        ^ anOutput wide.
    ]
    
    int length
    [
        stream stream := FileStream openForRead &path:self.
        int    fileLen := stream length.        
        
        stream close.
        
        ^ fileLen.
    ]
    
    file = File new &file_path:self.
}