////// --- default namespaces ---
////#define std'dictionary'*.
//////#define std'basic'*.
////#define std'patterns'*.
//////#define ext'convertors'*.
////
////// --- namespace shortcuts ---
////#define parser'* = calc'parser'*.
////
////// --- input signals ---
////#subject digit, letter, plus, unknown.
//
//// --- ELexicalAnalizer ---
//
////// --- ESumming ---
////
////#role ESumming
////{
////    #method parser'order = 2.
////    
////    #method numeric = self parser'left_node numeric + self parser'right_node numeric.
////}
////
//////// --- EDigit ---
//////
//////#role EDigit
//////{
//////    #method digit'append : aChar
//////    [
//////        parser'DerivationTree += Number::aChar.
//////
//////        ^ self.
//////    ]
//////}
//////
//////// --- ENextDigit ---
//////
//////#role ENextDigit
//////{
//////    #method digit'append : aChar
//////    [
//////        parser'DerivationTree parser'last_node append &literal:aChar.
//////        
//////        ^ self.
//////    ]
//////}
////
////// --- ESum ---
////
//////#role ESum
//////{
//////    #method plus'append
//////    [
//////        parser'DerivationTree += parser'TreeBiNode::ESumming.
//////    ]
//////}
//////
////////// --- Sum ---
////////
////////#class Sum
////////{
////////    #method save : aWriter = aWriter write &:(Real64Value::(self left_node) + self right_node).
////////
////////    #method node_level'get = 2.
////////}
////////
////////// --- Difference ---
////////
////////#class Difference
////////{
////////    #method save : aWriter = aWriter write &:(Real64Value::(self left_node) - self right_node).
////////
////////    #method node_level'get = 2.
////////}
////////
////////// --- Product ---
////////
////////#class Product
////////{
////////    #method save : aWriter = aWriter write &:(Real64Value::(self left_node) * self right_node).
////////
////////    #method node_level'get = 1.
////////}
////////
////////// --- Fraction ---
////////
////////#class Fraction
////////{
////////    #method save : aWriter = aWriter write &:(Real64Value::(self left_node) / self right_node).
////////
////////    #method node_level'get = 1.
////////}
