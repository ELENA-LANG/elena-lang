#import system.
#import system'text.

// --- File ---

#class File
{
    #field(file_path) thePath.
    
    #constructor new &path:aPath
    [
        thePath := aPath.
    ]

    #method file_path = thePath.

    #method literal = thePath literal.
    
    #method wide = thePath wide.
    
    #method textreader = thePath::fileExtOp textreader.
    
    #method textwriter = thePath::fileExtOp textwriter.
    
    #method(bool)is &available
        = thePath::fileExtOp is &available.
    
    #method(literal) literalContent
        = thePath::fileOp literalContent.
    
    #method(wide) wideContent
        = thePath::fileOp wideContent.
    
    #method(int)length
        = thePath::fileOp length.
}

// --- fileOp ---

#class(extension:file_path)fileOp
{
    #method read &textbuffer:anOutput
    [
        #var(textreader)aReader := self::fileExtOp textreader.
        #var(char:128)buffer.
        #var(int)read.
        #loop (aReader available)?
        [
            read := 128.
            aReader read &chararray:buffer &vint:read.
            
            anOutput write &chararray:buffer &int:read.
        ].
        
        aReader close.
    ]
    
    #method(literal) literalContent
    [
        #var(textbuffer)anOutput := TextBuilder new.
        
        self::fileOp read &textbuffer:anOutput.
        
        ^ anOutput literal.
    ]
    
    #method(wide) wideContent
    [
        #var(textbuffer)anOutput := TextBuilder new.
        
        self::fileOp read &textbuffer:anOutput.
        
        ^ anOutput wide.
    ]
    
    #method(int,w2)length
    [
        #var(stream)stream := FileStream openForRead &path:self.
        #var(int) fileLen := stream length.        
        
        stream close.
        
        ^ fileLen.
    ]    
}