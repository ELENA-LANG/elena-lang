public const int GWL_USERDATA            = -21;

public const int SWP_NOSIZE       = 0001h;
public const int SWP_NOMOVE       = 0002h;

// --- styles ---
public const int WS_OVERLAPPEDWINDOW     = 00CF0000h;
public const int WS_DIALOGWINDOW         = 00C80000h;
public const int WS_CHILD                = 40000000h;
public const int WS_VISIBLE              = 10000000h;
public const int WS_BORDER               = 00800000h;
public const int WS_TABSTOP              = 00010000h;
public const int WS_CLIPSIBLINGS         = 04000000h;
public const int WS_DLGFRAME             = 00400000h;
public const int WS_POPUPWINDOW          = 80880000h;
public const int WS_VSCROLL              = 00200000h;
public const int WS_HSCROLL              = 00100000h;

// --- ex styles ---
public const int WS_EX_DLGMODALFRAME     = 00000001h;
public const int WS_EX_OVERLAPPEDWINDOW  = 00000300h;
public const int EX_SDI_OVERLAPPEDWINDOW = 00040101h;
public const int WS_EX_CLIENTEDGE        = 00000200h;
public const int WS_EX_CONTROLPARENT     = 00010000h;
public const int WS_EX_NOPARENTNOTIFY    = 00000004h;
public const int WS_EX_TRANSPARENT       = 00000020h;

// --- message box flags ---
public const int MB_ICONWARNING     = 00000030h;
public const int MB_ICONEXCLAMATION = 00000030h;
public const int MB_ICONINFORMATION = 00000040h;
public const int MB_ICONASTERISK    = 00000040h;
public const int MB_ICONQUESTION    = 00000020h;
public const int MB_ICONSTOP        = 00000010h;
public const int MB_ICONERROR       = 00000010h;
public const int MB_ICONHAND        = 00000010h;

// --- message box buttons ---
public const int MB_ABORTRETRYIGNORE  = 00000002h;
public const int MB_CANCELTRYCONTINUE = 00000006h;
public const int MB_HELP              = 00000003h;
public const int MB_OK                = 00000000h;
public const int MB_OKCANCEL          = 00000001h;
public const int MB_RETRYCANCEL       = 00000005h;
public const int MB_YESNO             = 00000004h;
public const int MB_YESNOCANCEL       = 00000003h;

public const int BM_SETCHECK          = 00F1h;
public const int BM_GETCHECK          = 00F0h;

public const int IDNO                 = 7;
public const int IDOK                 = 1;
public const int IDRETRY              = 4;
public const int IDTRYAGAIN           = 10;
public const int IDYES                = 6;

// --- static styles ---
public const int SS_SIMPLE            = 0000000Bh;

// --- button styles ---

public const int BS_PUSHBUTTON        = 0; 
public const int BS_DEFPUSHBUTTON     = 1; 
public const int BS_AUTOCHECKBOX      = 3;
public const int BS_GROUPBOX          = 7;
public const int BS_AUTORADIOBUTTON   = 9;

public const int BST_CHECKED          = 1;
public const int BST_UNCHECKED        = 0;

// --- Notifications ---
public const int BN_CLICKED       = 0000h;
public const int EN_CHANGE        = 0300h;

// --- Window Color Constants

public const int COLOR_BACKGROUND = 5;

// --- Class Styles ---

public const int WS_ELENAWINDOW   = 0100Bh;
public const int CS_VREDRAW       = 00001h;
public const int CS_HREDRAW       = 00002h;

public const int HTCLIENT         = 1;

public const int IDC_ARROW        = 7F00h;

public const int MF_STRING        = 00000000h;
public const int MF_POPUP         = 00000010h;

// --- messages ---
public const int WM_DESTROY       = 02h;
public const int WM_SETTEXT       = 0Ch;
public const int WM_GETTEXT       = 0Dh;
public const int WM_GETTEXTLENGTH = 0Eh;
public const int WM_PAINT         = 0Fh;
public const int WM_CLOSE         = 10h;

public const int OFN_PATHMUSTEXIST = 00000800h;
public const int OFN_EXPLORER      = 00080000h;
public const int OFN_LONGNAMES     = 00200000h;
public const int OFN_HIDEREADONLY  = 00000004h;

public const int DS_CENTER         = 0800h;