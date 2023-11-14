// --- default namespaces ---
import forms.

class InfoForm :: ChildForm
{
    object infoLabel.
    object BtmOk.

    constructor new parent:aParent
        <= new parent:aParent;
    [
        // Form ------------------------------------------------------------------------
        $self set x:320 y:320.
        $self set width:400 height:230.
        $self set caption:"About the program".
        
        BtmOk := Button new.
        theControls append:BtmOk.

        // Labels
        infoLabel := Label new.
        theControls append:infoLabel.

        infoLabel set x:20 y:20.
        infoLabel set width:320 height:20.
        infoLabel set caption:"Special thanks to Alex Rakov, for the support.".            
        
        BtmOk set x:30 y:175.
        BtmOk set width:50 height:22.
        BtmOk set caption:"OK".            
        
        BtmOk set onClick: (:args)
            [ $self close ].
    ]
}

//#class SaveBox (forms'Dialog)
//{
//    // Panels
//    #field pnlFunc.
//    // Button
//    #field btmSalv.
//    // Text Box
//    #field ContTex.
//    #field nomeTex.
//    // Labels
//    #field lblInfC.
//    #field lblNomF.
//
//    #method oninit : Args
//    [
//        super oninit:Args.
//
//        // Button
//        btmSalv := controls'button::self.
//        // Text Box
//        nomeTex := controls'edit::self.
//        // Labels
//        lblNomF := controls'staticlabel::self.
//
//        // Form --------------------------------------------------------------------------
//        self
//            set &x:320 &y:320 set &width:180 & height:130
//            set &caption: "Save".
//
//        // Buttons -----------------------------------------------------------------------
//        btmSalv
//            set &x:15 &y:60 set &width:50 & height:22
//            set &caption: "Save"
//            open.
//
//        // Text Box ----------------------------------------------------------------------
//        nomeTex
//            set &x:15 &y:30 set &width:150 & height:22
//            set &caption: "calc.txt"
//            open.
//
//        // Labels ------------------------------------------------------------------------
//        lblNomF
//            set &x:16 &y:10 set &width:150 & height:22
//            set &caption: "Nome do arquivo"
//            open.
//
//        btmSalv~eevents += 
//            { onclick'eval = self $onClick.  }.
//    ]
//
//    #method $onClick
//    [
//        #var aValue := theParent calc_area_gui'calc_result.
//        
//        #var aFileName := nomeTex literal.
//        #var aWriter := io'LogWriter::aFileName.
//        aWriter write:"%n" write:aValue write:"%r%n".
//
//        aWriter free.
//        
//        self set &forms'dialog_result:basic'false.
//    ]
//}
