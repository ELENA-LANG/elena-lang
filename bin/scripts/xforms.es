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
            code (
              expression (
                variable_identifier = current 
                assign = 0
                expression (
                  reference = forms'SDIDialog
                  message = new
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
            property_parameter = 0
          )
          assign = 0
          expression (
             new_reference = system'Variant
             message = 0
             expression (
=>
               $identifier "=" value
<=
             )
          )
        )
=>;

   #define form_closing_tag ::= 
                           ">" "<" "/" "Form" ">";

   #define value ::= <= literal = "$literal" =>;

]]
        