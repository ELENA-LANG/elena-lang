#import system.

#symbol(const,int)GWL_USERDATA            = -21.

// --- styles ---
#symbol(const,int)WS_OVERLAPPEDWINDOW     = 00CF0000h.
#symbol(const,int)WS_DIALOGWINDOW         = 00C80000h.
#symbol(const,int)WS_CHILD                = 40000000h.
#symbol(const,int)WS_VISIBLE              = 10000000h.
#symbol(const,int)WS_BORDER               = 00800000h.
#symbol(const,int)WS_TABSTOP              = 00010000h.
#symbol(const,int)WS_CLIPSIBLINGS         = 04000000h.
#symbol(const,int)WS_DLGFRAME             = 00400000h.
#symbol(const,int)WS_POPUPWINDOW          = 80880000h.
#symbol(const,int)WS_VSCROLL              = 00200000h.
#symbol(const,int)WS_HSCROLL              = 00100000h.

// --- ex styles ---
#symbol(const,int)WS_EX_DLGMODALFRAME     = 00000001h.
#symbol(const,int)WS_EX_OVERLAPPEDWINDOW  = 00000300h.
#symbol(const,int)EX_SDI_OVERLAPPEDWINDOW = 00040101h.
#symbol(const,int)WS_EX_CLIENTEDGE        = 00000200h.
#symbol(const,int)WS_EX_CONTROLPARENT     = 00010000h.
//#symbol(const,int)WS_EX_NOPARENTNOTIFY    = 00000004h.
#symbol(const,int)WS_EX_TRANSPARENT       = 00000020h.

// --- static styles ---
#symbol(const,int)SS_SIMPLE               = 0000000Bh.

// --- button styles ---

#symbol(const,int)BS_PUSHBUTTON           = 0. 
#symbol(const,int)BS_DEFPUSHBUTTON        = 1. 
#symbol(const,int)BS_AUTOCHECKBOX         = 3.
#symbol(const,int)BS_GROUPBOX             = 7.
#symbol(const,int)BS_AUTORADIOBUTTON      = 9.

#symbol(const,int)BST_CHECKED             = 1.
#symbol(const,int)BST_UNCHECKED           = 0.

// --- combobox styles ---
#symbol(const,int)CBS_DROPDOWNLIST        = 3.
#symbol(const,int)CBS_DROPDOWN            = 2.

#symbol(const,int)ES_MULTILINE         = 0004h.
#symbol(const,int)ES_AUTOVSCROLL       = 0040h.
#symbol(const,int)ES_AUTOHSCROLL       = 0080h.
#symbol(const,int)ES_WANTRETURN        = 1000h.

// --- messages ---
#symbol(const,int)WM_DESTROY       = 02h.
#symbol(const,int)WM_SIZE          = 05h.
#symbol(const,int)WM_SETTEXT       = 0Ch.
#symbol(const,int)WM_GETTEXT       = 0Dh.
#symbol(const,int)WM_GETTEXTLENGTH = 0Eh.
#symbol(const,int)WM_PAINT         = 0Fh.
#symbol(const,int)WM_CLOSE         = 10h.
#symbol(const,int)WM_SETCURSOR     = 20h.

#symbol(const,int)SWP_NOSIZE       = 0001h.
#symbol(const,int)SWP_NOMOVE       = 0002h.

#symbol(const,int)BM_SETCHECK      = 00F1h.
#symbol(const,int)BM_GETCHECK      = 00F0h.

#symbol(const,int)CB_ADDSTRING     = 0143h.
#symbol(const,int)CB_GETCURSEL     = 0147h.
#symbol(const,int)CB_SETCURSEL     = 014Eh.
#symbol(const,int)CB_GETITEMDATA   = 0150h.

// --- Notifications ---
#symbol(const,int)BN_CLICKED       = 0000h.
#symbol(const,int)EN_CHANGE        = 0300h.

#symbol(const,int)CBN_SELCHANGE    = 0001h.

//#symbol(const,int)SW_SHOW          = 5.

// --- Window Color Constants

#symbol(const,int)COLOR_BACKGROUND = 5.

// --- Class Styles ---

#symbol(const,int)WS_ELENAWINDOW   = 0100Bh.
#symbol(const,int)CS_VREDRAW       = 00001h.
#symbol(const,int)CS_HREDRAW       = 00002h.

#symbol(const,int)HTCLIENT         = 1.

#symbol(const,int)IDC_ARROW        = 7F00h.

#symbol(const,int)MF_STRING        = 00000000h.
#symbol(const,int)MF_POPUP         = 00000010h.

// --- message box flags ---
#symbol(const,int)MB_ICONWARNING     = 00000030h.
#symbol(const,int)MB_ICONEXCLAMATION = 00000030h.
#symbol(const,int)MB_ICONINFORMATION = 00000040h.
#symbol(const,int)MB_ICONASTERISK    = 00000040h.
#symbol(const,int)MB_ICONQUESTION    = 00000020h.
#symbol(const,int)MB_ICONSTOP        = 00000010h.
#symbol(const,int)MB_ICONERROR       = 00000010h.
#symbol(const,int)MB_ICONHAND        = 00000010h.

// --- message box buttons ---
#symbol(const,int)MB_ABORTRETRYIGNORE  = 00000002h.
#symbol(const,int)MB_CANCELTRYCONTINUE = 00000006h.
//#symbol(const,int)MB_HELP              = 00000003h.
#symbol(const,int)MB_OK                = 00000000h.
#symbol(const,int)MB_OKCANCEL          = 00000001h.
#symbol(const,int)MB_RETRYCANCEL       = 00000005h.
#symbol(const,int)MB_YESNO             = 00000004h.
#symbol(const,int)MB_YESNOCANCEL       = 00000003h.

#symbol(const,int)IDNO                 = 7.
#symbol(const,int)IDOK                 = 1.
#symbol(const,int)IDRETRY              = 4.
#symbol(const,int)IDTRYAGAIN           = 10.
#symbol(const,int)IDYES                = 6.
