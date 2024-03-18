[[
   #grammar cf

   #define start     ::= form;
   #define start     ::= $eof;

   #define form      ::= 
<=
      root ( public_namespace (
        public_singleton (
          nameattr ( identifier = loader )
          script_function (
            parameter ( nameattr ( identifier = owner ) )
            code (
              expression (
                assign_operation (
                  new_variable ( identifier = current )
                  message_operation (
                    object ( reference = forms'SDIDialog )
                    message ( identifier = new )
                  )
                )
              )
=>
	"<" "Form" ">" form_closing_tag
<=
              returning (  
                expression (
                  object ( identifier = current )
                )
              )
            )
          )
        )
      ))
=>;

  #define form_closing_tag ::=
	"<" "/" "Form" ">";
]]
