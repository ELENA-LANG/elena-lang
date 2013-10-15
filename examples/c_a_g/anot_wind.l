//// --- default namespaces ---
//#define std'dictionary'*.
//#define gui'dictionary'*.
//#define gui'dictionary'events'*.
//#define gui'common'*.
//
//// --- namespace shortcuts ---
//#define forms'*    = gui'forms'*.
//#define controls'* = gui'controls'*.
//#define basic'*    = std'basic'*.
//#define io'*       = sys'io'*.
//
//#class InputBox (forms'Dialog)
//{
//    #field infoLabel.
//    #field BtmOK.
//    
//    #method oninit : Args
//    [
//        super oninit:Args.
//
//        infoLabel := controls'staticlabel::self.
//        BtmOk     := controls'button::self.
//        
//        self
//            set &caption:"About the program"
//            set &x:320 &y:320 set &width:400 &height:230.
//        
//        infoLabel
//            set &x:20 &y:20 set &width:320 &height:20
//            set &caption:"Special thanks to Alex Rakov, for the support."
//            open.
//        
//        BtmOK
//            set &x:30 &y:175 set &width:50 & height:22
//            set &caption:"Ok"
//            open.
//        
//        BtmOk~eevents += 
//            { onclick'eval = self set &forms'dialog_result:basic'false.  }.
//    ]
//}
//
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
