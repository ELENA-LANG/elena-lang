// --- default namespaces ---
//#define std'dictionary'*.
#define gui'dictionary'*.
//#define gui'common'*.
#define ext'convertors'*.

// --- namespace shortcuts ---
#define basic'*    = std'basic'*.
#define controls'* = gui'controls'*.
#define forms'*    = gui'forms'*.
#define graph'*    = gui'graphics'*.

// --- private subjects ---
#subject $TipoDoCalculo, $TipoDoObj, $onPaint, $onButtonClick/*calc_result*/.

#class window (forms'SDIDialog)
{
    // Panels
    #field pnlFormula.

    // Text Box
    #field valUm.
    #field valDois.
    #field valTres.

    // Buttons
    #field BtmCalc.
    #field BtmInfo.
    #field BtmSalv.
    #field BtmSair.

    // Labels
    #field valor.
    #field info.
    #field letter1.
    #field letter2.
    #field letter3.

    // Radio Button
    #field Tconta.
	#field Econta1.
	#field Econta2.
	#field Econta3.
    #field Econta4.

    // Image Formula
    #field theImageList.
    #field thePaintbox.
    #field theImageIndex.

    //Outros
    #field theResult.

//    #method calc_result = theResult.

    #method initializing : Args
    [
        super initializing:Args.

        theResult := basic'String.

        // -----------------------------------------------------------
        // Panels
        pnlFormula   := controls'Panel::self.

        // Text Box
        valUM        := controls'edit::self.
        valDois      := controls'edit::self.
        valTres      := controls'edit::self.

        // Buttons
        BtmCalc      := controls'Button::self.
        BtmInfo      := controls'Button::self.
        BtmSalv      := controls'Button::self.
        BtmSair      := controls'Button::self.

        // Labels
        valor        := controls'staticlabel::self.
        info         := controls'staticlabel::self.
        letter1      := controls'staticlabel::self.
        letter2      := controls'staticlabel::self.
        letter3      := controls'staticlabel::self.

        // Radio Button
        Tconta       := controls'radioButtonGroup::self.
		Econta1      := controls'radioButtonGroup::self.
		Econta2      := controls'radioButtonGroup::self.
		Econta3      := controls'radioButtonGroup::self.
        Econta4      := controls'radiobuttonGroup::self.

        // Images Formulas
        thePaintbox  := controls'Imagebox::pnlFormula.
        theImageList := controls'ImageList::pnlFormula.
        // -----------------------------------------------------------

        theImageIndex := basic'Integer << 0.
        
        // Form ------------------------------------------------------------------------
        self
            coordinates'set &x:250 &y:200 dimensions'set &width:600 & height:400
            caption'set:"Calc. Area GUI :-------------------------: ver. 0.2b".            

        // Panels ----------------------------------------------------------------------
        pnlFormula
            coordinates'set &x:190 &y:200 dimensions'set &width:400 &height:170
            caption'set:"Formula"
            open.

        // Radio Button ----------------------------------------------------------------
        Tconta
            coordinates'set &x:7 &y:1 dimensions'set &width:150 &height:100
            caption'set:"Escolha do calculo"
            control_item'append &caption:"&Paralelogramos" 
            control_item'append &caption:"&Trapezio" 
            control_item'append &caption:"&Circulos" 
            control_item'append &caption:"&Triangulos"
            open.

        Econta1
            coordinates'set &x:7 &y:105 dimensions'set &width:150 & height:100
            caption'set: " Escolha do calculo " 
            control_item'append &caption:"&Retangulo"
            control_item'append &caption:"&Quadrado"
            control_item'append &caption:"&Paralelogramo"
            control_item'append &caption:"&Losango"
            open.

        Econta2
            coordinates'set &x:7 &y:105 dimensions'set &width:150 & height:80
            caption'set: " Escolha do calculo "
            control_item'append &caption:"&Retangulo"
            control_item'append &caption:"&Isosceles"
            control_item'append &caption:"&Escaleno".

        Econta3
            coordinates'set &x:7 &y:105 dimensions'set &width:150 & height:80
            caption'set: " Escolha do calculo "
            control_item'append &caption:"&Circulo"
            control_item'append &caption:"&Coroa circular"
            control_item'append &caption:"&Setor circular".
            
        Econta4
            coordinates'set &x:7 &y:105 dimensions'set &width:150 &height:130
            caption'set: " Escolha do calculo "
            control_item'append &caption:"&Qualquer"
            control_item'append &caption:"&Retangulo"
            control_item'append &caption:"&Retangulo"
            control_item'append &caption:"&Func dos lados"
            control_item'append &caption:"&Func lds ang".

        Tconta << 0.
        Econta1 << 0.

        // Text Box ---------------------------------------------------------------------
        valUM
            coordinates'set &x:185 &y:10 dimensions'set &width:150 & height:22.

        valDois
            coordinates'set &x:185 &y:40 dimensions'set &width:150 & height:22.
        
        valTres
            coordinates'set &x:185 &y:70 dimensions'set &width:150 & height:22.

        // Buttons -----------------------------------------------------------------------
        BtmCalc
            coordinates'set &x:430 &y:10 dimensions'set &width:150 & height:22
            caption'set: "Calcular"
            open.
        
        BtmInfo
            coordinates'set &x:430 &y:40 dimensions'set &width:150 & height:22
            caption'set: "Infos"
            open.
        
        BtmSalv
            coordinates'set &x:430 &y:70 dimensions'set &width:150 & height:22
            caption'set: "Salvar"
            open.

        BtmSair
            coordinates'set &x:430 &y:100 dimensions'set &width:150 & height:22
            caption'set: "Sair"
            open.

        // Labels ------------------------------------------------------------------------
        valor
            coordinates'set &x:170 &y:105 dimensions'set &width:245 & height:22
            caption'set: "Res: "
            open.
        
        info
            coordinates'set &x:170 &y:135 dimensions'set &width:250 & height:22
            caption'set: "Info: "
            open.
        
        letter1
            coordinates'set &x:170 &y:15 dimensions'set &width:14 & height:22
            open.
        
        letter2
            coordinates'set &x:170 &y:45 dimensions'set &width:14 & height:22
            open.
        
        letter3
            coordinates'set &x:170 &y:75 dimensions'set &width:14 & height:22
            open.

        // Image Formula -----------------------------------------------------------------
        thePaintbox
            coordinates'set &x:12 &y:22 dimensions'set &width:379 &height:139
            open.

        // Image List Formulas -----------------------------------------------------------
        theImageList += "..\formulas\Paralelogramos\bitmap1.bmp".
        theImageList += "..\formulas\Paralelogramos\bitmap2.bmp".
        theImageList += "..\formulas\Paralelogramos\bitmap3.bmp".
		theImageList += "..\formulas\Paralelogramos\bitmap4.bmp".

        theImageList += "..\formulas\Trapezio\bitmap1.bmp".
        theImageList += "..\formulas\Trapezio\bitmap2.bmp".
        theImageList += "..\formulas\Trapezio\bitmap3.bmp".

        theImageList += "..\formulas\Circulo\bitmap1.bmp".
        theImageList += "..\formulas\Circulo\bitmap2.bmp".
        theImageList += "..\formulas\Circulo\bitmap3.bmp".
        
        theImageList += "..\formulas\Triangulos\bitmap1.bmp".
        theImageList += "..\formulas\Triangulos\bitmap2.bmp".
        theImageList += "..\formulas\Triangulos\bitmap3.bmp".
        theImageList += "..\formulas\Triangulos\bitmap4.bmp".
        theImageList += "..\formulas\Triangulos\bitmap5.bmp".

        // Events ------------------------------------------------------------------------
        // Images ------------------------------------------------------------------------
        thePaintbox event_handle'append:
            { painting &canvas:aCanvas = self $onPaint:aCanvas. }.
       
        // Radio Buttons -----------------------------------------------------------------
        Tconta event_handle'append:
            { index_changing &selected_index:anIndex = self $TipoDoCalculo:anIndex. }.

        Econta1 event_handle'append:
            { index_changing &selected_index:anInicio = self $TipoDoObj:anInicio. }.

        Econta2 event_handle'append:
            { index_changing &selected_index:anInicio = self $TipoDoObj:(4 + anInicio). }.

        Econta3 event_handle'append:
            { index_changing &selected_index:anInicio = self $TipoDoObj:(7 + anInicio). }.
        
        Econta4 event_handle'append:
            { index_changing &selected_index:anInicio = self $TipoDoObj:(10 + anInicio). }.

        // Buttons ----------------------------------------------------------------------
        BtmCalc event_handle'append:
            { clicking : Args = self $onButtonClick. }.
       
//        BtmInfo~eevents +=
//            { onclick'eval = self $onButtonClickInfo. }.
//            
//        BtmSalv~eevents +=
//            { onclick'eval = self $onButtonClickSave. }.

        BtmSair event_handle'append:
            { clicking : Args [ 'program stop. ] }.
            
        $self $TipoDoObj:0.
    ]

    // Methods --------------------------------------------------------------------------
    #method $onPaint : aCanvas
    [
        aCanvas write &graph'image:(theImageList@theImageIndex) &x:0 &y:0 &:graph'aimagedrawing.
    ]

    #method $TipoDoCalculo : anIndex
    [
        #if anIndex ==0?
        [
            Econta1 open.
            
            __cast(Econta2, Econta3, Econta4) close.
            
            Econta1 << 0.
        ]
        | == 1?
        [           
            Econta2 open.
            
            __cast(Econta1, Econta3, Econta4) close.
            
            Econta2 << 0.
        ]
        | == 2?
        [
            Econta3 open.
            
            __cast(Econta1, Econta2, Econta4) close.
            
            Econta3 << 0.
        ]
        | == 3?
        [
            Econta4 open.
            
            __cast(Econta1, Econta2, Econta3) close.
            
            Econta4 << 0.
        ].
    ]

	#method $TipoDoObj : anInicio
	[
        theImageIndex << anInicio.
        
        #if anInicio == 0? 
        [
            __cast(valUM, valDois, letter1, letter2) open.
            __cast(valTres, letter3) close.
            
            __cast(valUM, valDois, valTres) caption'set:"".
           
            letter1 caption'set:"h: ".
            letter2 caption'set:"b: ".
            
            info caption'set:"Info: ".
            valor caption'set:"Res: ".
        ]
        | == 1?
        [
            __cast(valUM, letter1) open.
            __cast(valDois, valTres, letter2, letter3) close.
            
            __cast(valUM, valDois, valTres) caption'set:"".
            
            letter1 caption'set:"b: ".
            
            info caption'set:"Info: ".
            valor caption'set:"Res: ".
        ]
        | == 2?
        [
            __cast(valUM, valDois, letter1, letter2) open.
            __cast(valTres, letter3) close.
            
            __cast(valUM, valDois, valTres) caption'set:"".
            
            letter1 caption'set:"h: ".
            letter2 caption'set:"b: ".
            
            info caption'set:"Info: ".
            valor caption'set:"Res: ".
        ]
        | == 3?
        [
            __cast(valUM, valDois, letter1, letter2) open.
            __cast(valTres, letter3) close.
            
            __cast(valUM, valDois, valTres) caption'set:"".
            
            letter1 caption'set:"D: ".
            letter2 caption'set:"d: ".
            
            info caption'set:"Info: ".
            valor caption'set:"Res: ".
        ]
        | == 4?
        [
            __cast(valUM, valDois, valTres, letter1, letter2, letter3) open.
            
            __cast(valUM, valDois, valTres) caption'set:"".
            
            letter1 caption'set:"B: ".
            letter2 caption'set:"b: ".
            letter3 caption'set:"h: ".
            
            info caption'set:"Info: ".
            valor caption'set:"Res: ".
        ]
        | == 5?
        [
            __cast(valUM, valDois, valTres, letter1, letter2, letter3) open.
            
            __cast(valUM, valDois, valTres) caption'set:"".
            
            letter1 caption'set:"B: ".
            letter2 caption'set:"b: ".
            letter3 caption'set:"h: ".
            
            info caption'set:"Info: ".
            valor caption'set:"Res: ".
        ]
        | == 6?
        [
            __cast(valUM, valDois, valTres, letter1, letter2, letter3) open.
            
            __cast(valUM, valDois, valTres) caption'set:"".
            
            letter1 caption'set:"B: ".
            letter2 caption'set:"b: ".
            letter3 caption'set:"h: ".
            
            info caption'set:"Info: ".
            valor caption'set:"Res: ".
        ]
        | == 7?
        [
            __cast(valUM, letter1) open.
            __cast(valDois, valTres, letter2, letter3) close.
            
            __cast(valUM, valDois, valTres) caption'set:"".
            
            letter1 caption'set:"r: ".
            
            info caption'set:"Info:".
            valor caption'set:"Res: ".
        ]
        | == 8?
        [
            __cast(valUM, valDois, letter1, letter2) open.
            __cast(valTres, letter3) close.
            
            __cast(valUM, valDois, valTres) caption'set:"".
            
            letter1 caption'set:"R: ".
            letter2 caption'set:"r: ".
            
            info caption'set:"Info:".
            valor caption'set:"Res: ".
        ]
        | == 9?
        [
            __cast(valUM, valDois, letter1, letter2) open.
            __cast(valTres, letter3) close.
            
            __cast(valUM, valDois, valTres) caption'set:"".
            
            letter1 caption'set:"Al: ".
            letter2 caption'set:"r: ".
            
            info caption'set:"Info: Al = Alfa ( angulo )".
            valor caption'set:"Res: ".
        ]
        | == 10?
        [
            __cast(valUM, valDois, letter1, letter2) open.
            __cast(valTres, letter3) close.
            
            __cast(valUM, valDois, valTres) caption'set:"".
            
            letter1 caption'set:"B: ".
            letter2 caption'set:"H: ".
            
            info caption'set:"Info: ".
            valor caption'set:"Res: ".
        ]
        | == 11?
        [
            __cast(valUM, valDois, letter1, letter2) open.
            __cast(valTres, letter3) close.
            
            __cast(valUM, valDois, valTres) caption'set:"".
            
            letter1 caption'set:"B: ".
            letter2 caption'set:"C: ".
            
            info caption'set:"Info: ".
            valor caption'set:"Res: ".
        ]
        | == 12?
        [
            __cast(valUM, letter1) open.
            __cast(valDois, valTres, letter2, letter3) close.
            
            __cast(valUM, valDois, valTres) caption'set:"".
            
            letter1 caption'set:"A: ".
            
            info caption'set:"Info: ".
            valor caption'set:"Res: ".
        ]
        | == 13?
        [
            __cast(valUM, valDois, valTres, letter1, letter2, letter3) open.
            
            __cast(valUM, valDois, valTres) caption'set:"".
            
            letter1 caption'set:"a: ".
            letter2 caption'set:"b: ".
            letter3 caption'set:"c: ".
            
            info caption'set:"Info: ".
            valor caption'set:"Res: ".
        ]
        | == 14?
        [
            __cast(valUM, valDois, valTres, letter1, letter2, letter3) open.
            
            __cast(valUM, valDois, valTres) caption'set:"".
            
            letter1 caption'set:"Al: ".
            letter2 caption'set:"a: ".
            letter3 caption'set:"b: ".
            
            info caption'set:"Info: Al = Alfa ( angulo )".
            valor caption'set:"Res: ".
        ].

		thePaintbox refresh.
	]

//    #method $onButtonClickInfo
//    [
//        anot_wind'InputBox::self open &forms'modal.
//    ]
//
//    #method $onButtonClickSave
//    [
//        #var asForm := anot_wind'SaveBox::self.
//        
//        asForm open &forms'modal.
//    ]

    #method $onButtonClick
    [   
        #if theImageIndex ==0? 
        [
            #var H := WideLiteralToReal::valUM.
            #var B := WideLiteralToReal::valDois.
            #var aResult := H * B.
            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
        ]
        | ==1?
        [
            #var B := WideLiteralToReal::valUM.
            #var aResult := B * B.
            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
        ]
        | ==2?
        [
            #var H := WideLiteralToReal::valUM.
            #var B := WideLiteralToReal::valDois.
            #var aResult := H * B.
            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
        ]
        | ==3?
        [
            #var DM := WideLiteralToReal::valUM.
            #var d := WideLiteralToReal::valDois.
            #var aResult := DM * d / 2.
            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
        ]
        | ==4?
        [
            #var BM := WideLiteralToReal::valUM.
            #var b := WideLiteralToReal::valDois.
            #var H := WideLiteralToReal::valTres.
            #var aResult := (BM + b)*H / 2.
            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
        ]
        | ==5?
        [
            #var BM := WideLiteralToReal::valUM.
            #var b := WideLiteralToReal::valDois.
            #var H := WideLiteralToReal::valTres.
            #var aResult := (BM + b)*H / 2.
            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
        ]
        | ==6?
        [
            #var BM := WideLiteralToReal::valUM.
            #var b := WideLiteralToReal::valDois.
            #var H := WideLiteralToReal::valTres.
            #var aResult := (BM + b)*H / 2.
            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
        ]
        | ==7?
        [
            #var R := WideLiteralToReal::valUM.
            #var aResult := std'math'pi * R * R.
            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
        ]
        | ==8?
        [
            #var RM := WideLiteralToReal::valUM.
            #var r := WideLiteralToReal::valDois.
            #var aResult := std'math'pi * (RM * RM - r * r).
            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
        ]
        | ==9?
        [
            #var Alfa := WideLiteralToReal::valUM.
            #var R := WideLiteralToReal::valDois.
            #var aResult := Alfa * std'math'pi * R * R / 360.
            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
        ]
        | ==10?
        [
            #var B := WideLiteralToReal::valUM.
            #var H := WideLiteralToReal::valDois.
            #var aResult := B * H / 2.
            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
        ]
        | ==11?
        [
            #var B := WideLiteralToReal::valUM.
            #var C := WideLiteralToReal::valDois.
            #var aResult := B * C / 2.
            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
        ]
        | ==12?
        [
            #var A := WideLiteralToReal::valUM.
            #var aResult := std'math'Rounded::(std'math'sqrt::std'math'pi * A * A / 4).
            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
        ]
        | ==13?
        [
            #var A := WideLiteralToReal::valUM.
            #var B := WideLiteralToReal::valDois.
            #var C := WideLiteralToReal::valTres.
            #var P := A + B + C / 2.
            #var val1 := P-A.
            #var val2 := P-B.
            #var val3 := P-C.
            #var aResult := std'math'sqrt::P*(val1 * val2 * val3).
            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
        ]
        | ==14?
        [
            #var Alfa := WideLiteralToReal::valUM.
            #var A := WideLiteralToReal::valDois.
            #var B := WideLiteralToReal::valTres.
            #var AlfaConv := (std'math'pi * Alfa) / 180.
            #var sinAl := std'math'sin::AlfaConv.
            #var aResult := sinAl * a * b / 2.
            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
        ].
        valor caption'set:theResult.
    ]
}