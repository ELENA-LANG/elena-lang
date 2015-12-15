#import system.

#class Label
{
//    #field type.
    #field theLabelName.
    
//    #constructor new : aName
//    [
//        labelName := aName.
//        type := assembler'TypeInstruction Label.
//    ]
//    
//    #method Name = labelName.
//    #method Name : aName [ labelName := aName. ]
//    #method Type = type.

    #method castType : target
        = target eval &label:$self.
}