// --- default namespaces ---
//import system'math.
//import system'drawing.
import forms;
import extensions;

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

    // Image Formula
    ImageList theImageList;
    Imagebox thePaintbox;

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

        // Images Formulas
        thePaintbox  := Imagebox.new();
        theImageList := ImageList.new();
        pnlFormula 
            .appendControl:theImageList
            .appendControl:thePaintbox;
        // -----------------------------------------------------------

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

        Tconta.SelectedIndex := 0;
        Econta1.SelectedIndex := 0;

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

        // Image Formula -----------------------------------------------------------------
        thePaintbox.setRegion(12, 22, 379, 139);

        // Image List Formulas -----------------------------------------------------------
        theImageList.assign:thePaintbox;
        theImageList.setRegion(0, 0, 379, 139);
        theImageList.appendImage:"..\formulas\Paralelogramos\bitmap1.bmp";
        theImageList.appendImage:"..\formulas\Paralelogramos\bitmap2.bmp";
        theImageList.appendImage:"..\formulas\Paralelogramos\bitmap3.bmp";
		theImageList.appendImage:"..\formulas\Paralelogramos\bitmap4.bmp";

        theImageList.appendImage:"..\formulas\Trapezio\bitmap1.bmp";
        theImageList.appendImage:"..\formulas\Trapezio\bitmap2.bmp";
        theImageList.appendImage:"..\formulas\Trapezio\bitmap3.bmp";

        theImageList.appendImage:"..\formulas\Circulo\bitmap1.bmp";
        theImageList.appendImage:"..\formulas\Circulo\bitmap2.bmp";
        theImageList.appendImage:"..\formulas\Circulo\bitmap3.bmp";
        
        theImageList.appendImage:"..\formulas\Triangulos\bitmap1.bmp";
        theImageList.appendImage:"..\formulas\Triangulos\bitmap2.bmp";
        theImageList.appendImage:"..\formulas\Triangulos\bitmap3.bmp";
        theImageList.appendImage:"..\formulas\Triangulos\bitmap4.bmp";
        theImageList.appendImage:"..\formulas\Triangulos\bitmap5.bmp";

        // Events ------------------------------------------------------------------------
        // Radio Buttons -----------------------------------------------------------------
        Tconta.onIndexChanged := (args){ self.TipoDoCalculo(Tconta.SelectedIndex) };

        Econta1.onIndexChanged := (args){ self.TipoDoObj(Econta1.SelectedIndex) };
        Econta2.onIndexChanged := (args){ self.TipoDoObj(4 + Econta2.SelectedIndex) };
        Econta3.onIndexChanged := (args){ self.TipoDoObj(7 + Econta3.SelectedIndex) };
        Econta4.onIndexChanged := (args){ self.TipoDoObj(10 + Econta4.SelectedIndex) };

        // Buttons ----------------------------------------------------------------------
        BtmCalc.onClick := (args)
            { self.onButtonClick() };
       
//        BtmInfo set onClick: (:args)
//            { $self $onButtonClickInfo:$self ].
////            
////        BtmSalv~eevents +=
////            { onclick'eval = self $onButtonClickSave. }.

        BtmSair.onClick := (args)
            { forward program.stop() }
    }

    private TipoDoCalculo(int index)
    {
        Econta1.Visible := false;
        Econta2.Visible := false;
        Econta3.Visible := false;
        Econta4.Visible := false;

        index =>
            0 {
                    Econta1.Visible := true;

                    Econta1.SelectedIndex := 0
              }
            1 {
                    Econta2.Visible := true;

                    Econta2.SelectedIndex := 0
              }
            2 {
                    Econta3.Visible := true;

                    Econta3.SelectedIndex := 0
              }
            3 {
                    Econta4.Visible := true;

                    Econta4.SelectedIndex := 0
              }
    }
    
    private TipoDoObj(int index)
	{
        theImageList.SelectedIndex := index;
     
        valTres.Visible := false;
        valDois.Visible := false;
        letter3.Visible := false;
        letter2.Visible := false;
     
        valUm.Value := "";
        valDois.Value := "";
        valTres.Value :="";
     
        valor.Caption := "Res: ";
        info.Caption  := "Info: ";
                
        index =>
            0 {
                valUm.Visible := true;
                valDois.Visible := true;
                letter1.Visible := true;
                letter2.Visible := true;
            
                letter1.Caption := "h: ";
                letter2.Caption := "b: ";
            }           
            1 {
                valUm.Visible := true;
                letter1.Visible := true;
            
                letter1.Caption := "b: "
            }
            2 {
                valUm.Visible := true;
                valDois.Visible := true;
                letter1.Visible := true;
                letter2.Visible := true;
            
                letter1.Caption := "h: ";
                letter2.Caption := "b: ";
            }            
            3 {
                valUm.Visible := true;
                valDois.Visible := true;
                letter1.Visible := true;
                letter2.Visible := true;
            
                letter1.Caption := "D: ";
                letter2.Caption := "d: ";
            }           
            4 {
                valUm.Visible := true;
                valDois.Visible := true;
                valTres.Visible := true;
                letter1.Visible := true;
                letter2.Visible := true;
                letter3.Visible := true;
            
                letter1.Caption := "B: ";
                letter2.Caption := "b: ";
                letter3.Caption := "h: ";
            }
            5 {
                valUm  .Visible := true;
                valDois.Visible := true;
                valTres.Visible := true;
                letter1.Visible := true;
                letter2.Visible := true;
                letter3.Visible := true;
            
                letter1.Caption := "B: ";
                letter2.Caption := "b: ";
                letter3.Caption := "h: ";
            }
            6 {
                valUm  .Visible := true;
                valDois.Visible := true;
                valTres.Visible := true;
                letter1.Visible := true;
                letter2.Visible := true;
                letter3.Visible := true;
            
                letter1.Caption := "B: ";
                letter2.Caption := "b: ";
                letter3.Caption := "h: ";
            }
            7 {
                valUm  .Visible := true;
                letter1.Visible := true;
            
                letter1.Caption := "r: ";
            }
            8 {
                valUm  .Visible := true;
                valDois.Visible := true;
                letter1.Visible := true;
                letter2.Visible := true;
            
                letter1.Caption := "R: ";
                letter2.Caption := "r: ";
            }
            9 {
                valUm  .Visible := true;
                valDois.Visible := true;
                letter1.Visible := true;
                letter2.Visible := true;
            
                letter1.Caption := "Al: ";
                letter2.Caption := "r: ";
            
                info.Caption := "Info: Al = Alfa ( angulo )"
            }
            10 {
                valUm  .Visible := true;
                valDois.Visible := true;
                letter1.Visible := true;
                letter2.Visible := true;
            
                letter1.Caption := "B: ";
                letter2.Caption := "H: ";
            }
            11 {
                valUm  .Visible := true;
                valDois.Visible := true;
                letter1.Visible := true;
                letter2.Visible := true;
            
                letter1.Caption := "B: ";
                letter2.Caption := "C: ";
            }
            12 {
                valUm  .Visible := true;
                letter1.Visible := true;
            
                letter1.Caption := "A: ";
            }
            13 {
                valUm  .Visible := true;
                valDois.Visible := true;
                valTres.Visible := true;
                letter1.Visible := true;
                letter2.Visible := true;
                letter3.Visible := true;
            
                letter1.Caption := "a: ";
                letter2.Caption := "b: ";
                letter3.Caption := "c: ";
            }
            14 {
                valUm  .Visible := true;
                valDois.Visible := true;
                valTres.Visible := true;
                letter1.Visible := true;
                letter2.Visible := true;
                letter3.Visible := true;
            
                letter1.Caption := "Al: ";
                letter2.Caption := "a: ";
                letter3.Caption := "b: ";
            
                info.Caption := "Info: Al = Alfa ( angulo )"
            };

		thePaintbox.refresh()
	}            
 
    private onButtonClick()
    {   
        var result := 0;
                
        (theImageList.SelectedIndex) =>
            0 {
                var H := valUm.Value.toReal();
                var B := valDois.Value.toReal();
                result := H * B
            }
            1 {
                var B := valUm.Value.toReal();
                result := B * B
            }
            2 {
                var H := valUm.Value.toReal();
                var B := valDois.Value.toReal();
                result := H * B
            }
            3 {
                var DM := valUm.Value.toReal();
                var d := valDois.Value.toReal();
                result := DM * d / 2
            }
            4 {
                var BM := valUm.Value.toReal();
                var b := valDois.Value.toReal();
                var H := valTres.Value.toReal();
                result := (BM + b)*H / 2
            }
            5 {
                var BM := valUm.Value.toReal();
                var b := valDois.Value.toReal();
                var H := valTres.Value.toReal();
                result := (BM + b)*H / 2
            }
            6 {
                var BM := valUm.Value.toReal();
                var b := valDois.Value.toReal();
                var H := valTres.Value.toReal();
                result := (BM + b)*H / 2
            }
            7 {
                var R := valUm.Value.toReal();
                result := Pi_value * R * R
            }
            8 {
                var RM := valUm.Value.toReal();
                var r := valDois.Value.toReal();
                result := Pi_value * (RM * RM - r * r);
            }
            9 {
                var Alfa := valUm.Value.toReal();
                var R := valDois.Value.toReal();
                result := Alfa * Pi_value * R * R / 360
            }
            10 {
                var B := valUm.Value.toReal();
                var H := valDois.Value.toReal();
                result := B * H / 2;
            }
            11 {
                var B := valUm.Value.toReal();
                var C := valDois.Value.toReal();
                result := B * C / 2;
            }
            12 {
                var A := valUm.Value.toReal();
                result := ((Pi_value.sqrt()) * A * A / 4).Rounded;
            }
            13 {
                var A := valUm.Value.toReal();
                var B := valDois.Value.toReal();
                var C := valTres.Value.toReal();
                var P := A + B + C / 2;
                var val1 := P-A;
                var val2 := P-B;
                var val3 := P-C;
                result := (P * (val1 * val2 * val3)).sqrt()
            }
            14 {
                var Alfa := valUm.Value.toReal();
                var A := valDois.Value.toReal();
                var B := valTres.Value.toReal();
                var AlfaConv := (Pi_value * Alfa) / 180;
                var sinAl := AlfaConv.sin();
                result := sinAl * A * B / 2;
            };
            
        valor.Caption := ("Res: " + result.toString() + " m^2");
    }
    
    private onButtonClickInfo(form)
    {
        //calc_area_gui'InfoForm new parent:aForm; run.
    }
}