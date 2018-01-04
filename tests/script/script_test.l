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
].