// ==== ver 3.4.1 ===

#import system.
#import system'text.

// --- File ---

#class File
{
    #field(type:file_path) thePath.
    
    #constructor new &path:aPath
    [
        thePath := aPath.
    ]

    #method file_path = thePath.

    #method literal = thePath literal.
    
    #method wide = thePath wide.
    
    #method textreader = thePath::fileExtOp textreader.
    
    #method textwriter = thePath::fileExtOp textwriter.
    
    #method(type:bool)is &available
        = thePath::fileExtOp is &available.
    
    #method(type:literal) literalContent
        = thePath::fileOp literalContent.
    
    #method(type:wide) wideContent
        = thePath::fileOp wideContent.
    
    #method(type:int)length
        = thePath::fileOp length.
}

// --- fileOp ---

#class(extension:file_path)fileOp
{
    #method read &textbuffer:anOutput
    [
        #var(type:textreader)aReader := self::fileExtOp textreader.
        #var(type:chararray,size:128)buffer.
        #var(type:int)read.
        #loop (aReader available)?
        [
            read := 128.
            aReader read &chararray:buffer &vint:read.
            
            anOutput write &chararray:buffer &int:read.
        ].
        
        aReader close.
    ]
    
    #method(type:literal) literalContent
    [
        #var(type:textbuffer)anOutput := TextBuffer new.
        
        self::fileOp read &textbuffer:anOutput.
        
        ^ anOutput literal.
    ]
    
    #method(type:wide) wideContent
    [
        #var(type:textbuffer)anOutput := TextBuffer new.
        
        self::fileOp read &textbuffer:anOutput.
        
        ^ anOutput wide.
    ]
    
    #method(type:int,suppress:w2)length
    [
        #var(type:stream)stream := FileStream openForRead &path:self.
        #var(type:int) fileLen := stream length.        
        
        stream close.
        
        ^ fileLen.
    ]    
}