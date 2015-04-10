#define system.
#define system'winforms.
////#define std'patterns'*.
//#define forms.
//#define controls'* = gui'controls'*.
//#define io'*       = sys'io'*.
////#define gui'common'*.
//#define gui'dictionary'*.
//#define gui'dialogs'*.
//
//// --- OpenFile ---
//
//#symbol OpenFile : aParent =
//[
//    #var aDialog := SaveFileDialog &&control_parent:aParent &caption:"Save Agenda" &:'dialogs'styles'AFileDialog.
//    
//    aDialog filter'write &caption:"Text Files" &io'file_mask:"*.txt".
//
//    ^ aDialog forms'modal'open io'path.
//].

// --- Main window ---

#class Window
{
    #field theHandle.
    
    // All Text Box's
    #field Name.
    #field SurName.
    #field OtName.
    #field Email.
    #field Age.
    #field PhoneNumber.
    
    // All Labels
    #field LabelName.
    #field LabelSurName.
    #field LabelOtName.
    #field LabelEmail.
    #field LabelAge.
    #field LabelPhoneNumber.
    #field LabelAbout.
    #field LabelDel.

    // All Buttons
    #field BtmExit.
    #field BtmSave.
    #field BtmDelTxt.
    #field BtmInfo.

//    #field theSavedPath.

    #constructor new
    [
        // Form
        theHandle := SDIWindowHandle new &wide:"Writer Account" &int:350 &int:250 &int:500 &int:265.
        
        // ------------------------------------ Labels ------------------------------------
        // LabelName
        LabelName := StaticLabelHandle new &handle:theHandle &wide:"Name" &int:12 &int:10 &int:70 &int:30.

        // LabelSurName
        LabelSurName := StaticLabelHandle new &handle:theHandle &wide:"Your Last Name:" &int:12 &int:40 &int:109 &int:20.

        // LabelOtName
        LabelOtName := StaticLabelHandle new &handle:theHandle &wide:"Other Names:" &int:12 &int:65 &int:89 &int:20.

        // LabelEmail
        LabelEmail := StaticLabelHandle new &handle:theHandle &wide:"Email:" &int:12 &int:90 &int:40 &int:20.

        // LabelAge
        LabelAge := StaticLabelHandle new &handle:theHandle &wide:"Age:" &int:12 &int:115 &int:27 &int:20.

        // LabelPhoneNumber
        LabelPhoneNumber := StaticLabelHandle new &handle:theHandle &wide:"Phone Number:" &int:12 &int:140 &int:100 &int:20.

        // LabelAbout
        LabelAbout := StaticLabelHandle new &handle:theHandle &wide:emptyWideLiteralValue &int:12 &int:165 &int:250 &int:35.
        
        // LabelDel
        LabelDel := StaticLabelHandle new &handle:theHandle &wide:emptyWideLiteralValue &int:300 &int:165 &int:250 &int:35.

        // ------------------------------------ Text Box's ------------------------------------
        // Name
        Name := EditHandle new &handle:theHandle &wide:"Your name" &int:125 &int:10 &int:200 &int:25.

        // SurName
        SurName := EditHandle new &handle:theHandle &wide:"Your Last Name" &int:125 &int:35 &int:200 &int:25.

        // OtName
        OtName := EditHandle new &handle:theHandle &wide:"Other Name" &int:125 &int:60 &int:200 &int:25.

        // Email
        Email := EditHandle new &handle:theHandle &wide:"Email" &int:125 &int:85 &int:200 &int:25.

        // Age
        Age := EditHandle new &handle:theHandle &wide:"Age" &int:125 &int:110 &int:200 &int:25.

        // PhoneNumber
        PhoneNumber := EditHandle new &handle:theHandle &wide:"Phone Number" &int:125 &int:135 &int:200 &int:25.

        // ------------------------------------ Buttons ------------------------------------
        // BtmSave
        BtmSave := ButtonHandle new &handle:theHandle &wide:"Save" &int:390 &int:10 &int:85 &int:25.

        // BtmDelTxt
        BtmDelTxt := ButtonHandle new &handle:theHandle &wide:"Del TXT" &int:390 &int:50 &int:85 &int:25.
//            enabled'set:false

        // BtmInfo
        BtmInfo := ButtonHandle new &handle:theHandle &wide:"About" &int:390 &int:80 &int:85 &int:25.

        // BtmExit
        BtmExit := ButtonHandle new &handle:theHandle &wide:"Exit" &int:390 &int:110 &int:85 &int:25.

        // ------------------------------------ Handlers  ------------------------------------    
        theHandle set &win_listener:WindowListener
        {
            onclick &handle:aControl &vint:aResult
            [
                // Exit
                (aControl == BtmExit handle)
                    ? [ 'program stop. ].
            ]
        }.
        
//        // Save
//        BtmSave event_handle'append:
//        {
//            clicking : Args
//            [
//                theSavedPath := OpenFile::self.
//
//                #var Writer := io'LogWriter::theSavedPath.
//                Writer write:"//-------------------------------------------------------------------------------%n". 
//                Writer write:"%r%nName: " write:Name write:" " write:SurName write:" " write:OtName.
//                Writer write:"%r%nEmail: " write:Email.
//                Writer write:"%r%nAge: " write:Age.
//                Writer write:"%r%nPhoneNumber: " write:PhoneNumber.
//                Writer write:"%r%n//-------------------------------------------------------------------------------%n".
//
//                Writer free.
//                
//                BtmDelTxt enabled'set:true.
//            ]
//        }.
//
//        // Del TXT
//        BtmDelTxt event_handle'append:
//        {
//            clicking : Args
//            [     
//                'io'FileManager delete &io'path:theSavedPath.
//                
//                BtmDelTxt enabled'set:false.
//            ]
//        }.
//
//        // About
//        BtmInfo event_handle'append:
//        {
//            clicking : Args
//            [
//                LabelAbout caption'set:"Created by Alexandre Bencz.%nThanks Alex Rakov, for support.".
//            ]
//        }.
    ]
    
    #method => theHandle.
}