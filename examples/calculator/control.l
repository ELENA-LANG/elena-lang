// --- namespaces ---
#define std'basic'*.
#define calc'parser'*.
//#define vm'* = sys'vm'engine'*.

// --- Controller --
#class Controller 
{
    #role Exiting
    {
        #method proceed [ $self fail. ]
    }

    #method proceed 
    [
        'program'output << "%n>".
    ]

    #method load : anInput 
    [
        #var aLine := String load:anInput.
        #if ("-q" == aLine)?
            [ #shift Exiting. ^ "Exiting". ]
            | [ ^ ExpressionFactory::aLine. ].
    ]  

    #method new    
    [
        // init factory from script
        'program'output << "ELENA Calculator. Evaluate the expression. To quit type -q".
        
//        #if vm'Controller run &vm'script_mode:vm'smDSA &vm'dsa_name:"startup" &sys'io'path:"calc.vl2"
//            | [ 'program'output << "%nFailed DSA Rules:" << vm'Controller vm'script_error << "%n". $self fail. ].
//            
//        #if vm'Controller run &vm'dsa_name:"startup" &sys'io'path:"calc.vl"
//            | [ 'program'output << "%nFailed:" << (vm'Controller vm'script_error | vm'vm_error) << "%n". $self fail. ].
//
//        theLine := String.
    ]
}
