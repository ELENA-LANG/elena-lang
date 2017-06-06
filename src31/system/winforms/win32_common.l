const int GWL_USERDATA            = -21.

// --- styles ---
const int WS_OVERLAPPEDWINDOW     = 00CF0000h.
const int WS_DIALOGWINDOW         = 00C80000h.
const int WS_CHILD                = 40000000h.
const int WS_VISIBLE              = 10000000h.
const int WS_BORDER               = 00800000h.
const int WS_TABSTOP              = 00010000h.
const int WS_CLIPSIBLINGS         = 04000000h.
const int WS_DLGFRAME             = 00400000h.
const int WS_POPUPWINDOW          = 80880000h.
const int WS_VSCROLL              = 00200000h.
const int WS_HSCROLL              = 00100000h.

// --- ex styles ---
const int WS_EX_DLGMODALFRAME     = 00000001h.
const int WS_EX_OVERLAPPEDWINDOW  = 00000300h.
const int EX_SDI_OVERLAPPEDWINDOW = 00040101h.
const int WS_EX_CLIENTEDGE        = 00000200h.
const int WS_EX_CONTROLPARENT     = 00010000h.
//#symbol(const,int)WS_EX_NOPARENTNOTIFY    = 00000004h.
const int WS_EX_TRANSPARENT       = 00000020h.

// --- static styles ---
const int SS_SIMPLE               = 0000000Bh.

// --- button styles ---

const int BS_PUSHBUTTON           = 0. 
const int BS_DEFPUSHBUTTON        = 1. 
const int BS_AUTOCHECKBOX         = 3.
const int BS_GROUPBOX             = 7.
const int BS_AUTORADIOBUTTON      = 9.

const int BST_CHECKED             = 1.
const int BST_UNCHECKED           = 0.

// --- combobox styles ---
const int CBS_DROPDOWNLIST        = 3.
const int CBS_DROPDOWN            = 2.

const int ES_MULTILINE         = 0004h.
const int ES_AUTOVSCROLL       = 0040h.
const int ES_AUTOHSCROLL       = 0080h.
const int ES_WANTRETURN        = 1000h.

// --- messages ---
const int WM_DESTROY       = 02h.
const int WM_SIZE          = 05h.
const int WM_SETTEXT       = 0Ch.
const int WM_GETTEXT       = 0Dh.
const int WM_GETTEXTLENGTH = 0Eh.
const int WM_PAINT         = 0Fh.
const int WM_CLOSE         = 10h.
const int WM_SETCURSOR     = 20h.

const int SWP_NOSIZE       = 0001h.
const int SWP_NOMOVE       = 0002h.

const int BM_SETCHECK      = 00F1h.
const int BM_GETCHECK      = 00F0h.

const int CB_ADDSTRING     = 0143h.
const int CB_GETCURSEL     = 0147h.
const int CB_SETCURSEL     = 014Eh.
const int CB_GETITEMDATA   = 0150h.

// --- Notifications ---
const int BN_CLICKED       = 0000h.
const int EN_CHANGE        = 0300h.

const int CBN_SELCHANGE    = 0001h.

//#symbol(const,int)SW_SHOW          = 5.

// --- Window Color Constants

const int COLOR_BACKGROUND = 5.

// --- Class Styles ---

const int WS_ELENAWINDOW   = 0100Bh.
const int CS_VREDRAW       = 00001h.
const int CS_HREDRAW       = 00002h.

const int HTCLIENT         = 1.

const int IDC_ARROW        = 7F00h.

const int MF_STRING        = 00000000h.
const int MF_POPUP         = 00000010h.

// --- message box flags ---
const int MB_ICONWARNING     = 00000030h.
const int MB_ICONEXCLAMATION = 00000030h.
const int MB_ICONINFORMATION = 00000040h.
const int MB_ICONASTERISK    = 00000040h.
const int MB_ICONQUESTION    = 00000020h.
const int MB_ICONSTOP        = 00000010h.
const int MB_ICONERROR       = 00000010h.
const int MB_ICONHAND        = 00000010h.

// --- message box buttons ---
const int MB_ABORTRETRYIGNORE  = 00000002h.
const int MB_CANCELTRYCONTINUE = 00000006h.
//#symbol(const,int)MB_HELP              = 00000003h.
const int MB_OK                = 00000000h.
const int MB_OKCANCEL          = 00000001h.
const int MB_RETRYCANCEL       = 00000005h.
const int MB_YESNO             = 00000004h.
const int MB_YESNOCANCEL       = 00000003h.

const int IDNO                 = 7.
const int IDOK                 = 1.
const int IDRETRY              = 4.
const int IDTRYAGAIN           = 10.
const int IDYES                = 6.
