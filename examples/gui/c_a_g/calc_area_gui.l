// --- default namespaces ---
//import system'math.
//import system'drawing.
import forms;
//import extensions.

public class MainWindow : SDIDialog
{
    // Panels
    Panel pnlFormula;

    // Text Box
    Edit valUm;
    Edit valDois;
    Edit valTres;

    // Buttons
    Button BtmCalc;
    Button BtmInfo;
    Button BtmSalv;
    Button BtmSair;

    // Labels
    Label valor;
    Label info;
    Label letter1;
    Label letter2;
    Label letter3;

    // Radio Button
    RadioButtonGroup Tconta;
	RadioButtonGroup Econta1;
	RadioButtonGroup Econta2;
	RadioButtonGroup Econta3;
    RadioButtonGroup Econta4;

//    // Image Formula
//    object theImageList.
//    object thePaintbox.

    constructor new()
        <= new()
    {
        // -----------------------------------------------------------
        // Panels
        pnlFormula   := Panel.new();
        self.appendControl:pnlFormula;

        // Text Box
        valUm        := Edit.new();
        valDois      := Edit.new();
        valTres      := Edit.new();
        self
            .appendControl:valUm
            .appendControl:valDois
            .appendControl:valTres;

        // Buttons
        BtmCalc      := Button.new();
        BtmInfo      := Button.new();
        BtmSalv      := Button.new();
        BtmSair      := Button.new();
        self
            .appendControl:BtmCalc
            .appendControl:BtmInfo
            .appendControl:BtmSalv
            .appendControl:BtmSair;

        // Labels
        valor        := Label.new();
        info         := Label.new();
        letter1      := Label.new();
        letter2      := Label.new();
        letter3      := Label.new();
        self
            .appendControl:valor
            .appendControl:info
            .appendControl:letter1
            .appendControl:letter2
            .appendControl:letter3;

        // Radio Button
        Tconta       := RadioButtonGroup.new();
		Econta1      := RadioButtonGroup.new();
		Econta2      := RadioButtonGroup.new();
		Econta3      := RadioButtonGroup.new();
        Econta4      := RadioButtonGroup.new();
        self
            .appendControl:Tconta
            .appendControl:Econta1
            .appendControl:Econta2
            .appendControl:Econta3
            .appendControl:Econta4;

//        // Images Formulas
//        thePaintbox  := Imagebox new.
//        theImageList := ImageList new.
//        pnlFormula controls; 
//            append:theImageList;
//            append:thePaintbox.
//        // -----------------------------------------------------------

        // Form ------------------------------------------------------------------------
        self.setRegion(250, 200, 600, 400);
        self.Caption := "Calc. Area GUI :-------------------------: ver. 0.4";

        // Panels ----------------------------------------------------------------------
        pnlFormula.setRegion(190, 200, 400, 170);
        pnlFormula.Caption := "Formula";            

        // Radio Button ----------------------------------------------------------------
        Tconta.setRegion(7, 1, 150, 100);
        Tconta.Caption := "Escolha do calculo";
        Tconta
            .appendItem:"&Paralelogramos"
            .appendItem:"&Trapezio"
            .appendItem:"&Circulos"
            .appendItem:"&Triangulos";

//        Econta1
        Econta1.setRegion(7, 105, 150, 100);
        Econta1.Caption := " Escolha do calculo ";
        Econta1
            .appendItem:"&Retangulo"
            .appendItem:"&Quadrado"
            .appendItem:"&Paralelogramo"
            .appendItem:"&Losango";

        Econta2.setRegion(7, 105, 150, 100);
        Econta2.Caption := " Escolha do calculo ";
        Econta2
            .appendItem:"&Retangulo"
            .appendItem:"&Isosceles"
            .appendItem:"&Escaleno";
        Econta2.Visible := false;

        Econta3.setRegion(7, 105, 150, 100);
        Econta3.Caption := " Escolha do calculo ";
        Econta3
            .appendItem:"&Circulo"
            .appendItem:"&Coroa circular"
            .appendItem:"&Setor circular";
        Econta3.Visible := false;

        Econta4.setRegion(7, 105, 150, 100);
        Econta4.Caption := " Escolha do calculo ";
        Econta4
            .appendItem:"&Qualquer"
            .appendItem:"&Retangulo"
            .appendItem:"&Func dos lados"
            .appendItem:"&Retangulo"
            .appendItem:"&Func lds ang";
        Econta4.Visible := false;

//        Tconta set selected_index:0.
//        Econta1 set selected_index:0.

        // Text Box ---------------------------------------------------------------------
        valUm.setRegion(185, 10, 150, 22);

        valDois.setRegion(185, 40, 150, 22);
        
        valTres.setRegion(185, 70, 150, 22);

        // Buttons -----------------------------------------------------------------------
        BtmCalc.setRegion(430, 10, 150, 22);
        BtmCalc.Caption := "Calcular";
        
        BtmInfo.setRegion(430, 40, 150, 22);
        BtmInfo.Caption := "Infos";
        
        BtmSalv.setRegion(430, 70, 150, 22);
        BtmSalv.Caption := "Salvar";

        BtmSair.setRegion(430, 100, 150, 22);
        BtmSair.Caption := "Sair";

        // Labels ------------------------------------------------------------------------
        valor.setRegion(170, 105, 245, 22);
        valor.Caption := "Res: ";
        
        info.setRegion(170, 135, 250, 22);
        info.Caption := "Info: ";
        
        letter1.setRegion(170, 15, 14, 22);
        
        letter2.setRegion(170, 45, 14, 22);
        
        letter3.setRegion(170, 75, 14, 22);

//        // Image Formula -----------------------------------------------------------------
//        thePaintbox set x:12 y:22.
//        thePaintbox set width:379 height:139.
//
//        // Image List Formulas -----------------------------------------------------------
//        theImageList assign:thePaintbox.
//        theImageList set width:379 height:139.
//        theImageList append path:"..\formulas\Paralelogramos\bitmap1.bmp".
//        theImageList append path:"..\formulas\Paralelogramos\bitmap2.bmp".
//        theImageList append path:"..\formulas\Paralelogramos\bitmap3.bmp".
//		theImageList append path:"..\formulas\Paralelogramos\bitmap4.bmp".
//
//        theImageList append path:"..\formulas\Trapezio\bitmap1.bmp".
//        theImageList append path:"..\formulas\Trapezio\bitmap2.bmp".
//        theImageList append path:"..\formulas\Trapezio\bitmap3.bmp".
//
//        theImageList append path:"..\formulas\Circulo\bitmap1.bmp".
//        theImageList append path:"..\formulas\Circulo\bitmap2.bmp".
//        theImageList append path:"..\formulas\Circulo\bitmap3.bmp".
//        
//        theImageList append path:"..\formulas\Triangulos\bitmap1.bmp".
//        theImageList append path:"..\formulas\Triangulos\bitmap2.bmp".
//        theImageList append path:"..\formulas\Triangulos\bitmap3.bmp".
//        theImageList append path:"..\formulas\Triangulos\bitmap4.bmp".
//        theImageList append path:"..\formulas\Triangulos\bitmap5.bmp".
//
//        // Events ------------------------------------------------------------------------
//        // Radio Buttons -----------------------------------------------------------------
//        Tconta set onIndexChanged: (:args)[ $self $TipoDoCalculo:(Tconta selected_index) ].
//
//        Econta1 set onIndexChanged: (:args)[ $self $TipoDoObj:(Econta1 selected_index). ].
//        Econta2 set onIndexChanged: (:args)[ $self $TipoDoObj:(4 + Econta2 selected_index). ].
//        Econta3 set onIndexChanged: (:args)[ $self $TipoDoObj:(7 + Econta3 selected_index). ].
//        Econta4 set onIndexChanged: (:args)[ $self $TipoDoObj:(10 + Econta4 selected_index). ].
//
//        // Buttons ----------------------------------------------------------------------
//        BtmCalc set onClick: (:args)
//            [ $self $onButtonClick ].
//       
//        BtmInfo set onClick: (:args)
//            [ $self $onButtonClickInfo:$self ].
////            
////        BtmSalv~eevents +=
////            { onclick'eval = self $onButtonClickSave. }.
//
//        BtmSair set onClick:(:args)
//            [ 'program stop. ].
    }

//    $TipoDoCalculo : anIndex
//    [
//        Econta1 visible := false.
//        Econta2 visible := false.
//        Econta3 visible := false.
//        Econta4 visible := false.
//
//        anIndex =>
//            0 [
//                    Econta1 visible := true.
//
//                    Econta1 selected_index := 0
//              ];
//            1 [
//                    Econta2 visible := true.
//
//                    Econta2 selected_index := 0
//              ];
//            2 [
//                    Econta3 visible := true.
//
//                    Econta3 selected_index := 0
//              ];
//            3 [
//                    Econta4 visible := true.
//
//                    Econta4 selected_index := 0
//              ].
//    }
    
//    $TipoDoObj : anInicio
//	[
//        theImageList selected_index := anInicio.
//     
//        valTres visible := false.
//        valDois visible := false.
//        letter3 visible := false.
//        letter2 visible := false.
//     
//        valUm   caption := "".
//        valDois caption := "".
//        valTres caption :="".
//     
//        valor caption := "Res: ".
//        info caption  := "Info: ".
//                
//        anInicio =>
//            0 [
//                valUm   visible := true.
//                valDois visible := true.
//                letter1 visible := true.
//                letter2 visible := true.
//            
//                letter1 caption := "h: ".
//                letter2 caption := "b: ".
//            ];            
//            1 [
//                valUm   visible := true.
//                letter1 visible := true.
//            
//                letter1 caption := "b: ".
//            ];            
//            2 [
//                valUm   visible := true.
//                valDois visible := true.
//                letter1 visible := true.
//                letter2 visible := true.
//            
//                letter1 caption := "h: ".
//                letter2 caption := "b: ".
//            ];            
//            3 [
//                valUm   visible := true.
//                valDois visible := true.
//                letter1 visible := true.
//                letter2 visible := true.
//            
//                letter1 caption := "D: ".
//                letter2 caption := "d: ".
//            ];            
//            4 [
//                valUm   visible := true.
//                valDois visible := true.
//                valTres visible := true.
//                letter1 visible := true.
//                letter2 visible := true.
//                letter3 visible := true.
//            
//                letter1 caption := "B: ".
//                letter2 caption := "b: ".
//                letter3 caption := "h: ".
//            ];
//            5 [
//                valUm   visible := true.
//                valDois visible := true.
//                valTres visible := true.
//                letter1 visible := true.
//                letter2 visible := true.
//                letter3 visible := true.
//            
//                letter1 caption := "B: ".
//                letter2 caption := "b: ".
//                letter3 caption := "h: ".
//            ];
//            6 [
//                valUm   visible := true.
//                valDois visible := true.
//                valTres visible := true.
//                letter1 visible := true.
//                letter2 visible := true.
//                letter3 visible := true.
//            
//                letter1 caption := "B: ".
//                letter2 caption := "b: ".
//                letter3 caption := "h: ".
//            ];
//            7 [
//                valUm   visible := true.
//                letter1 visible := true.
//            
//                letter1 caption := "r: ".
//            ];
//            8 [
//                valUm   visible := true.
//                valDois visible := true.
//                letter1 visible := true.
//                letter2 visible := true.
//            
//                letter1 caption := "R: ".
//                letter2 caption := "r: ".
//            ];
//            9 [
//                valUm   visible := true.
//                valDois visible := true.
//                letter1 visible := true.
//                letter2 visible := true.
//            
//                letter1 caption := "Al: ".
//                letter2 caption := "r: ".
//            
//                info caption := "Info: Al = Alfa ( angulo )".
//            ];
//            10 [
//                valUm   visible := true.
//                valDois visible := true.
//                letter1 visible := true.
//                letter2 visible := true.
//            
//                letter1 caption := "B: ".
//                letter2 caption := "H: ".
//            ];
//            11 [
//                valUm   visible := true.
//                valDois visible := true.
//                letter1 visible := true.
//                letter2 visible := true.
//            
//                letter1 caption := "B: ".
//                letter2 caption := "C: ".
//            ];
//            12 [
//                valUm   visible := true.
//                letter1 visible := true.
//            
//                letter1 caption := "A: ".
//            ];
//            13 [
//                valUm   visible := true.
//                valDois visible := true.
//                valTres visible := true.
//                letter1 visible := true.
//                letter2 visible := true.
//                letter3 visible := true.
//            
//                letter1 caption := "a: ".
//                letter2 caption := "b: ".
//                letter3 caption := "c: ".
//            ];
//            14 [
//                valUm   visible := true.
//                valDois visible := true.
//                valTres visible := true.
//                letter1 visible := true.
//                letter2 visible := true.
//                letter3 visible := true.
//            
//                letter1 caption := "Al: ".
//                letter2 caption := "a: ".
//                letter3 caption := "b: ".
//            
//                info caption := "Info: Al = Alfa ( angulo )".
//            ].
//
//		thePaintbox refresh.
//	]            
// 
//    $onButtonClick
//    [   
//        var aResult := 0.
//                
//        (theImageList selected_index) =>
//            0 [
//                var H := valUm value; toReal.
//                var B := valDois value; toReal.
//                aResult := H * B.
//            ];
//            1 [
//                var B := valUm value; toReal.
//                aResult := B * B.
//            ];
//            2 [
//                var H := valUm value; toReal.
//                var B := valDois value; toReal.
//                aResult := H * B.
//            ];
//            3 [
//                var DM := valUm value; toReal.
//                var d := valDois value; toReal.
//                aResult := DM * d / 2.
//            ];
//            4 [
//                var BM := valUm value; toReal.
//                var b := valDois value; toReal.
//                var H := valTres value; toReal.
//                aResult := (BM + b)*H / 2.
//            ];
//            5 [
//                var BM := valUm value; toReal.
//                var b := valDois value; toReal.
//                var H := valTres value; toReal.
//                aResult := (BM + b)*H / 2.
//            ];
//            6 [
//                var BM := valUm value; toReal.
//                var b := valDois value; toReal.
//                var H := valTres value; toReal.
//                aResult := (BM + b)*H / 2.
//            ];
//            7 [
//                var R := valUm value; toReal.
//                aResult := pi_value * R * R
//            ];
//            8 [
//                var RM := valUm value; toReal.
//                var r := valDois value; toReal.
//                aResult := pi_value * (RM * RM - r * r).
//            ];
//            9 [
//                var Alfa := valUm value; toReal.
//                var R := valDois value; toReal.
//                aResult := Alfa * pi_value * R * R / 360
//            ];
//            10 [
//                var B := valUm value; toReal.
//                var H := valDois value; toReal.
//                aResult := B * H / 2.
//            ];
//            11 [
//                var B := valUm value; toReal.
//                var C := valDois value; toReal.
//                aResult := B * C / 2.
//            ];
//            12 [
//                var A := valUm value; toReal.
//                aResult := ((pi_value sqrt) * A * A / 4) rounded.
//            ];
//            13 [
//                var A := valUm value; toReal.
//                var B := valDois value; toReal.
//                var C := valTres value; toReal.
//                var P := A + B + C / 2.
//                var val1 := P-A.
//                var val2 := P-B.
//                var val3 := P-C.
//                aResult := (P * (val1 * val2 * val3)) sqrt.
//            ];
//            14 [
//                var Alfa := valUm value; toReal.
//                var A := valDois value; toReal.
//                var B := valTres value; toReal.
//                var AlfaConv := (pi_value * Alfa) / 180.
//                var sinAl := AlfaConv sin.
//                aResult := sinAl * A * B / 2.
//            ].
//            
//        valor caption := ("Res: " + aResult literal + " m^2").                    
//    ]
//    
//    $onButtonClickInfo : aForm
//    [
//        calc_area_gui'InfoForm new parent:aForm; run.
//    ]
//    
//    dispatch => form.
}