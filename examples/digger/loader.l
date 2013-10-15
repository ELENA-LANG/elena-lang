#define gui'dialogs'*.
#define gui'dictionary'*.
#define sys'vm'*.

// --- Loader ---

#symbol Loader =
{
    run &dsa_name:aRuleName &dsa_path:aRulePath &script_path:aScriptPath
    [
        #if ScriptEngine 
            run &dsa_path:aRulePath &dsa_name:aRuleName
            run &script_path:aScriptPath &dsa_name:aRuleName
        | [
            MessageBox::{ &error_text:(aRuleName + " Error:" + ScriptEngine script_error | vm_error) &caption:"Digger" } run.
        ].
    ]
}.

// --- LoaderArgs ---

#symbol RuleNameLoaderArg : aRuleName =
{
    dsa_name = aRuleName.
    
    dsa_path = aRuleName + ".vl2".
    
    script_path = aRuleName + ".vl".
}.
