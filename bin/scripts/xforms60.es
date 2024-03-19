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
	"<" "Form" { val_property | property }* ">" form_closing_tag
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

  #define val_property ::=
<=
              expression (
                property_operation (
                  object ( identifier = current )                    
=>
        ":" prop_name "=" prop_value
<=
                )
              )
=>;

  #define property ::=
<=
              expression (
                property_operation (
                  object ( identifier = current )                    
=>
        prop_name "=" prop_str_value
<=
                )
              )
=>;

  #define form_closing_tag ::=
	"<" "/" "Form" ">";

  #define prop_name ::=
<= 
                message ( 
=>
                    identifier
<=
                )
=>;

  #define prop_value ::=
<=
                expression (
                   object (
                      integer =
=>
                      int_quote
<=
                   )
                )
=>;

  #define prop_str_value ::=
<=
                expression (
                   object (
                      literal =
=>
                      quote
<=
                   )
                )
=>;

  #define identifier   ::= <= identifier = $identifier =>;
  #define int_quote    ::= <= $intliteral =>;
  #define quote        ::= <= "$literal" =>;
]]
