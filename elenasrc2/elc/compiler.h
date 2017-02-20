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
//      ref_t  subj_ref;
//      int    size;

      Parameter()
      {
         offset = -1;
//         subj_ref = 0;
         class_ref = 0;
//         size = 0;
      }
      Parameter(int offset)
      {
         this->offset = offset;
//         this->subj_ref = 0;
         this->class_ref = 0;
//         this->size = 0;
      }
//      Parameter(int offset, ref_t subj_ref)
//      {
//         this->offset = offset;
//         this->subj_ref = subj_ref;
//         this->class_ref = 0;
//         this->size = 0;
//      }
      Parameter(int offset, /*ref_t subj_ref, */ref_t class_ref)
      {
         this->offset = offset;
         //this->subj_ref = subj_ref;
         this->class_ref = class_ref;
         //this->size = 0;
      }
//      Parameter(int offset, ref_t subj_ref, ref_t class_ref, int size)
//      {
//         this->offset = offset;
//         this->subj_ref = subj_ref;
//         this->class_ref = class_ref;
//         this->size = size;
//      }
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
//      okLiteralConstant,              // param - reference
//      okWideLiteralConstant,          // param - reference
//      okCharConstant,                 // param - reference
//      okIntConstant,                  // param - reference, extraparam - imm argument
//      okLongConstant,                 // param - reference
//      okRealConstant,                 // param - reference
//      okMessageConstant,              // param - reference
//      okExtMessageConstant,           // param - reference
//      okSignatureConstant,            // param - reference
//      okVerbConstant,                 // param - reference
//      okArrayConst,
      okField,                        // param - field offset, extraparam - class reference
//      okStaticField,                  // param - reference
//      okFieldAddress,                 // param - field offset, extraparam - class reference
      okOuter,                        // param - field offset, extraparam - class reference
      okOuterField,                   // param - field offset, extraparam - outer field offset
      okLocal,                        // param - local / out parameter offset, extraparam : class reference
      okParam,                        // param - parameter offset, extraparam = -1 (is stack safe) / 0
//      okParamField,
//      okSubject,                      // param - parameter offset
      okThisParam,                    // param - parameter offset, extraparam = -1 (stack allocated) / -2 (primitive array)
      okNil,
      okSuper,
//      okLocalAddress,                 // param - local offset, extraparam - class reference
//      okParams,                       // param - local offset
//      okBlockLocal,                   // param - local offset
//      okConstantRole,                 // param - role reference
//
//      okExternal,
//      okInternal,
   };

   struct ObjectInfo
   {
      ObjectKind kind;
      ref_t      param;
      ref_t      extraparam;
      ref_t      target;

      ObjectInfo()
      {
         this->kind = okUnknown;
         this->param = 0;
         this->extraparam = 0;
         this->target = 0;
      }
      ObjectInfo(ObjectKind kind)
      {
         this->kind = kind;
         this->param = 0;
         this->extraparam = 0;
         this->target = 0;
      }
      ObjectInfo(ObjectKind kind, ObjectInfo copy)
      {
         this->kind = kind;
         this->param = copy.param;
         this->extraparam = copy.extraparam;
         this->target = copy.target;
      }
      ObjectInfo(ObjectKind kind, ref_t param)
      {
         this->kind = kind;
         this->param = param;
         this->extraparam = 0;
         this->target = 0;
      }
      ObjectInfo(ObjectKind kind, int param)
      {
         this->kind = kind;
         this->param = (ref_t)param;
         this->extraparam = 0;
         this->target = 0;
      }
      ObjectInfo(ObjectKind kind, ref_t param, ref_t extraparam)
      {
         this->kind = kind;
         this->param = param;
         this->extraparam = extraparam;
         this->target = 0;
      }
      ObjectInfo(ObjectKind kind, ref_t param, int extraparam)
      {
         this->kind = kind;
         this->param = param;
         this->extraparam = (ref_t)extraparam;
         this->target = 0;
      }
      ObjectInfo(ObjectKind kind, ref_t param, ref_t extraparam, ref_t target)
      {
         this->kind = kind;
         this->param = param;
         this->extraparam = extraparam;
         this->target = target;
      }
      ObjectInfo(ObjectKind kind, ref_t param, int extraparam, ref_t target)
      {
         this->kind = kind;
         this->param = param;
         this->extraparam = (ref_t)extraparam;
         this->target = target;
      }
   };

   typedef Map<ident_t, ref_t>            ForwardMap;
   typedef MemoryMap<ident_t, Parameter>  LocalMap;
////   typedef MemoryMap<int, ref_t>          RoleMap;
//   typedef Map<ref_t, SubjectMap*>        ExtensionMap;

private:
   // - ModuleScope -
   struct ModuleScope : _CompilerScope
   {
      _ProjectManager* project;
      _Module*       debugModule;

      ident_t        sourcePath;
      ref_t          sourcePathRef;

      // default namespaces
      List<ident_t> defaultNs;
      ForwardMap    forwards;       // forward declarations

//      // symbol hints
//      Map<ref_t, ref_t> constantHints;
//
//      // extensions
//      SubjectMap        extensionHints;
//      ExtensionMap      extensions;

      // type hints
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

      // the module expression tree
      SyntaxTree expressionTree;

      ObjectInfo mapObject(SNode identifier);

      ref_t mapReference(ident_t reference, bool existing = false);
      //ref_t mapAttribute(ident_t reference, bool existing);

      ObjectInfo mapReferenceInfo(ident_t reference, bool existing = false);

//      void defineConstantSymbol(ref_t reference, ref_t classReference)
//      {
//         constantHints.add(reference, classReference);
//      }

      void raiseError(const char* message, int row, int col, ident_t terminal);
      void raiseWarning(int level, const char* message, int row, int col, ident_t terminal);

      void raiseError(const char* message, SNode terminal);
      void raiseWarning(int level, const char* message, SNode terminal);

      bool checkReference(ident_t referenceName);

      ref_t resolveIdentifier(ident_t name);

      ref_t mapNewSubject(ident_t terminal);
//
//      // NOTE : the function returns 0 for implicit subjects
//      // in any case output is set (for explicit one - the namespace is copied as well)
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
//      ref_t loadSymbolExpressionInfo(SymbolExpressionInfo& info, ident_t symbol);

      _Memory* loadAttributeInfo(ref_t reference/*, _Module* &argModule*/)
      {
         return loadAttributeInfo(module->resolveSubject(reference)/*, argModule*/);
      }
      _Memory* loadAttributeInfo(ident_t attribute/*, _Module* &argModule*/);

      void loadAttributes(_Module* module);
//      void loadExtensions(_Module* module, bool& duplicateExtensions);
      void loadActions(_Module* module);

      void saveSubject(ref_t attrRef, ref_t classReference, bool internalType);
//      bool saveExtension(ref_t message, ref_t type, ref_t role);
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
         //loadExtensions(extModule, duplicateExtensions);
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
         ObjectInfo object = mapTerminal(terminal.findChild(lxTerminal).identifier());
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

      virtual ref_t mapSubject(SNode terminal, bool implicitOnly = true)
      {
         if (parent) {
            return parent->mapSubject(terminal, implicitOnly);
         }
         else return moduleScope->mapSubject(terminal, implicitOnly);
      }

      virtual int getSourcePathRef()
      {
         if (parent) {
            return parent->getSourcePathRef();
         }
         else return 0;
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
      bool        withConstructors;
      int         declaredFlags;
//      ref_t       extensionMode;

      virtual int getMethodInfo(ref_t message, MethodAttribute attr)
      {
         return info.methodHints.get(ClassInfo::Attribute(message, attr));
      }
      virtual void setMethodInfo(ref_t message, MethodAttribute attr, int value, bool clearPreviousOne = true)
      {
         if (clearPreviousOne)
            info.methodHints.exclude(ClassInfo::Attribute(message, attr));

         info.methodHints.add(ClassInfo::Attribute(message, attr), value);
      }

      virtual bool isClosed()
      {
         return test(info.header.flags, elClosed);
      }

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

      virtual bool include(ref_t message)
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
//      bool  constant;
//      bool  preloaded;
//      ref_t typeRef;

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
      ref_t        resultRef;
//      bool         withOpenArg;
//      bool         stackSafe;
//      bool         classEmbeddable;
//      bool         generic;
//      bool         extensionTemplateMode;
////      bool         sealed;

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

//      ref_t getClassFlags(bool ownerClass = true)
//      {
//         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);
//
//         return scope ? scope->info.header.flags : 0;
//      }
//      ref_t getClassRef(bool ownerClass = true)
//      {
//         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);
//
//         return scope ? scope->reference : 0;
//      }

      virtual ObjectInfo mapTerminal(ident_t identifier);

      MethodScope(ClassScope* parent);
   };

   // - ActionScope -
   struct ActionScope : public MethodScope
   {
      bool subCodeMode;

      ActionScope(ClassScope* parent);

      virtual ObjectInfo mapTerminal(ident_t identifier);
   };

   // - CodeScope -
   struct CodeScope : public Scope
   {
      // scope local variables
      LocalMap     locals;
      int          level;

//      // scope stack allocation
//      int          reserved;  // allocated for the current statement
//      int          saved;     // permanently allocated

      int newLocal()
      {
         level++;

         return level;
      }

//      void mapLocal(ident_t local, int level/*, ref_t type*/)
//      {
//         locals.add(local, Parameter(level/*, type*/));
//      }
      void mapLocal(ident_t local, int level/*, ref_t type*/, ref_t class_ref/*, int size*/)
      {
         locals.add(local, Parameter(level/*, type*/, class_ref/*, size*/));
      }

//      void freeSpace()
//      {
//         reserved = saved;
//      }

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

//      ref_t getClassFlags(bool ownerClass = true)
//      {
//         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);
//
//         return scope ? scope->info.header.flags : 0;
//      }

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
      Map<ident_t, Outer>     outers;
      ClassInfo::FieldTypeMap outerFieldTypes;

      Outer mapSelf();

      ObjectInfo allocateRetVar();

//      bool markAsPresaved(ObjectInfo object);

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
   struct TemplateScope : ClassScope
   {
//      ref_t       templateRef;
      ForwardMap  parameters;
      SubjectMap  subjects;
//      bool        classMode;
//      bool        generationMode;
      int         sourceRef;

      virtual int getMethodInfo(ref_t message, MethodAttribute attr)
      {
         ClassScope* classScope = (ClassScope*)getScope(slClass);

         return classScope->getMethodInfo(message, attr);
      }
      virtual void setMethodInfo(ref_t message, MethodAttribute attr, int value, bool clearPreviousOne = true)
      {
         ClassScope* classScope = (ClassScope*)getScope(slClass);

         classScope->setMethodInfo(message, attr, value, clearPreviousOne);
      }

      virtual bool isClosed()
      {
         ClassScope* classScope = (ClassScope*)getScope(slClass);

         return classScope->isClosed();
      }

      virtual bool include(ref_t message)
      {
         ClassScope* classScope = (ClassScope*)getScope(slClass);

         return classScope->include(message);
      }

      virtual ref_t mapSubject(SNode terminal, IdentifierString& output)
      {
         ident_t name = terminal.findChild(lxTerminal).identifier();
         ref_t parameter = parameters.get(name);
         if (parameter != 0) {
            ref_t subjRef = subjects.get(parameter);
            output.append(moduleScope->module->resolveSubject(subjRef));

            return subjRef;
         }
         else return Scope::mapSubject(terminal, output);
      }

      virtual ref_t mapSubject(SNode terminal, bool implicitOnly = true)
      {
         ident_t identifier = terminal.findChild(lxTerminal).identifier();

         ref_t parameter = parameters.get(identifier);
         if (parameter != 0) {
            return subjects.get(parameter);
         }
         else return Scope::mapSubject(terminal, implicitOnly);
      }

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slTemplate) {
            return this;
         }
         //else if (level == slClass && classMode) {
         //   return this;
         //}
         else return parent->getScope(level);
      }

      virtual int getSourcePathRef()
      {
         return sourceRef;
      }

      void loadAttributeValues(SNode node);

//      void generateClassName(bool newName = false);
//
//      TemplateScope(ClassScope* parent)
//         : ClassScope(parent->moduleScope, parent->reference)
//      {
//         this->templateRef = 0;
//         this->parent = parent;
//         this->info.header.flags = 0;
//         this->classMode = false;
//         this->generationMode = false;
//         this->sourceRef = -1;
//      }
      TemplateScope(Scope* parent/*, ref_t attrRef*/)
         : ClassScope(parent->moduleScope, 0)
      {
         this->parent = parent;
//         this->templateRef = attrRef;
//         this->classMode = false;
//         this->generationMode = false;
//         this->sourceRef = -1;
      }
//      TemplateScope(ModuleScope* moduleScope, ref_t attrRef)
//         : ClassScope(moduleScope, 0)
//      {
//         this->parent = NULL;
//         this->templateRef = attrRef;
//         this->classMode = false;
//         this->generationMode = false;
//         this->sourceRef = -1;
//      }
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

//   //void insertDebugStep(SNode& node, int stepType)
//   //{
//   //   node.insertNode(lxBreakpoint, stepType);
//   //}
////   void appendDebugStep(SNode& node, int stepType)
////   {
////      node.appendNode(lxBreakpoint, stepType);
////   }
////   void setDebugStep(SNode& node, int stepType)
////   {
////      node.set(lxBreakpoint, stepType);
////   }
//
////   void raiseWarning(ModuleScope& scope, SNode node, ident_t message, int warningLevel, int warningMask, bool triggered = true);
////
////   void appendObjectInfo(CodeScope& scope, ObjectInfo object);
   void writeMessageInfo(SyntaxWriter& writer, ModuleScope& scope, ref_t messageRef);
   ref_t mapAttribute(SNode attribute, Scope& scope, int& attrValue);
//   ref_t mapAttribute(SNode attribute, ModuleScope& scope);
//   void initialize(Scope& scope, MethodScope& methodScope);
//
//   int checkMethod(ModuleScope& scope, ref_t reference, ref_t message)
//   {
//      _CompilerLogic::ChechMethodInfo dummy;
//
//      return _logic->checkMethod(scope, reference, message, dummy);
//   }

   ref_t resolveObjectReference(CodeScope& scope, ObjectInfo object);

//   ref_t mapExtension(CodeScope& scope, ref_t messageRef, ObjectInfo target);

   void importCode(SyntaxWriter& writer, SNode node, ModuleScope& scope, ident_t reference, ref_t message);

   InheritResult inheritClass(ClassScope& scope, ref_t parentRef, bool ignoreSealed);

   void declareParameterDebugInfo(SyntaxWriter& writer, SNode node, MethodScope& scope, bool withThis, bool withSelf);

   void declareTemplateParameters(SNode node, TemplateScope& templateScope);
   bool declareTemplate(SyntaxWriter& writer, SNode node, Scope* scope, ref_t attrRef, ObjectInfo& object, SNode attributeNode);
   bool declareTemplate(SyntaxWriter& writer, SNode node, Scope* scope, ref_t attrRef, SNode attributeNode)
   {
      ObjectInfo temp;

      return declareTemplate(writer, node, scope, attrRef, temp, attributeNode);
   }
////   bool copyFieldAttribute(Scope& scope, ref_t attrRef, SNode rootNode, SNode currentNode);

   void compileParentDeclaration(SNode baseNode, ClassScope& scope, ref_t parentRef, bool ignoreSealed = false);
   void compileParentDeclaration(SNode node, ClassScope& scope);
   void compileFieldDeclarations(SNode member, ClassScope& scope);

//   void compileSymbolAttributes(SNode node, SymbolScope& scope, SNode rootNode);
//   void compileSymbolAttributes(SNode node, SymbolScope& scope)
//   {
//      compileSymbolAttributes(node, scope, node);
//   }
   void declareClassAttribute(SyntaxWriter& writer, SNode node, ClassScope& scope, SNode rootNode);
   void declareClassAttributes(SyntaxWriter& writer, SNode node, ClassScope& scope);
   void declareLocalAttribute(SyntaxWriter& writer, SNode hints, CodeScope& scope, ObjectInfo& variable/*, int& size*/, SNode rootNode);
   void declareLocalAttributes(SyntaxWriter& writer, SNode hints, CodeScope& scope, ObjectInfo& variable/*, int& size*/);
   void declareFieldAttribute(SNode current, ClassScope& scope, SNode rootNode, ref_t& fieldRef);
   void declareFieldAttributes(SNode member, ClassScope& scope, ref_t& fieldRef);
   //void compileMethodAttributes(SNode hints, MethodScope& scope, SNode rootNode);
   void declareVMT(SyntaxWriter& writer, SNode member, ClassScope& scope);
   void declareClassVMT(SyntaxWriter& writer, SNode member, ClassScope& classClassScope, ClassScope& classScope);

//   void recognizeMemebers(SNode member, ClassScope& scope);
//   void readAttributes(SNode member, ClassScope& scope);
   void declareMethodAttribute(SyntaxWriter& writer, SNode current, MethodScope& scope, SNode rootNode);
   void declareMethodAttributes(SyntaxWriter& writer, SNode member, MethodScope& scope);
   void includeMethod(SNode member, ClassScope& classScope, MethodScope& scope);

//   void declareTemplateMethods(SNode node, ClassScope& scope);

   ref_t mapMessage(SNode node, CodeScope& scope, size_t& count/*, bool& argsUnboxing*/);

//   void compileSwitch(SNode node, CodeScope& scope);
   void declareVariable(SyntaxWriter& writer, SNode node, CodeScope& scope);

   ObjectInfo declareClosure(SyntaxWriter& writer, SNode node, CodeScope& ownerScope, int mode);
   ObjectInfo declareClosure(SyntaxWriter& writer, SNode node, CodeScope& ownerScope, InlineClassScope& scope);
//   ObjectInfo compileCollection(SNode objectNode, CodeScope& scope);
//   ObjectInfo compileCollection(SNode objectNode, CodeScope& scope, ref_t vmtReference);
//
//   ObjectInfo compileMessageReference(SNode objectNode, CodeScope& scope, int mode);
   void writeTerminal(SyntaxWriter& writer, SNode& terminal, CodeScope& scope, ObjectInfo object, int mode);
   void writeTerminalInfo(SyntaxWriter& writer, SNode node);

   ObjectInfo declareTerminal(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode);
   ObjectInfo declareObject(SyntaxWriter& writer, SNode objectNode, CodeScope& scope, int mode);

   ObjectInfo declareOperator(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode, int operator_id);
   ObjectInfo declareOperator(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode);
   void declareBranchingNodes(SyntaxWriter& writer, SNode loperandNode, CodeScope& scope, ref_t ifReference);
   ObjectInfo declareBranchingOperator(SyntaxWriter& writer, SNode& node, CodeScope& scope, int mode, int operator_id);

   ObjectInfo declareMessageParameters(SyntaxWriter& writer, SNode node, CodeScope& scope);   // returns an info of the first operand

   ObjectInfo declareMessage(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode);
   ObjectInfo declareMessage(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target, int messageRef, int mode);
   ObjectInfo declareExtensionMessage(SyntaxWriter& writer, SNode node, CodeScope& scope/*, ObjectInfo role, ref_t targetRef = 0*/);

////   ObjectInfo compileNewOperator(SNode node, CodeScope& scope/*, int mode*/);
   ObjectInfo declareAssigning(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode);
   ObjectInfo declareExtension(SyntaxWriter& writer, SNode node, CodeScope& scope);
   ObjectInfo declareExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode);
   ObjectInfo declareRetExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode);
   ObjectInfo declareAssigningExpression(SyntaxWriter& writer, SNode assigning, CodeScope& scope);

   ObjectInfo declareBranching(SyntaxWriter& writer, SNode thenNode, CodeScope& scope/*, ObjectInfo target, int verb, int subCodinteMode*/);

//   void compileTrying(SNode node, CodeScope& scope);
//   void compileAltOperation(SNode node, CodeScope& scope);
//   void compileLoop(SNode node, CodeScope& scope);
//   void compileThrow(SNode node, CodeScope& scope, int mode);
////   void compileTry(DNode node, CodeScope& scope);
//   void compileLock(SNode node, CodeScope& scope);
//
////   void compileExternalArguments(SNode node, CodeScope& scope/*, ExternalScope& externalScope*/);
////
////   int allocateStructure(bool bytearray, int& allocatedSize, int& reserved);
////   int allocateStructure(SNode node, int& size);
////   bool allocateStructure(CodeScope& scope, int size, bool bytearray, ObjectInfo& exprOperand);
////
////   ObjectInfo compileExternalCall(SNode node, CodeScope& scope);
////   ObjectInfo compileInternalCall(SNode node, CodeScope& scope, ref_t message, ObjectInfo info);
////
////   void compileConstructorResendExpression(SNode node, CodeScope& scope, ClassScope& classClassScope, bool& withFrame);
//   void compileConstructorDispatchExpression(SNode node, CodeScope& scope);
////   void compileResendExpression(SNode node, CodeScope& scope);
   void declareDispatchExpression(SyntaxWriter& writer, SNode node, CodeScope& scope);

   ObjectInfo declareCode(SyntaxWriter& writer, SNode node, CodeScope& scope);

   void declareArgumentList(SNode node, MethodScope& scope);
   ref_t declareInlineArgumentList(SNode node, MethodScope& scope);
   bool declareActionScope(SNode& node, ClassScope& scope, SNode argNode, ActionScope& methodScope, int mode/*, bool alreadyDeclared*/);

//   void declareSingletonClass(SNode node, ClassScope& scope, SNode hints);
//   void compileSingletonClass(SNode member, ClassScope& scope, SNode hints);
//
//   void declareSingletonAction(ClassScope& scope, SNode objNode);

   void declareActionMethod(SyntaxWriter& writer, SNode member, MethodScope& scope);
   void declareLazyExpressionMethod(SyntaxWriter& writer, SNode member, MethodScope& scope);
   void declareDispatcher(SyntaxWriter& writer, SNode node, MethodScope& scope/*, bool withGenericMethods = false*/);

//   void compileMethod(SNode node, MethodScope& scope);
   void declareMethod(SyntaxWriter& writer, SNode node, MethodScope& scope);
   void declareConstructor(SyntaxWriter& writer, SNode node, MethodScope& scope, ClassScope& classClassScope);

   void declareDefaultConstructor(SyntaxWriter& writer, SNode node, MethodScope& scope);
////   void compileDynamicDefaultConstructor(SNode node, MethodScope& scope);
////   void compileConstructor(SNode node, MethodScope& scope, ClassScope& classClassScope);
//////   void compileEmbeddableConstructor(DNode node, SyntaxWriter& writer, MethodScope& scope, ClassScope& classClassScope);
//
//   void compilePreloadedCode(SymbolScope& scope);
   void compileSymbolCode(ModuleScope& scope, ref_t reference);
//   void compileVirtualDispatchMethod(SyntaxWriter& writer, MethodScope& scope, LexicalType target, int argument = 0);

   void declareAction(SNode node, ClassScope& scope, SNode argNode, int mode/*, bool alreadyDeclared = false*/);
   void declareNestedVMT(SNode node, InlineClassScope& scope);

//   void compileVMT(SNode node, ClassScope& scope);
//   void compileTemplateMethods(SNode node, ClassScope& scope);
//
////   void declareVirtualMethods(ClassScope& scope);
//
//   ref_t generateTemplate(TemplateScope& scope);

   void generateClassField(ClassScope& scope, SNode node, ref_t fieldRef/*, bool singleField*/);
////   void generateClassStaticField(ClassScope& scope, SNode current);
////
////   void generateClassFlags(ClassScope& scope, SyntaxTree::Node root);
////   void generateClassFields(ClassScope& scope, SyntaxTree::Node root, bool singleField);
////   void generateMethodAttributes(ClassScope& scope, SyntaxTree::Node node, ref_t& message);
//   void generateMethodDeclarations(ClassScope& scope, SNode node, bool hideDuplicates, bool closed);
   void generateClassDeclaration(SyntaxWriter& writer, ClassScope& scope/*, bool closed*/);

   void generateClassImplementation(SNode node, ModuleScope& scope);

//   void buildClassDeclaration(SyntaxWriter& writer, SNode node, ClassScope& scope);
   void compileClassDeclaration(SyntaxWriter& writer, SNode node, ClassScope& scope);
   void compileClassImplementation(SNode node, ModuleScope& scope);
//   void compileClassClassDeclaration(SNode node, ClassScope& classClassScope, ClassScope& classScope);
   void compileClassClassDeclaration(SyntaxWriter& writer, SNode node, ClassScope& classClassScope, ClassScope& classScope);
////   void compileClassClassImplementation(SNode node, ClassScope& classClassScope, ClassScope& classScope);
   void compileSymbolDeclaration(SyntaxWriter& writer, SNode node, SymbolScope& scope);
//   void buildSymbolDeclaration(SNode node, SymbolScope& scope, SyntaxWriter& writer);
   void compileSymbolImplementation(SNode node, ModuleScope& scope);
////   bool compileSymbolConstant(SNode node, SymbolScope& scope, ObjectInfo retVal);
   void compileIncludeModule(SNode node, ModuleScope& scope);
////   void compileForward(SNode node, ModuleScope& scope);
   void declareSubject(SNode member, ModuleScope& scope);
////   void compileSubject(SNode member, ModuleScope& scope);
   void declareScope(SyntaxTree& buffer, SNode member, ModuleScope& scope);
//   void buildDeclaration(SyntaxWriter& writer, SNode member, ModuleScope& scope);
//
//   void compileDeclarations(SyntaxWriter& writer, SNode member, ModuleScope& scope);
//   void compileImplementations(SNode member, ModuleScope& scope);
   void compileIncludeSection(SNode node, ModuleScope& scope);

//   bool validate(_ProjectManager& project, _Module* module, int reference);

   bool typecastObject(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo source, ref_t subjectRef);
   bool boxObject(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo source, ref_t targetRef);

//   ObjectInfo assignResult(CodeScope& scope, SNode& node, ref_t targetRef, ref_t targetType = 0);

   bool convertObject(SNode node, CodeScope& scope, ref_t targetRef, ObjectInfo source);

//   void optimizeAssigning(ModuleScope& scope, SyntaxTree::Node node, WarningScope& warningScope);
//   void optimizeExtCall(ModuleScope& scope, SyntaxTree::Node node, WarningScope& warningScope);
//   void optimizeInternalCall(ModuleScope& scope, SyntaxTree::Node node, WarningScope& warningScope);
//   void optimizeCall(ModuleScope& scope, SyntaxTree::Node node, WarningScope& warningScope);
//   void optimizeOp(ModuleScope& scope, SyntaxTree::Node node, WarningScope& warningScope);
////   void optimizeNewOp(ModuleScope& scope, SyntaxTree::Node node, int warningLevel, int mode);
//
////   void optimizeBoxing(ModuleScope& scope, SNode node, WarningScope& warningScope, int mode);
//////   void optimizeTypecast(ModuleScope& scope, SyntaxTree::Node node, int warningLevel, int mode);
////   void optimizeArgUnboxing(ModuleScope& scope, SNode node, WarningScope& warningScope);
////   void optimizeNestedExpression(ModuleScope& scope, SNode node, WarningScope& warningScope);
////   void optimizeSyntaxNode(ModuleScope& scope, SNode node, WarningScope& warningScope, int mode);
////   void optimizeSyntaxExpression(ModuleScope& scope, SNode node, WarningScope& warningScope, int mode = 0);
   ref_t optimizeBoxing(SNode node, ModuleScope& scope, WarningScope& warningScope);
   ref_t optimizeMessageCall(SNode node, ModuleScope& scope, WarningScope& warningScope);
   ref_t optimizeExpression(SNode node, ModuleScope& scope, WarningScope& warningScope);
   void optimizeExpressionTree(SNode node, ModuleScope& scope, WarningScope& warningScope);
   void optimizeCode(SNode node, ModuleScope& scope, WarningScope& warningScope);
   void optimizeMethod(SNode node, ModuleScope& scope, WarningScope& warningScope);
   void optimizeClassTree(SNode node, ModuleScope& scope, WarningScope& warningScope);
////   void optimizeSymbolTree(SNode node, SourceScope& scope, int warningMask);
////
////   void defineEmbeddableAttributes(ClassScope& scope, SyntaxTree::Node node);
////
////   void createPackageInfo(_Module* module, _ProjectManager& project);

////
////   void compileModule(SNode node, ModuleScope& scope);
//   void buildTree(SyntaxWriter& writer, SNode node, ModuleScope& scope);
   void compileDeclaration(SyntaxTree& buffer, SNode node, ModuleScope& scope);

   void compileDeclarations(SNode node, ModuleScope& scope);
   void compileImplementations(SNode node, ModuleScope& scope);

public:
   void loadRules(StreamReader* optimization);
   void turnOnOptimiation(int level)
   {
      _optFlag |= level;
   }

   void compileModule(_ProjectManager& project, ident_t file, SyntaxTree& tree, ModuleInfo& moduleInfo, Unresolveds& unresolveds);

   ModuleInfo createModule(ident_t name, _ProjectManager& project, bool withDebugInfo);

   void validateUnresolved(Unresolveds& unresolveds, _ProjectManager& project);

   // _Compiler interface implementation
   virtual void injectVirtualReturningMethod(SyntaxWriter& writer, ref_t messagRef, LexicalType type, int argument);
//   virtual void injectBoxing(_CompilerScope& scope, SNode node, LexicalType boxingType, int argument, ref_t targetClassRef);
//   virtual void injectLocalBoxing(SNode node, int size);
//   virtual void injectConverting(SNode node, LexicalType convertOp, int convertArg, LexicalType createOp, int createArg, ref_t targetClassRef);
//   virtual void injectEmbeddableGet(SNode assignNode, SNode callNode, ref_t subject);
//   virtual void injectEmbeddableOp(SNode assignNode, SNode callNode, ref_t subject, int paramCount, int verb);
//   virtual void injectFieldExpression(SNode node);
   virtual void generateEnumListMember(_CompilerScope& scope, ref_t enumRef, ref_t memberRef);
   virtual ref_t readEnumListMember(_CompilerScope& scope, _Module* extModule, MemoryReader& reader);

   Compiler(_CompilerLogic* logic);
};

} // _ELENA_

#endif // compilerH
