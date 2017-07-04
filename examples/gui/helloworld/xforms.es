[[
   #grammar cf

   #define start     ::= 
<=
root
(
  symbol
  (
    identifier=mainForm
    expression
    (
      reference=xforms'Form
      message=new;
=>
    "<" "Form" property* form_closing_tag
<=
    )
  )
)
=>;

   #define form_closing_tag ::= 
    ">" member* "<" "/" "Form" ">";

   #define label_closing_tag ::= 
    ">" member* "<" "/" "Label" ">";

   #define member         ::=
<=
    message=appendControl
    expression
    (
      reference=xforms'Label
      message=new;
=>
         "<" "Label" property* label_closing_tag
<=
    );
=>;

   #define property       ::=
<=
      message=set
      message=
=>
      identifier "=" value 
<=    ; => ;

   #define identifier ::= <= $identifier =>;

   #define value ::= <= literal = "$literal" =>;

]]