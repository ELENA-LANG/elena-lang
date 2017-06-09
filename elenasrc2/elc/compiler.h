//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler class.
//
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef compilerH
#define compilerH

#include "elena.h"
#include "compilercommon.h"
#include "bcwriter.h"

namespace _ELENA_
{

// --- ModuleInfo ---
struct ModuleInfo
{
   _Module* codeModule;
   _Module* debugModule;

   ModuleInfo()
   {
      codeModule = debugModule = NULL;
   }

   ModuleInfo(_Module* codeModule, _Module* debugModule)
   {
      this->codeModule = codeModule;
      this->debugModule = debugModule;
   }
};

struct Unresolved
{
   ident_t    fileName;
   ref_t      reference;
   _Module*   module;
   size_t     row;
   size_t     col;           // virtual column

   Unresolved()
   {
      reference = 0;
   }
   Unresolved(ident_t fileName, ref_t reference, _Module* module, size_t row, size_t col)
   {
      this->fileName = fileName;
      this->reference = reference;
      this->module = module;
      this->row = row;
      this->col = col;
   }
};

typedef List<Unresolved> Unresolveds;

// --- Compiler class ---
class Compiler : public _Compiler
{
public:
   struct Parameter
   {
      int    offset;
      ref_t  class_ref;
      ref_t  subj_ref;
      int    size;

      Parameter()
      {
         offset = -1;
         subj_ref = 0;
         class_ref = 0;
         size = 0;
      }
      Parameter(int offset)
      {
         this->offset = offset;
         this->subj_ref = 0;
         this->class_ref = 0;
         this->size = 0;
      }
      Parameter(int offset, ref_t subj_ref)
      {
         this->offset = offset;
         this->subj_ref = subj_ref;
         this->class_ref = 0;
         this->size = 0;
      }
      Parameter(int offset, ref_t subj_ref, ref_t class_ref)
      {
         this->offset = offset;
         this->subj_ref = subj_ref;
         this->class_ref = class_ref;
         this->size = 0;
      }
      Parameter(int offset, ref_t subj_ref, ref_t class_ref, int size)
      {
         this->offset = offset;
         this->subj_ref = subj_ref;
         this->class_ref = class_ref;
         this->size = size;
      }
   };

   // InheritResult
   enum InheritResult
   {
      irNone = 0,
      irSuccessfull,
      irUnsuccessfull,
      irSealed,
      irInvalid,
      irObsolete
   };

   enum ObjectKind
   {
      okUnknown = 0,

      okObject,                       // param - class reference
      okSymbol,                       // param - reference
      okConstantSymbol,               // param - reference, extraparam - class reference
      okConstantClass,                // param - reference, extraparam - class reference
      okLiteralConstant,              // param - reference
      okWideLiteralConstant,          // param - reference
      okCharConstant,                 // param - reference
      okIntConstant,                  // param - reference, extraparam - imm argument
      okLongConstant,                 // param - reference
      okRealConstant,                 // param - reference
      okMessageConstant,              // param - reference
      okExtMessageConstant,           // param - reference
      okSignatureConstant,            // param - reference
      okVerbConstant,                 // param - reference
      okArrayConst,
      okField,                        // param - field offset, extraparam - class reference
      okStaticField,                  // param - reference
      okFieldAddress,                 // param - field offset, extraparam - class reference
      okOuter,                        // param - field offset, extraparam - class reference
      okOuterField,                   // param - field offset, extraparam - outer field offset
      okLocal,                        // param - local / out parameter offset, extraparam : class reference
      okParam,                        // param - parameter offset, extraparam = -1 (is stack safe) / 0
      okParamField,
      okSubject,                      // param - parameter offset
      okThisParam,                    // param - parameter offset, extraparam = -1 (stack allocated) / -2 (primitive array)
      okNil,
      okSuper,
      okLocalAddress,                 // param - local offset, extraparam - class reference
      okParams,                       // param - local offset
//      okBlockLocal,                   // param - local offset
      okConstantRole,                 // param - role reference
      okExplicitConstant,             // param - reference, extraparam - subject

      okExternal,
      okInternal,
   };

   struct ObjectInfo
   {
      ObjectKind kind;
      ref_t      param;
      ref_t      extraparam;
      ref_t      type;

      ObjectInfo()
      {
         this->kind = okUnknown;
         this->param = 0;
         this->extraparam = 0;
         this->type = 0;
      }
      ObjectInfo(ObjectKind kind)
      {
         this->kind = kind;
         this->param = 0;
         this->extraparam = 0;
         this->type = 0;
      }
      ObjectInfo(ObjectKind kind, ObjectInfo copy)
      {
         this->kind = kind;
         this->param = copy.param;
         this->extraparam = copy.extraparam;
         this->type = copy.type;
      }
      ObjectInfo(ObjectKind kind, ref_t param)
      {
         this->kind = kind;
         this->param = param;
         this->extraparam = 0;
         this->type = 0;
      }
      ObjectInfo(ObjectKind kind, int param)
      {
         this->kind = kind;
         this->param = (ref_t)param;
         this->extraparam = 0;
         this->type = 0;
      }
      ObjectInfo(ObjectKind kind, ref_t param, ref_t extraparam)
      {
         this->kind = kind;
         this->param = param;
         this->extraparam = extraparam;
         this->type = 0;
      }
      ObjectInfo(ObjectKind kind, ref_t param, int extraparam)
      {
         this->kind = kind;
         this->param = param;
         this->extraparam = (ref_t)extraparam;
         this->type = 0;
      }
      ObjectInfo(ObjectKind kind, ref_t param, ref_t extraparam, ref_t type)
      {
         this->kind = kind;
         this->param = param;
         this->extraparam = extraparam;
         this->type = type;
      }
      ObjectInfo(ObjectKind kind, ref_t param, int extraparam, ref_t type)
      {
         this->kind = kind;
         this->param = param;
         this->extraparam = (ref_t)extraparam;
         this->type = type;
      }
   };

   typedef Map<ident_t, ref_t>            ForwardMap;
   typedef MemoryMap<ident_t, Parameter>  LocalMap;
//   typedef MemoryMap<int, ref_t>          RoleMap;
   typedef Map<ref_t, SubjectMap*>        ExtensionMap;

private:
   // - ModuleScope -
   struct ModuleScope : _CompilerScope
   {
      _ProjectManager* project;
      _Module*         debugModule;

      ident_t          sourcePath;
      ref_t            sourcePathRef;

      // default namespaces
      List<ident_t> defaultNs;
      ForwardMap    forwards;       // forward declarations

      // symbol hints
      Map<ref_t, ref_t> constantHints;

      // extensions
      SubjectMap        extensionHints;
      ExtensionMap      extensions;

      // type / attribute hints
      MessageMap        attributes;

      // action hints
      SubjectMap        actionHints;

      ref_t packageReference;

      // warning mapiing
      bool warnOnUnresolved;
      bool warnOnWeakUnresolved;
      int  warningMask;

      // list of references to the current module which should be checked after the project is compiled
      Unresolveds* forwardsUnresolved;

      ObjectInfo mapObject(SNode identifier);

      ref_t mapReference(ident_t reference, bool existing = false);
      ref_t mapAttribute(SNode terminal, int& attrValue);

      ObjectInfo mapReferenceInfo(ident_t reference, bool existing = false);

      void defineConstantSymbol(ref_t reference, ref_t classReference)
      {
         constantHints.add(reference, classReference);
      }

      void raiseError(const char* message, int row, int col, ident_t terminal);
      void raiseWarning(int level, const char* message, int row, int col, ident_t terminal);

      void raiseError(const char* message, SNode terminal);
      void raiseWarning(int level, const char* message, SNode terminal);

      bool checkReference(ident_t referenceName);

      ref_t resolveIdentifier(ident_t name);

      ref_t mapNewSubject(ident_t terminal);

      // NOTE : the function returns 0 for implicit subjects
      // in any case output is set (for explicit one - the namespace is copied as well)
      ref_t mapSubject(SNode terminal, IdentifierString& output);
      ref_t mapSubject(SNode terminal, bool explicitOnly = true);
      ref_t resolveAttributeRef(ident_t name, bool explicitOnly = true);

      ref_t mapTerminal(SNode terminal, bool existing = false);

      ObjectInfo defineObjectInfo(ref_t reference, bool checkState = false);

      virtual _Module* loadReferenceModule(ref_t& reference);

      ref_t loadClassInfo(ClassInfo& info, ident_t vmtName, bool headerOnly = false);
      virtual ref_t loadClassInfo(ClassInfo& info, ref_t reference, bool headerOnly = false)
      {
         return loadClassInfo(info, module->resolveReference(reference), headerOnly);
      }
      ref_t loadSymbolExpressionInfo(SymbolExpressionInfo& info, ident_t symbol);

      _Memory* loadAttributeInfo(ref_t reference/*, _Module* &argModule*/)
      {
         return loadAttributeInfo(module->resolveSubject(reference)/*, argModule*/);
      }
      _Memory* loadAttributeInfo(ident_t attribute/*, _Module* &argModule*/);

      void loadAttributes(_Module* module);
      void loadExtensions(_Module* module, bool& duplicateExtensions);
      void loadActions(_Module* module);

      void saveSubject(ref_t attrRef, ref_t classReference, bool internalType);
      bool saveExtension(ref_t message, ref_t type, ref_t role);
      void saveAction(ref_t message, ref_t reference);

      void validateReference(SNode terminal, ref_t reference);

      ref_t getBaseLazyExpressionClass();

      void importClassInfo(ClassInfo& copy, ClassInfo& target, _Module* exporter, bool headerOnly);

      void loadModuleInfo(_Module* extModule)
      {
         bool dummy;
         loadModuleInfo(extModule, dummy);
      }
      void loadModuleInfo(_Module* extModule, bool& duplicateExtensions)
      {
         loadAttributes(extModule);
         loadExtensions(extModule, duplicateExtensions);
         loadActions(extModule);
      }

      ref_t mapNestedExpression();

//      bool defineForward(ident_t forward, ident_t referenceName)
//      {
//         ObjectInfo info = mapReferenceInfo(referenceName, false);
//      
//         return forwards.add(forward, info.param, true);
//      }

      ModuleScope(_ProjectManager* project, ident_t sourcePath, _Module* module, _Module* debugModule, Unresolveds* forwardsUnresolved);
   };

   // - Scope -
   struct Scope
   {
      enum ScopeLevel
      {
         slClass,
         slSymbol,
         slMethod,
         slCode,
         slOwnerClass,
         slTemplate,
      };

      ModuleScope* moduleScope;
      Scope*       parent;

      void raiseError(const char* message, SNode terminal)
      {
         moduleScope->raiseError(message, terminal);
      }
      void raiseWarning(int level, const char* message, SNode terminal)
      {
         moduleScope->raiseWarning(level, message, terminal);
      }

      ObjectInfo mapObject(SNode terminal)
      {
         ObjectInfo object = mapTerminal(terminal.identifier());
         if (object.kind == okUnknown) {
            return moduleScope->mapObject(terminal);
         }
         else return object;
      }

      virtual ObjectInfo mapTerminal(ident_t identifier)
      {
         if (parent) {
            return parent->mapTerminal(identifier);
         }
         else return ObjectInfo();
      }

      virtual Scope* getScope(ScopeLevel level)
      {
         if (parent) {
            return parent->getScope(level);
         }
         else return NULL;
      }

      virtual ref_t mapSubject(SNode terminal, IdentifierString& output)
      {
         if (parent) {
            return parent->mapSubject(terminal, output);
         }
         else return moduleScope->mapSubject(terminal, output);
      }

      virtual ref_t mapSubject(SNode terminal, bool explicitOnly = true)
      {
         if (parent) {
            return parent->mapSubject(terminal, explicitOnly);
         }
         else return moduleScope->mapSubject(terminal, explicitOnly);
      }

      Scope(ModuleScope* moduleScope)
      {
         this->parent = NULL;
         this->moduleScope = moduleScope;
      }
      Scope(Scope* parent)
      {
         this->parent = parent;
         this->moduleScope = parent->moduleScope;
      }
   };

   // - SourceScope -
   struct SourceScope : public Scope
   {
//      CommandTape    tape;
      ref_t          reference;

      SourceScope(ModuleScope* parent, ref_t reference);
   };

   // - ClassScope -
   struct ClassScope : public SourceScope
   {
      ClassInfo   info;
      ref_t       extensionMode;

//      virtual int getMethodInfo(ref_t message, MethodAttribute attr)
//      {
//         return info.methodHints.get(ClassInfo::Attribute(message, attr));
//      }
//      virtual void setMethodInfo(ref_t message, MethodAttribute attr, int value, bool clearPreviousOne = true)
//      {
//         if (clearPreviousOne)
//            info.methodHints.exclude(ClassInfo::Attribute(message, attr));
//
//         info.methodHints.add(ClassInfo::Attribute(message, attr), value);
//      }
//
//      virtual bool isClosed()
//      {
//         return test(info.header.flags, elClosed);
//      }

      ObjectInfo mapField(ident_t identifier);

      virtual ObjectInfo mapTerminal(ident_t identifier);

//      void compileClassAttribute(SyntaxTree::Node hint);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slClass || level == slOwnerClass) {
            return this;
         }
         else return Scope::getScope(level);
      }

      void save()
      {
         // save class meta data
         MemoryWriter metaWriter(moduleScope->module->mapSection(reference | mskMetaRDataRef, false), 0);
         metaWriter.Memory()->trim(0);
         info.save(&metaWriter);
      }

      bool include(ref_t message)
      {
         // check if the method is inhreited and update vmt size accordingly
         ClassInfo::MethodMap::Iterator it = info.methods.getIt(message);
         if (it.Eof()) {
            info.methods.add(message, true);

            return true;
         }
         else {
            (*it) = true;

            return false;
         }
      }

      ClassScope(ModuleScope* parent, ref_t reference);
   };

   // - SymbolScope -
   struct SymbolScope : public SourceScope
   {
      bool  constant;
      bool  staticOne;
      bool  preloaded;
      ref_t typeRef;
      ref_t outputRef;

      virtual ObjectInfo mapTerminal(ident_t identifier);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slSymbol) {
            return this;
         }
         else return Scope::getScope(level);
      }

      SymbolScope(ModuleScope* parent, ref_t reference);
   };

   // - MethodScope -
   struct MethodScope : public Scope
   {
      ref_t        message;
      LocalMap     parameters;
      int          reserved;           // defines inter-frame stack buffer (excluded from GC frame chain)
      int          rootToFree;         // by default is 1, for open argument - contains the list of normal arguments as well
      int          hints;
      bool         withOpenArg;
      bool         stackSafe;
      bool         classEmbeddable;
      bool         generic;
      bool         extensionMode;

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slMethod) {
            return this;
         }
         else return parent->getScope(level);
      }

      ref_t getReturningRef(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);

         return scope->info.methodHints.get(ClassInfo::Attribute(message, maReference));
      }

      ref_t getReturningType(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);

         return scope->info.methodHints.get(ClassInfo::Attribute(message, maType));
      }

      ref_t getClassFlags(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);

         return scope ? scope->info.header.flags : 0;
      }
      ref_t getClassRef(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);

         return scope ? scope->reference : 0;
      }

      virtual ObjectInfo mapTerminal(ident_t identifier);

      MethodScope(ClassScope* parent);
   };

   // - ActionScope -
   struct ActionScope : public MethodScope
   {
      bool subCodeMode;
      bool  singletonMode;  // indicates that the symbol is a singleton closure

      ActionScope(ClassScope* parent);

      virtual ObjectInfo mapTerminal(ident_t identifier);
   };

   // - CodeScope -
   struct CodeScope : public Scope
   {
      // scope local variables
      LocalMap     locals;
      int          level;

      // scope stack allocation
      int          reserved;  // allocated for the current statement
      int          saved;     // permanently allocated

      int newLocal()
      {
         level++;

         return level;
      }

      void mapLocal(ident_t local, int level/*, ref_t type*/)
      {
         locals.add(local, Parameter(level/*, type*/));
      }
      void mapLocal(ident_t local, int level, ref_t type, ref_t class_ref, int size)
      {
         locals.add(local, Parameter(level, type, class_ref, size));
      }

      void freeSpace()
      {
         reserved = saved;
      }

      virtual ObjectInfo mapTerminal(ident_t identifier);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slCode) {
            return this;
         }
         else return parent->getScope(level);
      }

      int getMessageID()
      {
         MethodScope* scope = (MethodScope*)getScope(slMethod);

         return scope ? scope->message : 0;
      }

      ref_t getClassRefId(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);

         return scope ? scope->reference : 0;
      }

      ref_t getClassFlags(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);

         return scope ? scope->info.header.flags : 0;
      }

      CodeScope(SymbolScope* parent);
      CodeScope(MethodScope* parent);
      CodeScope(CodeScope* parent);
   };

   // - InlineClassScope -
   struct InlineClassScope : public ClassScope
   {
      struct Outer
      {
         ref_t      reference;
         bool       preserved;
         ObjectInfo outerObject;

         Outer()
         {
            reference = INVALID_REF;
            preserved = false;
         }
         Outer(int reference, ObjectInfo outerObject)
         {
            this->reference = reference;
            this->outerObject = outerObject;
            this->preserved = false;
         }
      };

      bool                    returningMode;
      bool                    closureMode;
      Map<ident_t, Outer>     outers;
      ClassInfo::FieldTypeMap outerFieldTypes;

      Outer mapSelf();
      Outer mapOwner();

      ObjectInfo allocateRetVar();

      bool markAsPresaved(ObjectInfo object);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slClass) {
            return this;
         }
         else return Scope::getScope(level);
      }

      virtual ObjectInfo mapTerminal(ident_t identifier);

      InlineClassScope(CodeScope* owner, ref_t reference);
   };

   // --- TemplateScope ---
   struct TemplateScope : Scope
   {
      enum Type
      {
         ttNone           = 0,
         ttFieldTemplate  = 1,
         ttMethodTemplate = 2,
         ttCodeTemplate   = 3,
         ttAttrTemplate   = 4
      };

      Type         type;
      ref_t        templateRef;  // NOTE : should be zero for template declaration
      ref_t        reference;
      ForwardMap   parameters;
      ForwardMap   fields;
      SubjectMap   subjects;
//      bool         classMode;
//      bool         embeddableMode;
//      bool         codeMode;
//      bool         fieldMode;
      ident_t      sourcePath;

      SNode        exprNode;     // used for code template
      SNode        codeNode;
      SNode        elseNode;
      SNode        nestedNode;

      SyntaxTree* autogeneratedTree;

      bool isAttribute(SNode terminal);

      int mapAttribute(SNode terminal);

      void loadAttributeValues(SNode node, bool prefixMode = false);
      void loadParameters(SNode node);
      void loadFields(SNode node);

      void copySubject(SyntaxWriter& writer, SNode terminal);
      void copyIdentifier(SyntaxWriter& writer, SNode terminal);

      ref_t mapAttribute(SNode terminal, int& attrValue/*, bool variableMode = false*/);
      ref_t mapTemplate(SNode terminal, int prefixCounter = 0);
      int mapIdentifier(SNode terminal);

      bool generateClassName();

      TemplateScope(TemplateScope* parent, ref_t attrRef)
         : Scope(parent)
      {
         type = Type::ttNone;
         templateRef = attrRef;
         reference = 0;
//         classMode = false;
//         embeddableMode = false;
//         fieldMode = codeMode = false;
         sourcePath = NULL;
         autogeneratedTree = parent->autogeneratedTree;
      }
      TemplateScope(ModuleScope* moduleScope)
         : Scope(moduleScope)
      {
         type = Type::ttNone;
         templateRef = 0;
         reference = 0;
//         classMode = false;
//         embeddableMode = false;
//         fieldMode = codeMode = false;
         sourcePath = NULL;
         autogeneratedTree = NULL;
      }
   };

   struct WarningScope
   {
      ident_t terminal;

      int warningMask;
      int col;
      int row;

      void raise(ModuleScope& scope, int level, ident_t message, SNode node)
      {
         if (test(warningMask, level)) {
            if (col != 0) {
               scope.raiseWarning(level, message, row, col, terminal);
            }
            else if(node != lxNone)
               scope.raiseWarning(level, message, node);
         }
      }

      WarningScope(int mask)
      {
         warningMask = mask;
         col = row = 0;
         terminal = NULL;
      }
      WarningScope()
      {
         warningMask = 0;
         col = row = 0;
         terminal = NULL;
      }
   };

   _CompilerLogic*  _logic;

   ByteCodeWriter _writer;

   MessageMap     _verbs;                            // list of verbs
   MessageMap     _operators;                        // list of operators

   int            _optFlag;

   // optimization rules
   TransformTape _rules;

   // optmimization routines
   bool applyRules(CommandTape& tape);
   bool optimizeIdleBreakpoints(CommandTape& tape);
   bool optimizeJumps(CommandTape& tape);
   void optimizeTape(CommandTape& tape);

////   void raiseWarning(ModuleScope& scope, SNode node, ident_t message, int warningLevel, int warningMask, bool triggered = true);
////
////   void appendObjectInfo(CodeScope& scope, ObjectInfo object);
   void writeMessageInfo(SyntaxWriter& writer, ModuleScope& scope, ref_t messageRef);
////   ref_t mapAttribute(SNode attribute, Scope& scope, int& attrValue);
//   ref_t mapAttribute(SNode attribute, ModuleScope& scope);
   void initialize(ClassScope& scope, MethodScope& methodScope);

   pos_t saveSourcePath(ModuleScope& scope, ident_t path);

   int checkMethod(ModuleScope& scope, ref_t reference, ref_t message)
   {
      _CompilerLogic::ChechMethodInfo dummy;

      return _logic->checkMethod(scope, reference, message, dummy);
   }

   ref_t resolveObjectReference(ModuleScope& scope, ObjectInfo object);
   ref_t resolveObjectReference(CodeScope& scope, ObjectInfo object);

   ref_t mapExtension(CodeScope& scope, ref_t messageRef, ObjectInfo target);

   void importCode(SyntaxWriter& writer, SNode node, ModuleScope& scope, ident_t reference, ref_t message);

   InheritResult inheritClass(ClassScope& scope, ref_t parentRef, bool ignoreSealed);

   /// NOTE : the method is used to set template pseudo variable
   void declareParameterDebugInfo(SyntaxWriter& writer, SNode node, MethodScope& scope, bool withThis, bool withSelf);

////   int countFields(SNode node);

   void compileParentDeclaration(SNode baseNode, ClassScope& scope, ref_t parentRef, bool ignoreSealed = false);
   void compileParentDeclaration(SNode node, ClassScope& scope);
   void generateClassFields(SNode member, ClassScope& scope, bool singleField);

   void declareSymbolAttributes(SNode node, SymbolScope& scope);
   void declareClassAttributes(SNode node, ClassScope& scope);
   void declareLocalAttributes(SNode hints, CodeScope& scope, ObjectInfo& variable, int& size);
   void declareFieldAttributes(SNode member, ClassScope& scope, ref_t& fieldType, ref_t& fieldRef, int& size);
   void declareVMT(SNode member, ClassScope& scope);
//   void declareClassVMT(SNode member, ClassScope& classClassScope, ClassScope& classScope);
//
////   void declareMethodAttribute(SyntaxWriter& writer, SNode current, MethodScope& scope, SNode rootNode);
   void declareMethodAttributes(SNode member, MethodScope& scope);
//   //void includeMethod(SNode member, ClassScope& classScope, MethodScope& scope);

   ref_t mapMessage(SNode node, CodeScope& scope, size_t& count/*, bool& argsUnboxing*/);

   void compileSwitch(SyntaxWriter& writer, SNode node, CodeScope& scope);
   void compileVariable(SyntaxWriter& writer, SNode node, CodeScope& scope);

   ObjectInfo compileClosure(SyntaxWriter& writer, SNode node, CodeScope& ownerScope, int mode);
   ObjectInfo compileClosure(SyntaxWriter& writer, SNode node, CodeScope& ownerScope, InlineClassScope& scope);
   ObjectInfo compileCollection(SyntaxWriter& writer, SNode objectNode, CodeScope& scope);
   ObjectInfo compileCollection(SyntaxWriter& writer, SNode objectNode, CodeScope& scope, ref_t vmtReference);

   ObjectInfo compileMessageReference(SyntaxWriter& writer, SNode objectNode, CodeScope& scope, int mode);
   void writeTerminal(SyntaxWriter& writer, SNode& terminal, CodeScope& scope, ObjectInfo object, int mode);
   void writeTerminalInfo(SyntaxWriter& writer, SNode node);

   ObjectInfo compileTerminal(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode);
   ObjectInfo compileObject(SyntaxWriter& writer, SNode objectNode, CodeScope& scope, int mode);

   ObjectInfo compileOperator(SyntaxWriter& writer, SNode node, CodeScope& scope, int operator_id, int paramCount, ObjectInfo loperand, ObjectInfo roperand, ObjectInfo roperand2);
   ObjectInfo compileOperator(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode, int operator_id);
   ObjectInfo compileOperator(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode);
   void compileBranchingNodes(SyntaxWriter& writer, SNode loperandNode, CodeScope& scope, ref_t ifReference, bool loopMode, bool switchMode);
   void compileBranchingOperand(SyntaxWriter& writer, SNode roperandNode, CodeScope& scope, int mode, int operator_id, ObjectInfo loperand, ObjectInfo& retVal);
   ObjectInfo compileBranchingOperator(SyntaxWriter& writer, SNode& node, CodeScope& scope, int mode, int operator_id);

   ObjectInfo compileMessageParameters(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode = 0);   // returns an info of the first operand

   ObjectInfo compileMessage(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode);
   ObjectInfo compileMessage(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target, int messageRef, int mode);
   ObjectInfo compileExtensionMessage(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo role, ref_t targetRef = 0, ref_t targetType = 0);

   ObjectInfo compileNewOperator(SyntaxWriter& writer, SNode node, CodeScope& scope/*, int mode*/);
   ObjectInfo compileAssigning(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode);
   ObjectInfo compileExtension(SyntaxWriter& writer, SNode node, CodeScope& scope);
   ObjectInfo compileExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode);
   ObjectInfo compileRetExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode);
   ObjectInfo compileAssigningExpression(SyntaxWriter& writer, SNode assigning, CodeScope& scope);

   ObjectInfo compileBranching(SyntaxWriter& writer, SNode thenNode, CodeScope& scope/*, ObjectInfo target, int verb, int subCodinteMode*/);

   void compileTrying(SyntaxWriter& writer, SNode node, CodeScope& scope);
   void compileAltOperation(SyntaxWriter& writer, SNode node, CodeScope& scope);
   void compileLoop(SyntaxWriter& writer, SNode node, CodeScope& scope);
//   void compileThrow(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode);

   int allocateStructure(bool bytearray, int& allocatedSize, int& reserved);
   int allocateStructure(SNode node, int& size);
   bool allocateStructure(CodeScope& scope, int size, bool bytearray, ObjectInfo& exprOperand);

   ObjectInfo compileExternalCall(SyntaxWriter& writer, SNode node, CodeScope& scope);
   ObjectInfo compileInternalCall(SyntaxWriter& writer, SNode node, CodeScope& scope, ref_t message, ObjectInfo info);

   void compileConstructorResendExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, ClassScope& classClassScope, bool& withFrame);
   void compileConstructorDispatchExpression(SyntaxWriter& writer, SNode node, CodeScope& scope);
   void compileResendExpression(SyntaxWriter& writer, SNode node, CodeScope& scope);
   void compileDispatchExpression(SyntaxWriter& writer, SNode node, CodeScope& scope);

   ObjectInfo compileCode(SyntaxWriter& writer, SNode node, CodeScope& scope);

   void declareArgumentList(SNode node, MethodScope& scope);
   ref_t declareInlineArgumentList(SNode node, MethodScope& scope);
   bool declareActionScope(SNode& node, ClassScope& scope, SNode argNode, ActionScope& methodScope, int mode/*, bool alreadyDeclared*/);

//   void declareSingletonClass(SNode node, ClassScope& scope);
////   void compileSingletonClass(SNode member, ClassScope& scope, SNode hints);
//
////   void declareSingletonAction(ClassScope& scope, SNode objNode);

   void compileActionMethod(SyntaxWriter& writer, SNode member, MethodScope& scope);
   void compileLazyExpressionMethod(SyntaxWriter& writer, SNode member, MethodScope& scope);
   void compileDispatcher(SyntaxWriter& writer, SNode node, MethodScope& scope, bool withGenericMethods = false, bool withOpenArgGenerics = false);

   void compileMethod(SyntaxWriter& writer, SNode node, MethodScope& scope);
   void compileConstructor(SyntaxWriter& writer, SNode node, MethodScope& scope, ClassScope& classClassScope);

   void compileDefaultConstructor(SyntaxWriter& writer, MethodScope& scope);
   void compileDynamicDefaultConstructor(SyntaxWriter& writer, MethodScope& scope);
////   void compileConstructor(SNode node, MethodScope& scope, ClassScope& classClassScope);
////   void compileEmbeddableConstructor(DNode node, SyntaxWriter& writer, MethodScope& scope, ClassScope& classClassScope);

   void compilePreloadedCode(SymbolScope& scope);
   void compileSymbolCode(ClassScope& scope);
////   void compileVirtualDispatchMethod(SyntaxWriter& writer, MethodScope& scope, LexicalType target, int argument = 0);

   void compileAction(SNode node, ClassScope& scope, SNode argNode, int mode/*, bool alreadyDeclared = false*/);
   void compileNestedVMT(SNode node, InlineClassScope& scope);

   void compileVMT(SyntaxWriter& writer, SNode node, ClassScope& scope);
   void compileClassVMT(SyntaxWriter& writer, SNode node, ClassScope& classClassScope, ClassScope& classScope);
////   void compileTemplateMethods(SNode node, ClassScope& scope);
//
////   void declareVirtualMethods(ClassScope& scope);
//
   void generateClassField(ClassScope& scope, SNode node, ref_t typeRef, ref_t fieldRef, int sizeHint, bool singleField);
//   void generateClassStaticField(ClassScope& scope, SNode current);

   void generateClassFlags(ClassScope& scope, SNode node);
   void generateMethodAttributes(ClassScope& scope, SyntaxTree::Node node, ref_t message);
   void generateMethodDeclaration(SNode current, ClassScope& scope, bool hideDuplicates, bool closed);
   void generateMethodDeclarations(SNode node, ClassScope& scope, bool closed, bool classClassMode);
   void generateClassDeclaration(SNode node, ClassScope& scope, bool classClassMode, bool closureDeclarationMode = false);

   void generateClassImplementation(SNode node, ClassScope& scope);

   void compileClassDeclaration(SNode node, ClassScope& scope);
   void compileClassImplementation(SyntaxTree& expressionTree, SNode node, ClassScope& scope);
   void compileClassClassDeclaration(SNode node, ClassScope& classClassScope, ClassScope& classScope);
   void compileClassClassImplementation(SyntaxTree& expressionTree, SNode node, ClassScope& classClassScope, ClassScope& classScope);
   void compileSymbolDeclaration(SNode node, SymbolScope& scope);
   void compileSymbolImplementation(SyntaxTree& expressionTree, SNode node, SymbolScope& scope);
   bool compileSymbolConstant(SNode node, SymbolScope& scope, ObjectInfo retVal);
   void compileIncludeModule(SNode node, ModuleScope& scope);
//   void compileForward(SNode node, ModuleScope& scope);
   void declareSubject(SyntaxWriter& writer, SNode member, ModuleScope& scope);

   bool validate(_ProjectManager& project, _Module* module, int reference);

   ObjectInfo assignResult(SyntaxWriter& writer, CodeScope& scope, ref_t targetRef, ref_t targetType = 0);

   bool convertObject(SyntaxWriter& writer, ModuleScope& scope, ref_t targetRef, ref_t targetType, ref_t sourceRef, ref_t sourceType);
   bool typecastObject(SyntaxWriter& writer, ref_t targetType);

   void compileExternalArguments(SNode node, ModuleScope& scope, WarningScope& warningScope);

   ref_t optimizeOp(SNode current, ModuleScope& scope, WarningScope& warningScope);
   ref_t optimizeSymbol(SNode& node, ModuleScope& scope, WarningScope& warningScope);
   ref_t optimizeAssigning(SNode node, ModuleScope& scope, WarningScope& warningScope);
   ref_t optimizeBoxing(SNode node, ModuleScope& scope, WarningScope& warningScope, int mode);
   ref_t optimizeArgBoxing(SNode node, ModuleScope& scope, WarningScope& warningScope, int mode);
   ref_t optimizeArgUnboxing(SNode node, ModuleScope& scope, WarningScope& warningScope, int mode);
   ref_t optimizeMessageCall(SNode node, ModuleScope& scope, WarningScope& warningScope);
   ref_t optimizeExpression(SNode node, ModuleScope& scope, WarningScope& warningScope, int mode = 0);
   ref_t optimizeInternalCall(SyntaxTree::Node node, ModuleScope& scope, WarningScope& warningScope);
   ref_t optimizeExtCall(SyntaxTree::Node node, ModuleScope& scope, WarningScope& warningScope);
   ref_t optimizeNestedExpression(SNode node, ModuleScope& scope, WarningScope& warningScope);
   void optimizeExpressionTree(SNode node, ModuleScope& scope, WarningScope& warningScope, int mode = 0);
   void optimizeCode(SNode node, ModuleScope& scope, WarningScope& warningScope);
   void optimizeMethod(SNode node, ModuleScope& scope, WarningScope& warningScope);
   void optimizeClassTree(SNode node, ClassScope& scope, WarningScope& warningScope);
   void optimizeSymbolTree(SNode node, SourceScope& scope, int warningMask);

   void defineEmbeddableAttributes(ClassScope& scope, SyntaxTree::Node node);

   void createPackageInfo(_Module* module, _ProjectManager& project);

   void compileDeclarations(SNode node, ModuleScope& scope);
   void compileImplementations(SNode node, ModuleScope& scope);

   void copyAttributeTree(SyntaxWriter& writer, SNode node, TemplateScope& scope/*, bool variableMode*/);
   void copyTreeNode(SyntaxWriter& writer, SNode node, TemplateScope& scope/*, bool methodMode = false*/);
   void copyExpressionTree(SyntaxWriter& writer, SNode node, TemplateScope& scope);
   void copyMethodTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, SyntaxTree& buffer);
   void copyFieldTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, SyntaxTree& buffer);

   void generateScopeMembers(SNode node, TemplateScope& scope);

   bool generateTemplate(SyntaxWriter& writer, TemplateScope& scope, bool declaringClass/*, bool embeddableMode*/);
   bool generateTemplateCode(SyntaxWriter& writer, TemplateScope& scope);
   void generateAttributes(SyntaxWriter& writer, SNode node, TemplateScope& scope, SNode attributes/*, bool variableMode, bool embeddableMode = false*/);
   void copyTemplateTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, SNode attributeValues/*, bool variableMode, bool embeddableMode*/);
   void generateVariableTree(SyntaxWriter& writer, SNode node, TemplateScope& scope);
   void generateArrayVariableTree(SyntaxWriter& writer, SNode node, TemplateScope& scope);
   void generateTemplateVariableTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, bool templateMode);
   void generateCodeTemplateTree(SyntaxWriter& writer, SNode node, TemplateScope& scope);
   void generateMessageTree(SyntaxWriter& writer, SNode node, TemplateScope& scope/*, bool operationMode*/);
   void generateSwitchTree(SyntaxWriter& writer, SNode current, TemplateScope& scope);
   void generateObjectTree(SyntaxWriter& writer, SNode node, TemplateScope& scope/*, int mode = 0*/);
   void generateExpressionTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, bool explicitOne = true);
   void generateSymbolTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, SNode attributes);
   void generateCodeTree(SyntaxWriter& writer, SNode node, TemplateScope& scope);
   void generateMethodTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, SNode attributes, SyntaxTree& buffer, bool templateMode = false);
   void generateFieldTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, SNode attributes, SyntaxTree& buffer, bool templateMode = false);
   void generateFieldTemplateTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, SNode attributes, SyntaxTree& buffer, bool templateMode = false);
   void generateTemplateTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, SNode attributes);
   // nested = -1 - nested class, -2 - singleton
   void generateClassTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, SNode attributes, int nested = 0);
   bool generateMethodScope(SNode node, TemplateScope& scope, SNode attributes);
   bool generateSingletonScope(SyntaxWriter& writer, SNode node, TemplateScope& scope, SNode attributes);
   void generateScope(SyntaxWriter& writer, SNode node, TemplateScope& scope, SNode attributes);

   bool generateDeclaration(SyntaxWriter& writer, SNode node, TemplateScope& scope, SNode attributes);

   void compileSyntaxTree(SyntaxTree& tree, ModuleScope& scope);

   void saveTemplate(_Memory* target, SNode node, ModuleScope& scope, SNode attributes, TemplateScope::Type type/*, SyntaxTree& autogenerated*/);
   void generateNewAttribute(SNode node, ModuleScope& scope, SNode attributes);
   void generateSyntaxTree(SyntaxWriter& writer, SNode node, ModuleScope& scope, SyntaxTree& autogenerated);

public:
   void loadRules(StreamReader* optimization);
   void turnOnOptimiation(int level)
   {
      _optFlag |= level;
   }

   void compileModule(_ProjectManager& project, ident_t file, SyntaxTree& tree, ModuleInfo& moduleInfo, Unresolveds& unresolveds);
   void compileSyntaxTree(_ProjectManager& project, ident_t file, SyntaxTree& tree, ModuleInfo& moduleInfo, Unresolveds& unresolveds);

   ModuleInfo createModule(ident_t name, _ProjectManager& project, bool withDebugInfo);

   void validateUnresolved(Unresolveds& unresolveds, _ProjectManager& project);

//   // _Compiler interface implementation
////   virtual void injectVirtualReturningMethod(SyntaxWriter& writer, ref_t messagRef, LexicalType type, int argument);
   virtual void injectBoxing(SyntaxWriter& writer, _CompilerScope& scope, LexicalType boxingType, int argument, ref_t targetClassRef);
   virtual void injectLocalBoxing(SNode node, int size);
   virtual void injectConverting(SyntaxWriter& writer, LexicalType convertOp, int convertArg, LexicalType createOp, int createArg, ref_t targetClassRef, bool stacksafe);
   virtual void injectEmbeddableGet(SNode assignNode, SNode callNode, ref_t subject);
   virtual void injectEmbeddableOp(SNode assignNode, SNode callNode, ref_t subject, int paramCount, int verb);
   virtual void injectFieldExpression(SyntaxWriter& writer);
   virtual void generateEnumListMember(_CompilerScope& scope, ref_t enumRef, ref_t memberRef);
   virtual ref_t readEnumListMember(_CompilerScope& scope, _Module* extModule, MemoryReader& reader);

   Compiler(_CompilerLogic* logic);
};

} // _ELENA_

#endif // compilerH
