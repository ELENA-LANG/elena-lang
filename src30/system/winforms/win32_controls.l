#define system.

// --- classes ---
#symbol StaticClass = "Static" wide. // !! temporal
#symbol EditClass   = "Edit" wide. // !! temporal
#symbol ButtonClass = "Button" wide. // !! temporal

// --- BaseLabelHandle ---

#class BaseLabelHandle :: ControlHandle
{
}

// --- StaticLabelHandle ---

#class(sealed) StaticLabelHandle :: BaseLabelHandle
{
    #constructor new &handle:aParent &wide:aCaption &int:x &int:y &int:width &int:height
    [
        $self create 
            &int:0 &wide:StaticClass &wide:aCaption 
            &int:(WS_CHILD or:SS_SIMPLE or:WS_VISIBLE) 
            &int:x &int:y &int:width &int:height 
            &handle:aParent 
            &hinstance:CurrentInstance
            &object:nil.
    ]
}

// --- BaseEditHandle ---

#class BaseEditHandle :: ControlHandle
{
}

// --- EditHandle ---

#class(sealed) EditHandle :: BaseEditHandle
{
    #constructor new &handle:aParent &wide:aText &int:x &int:y &int:width &int:height
    [
        $self create 
            &int:WS_EX_CLIENTEDGE
            &wide:EditClass &wide:aText
            &int:(WS_CHILD or:WS_BORDER or:WS_VISIBLE or:WS_TABSTOP) 
            &int:x &int:y &int:width &int:height 
            &handle:aParent 
            &hinstance:CurrentInstance
            &object:nil.
    ]
}

// --- BaseButtonHandle ---

#class BaseButtonHandle :: ControlHandle
{
}

// --- ButtonHandle ---

#class(sealed) ButtonHandle :: BaseButtonHandle
{
    #constructor new &handle:aParent &wide:aText &int:x &int:y &int:width &int:height
    [
        $self create 
            &int:0
            &wide:ButtonClass &wide:aText
            &int:(WS_CHILD or:BS_PUSHBUTTON or:WS_VISIBLE or:WS_TABSTOP) 
            &int:x &int:y &int:width &int:height 
            &handle:aParent 
            &hinstance:CurrentInstance
            &object:nil.
    ]
}
