[[
   #grammar cf

   #define start     ::= form;
   #define start     ::= $eof;

   #define form      ::= 
<=
      root ( public_namespace (
        singleton (
          nameattr ( identifier = program )
          script_function (
            parameter ( nameattr ( identifier = owner ) )
            code (
              assign_operation (
                new_variable ( identifier = current )
                message_operation (
                  object ( reference = forms'SDIDialog )
                  message = new
                )
              )
=>
	"<" "Form" ">" form_closing_tag
<=
            )
          )
        )
      ))
=>;

  #define form_closing_tag ::=
	"<" "/" "Form" ">";
]]
