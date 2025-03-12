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
           expression (
             assign_operation (
               new_variable ( identifier = current )
               expression (
                 object ( identifier = self )
               )
             )
           )
=>
     { val_form_prop | form_prop }* ">" member* form_closing_tag
<=
       )
    )
=>;

  #define field_list ::= <= $buffer => $eps;

  #define member ::= { label | button | radiobuttongroup | panel | edit | combobox };

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
               object ( super_identifier = current )
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
               object ( super_identifier = current )
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

  #define combobox ::=
<=
         code (
           expression (
             assign_operation (
               new_variable ( identifier = current )
               message_operation (
                 object ( reference = forms'Combobox )
                 message ( identifier = new )
               )
             )
           )
=>
    "<" "Combobox" combobox_name_prop { val_prop | prop }* ">" combobox_closing_tag
<=
           expression (
             message_operation (
               object ( super_identifier = current )
               message ( identifier = appendControl )
               expression (
                 object ( identifier = current )
               )
             )
           )
         )
=>;

  #define combobox_name_prop ::= ":" "Name" "=" combobox_declaration control_assigning;

  #define combobox_declaration ::= %<= field ( type ( reference = forms'Combobox ) nameattr ( identifier = $current ) ) =>;

  #define edit ::=
<=
         code (
           expression (
             assign_operation (
               new_variable ( identifier = current )
               message_operation (
                 object ( reference = forms'Edit )
                 message ( identifier = new )
               )
             )
           )
=>
    "<" "Edit" edit_name_prop { val_prop | prop }* ">" edit_closing_tag
<=
           expression (
             message_operation (
               object ( super_identifier = current )
               message ( identifier = appendControl )
               expression (
                 object ( identifier = current )
               )
             )
           )
         )
=>;

  #define edit_name_prop ::= ":" "Name" "=" edit_declaration control_assigning;

  #define edit_declaration ::= %<= field ( type ( reference = forms'Edit ) nameattr ( identifier = $current ) ) =>;

  #define radiobuttongroup ::=
<=
         code (
           expression (
             assign_operation (
               new_variable ( identifier = current )
               message_operation (
                 object ( reference = forms'RadioButtonGroup )
                 message ( identifier = new )
               )
             )
           )
=>
    "<" "RadioButtonGroup" rbgroup_name_prop { val_prop | prop }* ">" item* rbgroup_closing_tag
<=
           expression (
             message_operation (
               object ( super_identifier = current )
               message ( identifier = appendControl )
               expression (
                 object ( identifier = current )
               )
             )
           )
         )
=>;

  #define rbgroup_name_prop ::= ":" "Name" "=" rbgroup_declaration control_assigning;

  #define rbgroup_declaration ::= %<= field ( type ( reference = forms'RadioButtonGroup ) nameattr ( identifier = $current ) ) =>;

  #define panel ::=
<=
         code (
           expression (
             assign_operation (
               new_variable ( identifier = current )
               message_operation (
                 object ( reference = forms'Panel )
                 message ( identifier = new )
               )
             )
           )
=>
    "<" "Panel" panel_name_prop { val_prop | prop }* ">" member* panel_closing_tag
<=
           expression (
             message_operation (
               object ( super_identifier = current )
               message ( identifier = appendControl )
               expression (
                 object ( identifier = current )
               )
             )
           )
         )
=>;

  #define panel_name_prop ::= ":" "Name" "=" panel_declaration control_assigning;

  #define panel_declaration ::= %<= field ( type ( reference = forms'Panel ) nameattr ( identifier = $current ) ) =>;

  #define item ::=
<=
           expression (
             message_operation (
               object ( identifier = current )
               message ( identifier = appendItem )
=>
    "<" "Item" itemval_prop ">" item_closing_tag
<=
             )
           ) 
=>;

  #define itemval_prop ::= "Caption" "=" prop_str_value;

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

  #define prop_value ::=
<=
                expression (
                   object (
                      identifier =
=>
                      nonint_quote
<=
                   )
                )
=>;

  #define prop_value ::=
<=
                 expression (
                   closure_op (
                     expression (
                       object (
                         identifier =
=>
                         closure_quote
<=     
                       )
                     )
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

  #define ident_value  ::= <= identifier = $literal =>; 
  #define identifier   ::= <= identifier = $identifier =>;
  #define int_quote    ::= <= $regex "^""(?<current>\d+)""$" =>;
  #define nonint_quote ::= <= $regex "^""(?<current>[a-zA-Z_]\w*)""$" =>;
  #define closure_quote ::= <= $regex "^""&(?<current>[a-zA-Z_]\w*)""$" =>;
  #define quote         ::= <= "$literal" =>;

  #define form_closing_tag ::=
	"<" "/" "Form" ">";

  #define label_closing_tag ::=
	"<" "/" "Label" ">";

  #define button_closing_tag ::=
	"<" "/" "Button" ">";

  #define edit_closing_tag ::=
	"<" "/" "Edit" ">";

  #define rbgroup_closing_tag ::=
	"<" "/" "RadioButtonGroup" ">";

  #define item_closing_tag ::=
	"<" "/" "Item" ">";

  #define panel_closing_tag ::=
	"<" "/" "Panel" ">";

  #define combobox_closing_tag ::=
	"<" "/" "Combobox" ">";
]]
