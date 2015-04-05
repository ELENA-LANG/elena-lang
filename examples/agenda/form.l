#define system.
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

#class Window :: SDIDialog
{
////    // All Text Box's
////    #field Name.
////    #field SurName.
////    #field OtName.
////    #field Email.
////    #field Age.
////    #field PhoneNumber.
////    
////    // All Labels
////    #field LabelName.
////    #field LabelSurName.
////    #field LabelOtName.
////    #field LabelEmail.
////    #field LabelAge.
////    #field LabelPhoneNumber.
////    #field LabelAbout.
////    #field LabelDel.
////
////    // All Buttons
////    #field BtmExit.
////    #field BtmSave.
////    #field BtmDelTxt.
////    #field BtmInfo.
////
////    #field theSavedPath.
////
////    #method initializing : Args
////    [
////        super initializing:Args.
////
////        // --- Create controls the Labels ---
////        LabelName := controls'StaticLabel::self.
////        LabelSurName := controls'StaticLabel::self.
////        LabelOtName := controls'StaticLabel::self.
////        LabelEmail := controls'StaticLabel::self.
////        LabelAge := controls'StaticLabel::self.
////        LabelPhoneNumber := controls'StaticLabel::self.
////        LabelAbout := controls'StaticLabel::self.
////        LabelDel := controls'StaticLabel::self.
////
////        // --- Create the Text Box controls ---
////        Name := controls'Edit::self.
////        SurName := controls'Edit::self.
////        OtName := controls'Edit::self.
////        Email := controls'Edit::self.
////        Age := controls'Edit::self.
////        PhoneNumber := controls'Edit::self.
////
////        // --- Create controls of the Buttons ---
////        BtmExit := controls'Button::self.
////        BtmSave := controls'Button::self.
////        BtmDelTxt := controls'Button::self.
////        BtmInfo := controls'Button::self.
////
////        // -------------------------------------------------------------------------------
////        
////        // Form
////        self
////            dimensions'set &width:500 &height:265
////            coordinates'set &x:350 &y:250
////            caption'set:"Writer Account".
////
////        // ------------------------------------ Labels ------------------------------------
////        // LabelName
////        Labelname
////            dimensions'set &width:70 &height:30
////            coordinates'set &x:12 &y:10
////            caption'set:"Name:"
////            open.
////        
////        // LabelSurName
////        LabelSurName
////            dimensions'set &width:109 &height:20
////            coordinates'set &x:12 &y:40
////            caption'set:"Your Last Name:"
////            open.
////
////        // LabelOtName
////        LabelOtName
////            dimensions'set &width:89 &height:20
////            coordinates'set &x:12 &y:65
////            caption'set:"Other Names:"
////            open.
////
////        // LabelEmail
////        LabelEmail
////            dimensions'set &width:40 &height:20
////            coordinates'set &x:12 &y:90
////            caption'set:"Email:"
////            open.
////
////        // LabelAge
////        LabelAge
////            dimensions'set &width:27 &height:20
////            coordinates'set &x:12 &y:115
////            caption'set:"Age:"
////            open.
////
////        // LabelPhoneNumber
////        LabelPhoneNumber
////            dimensions'set &width:100 &height:20
////            coordinates'set &x:12 &y:140
////            caption'set:"Phone Number:"
////            open.
////
////        // LabelAbout
////        LabelAbout
////            dimensions'set &width:250 &height:35
////            coordinates'set &x:12 &y:165
////            open.
////        
////        // LabelDel
////        LabelDel
////            dimensions'set &width:250 &height:35
////            coordinates'set &x:300 &y:165
////            open.
////
////        // ------------------------------------ Text Box's ------------------------------------
////        // Name
////        Name << "Your name".
////        Name
////            dimensions'set &width:200 &height:25
////            coordinates'set &x:125 &y:10
////            open.
////
////        // SurName
////        SurName << "Your Last Name".
////        SurName
////            dimensions'set &width:200 &height:25
////            coordinates'set &x:125 &y:35
////            open.
////
////        // OtName
////        OtName << "Other Name".
////        OtName
////            dimensions'set &width:200 &height:25
////            coordinates'set &x:125 &y:60
////            open.
////
////        // Email
////        Email << "Email".
////        Email
////            dimensions'set &width:200 &height:25
////            coordinates'set &x:125 &y:85
////            open.
////
////        // Age
////        Age << "Age".
////        Age
////            dimensions'set &width:200 &height:25
////            coordinates'set &x:125 &y:110
////            open.
////
////        // PhoneNumber
////        PhoneNumber << "Phone Number".
////        PhoneNumber
////            dimensions'set &width:200 &height:25
////            coordinates'set &x:125 &y:135
////            open.
////
////        // ------------------------------------ Buttons ------------------------------------
////        // BtmSave
////        BtmSave
////            dimensions'set &width:65 &height:20
////            coordinates'set &x:410 &y:20
////            caption'set:"Save"
////            open.
////
////        // BtmDelTxt
////        BtmDelTxt
////            dimensions'set &width:65 &height:20
////            coordinates'set &x:410 &y:50
////            caption'set:"Del TXT"
////            enabled'set:false
////            open.
////
////        // BtmInfo
////        BtmInfo
////            dimensions'set &width:65 &height:20
////            coordinates'set &x:410 &y:80
////            caption'set:"About"
////            open.
////
////        // BtmExit
////        BtmExit
////            dimensions'set &width:65 &height:20
////            coordinates'set &x:410 &y:110
////            caption'set:"Exit"
////            open.
////
////        // ------------------------------------ Handlers  ------------------------------------    
////        // Exit
////        BtmExit event_handle'append:
////        {
////            clicking : Args [ 'program stop. ]
////        }.
////
////        // Save
////        BtmSave event_handle'append:
////        {
////            clicking : Args
////            [
////                theSavedPath := OpenFile::self.
////
////                #var Writer := io'LogWriter::theSavedPath.
////                Writer write:"//-------------------------------------------------------------------------------%n". 
////                Writer write:"%r%nName: " write:Name write:" " write:SurName write:" " write:OtName.
////                Writer write:"%r%nEmail: " write:Email.
////                Writer write:"%r%nAge: " write:Age.
////                Writer write:"%r%nPhoneNumber: " write:PhoneNumber.
////                Writer write:"%r%n//-------------------------------------------------------------------------------%n".
////
////                Writer free.
////                
////                BtmDelTxt enabled'set:true.
////            ]
////        }.
////
////        // Del TXT
////        BtmDelTxt event_handle'append:
////        {
////            clicking : Args
////            [     
////                'io'FileManager delete &io'path:theSavedPath.
////                
////                BtmDelTxt enabled'set:false.
////            ]
////        }.
////
////        // About
////        BtmInfo event_handle'append:
////        {
////            clicking : Args
////            [
////                LabelAbout caption'set:"Created by Alexandre Bencz.%nThanks Alex Rakov, for support.".
////            ]
////        }.
////    ]
}