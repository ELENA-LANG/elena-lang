// --- default namespaces ---
#define system.
#define system'math.
#define system'drawing.
#define forms.
#define extensions.

#class Window
{
    // Form
    #field form.
    
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

    #constructor new
    [
        form := SDIDialog new.
        
        // -----------------------------------------------------------
        // Panels
        pnlFormula   := Panel new.
        form controls append:pnlFormula.

        // Text Box
        valUm        := Edit new.
        valDois      := Edit new.
        valTres      := Edit new.
        form controls 
            append:valUm
            append:valDois
            append:valTres.

        // Buttons
        BtmCalc      := Button new.
        BtmInfo      := Button new.
        BtmSalv      := Button new.
        BtmSair      := Button new.
        form controls 
            append:BtmCalc
            append:BtmInfo
            append:BtmSalv
            append:BtmSair.

        // Labels
        valor        := Label new.
        info         := Label new.
        letter1      := Label new.
        letter2      := Label new.
        letter3      := Label new.
        form controls 
            append:valor
            append:info
            append:letter1
            append:letter2
            append:letter3.

        // Radio Button
        Tconta       := RadioButtonGroup new.
		Econta1      := RadioButtonGroup new.
		Econta2      := RadioButtonGroup new.
		Econta3      := RadioButtonGroup new.
        Econta4      := RadioButtonGroup new.
        form controls 
            append:Tconta
            append:Econta1
            append:Econta2
            append:Econta3
            append:Econta4.

        // Images Formulas
        thePaintbox  := Imagebox new.
        theImageList := ImageList new.
        pnlFormula controls 
            append:theImageList
            append:thePaintbox.
        // -----------------------------------------------------------

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

        Tconta set &selected_index:0.
        Econta1 set &selected_index:0.

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

        // Image Formula -----------------------------------------------------------------
        thePaintbox set &x:12 &y:22.
        thePaintbox set &width:379 &height:139.

        // Image List Formulas -----------------------------------------------------------
        theImageList assign:thePaintbox.
        theImageList set &width:379 &height:139.
        theImageList append &path:"..\formulas\Paralelogramos\bitmap1.bmp".
        theImageList append &path:"..\formulas\Paralelogramos\bitmap2.bmp".
        theImageList append &path:"..\formulas\Paralelogramos\bitmap3.bmp".
		theImageList append &path:"..\formulas\Paralelogramos\bitmap4.bmp".

        theImageList append &path:"..\formulas\Trapezio\bitmap1.bmp".
        theImageList append &path:"..\formulas\Trapezio\bitmap2.bmp".
        theImageList append &path:"..\formulas\Trapezio\bitmap3.bmp".

        theImageList append &path:"..\formulas\Circulo\bitmap1.bmp".
        theImageList append &path:"..\formulas\Circulo\bitmap2.bmp".
        theImageList append &path:"..\formulas\Circulo\bitmap3.bmp".
        
        theImageList append &path:"..\formulas\Triangulos\bitmap1.bmp".
        theImageList append &path:"..\formulas\Triangulos\bitmap2.bmp".
        theImageList append &path:"..\formulas\Triangulos\bitmap3.bmp".
        theImageList append &path:"..\formulas\Triangulos\bitmap4.bmp".
        theImageList append &path:"..\formulas\Triangulos\bitmap5.bmp".

        // Events ------------------------------------------------------------------------
        // Radio Buttons -----------------------------------------------------------------
        Tconta set &onIndexChanged: args [ $self $TipoDoCalculo:(Tconta selected_index). ].

        Econta1 set &onIndexChanged: args [ $self $TipoDoObj:(Econta1 selected_index). ].
        Econta2 set &onIndexChanged: args [ $self $TipoDoObj:(4 + Econta2 selected_index). ].
        Econta3 set &onIndexChanged: args [ $self $TipoDoObj:(7 + Econta3 selected_index). ].
        Econta4 set &onIndexChanged: args [ $self $TipoDoObj:(10 + Econta4 selected_index). ].

        // Buttons ----------------------------------------------------------------------
        BtmCalc set &onClick:args
            [ $self $onButtonClick. ].
       
        #var aForm := $self. // !! should be possible to pass self
        BtmInfo set &onClick:args
            [ $self $onButtonClickInfo:aForm. ].
//            
//        BtmSalv~eevents +=
//            { onclick'eval = self $onButtonClickSave. }.

        BtmSair set &onClick:args
            [ 'program stop. ].
    ]

    #method $TipoDoCalculo : anIndex
    [
        Econta1 set &visible:false.
        Econta2 set &visible:false.
        Econta3 set &visible:false.
        Econta4 set &visible:false.

        anIndex =>
            0 ? [
                    Econta1 set &visible:true.

                    Econta1 set &selected_index:0.
                ]
            1 ? [
                    Econta2 set &visible:true.

                    Econta2 set &selected_index:0.
                ]
            2 ? [
                    Econta3 set &visible:true.

                    Econta3 set &selected_index:0.
                ]
            3 ? [
                    Econta4 set &visible:true.

                    Econta4 set &selected_index:0.
                ].
    ]
    
    #method $TipoDoObj : anInicio
	[
        theImageList set &selected_index:anInicio.
     
        valTres set &visible:false.
        valDois set &visible:false.
        letter3 set &visible:false.
        letter2 set &visible:false.
     
        valUm set &caption:"".
        valDois set &caption:"".
        valTres set &caption:"".
     
        valor set &caption:"Res: ".
        info set &caption:"Info: ".
                
        anInicio =>
            0 ? [
                valUm set &visible:true.
                valDois set &visible:true.
                letter1 set &visible:true.
                letter2 set &visible:true.
            
                letter1 set &caption:"h: ".
                letter2 set &caption:"b: ".
            ]             
            1 ? [
                valUm set &visible:true.
                letter1 set &visible:true.
            
                letter1 set &caption:"b: ".
            ]             
            2 ? [
                valUm set &visible:true.
                valDois set &visible:true.
                letter1 set &visible:true.
                letter2 set &visible:true.
            
                letter1 set &caption:"h: ".
                letter2 set &caption:"b: ".
            ]             
            3 ? [
                valUm set &visible:true.
                valDois set &visible:true.
                letter1 set &visible:true.
                letter2 set &visible:true.
            
                letter1 set &caption:"D: ".
                letter2 set &caption:"d: ".
            ]             
            4 ? [
                valUm set &visible:true.
                valDois set &visible:true.
                valTres set &visible:true.
                letter1 set &visible:true.
                letter2 set &visible:true.
                letter3 set &visible:true.
            
                letter1 set &caption:"B: ".
                letter2 set &caption:"b: ".
                letter3 set &caption:"h: ".
            ]             
            5 ? [
                valUm set &visible:true.
                valDois set &visible:true.
                valTres set &visible:true.
                letter1 set &visible:true.
                letter2 set &visible:true.
                letter3 set &visible:true.
            
                letter1 set &caption:"B: ".
                letter2 set &caption:"b: ".
                letter3 set &caption:"h: ".
            ]             
            6 ? [
                valUm set &visible:true.
                valDois set &visible:true.
                valTres set &visible:true.
                letter1 set &visible:true.
                letter2 set &visible:true.
                letter3 set &visible:true.
            
                letter1 set &caption:"B: ".
                letter2 set &caption:"b: ".
                letter3 set &caption:"h: ".
            ]             
            7 ? [
                valUm set &visible:true.
                letter1 set &visible:true.
            
                letter1 set &caption:"r: ".
            ]             
            8 ? [
                valUm set &visible:true.
                valDois set &visible:true.
                letter1 set &visible:true.
                letter2 set &visible:true.
            
                letter1 set &caption:"R: ".
                letter2 set &caption:"r: ".
            ]             
            9 ? [
                valUm set &visible:true.
                valDois set &visible:true.
                letter1 set &visible:true.
                letter2 set &visible:true.
            
                letter1 set &caption:"Al: ".
                letter2 set &caption:"r: ".
            
                info set &caption:"Info: Al = Alfa ( angulo )".
            ]             
            10 ? [
                valUm set &visible:true.
                valDois set &visible:true.
                letter1 set &visible:true.
                letter2 set &visible:true.
            
                letter1 set &caption:"B: ".
                letter2 set &caption:"H: ".
            ]             
            11 ? [
                valUm set &visible:true.
                valDois set &visible:true.
                letter1 set &visible:true.
                letter2 set &visible:true.
            
                letter1 set &caption:"B: ".
                letter2 set &caption:"C: ".
            ]             
            12 ? [
                valUm set &visible:true.
                letter1 set &visible:true.
            
                letter1 set &caption:"A: ".
            ]             
            13 ? [
                valUm set &visible:true.
                valDois set &visible:true.
                valTres set &visible:true.
                letter1 set &visible:true.
                letter2 set &visible:true.
                letter3 set &visible:true.
            
                letter1 set &caption:"a: ".
                letter2 set &caption:"b: ".
                letter3 set &caption:"c: ".
            ]             
            14 ? [
                valUm set &visible:true.
                valDois set &visible:true.
                valTres set &visible:true.
                letter1 set &visible:true.
                letter2 set &visible:true.
                letter3 set &visible:true.
            
                letter1 set &caption:"Al: ".
                letter2 set &caption:"a: ".
                letter3 set &caption:"b: ".
            
                info set &caption:"Info: Al = Alfa ( angulo )".
            ].

		thePaintbox refresh.
	]            
 
    #method $onButtonClick
    [   
        #var aResult := 0.
                
        (theImageList selected_index) =>
            0 ? [
                #var H := valUm value toReal.
                #var B := valDois value toReal.
                aResult := H * B.
            ]             
            1 ? [
                #var B := valUm value toReal.
                aResult := B * B.
            ]             
            2 ? [
                #var H := valUm value toReal.
                #var B := valDois value toReal.
                aResult := H * B.
            ]             
            3 ? [
                #var DM := valUm value toReal.
                #var d := valDois value toReal.
                aResult := DM * d / 2.
            ]             
            4 ? [
                #var BM := valUm value toReal.
                #var b := valDois value toReal.
                #var H := valTres value toReal.
                aResult := (BM + b)*H / 2.
            ]             
            5 ? [
                #var BM := valUm value toReal.
                #var b := valDois value toReal.
                #var H := valTres value toReal.
                aResult := (BM + b)*H / 2.
            ]             
            6 ? [
                #var BM := valUm value toReal.
                #var b := valDois value toReal.
                #var H := valTres value toReal.
                aResult := (BM + b)*H / 2.
            ]             
            7 ? [
                #var R := valUm value toReal.
                aResult := pi_value * R * R.
            ]             
            8 ? [
                #var RM := valUm value toReal.
                #var r := valDois value toReal.
                aResult := pi_value * (RM * RM - r * r).
            ]             
            9 ? [
                #var Alfa := valUm value toReal.
                #var R := valDois value toReal.
                aResult := Alfa * pi_value * R * R / 360.
            ]             
            10 ? [
                #var B := valUm value toReal.
                #var H := valDois value toReal.
                aResult := B * H / 2.
            ]             
            11 ? [
                #var B := valUm value toReal.
                #var C := valDois value toReal.
                aResult := B * C / 2.
            ]             
            12 ? [
                #var A := valUm value toReal.
                aResult := ((pi_value sqrt) * A * A / 4) rounded.
            ]             
            13 ? [
                #var A := valUm value toReal.
                #var B := valDois value toReal.
                #var C := valTres value toReal.
                #var P := A + B + C / 2.
                #var val1 := P-A.
                #var val2 := P-B.
                #var val3 := P-C.
                aResult := (P * (val1 * val2 * val3)) sqrt.
            ]             
            14 ? [
                #var Alfa := valUm value toReal.
                #var A := valDois value toReal.
                #var B := valTres value toReal.
                #var AlfaConv := (pi_value * Alfa) / 180.
                #var sinAl := AlfaConv sin.
                aResult := sinAl * A * B / 2.
            ].
            
        valor set &caption:("Res: " + aResult literal + " m^2").                    
    ]
    
    #method $onButtonClickInfo : aForm
    [
        calc_area_gui'InfoForm new &parent:aForm run.
    ]
    
    #method => form.
}