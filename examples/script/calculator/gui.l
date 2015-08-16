#define types'*    = sys'types'*.
#define forms'*    = gui'forms'*.
#define controls'* = gui'controls'*.
#define parser'*   = calc'parser'*.

#define std'basic'*.
#define std'dictionary'*.
#define sys'dynamic'*.
#define gui'dictionary'*.
#define gui'common'*.
#define gui'events'dictionary'*.

// --- Main Window --

#class CalcWindow (forms'SDIDialog)
{
    #field theEdit.
    #field theButton.

    #field theButton0.
    #field theButton1.
    #field theButton2.
    #field theButton3.
    #field theButton4.
    #field theButton5.
    #field theButton6.
    #field theButton7.
    #field theButton8.
    #field theButton9.
    #field theButtonDot.
    #field theButtonDiv.
    #field theButtonMul.
    #field theButtonSub.
    #field theButtonAdd.

//    #field theHistory.
//    #field theLabel.
//
//    #role EmptyHistory
//    {
//        #method $onHistoryClick
//        [
//        ]
//
//        #method $addToHistory : anExpression
//        [
//            theHistory->gprop'Items += anExpression.
//
//            #shift.
//        ]              
//    }

    #method forms'initializing
    [
        super forms'initializing.
        
        theEdit := controls'Edit::self.

        theButton0 := controls'Button::self.
        theButton1 := controls'Button::self.
        theButton2 := controls'Button::self.
        theButton3 := controls'Button::self.
        theButton4 := controls'Button::self.
        theButton5 := controls'Button::self.
        theButton6 := controls'Button::self.
        theButton7 := controls'Button::self.
        theButton8 := controls'Button::self.
        theButton9 := controls'Button::self.
        theButtonDot := controls'Button::self.
        theButtonDiv := controls'Button::self.
        theButtonMul := controls'Button::self.
        theButtonSub := controls'Button::self.
        theButtonAdd := controls'Button::self.
    
        theButton := controls'Button::self.

        theButton~eevents +=
            { onclick'eval = self $onButtonClick. }.

//        theLabel := self->gcomp'StaticLabel.
//        theHistory := self->gcomp'ListBox.

//        theHistory->sys'events'Handlers +=           
//            { gcomp'$onClick = self::$onHistoryClick. }.

        self
            set &width:310 &height:220
            set &x:50 &y:50
            set &caption:"Calculator ELENA Sample".

        theEdit << "1+12*3-14".
        theEdit
            set &width:200 &height:24
            set &x:10 &y:10
            open.

        theButton7
            set &width:30 &height:30
            set &x:10 &y:40
            set &caption:"7"
            open.

        theButton8
            set &width:30 &height:30
            set &x:45 &y:40
            set &caption:"8"
            open.

        theButton9
            set &width:30 &height:30
            set &x:80 &y:40
            set &caption:"9"
            open.

        theButton4
            set &width:30 &height:30
            set &x:10 &y:75
            set &caption:"4"
            open.

        theButton5
            set &width:30 &height:30
            set &x:45 &y:75
            set &caption:"5"
            open.

        theButton6
            set &width:30 &height:30
            set &x:80 &y:75
            set &caption:"6"
            open.

        theButton3
            set &width:30 &height:30
            set &x:10 &y:110
            set &caption:"3"
            open.

        theButton2
            set &width:30 &height:30
            set &x:45 &y:110
            set &caption:"2"
            open.

        theButton1
            set &width:30 &height:30
            set &x:80 &y:110
            set &caption:"1"
            open.

        theButton0
            set &width:65 &height:30
            set &x:10 &y:145
            set &caption:"0"
            open.

        theButtonDot
            set &width:30 &height:30
            set &x:80 &y:145
            set &caption:"."
            open.

        theButtonDiv
            set &width:30 &height:30
            set &x:115 &y:40
            set &caption:"/"
            open.

        theButtonMul
            set &width:30 &height:30
            set &x:115 &y:75
            set &caption:"*"
            open.

        theButtonSub
            set &width:30 &height:30
            set &x:115 &y:110
            set &caption:"-"
            open.

        theButtonAdd
            set &width:30 &height:30
            set &x:115 &y:145
            set &caption:"+"
            open.

        theButton
            set &width:70 &height:30
            set &x:220 &y:10
            set &caption:"Evaluate"
            open.

        #var Buttons := (theButton0, theButton1, theButton2, theButton3, theButton4, theButton5, theButton6, theButton7, theButton8,
                            theButton9, theButtonDot, theButtonDiv, theButtonMul, theButtonSub, theButtonAdd).

        Buttons~EGroupEvents += 
        {
            onclick &sender:aButton
            [
                #if (theEdit length == 0)?
                    [ theEdit << aButton caption. ]
                    | [ theEdit += aButton caption. ].
            ]
        }.

//        theLabel->gprop'Size << gprop'Point{ X=200. Y=20. }.
//        theLabel->gprop'Location << gprop'Point{ X=10. Y=45. }.
//        theLabel->gprop'Caption << "History:".
//        theLabel->gprop'Visible << basic'True.
//
//        theHistory->gprop'Size << gprop'Point{ X=200. Y=100. }.
//        theHistory->gprop'Location << gprop'Point{ X=10. Y=60. }.
//        theHistory->gprop'Visible << basic'True.

        // init factory
        parser'TokenFactoryCollection group 
            += parser'ESubExpression
            += parser'ENumeric
            += parser'ESummary
            += parser'EDifference
            += parser'EProduct
            += parser'EFraction.
    ]

//    #method $addToHistory : anExpression
//    [
//        (theHistory->gprop'Items->vprop'Indexer << { vprop'$Index= 0. }) insert:anExpression.
//    ]
//
//    #method $onHistoryClick
//    [
//        #var anExpression := (theHistory->gprop'Items @ theHistory->gprop'SelectedIndex)->basic'LiteralValue.
//
//        theEdit << anExpression.
//    ]

	#method $onButtonClick
	[
        //self $addToHistory:anExpression.
        theEdit << parser'ExpressionFactory::theEdit save:WideStrConvertor.
	]
}