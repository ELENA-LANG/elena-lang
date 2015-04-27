// --- default namespaces ---
#define system.
#define forms.
////#define gui'common'*.
//#define ext'convertors'*.

#class Window :: SDIDialog
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

//    // Image Formula
//    #field theImageList.
//    #field thePaintbox.
//    #field theImageIndex.
//
//    //Outros
//    #field theResult.
//
    #field theController.

////    #method calc_result = theResult.

    #constructor new
        <= (new)
    [
//        theResult := basic'String.

        // -----------------------------------------------------------
        // Panels
        pnlFormula   := Panel new.
        theControls append:pnlFormula.

        // Text Box
        valUm        := Edit new.
        valDois      := Edit new.
        valTres      := Edit new.
        theControls append:valUm.
        theControls append:valDois.
        theControls append:valTres.

        // Buttons
        BtmCalc      := Button new.
        BtmInfo      := Button new.
        BtmSalv      := Button new.
        BtmSair      := Button new.
        theControls append:BtmCalc.
        theControls append:BtmInfo.
        theControls append:BtmSalv.
        theControls append:BtmSair.

        // Labels
        valor        := Label new.
        info         := Label new.
        letter1      := Label new.
        letter2      := Label new.
        letter3      := Label new.
        theControls append:valor.
        theControls append:info.
        theControls append:letter1.
        theControls append:letter2.
        theControls append:letter3.

        // Radio Button
        Tconta       := RadioButtonGroup new.
		Econta1      := RadioButtonGroup new.
		Econta2      := RadioButtonGroup new.
		Econta3      := RadioButtonGroup new.
        Econta4      := RadioButtonGroup new.
        theControls append:Tconta.
        theControls append:Econta1.
        theControls append:Econta2.
        theControls append:Econta3.
        theControls append:Econta4.

//        // Images Formulas
//        thePaintbox  := controls'Imagebox::pnlFormula.
//        theImageList := controls'ImageList::pnlFormula.
//        // -----------------------------------------------------------
//
//        theImageIndex := basic'Integer << 0.

        // Form ------------------------------------------------------------------------
        $self set &x:250 &y:200.
        $self set &width:600 &height:400.
        $self set &caption:"Calc. Area GUI :-------------------------: ver. 0.3".            

        // Panels ----------------------------------------------------------------------
        pnlFormula set &x:190 &y:200.
        pnlFormula set &width:400 &height:170.
        pnlFormula set &caption:"Formula".            

        // Radio Button ----------------------------------------------------------------
        Tconta set &x:7 &y:1.
        Tconta set &width:150 &height:100.
        Tconta set &caption:"Escolha do calculo".
        Tconta items
            append:"&Paralelogramos"
            append:"&Trapezio"
            append:"&Circulos"
            append:"&Triangulos".

//        Econta1
        Econta1 set &x:7 &y:105.
        Econta1 set &width:150 &height:100.
        Econta1 set &caption:" Escolha do calculo ".
        Econta1 items
            append:"&Retangulo"
            append:"&Quadrado"
            append:"&Paralelogramo"
            append:"&Losango".
        Econta1 set &visible:false.

        Econta2 set &x:7 &y:105.
        Econta2 set &width:150 &height:100.
        Econta2 set &caption:" Escolha do calculo ".
        Econta2 items
            append:"&Retangulo"
            append:"&Isosceles"
            append:"&Escaleno".
        Econta2 set &visible:false.

        Econta3 set &x:7 &y:105.
        Econta3 set &width:150 &height:100.
        Econta3 set &caption:" Escolha do calculo ".
        Econta3 items
            append:"&Circulo"
            append:"&Coroa circular"
            append:"&Setor circular".
        Econta3 set &visible:false.

        Econta4 set &x:7 &y:105.
        Econta4 set &width:150 &height:100.
        Econta4 set &caption:" Escolha do calculo ".
        Econta4 items
            append:"&Qualquer"
            append:"&Retangulo"
            append:"&Func dos lados"
            append:"&Retangulo"
            append:"&Func lds ang".
        Econta4 set &visible:false.

//        Tconta << 0.
//        Econta1 << 0.

        // Text Box ---------------------------------------------------------------------
        valUm set &x:185 &y:10.
        valUm set &width:150 & height:22.

        valDois set &x:185 &y:40.
        valDois set &width:150 &height:22.
        
        valTres set &x:185 &y:70.
        valTres set &width:150 &height:22.

        // Buttons -----------------------------------------------------------------------
        BtmCalc set &x:430 &y:10.
        BtmCalc set &width:150 & height:22.
        BtmCalc set &caption: "Calcular".
        
        BtmInfo set &x:430 &y:40.
        BtmInfo set &width:150 & height:22.
        BtmInfo set &caption: "Infos".
        
        BtmSalv set &x:430 &y:70.
        BtmSalv set &width:150 & height:22.
        BtmSalv set &caption: "Salvar".

        BtmSair set &x:430 &y:100.
        BtmSair set &width:150 & height:22.
        BtmSair set &caption: "Sair".

        // Labels ------------------------------------------------------------------------
        valor set &x:170 &y:105.
        valor set &width:245 & height:22.
        valor set &caption: "Res: ".
        
        info set &x:170 &y:135.
        info set &width:250 & height:22.
        info set &caption: "Info: ".
        
        letter1 set &x:170 &y:15.
        letter1 set &width:14 & height:22.
        
        letter2 set &x:170 &y:45.
        letter2 set &width:14 & height:22.
        
        letter3 set &x:170 &y:75.
        letter3 set &width:14 & height:22.

//        // Image Formula -----------------------------------------------------------------
//        thePaintbox
//            coordinates'set &x:12 &y:22 dimensions'set &width:379 &height:139
//            open.
//
//        // Image List Formulas -----------------------------------------------------------
//        theImageList += "..\formulas\Paralelogramos\bitmap1.bmp".
//        theImageList += "..\formulas\Paralelogramos\bitmap2.bmp".
//        theImageList += "..\formulas\Paralelogramos\bitmap3.bmp".
//		theImageList += "..\formulas\Paralelogramos\bitmap4.bmp".
//
//        theImageList += "..\formulas\Trapezio\bitmap1.bmp".
//        theImageList += "..\formulas\Trapezio\bitmap2.bmp".
//        theImageList += "..\formulas\Trapezio\bitmap3.bmp".
//
//        theImageList += "..\formulas\Circulo\bitmap1.bmp".
//        theImageList += "..\formulas\Circulo\bitmap2.bmp".
//        theImageList += "..\formulas\Circulo\bitmap3.bmp".
//        
//        theImageList += "..\formulas\Triangulos\bitmap1.bmp".
//        theImageList += "..\formulas\Triangulos\bitmap2.bmp".
//        theImageList += "..\formulas\Triangulos\bitmap3.bmp".
//        theImageList += "..\formulas\Triangulos\bitmap4.bmp".
//        theImageList += "..\formulas\Triangulos\bitmap5.bmp".
//
//        // Events ------------------------------------------------------------------------
//        // Images ------------------------------------------------------------------------
//        thePaintbox event_handle'append:
//            { painting &canvas:aCanvas = self $onPaint:aCanvas. }.
//       
//        // Radio Buttons -----------------------------------------------------------------
//        Tconta event_handle'append:
//            { index_changing &selected_index:anIndex = self $TipoDoCalculo:anIndex. }.
//
//        Econta1 event_handle'append:
//            { index_changing &selected_index:anInicio = self $TipoDoObj:anInicio. }.
//
//        Econta2 event_handle'append:
//            { index_changing &selected_index:anInicio = self $TipoDoObj:(4 + anInicio). }.
//
//        Econta3 event_handle'append:
//            { index_changing &selected_index:anInicio = self $TipoDoObj:(7 + anInicio). }.
//        
//        Econta4 event_handle'append:
//            { index_changing &selected_index:anInicio = self $TipoDoObj:(10 + anInicio). }.
//
//        // Buttons ----------------------------------------------------------------------
//        BtmCalc event_handle'append:
//            { clicking : Args = self $onButtonClick. }.
//       
////        BtmInfo~eevents +=
////            { onclick'eval = self $onButtonClickInfo. }.
////            
////        BtmSalv~eevents +=
////            { onclick'eval = self $onButtonClickSave. }.
//
//        BtmSair event_handle'append:
//            { clicking : Args [ 'program stop. ] }.
//            
//        $self $TipoDoObj:0.
    ]

//    // Methods --------------------------------------------------------------------------
//    #method $onPaint : aCanvas
//    [
//        aCanvas write &graph'image:(theImageList@theImageIndex) &x:0 &y:0 &:graph'aimagedrawing.
//    ]
//
//    #method $TipoDoCalculo : anIndex
//    [
//        #if anIndex ==0?
//        [
//            Econta1 open.
//            
//            __cast(Econta2, Econta3, Econta4) close.
//            
//            Econta1 << 0.
//        ]
//        | == 1?
//        [           
//            Econta2 open.
//            
//            __cast(Econta1, Econta3, Econta4) close.
//            
//            Econta2 << 0.
//        ]
//        | == 2?
//        [
//            Econta3 open.
//            
//            __cast(Econta1, Econta2, Econta4) close.
//            
//            Econta3 << 0.
//        ]
//        | == 3?
//        [
//            Econta4 open.
//            
//            __cast(Econta1, Econta2, Econta3) close.
//            
//            Econta4 << 0.
//        ].
//    ]
//
//	#method $TipoDoObj : anInicio
//	[
//        theImageIndex << anInicio.
//        
//        #if anInicio == 0? 
//        [
//            __cast(valUM, valDois, letter1, letter2) open.
//            __cast(valTres, letter3) close.
//            
//            __cast(valUM, valDois, valTres) caption'set:"".
//           
//            letter1 caption'set:"h: ".
//            letter2 caption'set:"b: ".
//            
//            info caption'set:"Info: ".
//            valor caption'set:"Res: ".
//        ]
//        | == 1?
//        [
//            __cast(valUM, letter1) open.
//            __cast(valDois, valTres, letter2, letter3) close.
//            
//            __cast(valUM, valDois, valTres) caption'set:"".
//            
//            letter1 caption'set:"b: ".
//            
//            info caption'set:"Info: ".
//            valor caption'set:"Res: ".
//        ]
//        | == 2?
//        [
//            __cast(valUM, valDois, letter1, letter2) open.
//            __cast(valTres, letter3) close.
//            
//            __cast(valUM, valDois, valTres) caption'set:"".
//            
//            letter1 caption'set:"h: ".
//            letter2 caption'set:"b: ".
//            
//            info caption'set:"Info: ".
//            valor caption'set:"Res: ".
//        ]
//        | == 3?
//        [
//            __cast(valUM, valDois, letter1, letter2) open.
//            __cast(valTres, letter3) close.
//            
//            __cast(valUM, valDois, valTres) caption'set:"".
//            
//            letter1 caption'set:"D: ".
//            letter2 caption'set:"d: ".
//            
//            info caption'set:"Info: ".
//            valor caption'set:"Res: ".
//        ]
//        | == 4?
//        [
//            __cast(valUM, valDois, valTres, letter1, letter2, letter3) open.
//            
//            __cast(valUM, valDois, valTres) caption'set:"".
//            
//            letter1 caption'set:"B: ".
//            letter2 caption'set:"b: ".
//            letter3 caption'set:"h: ".
//            
//            info caption'set:"Info: ".
//            valor caption'set:"Res: ".
//        ]
//        | == 5?
//        [
//            __cast(valUM, valDois, valTres, letter1, letter2, letter3) open.
//            
//            __cast(valUM, valDois, valTres) caption'set:"".
//            
//            letter1 caption'set:"B: ".
//            letter2 caption'set:"b: ".
//            letter3 caption'set:"h: ".
//            
//            info caption'set:"Info: ".
//            valor caption'set:"Res: ".
//        ]
//        | == 6?
//        [
//            __cast(valUM, valDois, valTres, letter1, letter2, letter3) open.
//            
//            __cast(valUM, valDois, valTres) caption'set:"".
//            
//            letter1 caption'set:"B: ".
//            letter2 caption'set:"b: ".
//            letter3 caption'set:"h: ".
//            
//            info caption'set:"Info: ".
//            valor caption'set:"Res: ".
//        ]
//        | == 7?
//        [
//            __cast(valUM, letter1) open.
//            __cast(valDois, valTres, letter2, letter3) close.
//            
//            __cast(valUM, valDois, valTres) caption'set:"".
//            
//            letter1 caption'set:"r: ".
//            
//            info caption'set:"Info:".
//            valor caption'set:"Res: ".
//        ]
//        | == 8?
//        [
//            __cast(valUM, valDois, letter1, letter2) open.
//            __cast(valTres, letter3) close.
//            
//            __cast(valUM, valDois, valTres) caption'set:"".
//            
//            letter1 caption'set:"R: ".
//            letter2 caption'set:"r: ".
//            
//            info caption'set:"Info:".
//            valor caption'set:"Res: ".
//        ]
//        | == 9?
//        [
//            __cast(valUM, valDois, letter1, letter2) open.
//            __cast(valTres, letter3) close.
//            
//            __cast(valUM, valDois, valTres) caption'set:"".
//            
//            letter1 caption'set:"Al: ".
//            letter2 caption'set:"r: ".
//            
//            info caption'set:"Info: Al = Alfa ( angulo )".
//            valor caption'set:"Res: ".
//        ]
//        | == 10?
//        [
//            __cast(valUM, valDois, letter1, letter2) open.
//            __cast(valTres, letter3) close.
//            
//            __cast(valUM, valDois, valTres) caption'set:"".
//            
//            letter1 caption'set:"B: ".
//            letter2 caption'set:"H: ".
//            
//            info caption'set:"Info: ".
//            valor caption'set:"Res: ".
//        ]
//        | == 11?
//        [
//            __cast(valUM, valDois, letter1, letter2) open.
//            __cast(valTres, letter3) close.
//            
//            __cast(valUM, valDois, valTres) caption'set:"".
//            
//            letter1 caption'set:"B: ".
//            letter2 caption'set:"C: ".
//            
//            info caption'set:"Info: ".
//            valor caption'set:"Res: ".
//        ]
//        | == 12?
//        [
//            __cast(valUM, letter1) open.
//            __cast(valDois, valTres, letter2, letter3) close.
//            
//            __cast(valUM, valDois, valTres) caption'set:"".
//            
//            letter1 caption'set:"A: ".
//            
//            info caption'set:"Info: ".
//            valor caption'set:"Res: ".
//        ]
//        | == 13?
//        [
//            __cast(valUM, valDois, valTres, letter1, letter2, letter3) open.
//            
//            __cast(valUM, valDois, valTres) caption'set:"".
//            
//            letter1 caption'set:"a: ".
//            letter2 caption'set:"b: ".
//            letter3 caption'set:"c: ".
//            
//            info caption'set:"Info: ".
//            valor caption'set:"Res: ".
//        ]
//        | == 14?
//        [
//            __cast(valUM, valDois, valTres, letter1, letter2, letter3) open.
//            
//            __cast(valUM, valDois, valTres) caption'set:"".
//            
//            letter1 caption'set:"Al: ".
//            letter2 caption'set:"a: ".
//            letter3 caption'set:"b: ".
//            
//            info caption'set:"Info: Al = Alfa ( angulo )".
//            valor caption'set:"Res: ".
//        ].
//
//		thePaintbox refresh.
//	]
//
////    #method $onButtonClickInfo
////    [
////        anot_wind'InputBox::self open &forms'modal.
////    ]
////
////    #method $onButtonClickSave
////    [
////        #var asForm := anot_wind'SaveBox::self.
////        
////        asForm open &forms'modal.
////    ]
//
//    #method $onButtonClick
//    [   
//        #if theImageIndex ==0? 
//        [
//            #var H := WideLiteralToReal::valUM.
//            #var B := WideLiteralToReal::valDois.
//            #var aResult := H * B.
//            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
//        ]
//        | ==1?
//        [
//            #var B := WideLiteralToReal::valUM.
//            #var aResult := B * B.
//            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
//        ]
//        | ==2?
//        [
//            #var H := WideLiteralToReal::valUM.
//            #var B := WideLiteralToReal::valDois.
//            #var aResult := H * B.
//            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
//        ]
//        | ==3?
//        [
//            #var DM := WideLiteralToReal::valUM.
//            #var d := WideLiteralToReal::valDois.
//            #var aResult := DM * d / 2.
//            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
//        ]
//        | ==4?
//        [
//            #var BM := WideLiteralToReal::valUM.
//            #var b := WideLiteralToReal::valDois.
//            #var H := WideLiteralToReal::valTres.
//            #var aResult := (BM + b)*H / 2.
//            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
//        ]
//        | ==5?
//        [
//            #var BM := WideLiteralToReal::valUM.
//            #var b := WideLiteralToReal::valDois.
//            #var H := WideLiteralToReal::valTres.
//            #var aResult := (BM + b)*H / 2.
//            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
//        ]
//        | ==6?
//        [
//            #var BM := WideLiteralToReal::valUM.
//            #var b := WideLiteralToReal::valDois.
//            #var H := WideLiteralToReal::valTres.
//            #var aResult := (BM + b)*H / 2.
//            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
//        ]
//        | ==7?
//        [
//            #var R := WideLiteralToReal::valUM.
//            #var aResult := std'math'pi * R * R.
//            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
//        ]
//        | ==8?
//        [
//            #var RM := WideLiteralToReal::valUM.
//            #var r := WideLiteralToReal::valDois.
//            #var aResult := std'math'pi * (RM * RM - r * r).
//            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
//        ]
//        | ==9?
//        [
//            #var Alfa := WideLiteralToReal::valUM.
//            #var R := WideLiteralToReal::valDois.
//            #var aResult := Alfa * std'math'pi * R * R / 360.
//            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
//        ]
//        | ==10?
//        [
//            #var B := WideLiteralToReal::valUM.
//            #var H := WideLiteralToReal::valDois.
//            #var aResult := B * H / 2.
//            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
//        ]
//        | ==11?
//        [
//            #var B := WideLiteralToReal::valUM.
//            #var C := WideLiteralToReal::valDois.
//            #var aResult := B * C / 2.
//            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
//        ]
//        | ==12?
//        [
//            #var A := WideLiteralToReal::valUM.
//            #var aResult := std'math'Rounded::(std'math'sqrt::std'math'pi * A * A / 4).
//            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
//        ]
//        | ==13?
//        [
//            #var A := WideLiteralToReal::valUM.
//            #var B := WideLiteralToReal::valDois.
//            #var C := WideLiteralToReal::valTres.
//            #var P := A + B + C / 2.
//            #var val1 := P-A.
//            #var val2 := P-B.
//            #var val3 := P-C.
//            #var aResult := std'math'sqrt::P*(val1 * val2 * val3).
//            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
//        ]
//        | ==14?
//        [
//            #var Alfa := WideLiteralToReal::valUM.
//            #var A := WideLiteralToReal::valDois.
//            #var B := WideLiteralToReal::valTres.
//            #var AlfaConv := (std'math'pi * Alfa) / 180.
//            #var sinAl := std'math'sin::AlfaConv.
//            #var aResult := sinAl * a * b / 2.
//            theResult << ("Res: " + ext'text'PrintableValue::aResult + "m^2").
//        ].
//        valor caption'set:theResult.
//    ]
}