// --- default namespaces ---
import forms;
import system'io;

public class InfoForm : ChildForm
{
   Label infoLabel;
   Button BtmOk;

   constructor new(parent)
        <= super new(parent)
   {
      BtmOk      := Button.new();
      infoLabel  := Label.new();

      self
         .appendControl(BtmOk)
         .appendControl(infoLabel);

      // Form ------------------------------------------------------------------------
      self.setRegion(320, 320, 400, 230);
      self.Caption := "About the program";
        
      // infoLabel
      infoLabel.setRegion(20, 20, 320, 20);
      infoLabel.Caption := "Special thanks to Alex Rakov, for the support.";
      
      // BtmOk
      BtmOk.setRegion(30, 175, 50, 22);
      BtmOk.Caption := "OK";

      BtmOk.onClick := (args)
          { self.close() };
   }
}

public class SaveForm : ChildForm
{
   // Button
   Button btmSalv;
   // Text Box
   Edit    nomeTex;
   // Labels
   Label   lblInfC;
   Label   lblNomF;
   
   string  _result;

   constructor new(parent, result)
      <= super new(parent)
   {
      _result := result;

      // Button
      btmSalv := Button.new();
      // Text Box
      nomeTex := Edit.new();
      // Labels
      lblNomF := Label.new();

      self
         .appendControl(btmSalv)
         .appendControl(nomeTex)
         .appendControl(lblNomF);

      // Form --------------------------------------------------------------------------
      self.setRegion(320, 320, 180, 130);
      self.Caption := "Save";

      // Buttons -----------------------------------------------------------------------
      btmSalv.setRegion(15, 60, 50, 22);
      btmSalv.Caption := "Save";

      // Text Box ----------------------------------------------------------------------
      nomeTex.setRegion(15, 30, 150, 22);
      nomeTex.Caption := "calc.txt";

      // Labels ------------------------------------------------------------------------
      lblNomF.setRegion(16, 10, 150, 22);
      lblNomF.Caption := "File name";

      btmSalv.onClick := (args)
          { self.onButtonClick() };
   }

   private onButtonClick()
   {
      var fileName := nomeTex.Caption;
      var writer := File.assign(fileName).logger();
      writer.writeLine(_result);

      writer.close();
      
      self.close();
   }
}
