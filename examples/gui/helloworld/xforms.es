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
    ">" body "<" "/" "Form" ">";

   #define body           ::=
        $eps;

   #define property       ::=
<=
      message=set
      message=
=>
        identifier "=" value <= ; => ;

   #define identifier ::= <= $identifier =>;

   #define value ::= <= literal = "$literal" =>;

]]