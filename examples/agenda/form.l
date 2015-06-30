#define system.
#define extensions.
#define forms.
#define system'io.

// --- Main window ---

#class Window :: SDIDialog
{
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

    #field(type:file) theSavedPath.

    #constructor new
        <= (new)
    [
        // Controls
        LabelName := Label new.
        LabelSurName := Label new.
        LabelOtName := Label new.
        LabelEmail := Label new.
        LabelAge := Label new.
        LabelPhoneNumber := Label new.
        LabelAbout := Label new.
        LabelDel := Label new.
        Name := Edit new.
        SurName := Edit new.
        OtName := Edit new.
        Email := Edit new.
        Age := Edit new.
        PhoneNumber := Edit new.
        BtmExit := Button new.
        BtmSave := Button new.
        BtmDelTxt := Button new.
        BtmInfo := Button new.
    
        theControls append:LabelName.
        theControls append:LabelSurName.
        theControls append:LabelOtName.
        theControls append:LabelEmail.
        theControls append:LabelAge.
        theControls append:LabelPhoneNumber.
        theControls append:LabelAbout.
        theControls append:LabelDel.
        theControls append:Name.
        theControls append:SurName.
        theControls append:OtName.
        theControls append:Email.
        theControls append:Age.
        theControls append:PhoneNumber.
        theControls append:BtmExit.
        theControls append:BtmSave.
        theControls append:BtmDelTxt.
        theControls append:BtmInfo.
        
        // Form
        $self set &caption:"Writer Account".
        $self set &x:350 &y:250.
        $self set &width:500 &height:265.
        
        // ------------------------------------ Labels ------------------------------------
        // LabelName        
        LabelName set &caption:"Your Name:".
        LabelName set &x:12 &y:10.
        LabelName set &width:109 &height:30.

        // LabelSurName
        LabelSurName set &caption:"Your Last Name:".
        LabelSurName set &x:12 &y:40.
        LabelSurName set &width:109 &height:20.

        // LabelOtName
        LabelOtName set &caption:"Other Names:".
        LabelOtName set &x:12 &y:65.
        LabelOtName set &width:109 &height:20.

        // LabelEmail
        LabelEmail set &caption:"Email:".
        LabelEmail set &x:12 &y:90.
        LabelEmail set &width:109 &height:20.

        // LabelAge
        LabelAge set &caption:"Age:".
        LabelAge set &x:12 &y:115.
        LabelAge set &width:109 &height:20.

        // LabelPhoneNumber
        LabelPhoneNumber set &caption:"Phone Number:".
        LabelPhoneNumber set &x:12 &y:140.
        LabelPhoneNumber set &width:109 &height:20.

        // LabelAbout
        LabelAbout set &caption:emptyWideLiteralValue.
        LabelAbout set &x:12 &y:165.
        LabelAbout set &width:400 &height:35.

        // LabelDel
        LabelDel set &caption:emptyWideLiteralValue.
        LabelDel set &x:300 &y:185.
        LabelDel set &width:400 &height:35.

        // ------------------------------------ Text Box's ------------------------------------
        // Name
        Name << "Your name".
        Name set &x:125 &y:10.
        Name set &width:200 &height:25.

        // SurName
        SurName << "Your Last Name".
        SurName set &x:125 &y:35.
        SurName set &width:200 &height:25.

        // OtName
        OtName << "Other Name".
        OtName set &x:125 &y:60.
        OtName set &width:200 &height:25.

        // Email
        Email << "Email".
        Email set &x:125 &y:85.
        Email set &width:200 &height:25.

        // Age
        Age << "Age".
        Age set &x:125 &y:110.
        Age set &width:200 &height:25.

        // PhoneNumber
        PhoneNumber << "Phone Number".
        PhoneNumber set &x:125 &y:135.
        PhoneNumber set &width:200 &height:25.

        // ------------------------------------ Buttons ------------------------------------
        // BtmSave
        BtmSave set &caption:"Save".
        BtmSave set &x:390 &y:10.
        BtmSave set &width:82 &height:25.

        // BtmDelTxt
        BtmDelTxt set &caption:"Del TXT".
        BtmDelTxt set &x:390 &y:50.
        BtmDelTxt set &width:82 &height:25.
        BtmDelTxt set &enabled:false.

        // BtmInfo
        BtmInfo set &caption:"About".
        BtmInfo set &x:390 &y:80.
        BtmInfo set &width:82 &height:25.

        // BtmExit
        BtmExit set &caption:"Exit".
        BtmExit set &x:390 &y:110.
        BtmExit set &width:82 &height:25.
        
        // ------------------------------------ Handlers  ------------------------------------
        // Save
        BtmSave set &onClick: args 
            [
                #var aDialog := SaveFileDialog new:$self.
                aDialog addFilter &caption:"Text Files" &extension:"*.txt".
                aDialog set &caption:"Select a file".
                
                (aDialog run) ?
                [
                    theSavedPath := File new &path:(aDialog selected).
                    
                    #var aWriter := theSavedPath logger.
                    
                    aWriter 
                        writeLine:"//-------------------------------------------------------------------------------"
                        writeLine:"Name: ":Name:" ":SurName:" ":OtName
                        writeLine:"Email: ":Email
                        writeLine:"Age: ":Age
                        writeLine:"PhoneNumber:":PhoneNumber
                        writeLine:"//-------------------------------------------------------------------------------".

                    aWriter close.

                    BtmDelTxt set &enabled:true.
                ].
            ].
        
        // Del TXT
        BtmDelTxt set &onClick: args 
            [
                theSavedPath delete.
            ].

        // Exit
        BtmExit set &onClick: args 
            [
                'program stop.
            ].
            
        // About
        BtmInfo set &onClick: args 
            [
                LabelAbout set &caption:"Created by Alexandre Bencz."#10"Thanks Alex Rakov, for support.".
            ].
    ]
}