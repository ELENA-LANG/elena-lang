#import system.

#class Label
{
    #field type.
    #field labelName.
    
    #constructor new : aName
    [
        labelName := aName.
        type := assembler'TypeInstruction Label.
    ]
    
    #method Name = labelName.
    #method Name : aName [ labelName := aName. ]
    #method Type = type.
}