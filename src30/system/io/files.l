#define system.

// --- File ---

#class File
{
    #field thePath.
    
    #constructor new &path:aPath
    [
        thePath := aPath.
    ]

    #method file = $self.
            
    #method openForRead = thePath::fileOp openForRead.
    
    #method openForReWrite = thePath::fileOp openForReWrite.
    
    #method openForAppend = thePath::fileOp openForAppend.
    
    #method is &available
        = thePath::fileOp is &available.
    
    #method delete
        = thePath::fileOp delete.
    
    #method path = thePath.
    
    #method literal = thePath literal.
    
    #method wide = thePath wide.
}
