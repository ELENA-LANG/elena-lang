import extensions;

packed tagOFNA
{
   int         lStructSize;
   Handle      hwndOwner;
   handle      hInstance;
   pointer     lpstrFilter;
   pointer     lpstrCustomFilter;
   int         nMaxCustFilter;
   int         nFilterIndex;
   pointer     lpstrFile;
   int         nMaxFile;
   pointer     lpstrFileTitle;
   int         nMaxFileTitle;
   pointer     lpstrInitialDir;
   pointer     lpstrTitle;
   int         Flags;
   short       nFileOffset;
   short       nFileExtension;
   pointer     lpstrDefExt;
   int         lCustData;
   pointer     lpfnHook;
   pointer     lpTemplateName;
   pointer     pvReserved;
   int         dwReserved;
   int         FlagsEx;
}

public program()
{
   tagOFNA st;
   int size := $size st;
}
