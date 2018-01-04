import extensions'scripting.

//symbol program2 =
//[
//    var n := script_test'mytest'States state0.
//].

//class tester =
//{
//    newLeaf : n
//    [
//        var l := n level.
//        var s := n evalToken:"2".
//    ]
//    
//    addContent : ch
//    [
//    ]
//    
//    digit
//    [
//    ]
//}.

symbol program =
[
    var o := jscript eval path("test.es").

    var r := o test("Hello World").

//    var obj := factory eval(Library new); first/*; eval*/.
//    
//    var m := script_test'mytest'States state0.
//    
//    var n := obj state0.
//
//    var nmix := Group::(tester, n).
//    var mmix := Group::(tester, m).
//
////    nmix digit:3.
////    mmix digit:"3".
//                
//    var nn := nmix digit:3.
//    var mm := mmix digit:3.
].