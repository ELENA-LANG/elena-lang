// ==== ver 3.4.1 ===

#import system.

#symbol(const,type:int)GWL_USERDATA            = -21.

// --- styles ---
#symbol(const,type:int)WS_OVERLAPPEDWINDOW     = 00CF0000h.
#symbol(const,type:int)WS_DIALOGWINDOW         = 00C80000h.
#symbol(const,type:int)WS_CHILD                = 40000000h.
#symbol(const,type:int)WS_VISIBLE              = 10000000h.
#symbol(const,type:int)WS_BORDER               = 00800000h.
#symbol(const,type:int)WS_TABSTOP              = 00010000h.
#symbol(const,type:int)WS_CLIPSIBLINGS         = 04000000h.
#symbol(const,type:int)WS_DLGFRAME             = 00400000h.
#symbol(const,type:int)WS_POPUPWINDOW          = 80880000h.
#symbol(const,type:int)WS_VSCROLL              = 00200000h.
#symbol(const,type:int)WS_HSCROLL              = 00100000h.

// --- ex styles ---
#symbol(const,type:int)WS_EX_DLGMODALFRAME     = 00000001h.
#symbol(const,type:int)WS_EX_OVERLAPPEDWINDOW  = 00000300h.
#symbol(const,type:int)EX_SDI_OVERLAPPEDWINDOW = 00040101h.
#symbol(const,type:int)WS_EX_CLIENTEDGE        = 00000200h.
#symbol(const,type:int)WS_EX_CONTROLPARENT     = 00010000h.
//#symbol(const,type:int)WS_EX_NOPARENTNOTIFY    = 00000004h.
#symbol(const,type:int)WS_EX_TRANSPARENT       = 00000020h.

// --- static styles ---
#symbol(const,type:int)SS_SIMPLE               = 0000000Bh.

// --- button styles ---

#symbol(const,type:int)BS_PUSHBUTTON           = 0. 
#symbol(const,type:int)BS_DEFPUSHBUTTON        = 1. 
#symbol(const,type:int)BS_AUTOCHECKBOX         = 3.
#symbol(const,type:int)BS_GROUPBOX             = 7.
#symbol(const,type:int)BS_AUTORADIOBUTTON      = 9.

#symbol(const,type:int)BST_CHECKED             = 1.
#symbol(const,type:int)BST_UNCHECKED           = 0.

// --- combobox styles ---
#symbol(const,type:int)CBS_DROPDOWNLIST        = 3.
#symbol(const,type:int)CBS_DROPDOWN            = 2.

#symbol(const,type:int)ES_MULTILINE         = 0004h.
#symbol(const,type:int)ES_AUTOVSCROLL       = 0040h.
#symbol(const,type:int)ES_AUTOHSCROLL       = 0080h.
#symbol(const,type:int)ES_WANTRETURN        = 1000h.

// --- messages ---
#symbol(const,type:int)WM_DESTROY       = 02h.
#symbol(const,type:int)WM_SIZE          = 05h.
#symbol(const,type:int)WM_SETTEXT       = 0Ch.
#symbol(const,type:int)WM_GETTEXT       = 0Dh.
#symbol(const,type:int)WM_GETTEXTLENGTH = 0Eh.
#symbol(const,type:int)WM_PAINT         = 0Fh.
#symbol(const,type:int)WM_CLOSE         = 10h.
#symbol(const,type:int)WM_SETCURSOR     = 20h.

#symbol(const,type:int)SWP_NOSIZE       = 0001h.
#symbol(const,type:int)SWP_NOMOVE       = 0002h.

#symbol(const,type:int)BM_SETCHECK      = 00F1h.
#symbol(const,type:int)BM_GETCHECK      = 00F0h.

#symbol(const,type:int)CB_ADDSTRING     = 0143h.

// --- Notifications ---
#symbol(const,type:int)BN_CLICKED       = 0000h.
#symbol(const,type:int)EN_CHANGE        = 0300h.

//#symbol(const,type:int)SW_SHOW          = 5.

// --- Window Color Constants

#symbol(const,type:int)COLOR_BACKGROUND = 5.

// --- Class Styles ---

#symbol(const,type:int)WS_ELENAWINDOW   = 0100Bh.
#symbol(const,type:int)CS_VREDRAW       = 00001h.
#symbol(const,type:int)CS_HREDRAW       = 00002h.

#symbol(const,type:int)HTCLIENT         = 1.

#symbol(const,type:int)IDC_ARROW        = 7F00h.

#symbol(const,type:int)MF_STRING        = 00000000h.
#symbol(const,type:int)MF_POPUP         = 00000010h.

// --- message box flags ---
#symbol(const,type:int)MB_ICONWARNING     = 00000030h.
#symbol(const,type:int)MB_ICONEXCLAMATION = 00000030h.
#symbol(const,type:int)MB_ICONINFORMATION = 00000040h.
#symbol(const,type:int)MB_ICONASTERISK    = 00000040h.
#symbol(const,type:int)MB_ICONQUESTION    = 00000020h.
#symbol(const,type:int)MB_ICONSTOP        = 00000010h.
#symbol(const,type:int)MB_ICONERROR       = 00000010h.
#symbol(const,type:int)MB_ICONHAND        = 00000010h.

// --- message box buttons ---
#symbol(const,type:int)MB_ABORTRETRYIGNORE  = 00000002h.
#symbol(const,type:int)MB_CANCELTRYCONTINUE = 00000006h.
//#symbol(const,type:int)MB_HELP              = 00000003h.
#symbol(const,type:int)MB_OK                = 00000000h.
#symbol(const,type:int)MB_OKCANCEL          = 00000001h.
#symbol(const,type:int)MB_RETRYCANCEL       = 00000005h.
#symbol(const,type:int)MB_YESNO             = 00000004h.
#symbol(const,type:int)MB_YESNOCANCEL       = 00000003h.

#symbol(const,type:int)IDNO                 = 7.
#symbol(const,type:int)IDOK                 = 1.
#symbol(const,type:int)IDRETRY              = 4.
#symbol(const,type:int)IDTRYAGAIN           = 10.
#symbol(const,type:int)IDYES                = 6.
