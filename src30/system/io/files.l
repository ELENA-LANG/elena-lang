#define system.

// --- File Access Modes ---
#symbol(const) $FILE_READ_DATA     = 00000001h.
#symbol(const) $FILE_APPEND_DATA   = 00000004h.
#symbol(const) $FILE_GENERIC_READ  = 80000000h.
#symbol(const) $FILE_GENERIC_WRITE = 40000000h.

#symbol(const) $OPEN_EXISTING      = 0003h.
#symbol(const) $CREATE_ALWAYS      = 0002h.
#symbol(const) $OPEN_ALWAYS        = 0004h.

#symbol(const) $FILE_SHARE_READ    = 00000001h.
#symbol(const) $FILE_SHARE_WRITE   = 00000002h.

// --- IOException ---

#class IOException : Exception
{
}

// --- FileStream ---

#class FileStream : Handle
{
    #constructor new &literal:aPath &int:dwDesiredAccess &int:dwShareMode &int:dwCreationDisposition &int:dwFlagsAndAttributes
    [
        theData := system'external'KERNEL32 CreateFileW
                      &literal:aPath
                      &int:dwDesiredAccess
                      &int:dwShareMode
                      &int:0
                      &int:dwCreationDisposition
                      &int:dwFlagsAndAttributes
                      &int:0.
                      
        (-1 == theData)
            ? [ #throw IOException new:"Cannot open the file". ].
    ]
    
    #method write &bytearray:aDump &length:aLength
    [
        system'external'KERNEL32 WriteFile
            &handle    : $self
            &bytearray : aDump
            &int       : aLength
            &int       : 0
            &int       : 0.
    ]

    #method read &bytearray:aDump &out'length:aLength
    [
        system'external'KERNEL32 ReadFile
            &handle      : $self
            &bytearray   : aDump
            &int         : aLength
            &out'length  : aLength
            &int         : 0.
    ]
}

// --- fileControl ---

#symbol fileControl =
{
    openForRead &path:aPath
        = FileStream new &literal::aPath &int:$FILE_GENERIC_READ &int:$FILE_SHARE_READ &int:$OPEN_EXISTING &int:0.
}.