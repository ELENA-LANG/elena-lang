#define std'dictionary'*.
#define std'patterns'*.
#define ext'routines'*.

#subject __amb_index, __amb_available.

// --- Joinable --

#symbol Joinable &former:aFirst &later:aSecond
    = aFirst @ (aFirst length - 1) == aSecond @ 0.

// --- AmbIndexer ---

#class AmbIndexer (BaseIndexer)
{
    #field theAmbValue.
    
    #initializer : Values
    [
        theAmbValue := Values.
    ]
     
    #method index = theAmbValue __amb_index.
     
    #method index'write : anIndex
    [
        theAmbValue __amb_index'write:anIndex.
    ]

    #method available'is = theAmbValue __amb_available'is.
            
    #method content = theAmbValue.
}

// --- AmbValue ---

#class AmbValue
{
    #field theValues.
    #field theCurrent.

    #role Unassigned
    {
        #method __amb_index = 0.
        
        #method __amb_index'write : anIndex
        [
            theCurrent := theValues indexer.
            
            #shift.
            
            self __amb_index'write:anIndex.
        ]
    }
    
    #initializer : Values
    [
        theValues := Values.
        theCurrent := nil.
        
        #shift Unassigned.
    ]
            
    #method indexer = AmbIndexer::$self.

    #method __amb_index = theCurrent index.

    #method __amb_available'is = theCurrent available'is.

    #method __amb_index'write : anIndex
    [
        theCurrent index'write:anIndex.
    ]
    
    #outer theCurrent content.
}

// --- AmbOperator ---

#class AmbOperator
{
    #method seek : anExpression
    [
        (CombinatorWithRepetition::self)~foreach seek: anExpression.
    ]
}

// --- Program ---

#symbol Program1 =
[
    #var A := AmbValue::(1, 2, 3).
    #var B := AmbValue::(4, 5, 6).

    (A, B)~AmbOperator seek: = (A * B == 8).

    'program'Output << A << "*" << B << "=8".
].

#symbol Program2 =
[
    #var A := AmbValue::("the","that","a").
    #var B := AmbValue::("frog", "elephant", "thing").
    #var C := AmbValue::("walked", "treaded", "grows").
    #var D := AmbValue::("slowly", "quickly").
    
    (A, B, C, D)~AmbOperator seek:
        = (joinable &&former:A &later:B and:(joinable &&former:B &later:C) and:(joinable &&former:C &later:D)).
    
    'program'Output << A << " " << B << " " << C << " " << D.
].
