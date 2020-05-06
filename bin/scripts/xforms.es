[[
   #grammar cf

   #define start     ::= form;
   #define start     ::= $eof;

   #define form      ::= 
<=
      root ( public_namespace (
        class (
          nameattr ( identifier = program )
          script_function (
            code (
              expression (
                variable_idetifier = current 
                assign
                expression (
                  new_reference = xforms'SDIDialog
                  message
                )
              )
=>
                          "<" "Form" property* form_closing_tag
<=
              returning (  
                expression (
                  identifier = current
                )
              )
            )
          )
        )
      ) )
=>;

   #define property       ::=
<=
        expression (
          expression (
            identifier = current

            message (
              identifier = $current
            )
            property_parameter
          )
          assign
          expression (
=>
              $identifier "=" value
<=
          )
        )
=>;

   #define form_closing_tag ::= 
                           ">" "<" "/" "Form" ">";

   #define value ::= <= literal = "$literal" =>;

]]
        