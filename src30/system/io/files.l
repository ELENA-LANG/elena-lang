// ==== ver 3.4.1 ===

#import system.
#import system'text.

// --- File ---

#class File
{
    #field thePath.
    
    #constructor new &path:aPath
    [
        thePath := aPath.
    ]

    #method file = $self.
    
    #method path = thePath.
    
    #method literal = thePath literal.
    
    #method wide = thePath wide.
    
    #method textreader = thePath::fileExtOp textreader.
    
    #method textwriter = thePath::fileExtOp textwriter.
}

// --- fileOp ---

#class(extension:file)fileOp
{
    #method read &textbuffer:anOutput
    [
        #var(type:textreader)aReader := self textreader.
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
}