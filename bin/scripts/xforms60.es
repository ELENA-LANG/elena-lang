[[
  #grammar cf

  #define start         ::= form $eof;
  #define start         ::= $eof;

  #define form          ::=
<=
root ( 
 public_namespace (
  public_textblock (
=>
    "<" "Form" name_attr form_body field_list
<=
  )
 )
)
=>;

  #define form_body ::=
<=
    method (
       nameattr( identifier = onInit )
       code (
=>
     { val_form_prop | form_prop }* ">" member* form_closing_tag
<=
       )
    )
=>;

  #define field_list ::= <= $buffer => $eps;

  #define member ::= { label | button };

  #define label ::=
<=
         code (
           expression (
             assign_operation (
               new_variable ( identifier = current )
               message_operation (
                 object ( reference = forms'Label )
                 message ( identifier = new )
               )
             )
           )
=>
    "<" "Label" label_name_prop { val_prop | prop }* ">" label_closing_tag
<=
           expression (
             message_operation (
               object ( identifier = self )
               message ( identifier = appendControl )
               expression (
                 object ( identifier = current )
               )
             )
           )
         )
=>;

  #define label_name_prop ::= ":" "Name" "=" label_declaration control_assigning;

  #define label_declaration ::= %<= field ( type ( reference = forms'Label ) nameattr ( identifier = $current ) ) =>;

  #define button ::=
<=
         code (
           expression (
             assign_operation (
               new_variable ( identifier = current )
               message_operation (
                 object ( reference = forms'Button )
                 message ( identifier = new )
               )
             )
           )
=>
    "<" "Button" button_name_prop { val_prop | prop }* ">" button_closing_tag
<=
           expression (
             message_operation (
               object ( identifier = self )
               message ( identifier = appendControl )
               expression (
                 object ( identifier = current )
               )
             )
           )
         )
=>;

  #define button_name_prop ::= ":" "Name" "=" button_declaration control_assigning;

  #define button_declaration ::= %<= field ( type ( reference = forms'Button ) nameattr ( identifier = $current ) ) =>;

  #define control_assigning ::=
<= 
           expression (
             assign_operation (
               object (
=>
      ident_value
<=
               )
               expression
               (
                  object ( identifier = current )
               )
             )
           )
=>;

  #define val_form_prop ::=
<=
           expression (
             property_operation (
               object ( identifier = self )                    
=>
        prop_name "=" prop_str_value
<=
             )
           )
=>;

  #define form_prop ::=
<=
           expression (
             property_operation (
               object ( identifier = self )                    
=>
        ":" prop_name "=" prop_value
<=
             )
           )
=>;

  #define val_prop ::=
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

  #define prop ::=
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

  #define name_attr ::=
<= 
    nameattr (
=>
     ":" "Name" "=" ident_value
<=
    )
=>;

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

  #define ident_value ::= <= identifier = $literal =>; 
  #define identifier  ::= <= identifier = $identifier =>;
  #define int_quote   ::= <= $intliteral =>;
  #define quote       ::= <= "$literal" =>;

  #define form_closing_tag ::=
	"<" "/" "Form" ">";

  #define label_closing_tag ::=
	"<" "/" "Label" ">";

  #define button_closing_tag ::=
	"<" "/" "Button" ">";

]]
