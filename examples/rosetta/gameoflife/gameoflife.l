#import cellular.

program =
[
    var model := Model new.

    model onUpdate &listener_func:
        (&space:sp) [ sp print ].

    model eval.
].