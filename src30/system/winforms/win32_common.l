#define system.

#symbol(const,type:int)GWL_USERDATA            = -21.

// --- styles ---
#symbol(const,type:int)WS_OVERLAPPEDWINDOW     = 00CF0000h.
#symbol(const,type:int)WS_CHILD                = 40000000h.
#symbol(const,type:int)WS_VISIBLE              = 10000000h.
#symbol(const,type:int)WS_BORDER               = 00800000h.
#symbol(const,type:int)WS_TABSTOP              = 00010000h.
//#symbol(const,type:int)WS_CLIPSIBLINGS         = 04000000h.

// --- ex styles ---
#symbol(const,type:int)WS_EX_OVERLAPPEDWINDOW  = 00000300h.
#symbol(const,type:int)EX_SDI_OVERLAPPEDWINDOW = 00040101h.
#symbol(const,type:int)WS_EX_CLIENTEDGE        = 00000200h.
//#symbol(const,type:int)WS_EX_CONTROLPARENT     = 00010000h.
//#symbol(const,type:int)WS_EX_NOPARENTNOTIFY    = 00000004h.
//#symbol(const,type:int)WS_EX_TRANSPARENT       = 00000020h.

// --- static styles ---
#symbol(const,type:int)SS_SIMPLE               = 0000000Bh.

// --- button styles ---
#symbol(const,type:int)BS_DEFPUSHBUTTON        = 1. 
#symbol(const,type:int)BS_PUSHBUTTON           = 0. 

//// --- combobox styles ---
//#symbol(const,type:int)CBS_DROPDOWNLIST        = 3.
//#symbol(const,type:int)CBS_DROPDOWN            = 2.

// --- messages ---
#symbol(const,type:int)WM_DESTROY       = 02h.
#symbol(const,type:int)WM_SETTEXT       = 0Ch.
#symbol(const,type:int)WM_GETTEXT       = 0Dh.
#symbol(const,type:int)WM_GETTEXTLENGTH = 0Eh.
#symbol(const,type:int)WM_CLOSE         = 10h.
#symbol(const,type:int)WM_SETCURSOR     = 20h.

#symbol(const,type:int)SWP_NOSIZE       = 0001h.
#symbol(const,type:int)SWP_NOMOVE       = 0002h.

//// --- Notifications ---
//#symbol(const,type:int)BN_CLICKED       = 0.
//
//#symbol(const,type:int)SW_SHOW          = 5.

// --- Window Color Constants

#symbol(const,type:int)COLOR_BACKGROUND = 5.

// --- Class Styles ---

#symbol(const,type:int)WS_ELENAWINDOW   = 0100Bh.
//#symbol(const,type:int)CS_VREDRAW       = 00001h.
//#symbol(const,type:int)CS_HREDRAW       = 00002h.

#symbol(const,type:int)HTCLIENT         = 1.

#symbol(const,type:int)IDC_ARROW        = 7F00h.

// --- HINSTANCE ---

#static(type:hinstance) CurrentInstance = HInstance current.
