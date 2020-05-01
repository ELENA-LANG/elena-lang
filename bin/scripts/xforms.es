[[
   #grammar cf

   #define start     ::= 
<=
      root ( public_namespace (
        class (
          nameattr ( identifier = program )
          script_function (
=>
                        top_object
<=
          )
        )
      ) )
=>;

   #define start     ::= 
                        $eof;

   #define top_object    ::=
<=
      code (
=>
                        class "(" member+ ")"
<=
         returning (  
            expression (
               identifier = current
            )
         )
      )
=>;

   #define object    ::=
                        class "(" member+ ")";

   #define class     ::= 
<=
        expression (
           variable_idetifier = current 
           assign
           expression (
              new_reference =
=>
                        class_ref
<=
              message
           )
        )
=>;

   #define member    ::= field;
   #define member    ::= prop_code;

   #define prop_code ::=
<=
      code (
=>
                         prop
<=
      )
=>; 

   #define prop      ::=
       $identifier "=" object
<=
    expression (
      expression (
        expression (
           prev_identifier = current
        )
        message (
           identifier = $current
        )
        property_parameter
      )
      assign
      expression (
        identifier = current
      )
    )
=>;

   #define field     ::= 
<=
        expression (
=>
                        property "=" prop_value
<=
        )
=>;

   #define property ::=
<=
        expression (
          identifier = current

          message (
            identifier =
=>
                        prop_ident
<=
          )
          property_parameter
        )
=>;

   #define prop_value::=
<=
     assign
     expression (
=>
               value
<=
    )
=>;  

   #define value     ::= int_value;

   #define class_ref ::= <= $reference =>; 

   #define prop_ident::= <= $identifier =>;

   #define int_value ::= <= integer = $numeric =>; 
]]
