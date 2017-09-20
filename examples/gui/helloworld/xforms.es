[[
   #grammar cf

   #define start     ::= $eof;
   #define start     ::= form $eof;

   #define form     ::=
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
    message=appendAndReturnControl
    expression
    (
      reference=xforms'Label
      message=new
    );
=>
         "<" "Label" property* label_closing_tag
<=
    message=owner;
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
