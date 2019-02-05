import forms;
//import extensions.

public class MainWindow : SDIDialog
{
//    object lblClicks.
//    object btmClickMe.
//    
//    //Store how much clicks the user doed
//    object clicksCount.
    
    constructor new()
       <= new()
    {        
//        lblClicks := Label new.
//        btmClickMe := Button new.
//        
//        clicksCount := 0.
//        
//        form controls; 
//            append:lblClicks;
//            append:btmClickMe.
//    
        self.Caption := "Rosseta Code";
        self.setRegion(100, 100, 160, 80);
    
//        lblClicks 
//            set x:10 y:2;
//            set width:160 height:20;
//            set caption:"Clicks: 0".
//        
//        btmClickMe 
//            set x:7 y:20;
//            set width:140 height:30;
//            set caption:"Click me";
//            set onClick(:args)[ $self $onButtonClick ]
    }
    
//    $onButtonClick
//    [
//        clicksCount := clicksCount + 1.
//        lblClicks set caption("Clicks: " + clicksCount literal).
//    ]
//    
//    dispatch => form.
}