#define system.

// --- File Access Modes ---
#symbol $FILE_READ_DATA     = 00000001h.
#symbol $FILE_APPEND_DATA   = 00000004h.
#symbol $FILE_GENERIC_READ  = 80000000h.
#symbol $FILE_GENERIC_WRITE = 40000000h.

#symbol $OPEN_EXISTING      = 0003h.
#symbol $CREATE_ALWAYS      = 0002h.
#symbol $OPEN_ALWAYS        = 0004h.

#symbol $FILE_SHARE_READ    = 00000001h.
#symbol $FILE_SHARE_WRITE   = 00000002h.

// --- IOException ---

#class IOException : Exception
{
}

// --- FileStream ---

#class FileStream : system'Handler
{
    #constructor new &type'widestr:aPath &type'int:dwDesiredAccess &type'int:dwShareMode &type'int:dwCreationDisposition &type'int:dwFlagsAndAttributes
    [
        theData << system'external'KERNEL32 CreateFileW
                      &type'widestr:aPath
                      &type'int:dwDesiredAccess
                      &type'int:dwShareMode
                      &type'int:0
                      &type'int:dwCreationDisposition
                      &type'int:dwFlagsAndAttributes
                      &type'int:0.
                      
        (-1 == theData)
            ? [ #throw IOException new:"Cannot open the file". ].
    ]
    
    #method write &type'bytearray:aDump &type'length:aLength
    [
        system'external'KERNEL32 WriteFile
            &type'handle    : $self
            &type'bytearray : aDump
            &type'int       : aLength
            &out'type'int   : nil
            &type'int       : 0.
    ]

    #method read &type'bytearray:aDump &type'length:aLength
    [
        system'external'KERNEL32 ReadFile
            &type'handle        : $self
            &out'type'bytearray : aDump
            &type'int           : aLength
            &out'type'length    : aDump
            &type'int           : 0.
    ]
}

// --- fileControl ---

#symbol fileControl =
{
    openForRead &path:aPath
        = FileStream new &type'widestr::aPath &type'int:$FILE_GENERIC_READ &type'int:$FILE_SHARE_READ &type'int:$OPEN_EXISTING &type'int:0.
}.