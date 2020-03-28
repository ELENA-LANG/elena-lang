import extensions;
import forms;
import system'io;

import system'drawing;

// --- Main window ---

public class MainWindow : SDIDialog
{
    // All Text Box's
    Edit Name;
    Edit SurName;
    Edit OtName;
    Edit Email;
    Edit Age;
    Edit PhoneNumber;
    
    // All Labels
    Label LabelName;
    Label LabelSurName;
    Label LabelOtName;
    Label LabelEmail;
    Label LabelAge;
    Label LabelPhoneNumber;
    Label LabelAbout;
    Label LabelDel;

    // All Buttons
    Button BtmExit;
    Button BtmSave;
    Button BtmDelTxt;
    Button BtmInfo;

    File   theSavedFile;

    constructor new()
        <= new()
    {
        // Controls
        LabelName := Label.new();
        LabelSurName := Label.new();
        LabelOtName := Label.new();
        LabelEmail := Label.new();
        LabelAge := Label.new();
        LabelPhoneNumber := Label.new();
        LabelAbout := Label.new();
        LabelDel := Label.new();
        Name := Edit.new();
        SurName := Edit.new();
        OtName := Edit.new();
        Email := Edit.new();
        Age := Edit.new();
        PhoneNumber := Edit.new();
        BtmExit := Button.new();
        BtmSave := Button.new();
        BtmDelTxt := Button.new();
        BtmInfo := Button.new();
    
        self
            .appendControl:LabelName
            .appendControl:LabelSurName
            .appendControl:LabelOtName
            .appendControl:LabelEmail
            .appendControl:LabelAge
            .appendControl:LabelPhoneNumber
            .appendControl:LabelAbout
            .appendControl:LabelDel
            .appendControl:Name
            .appendControl:SurName
            .appendControl:OtName
            .appendControl:Email
            .appendControl:Age
            .appendControl:PhoneNumber
            .appendControl:BtmExit
            .appendControl:BtmSave
            .appendControl:BtmDelTxt
            .appendControl:BtmInfo;
        
        // Form
        self.Caption := "Writer Account";
        self.setRegion(350, 250, 500, 265);
        
        // ------------------------------------ Labels ------------------------------------
        // LabelName        
        LabelName.Caption := "Your Name:";
        LabelName.setRegion(12, 10, 109, 30);

        // LabelSurName
        LabelSurName.Caption:="Your Last Name:";
        LabelSurName.setRegion(12, 40, 109, 20);

        // LabelOtName
        LabelOtName.Caption:="Other Names:";
        LabelOtName.setRegion(12, 65, 109, 20);

        // LabelEmail
        LabelEmail.Caption := "Email:";
        LabelEmail.setRegion(12, 90, 109, 20);

        // LabelAge
        LabelAge.Caption := "Age:";
        LabelAge.setRegion(12, 115, 109, 20);

        // LabelPhoneNumber
        LabelPhoneNumber.Caption := "Phone Number:";
        LabelPhoneNumber.setRegion(12, 140, 109, 20);

        // LabelAbout
        LabelAbout.Caption := emptyWideString;
        LabelAbout.setRegion(12, 165, 400, 35);

        // LabelDel
        LabelDel.Caption := emptyWideString;
        LabelDel.setRegion(300, 185, 400, 35);

        // ------------------------------------ Text Box's ------------------------------------
        // Name
        Name.Value := "Your name";
        Name.setRegion(125, 10, 200, 25);

        // SurName
        SurName.Value := "Your Last Name";
        SurName.setRegion(125, 35, 200, 25);

        // OtName
        OtName.Value := "Other Name";
        OtName.setRegion(125, 60, 200, 25);

        // Email
        Email.Value := "Email";
        Email.setRegion(125, 85, 200, 25);

        // Age
        Age.Value := "Age";
        Age.setRegion(125, 110, 200, 25);

        // PhoneNumber
        PhoneNumber.Value := "Phone Number";
        PhoneNumber.setRegion(125, 135, 200, 25);

        // ------------------------------------ Buttons ------------------------------------
        // BtmSave
        BtmSave.Caption:="Save";
        BtmSave.setRegion(390, 10, 82, 25);

        // BtmDelTxt
        BtmDelTxt.Caption:="Del TXT";
        BtmDelTxt.setRegion(390, 50, 82, 25);
        BtmDelTxt.Enabled:=false;

        // BtmInfo
        BtmInfo.Caption:="About";
        BtmInfo.setRegion(390, 80, 82, 25);

        // BtmExit
        BtmExit.Caption:="Exit";
        BtmExit.setRegion(390, 110, 82, 25);
        
        // ------------------------------------ Handlers  ------------------------------------
        // Save
        BtmSave.onClick := (args)
            {
                var dialog := SaveFileDialog.new(self);
                dialog.addFilter("Text Files", "*.txt");
                dialog.Caption := "Select a file";
                
                if (dialog.run())
                {
                    theSavedFile := File.assign(dialog.Selected);
                    
                    var writer := theSavedFile.logger();
                    
                    writer 
                        .printLine:"//-------------------------------------------------------------------------------"
                        .printLine("Name: ",Name.Value," ",SurName.Value," ",OtName.Value)
                        .printLine("Email: ",Email.Value)
                        .printLine("Age: ",Age.Value)
                        .printLine("PhoneNumber:",PhoneNumber.Value)
                        .printLine:"//-------------------------------------------------------------------------------";

                    writer.close();

                    BtmDelTxt.Enabled := true
                }
            };
        
        // Del TXT
        BtmDelTxt.onClick := (args)
        {
            theSavedFile.delete()
        };

        // Exit
        BtmExit.onClick := (args)
        {
            forward program.stop()
        };
            
        // About
        BtmInfo.onClick := (args)
        {
            LabelAbout.Caption := "Created by Alexandre Bencz."$10"Thanks Alex Rakov, for support." 
        }
    }
}