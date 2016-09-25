//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler class.
//
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef compilerH
#define compilerH

#include "parser.h"
#include "bcwriter.h"
#include "compilercommon.h" 

namespace _ELENA_
{

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
      okIntConstant,                  // param - reference, extraparam - imm argument
//      okLongConstant,                 // param - reference 
//      okRealConstant,                 // param - reference 
//      okMessageConstant,              // param - reference 
//      okExtMessageConstant,           // param - reference 
//      okSignatureConstant,            // param - reference 
//      okVerbConstant,                 // param - reference 
//      okArrayConst,
      okField,                        // param - field offset, extraparam - class reference
//      okStaticField,                  // param - reference
//      okFieldAddress,                 // param - field offset
      okOuter,                        // param - field offset
      okOuterField,                   // param - field offset, extraparam - outer field offset
      okLocal,                        // param - local / out parameter offset, extraparam : class reference
      okParam,                        // param - parameter offset, extraparam - class reference
//      okParamField,
//      okSubject,                      // param - parameter offset
      okThisParam,                    // param - parameter offset
//      okNil,
      okSuper,
      okLocalAddress,                 // param - local offset, extraparam - class reference
//      okParams,                       // param - local offset
//      okBlockLocal,                   // param - local offset
//      okConstantRole,                 // param - role reference
//   
//      okTemplateTarget,
//      okTemplateLocal,
//   //   okTemplateTarget,
//
//      okExternal,
//      okInternal,
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
//      ObjectInfo(ObjectKind kind, ObjectInfo copy)
//      {
//         this->kind = kind;
//         this->param = copy.param;
//         this->extraparam = copy.extraparam;
//         this->type = copy.type;
//      }
      ObjectInfo(ObjectKind kind, ref_t param)
      {
         this->kind = kind;
         this->param = param;
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
      ObjectInfo(ObjectKind kind, ref_t param, ref_t extraparam, ref_t type)
      {
         this->kind = kind;
         this->param = param;
         this->extraparam = extraparam;
         this->type = type;
      }
   };

   typedef Map<ident_t, ref_t, false>     ForwardMap;
   typedef Map<ident_t, Parameter, false> LocalMap;
   typedef Map<ref_t, ref_t>              SubjectMap;   
//   typedef MemoryMap<int, ref_t>          RoleMap;
   typedef List<Unresolved>               Unresolveds;
//   typedef Map<ref_t, SubjectMap*>        ExtensionMap;
//
//   struct TemplateInfo
//   {
//      ref_t   templateRef;
//      ref_t   templateParent;
//      ref_t   targetType;
//      int     targetOffset;
//      ref_t   messageSubject;
//      ref_t   targetMessage;
//      ref_t   ownerRef;
//
//      int     sourceCol, sourceRow;
//
//      RoleMap parameters;
//
//      void save(MemoryWriter& writer)
//      {
//         writer.writeDWord(templateRef);
//         writer.writeDWord(templateParent);
//         writer.writeDWord(targetType);
//         writer.writeDWord(targetOffset);
//         writer.writeDWord(messageSubject);
//         writer.writeDWord(targetMessage);
//         writer.writeDWord(ownerRef);
//         writer.writeDWord(sourceCol);
//         writer.writeDWord(sourceRow);
//
//         parameters.write(&writer);
//      }
//
//      void load(MemoryReader& reader)
//      {
//         templateRef = reader.getDWord();
//         templateParent = reader.getDWord();
//         targetType = reader.getDWord();
//         targetOffset = reader.getDWord();
//         messageSubject = reader.getDWord();
//         targetMessage = reader.getDWord();
//         ownerRef = reader.getDWord();
//         sourceCol = reader.getDWord();
//         sourceRow = reader.getDWord();
//
//         parameters.read(&reader);
//      }
//
//      TemplateInfo()
//      {
//         targetType = 0;
//         templateRef = 0;
//         templateParent = 0;
//         targetOffset = -1;
//         messageSubject = 0;
//         ownerRef = 0;
//         targetMessage = 0;
//
//         sourceCol = sourceRow = 0;
//      }
//
//      TemplateInfo(ref_t templateRef, ref_t targetType)
//      {
//         this->templateRef = templateRef;
//         this->templateParent = 0;
//         this->targetType = targetType;
//         this->targetOffset = -1;
//         this->messageSubject = 0;
//         this->ownerRef = 0;
//
//         this->sourceCol = this->sourceRow = 0;
//      }
//
//      TemplateInfo(ref_t templateRef, ref_t targetType, int targetOffset)
//      {
//         this->templateRef = templateRef;
//         this->templateParent = 0;
//         this->targetType = targetType;
//         this->targetOffset = targetOffset;
//         this->messageSubject = 0;
//         this->ownerRef = 0;
//
//         this->sourceCol = this->sourceRow = 0;
//      }
//   };

private:
   // - ModuleScope -
   struct ModuleScope : _CompilerScope
   {
      _ProjectManager* project;
      _Module*       module;
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
      SubjectMap        attributeHints;

      // action hints
      SubjectMap        actionHints;

      // cached references
      ref_t superReference;
//      ref_t intReference;
//      ref_t longReference;
//      ref_t realReference;
//      ref_t literalReference;
//      ref_t wideReference;
//      ref_t charReference;
//      ref_t trueReference;
//      ref_t falseReference;
//      ref_t paramsReference;
//      ref_t signatureReference;
//      ref_t messageReference;
//      ref_t verbReference;
//      ref_t arrayReference;

      ref_t packageReference;

//      // cached subjects / hints
//      ref_t boolType;

      // warning mapiing
      bool warnOnUnresolved;
      bool warnOnWeakUnresolved;
      int  warningMask;

      // list of references to the current module which should be checked after the project is compiled
      Unresolveds* forwardsUnresolved;

      ObjectInfo mapObject(SNode identifier);

      ref_t mapReference(ident_t reference, bool existing = false);
      ref_t mapAttribute(ident_t reference, bool existing);

//      ObjectInfo mapReferenceInfo(ident_t reference, bool existing = false);
//
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

      ref_t mapNewAttribute(ident_t terminal);

      // NOTE : the function returns 0 for implicit subjects
      // in any case output is set (for explicit one - the namespace is copied as well)
      ref_t mapAttribute(SNode terminal, IdentifierString& output);
      ref_t mapAttribute(SNode terminal, bool explicitOnly = true);
      ref_t resolveAttributeRef(ident_t name, bool explicitOnly = true);

      ref_t mapTerminal(SNode terminal, bool existing = false);

      ObjectInfo defineObjectInfo(ref_t reference, bool checkState = false);

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

////      bool recognizePrimitive(ident_t name, ident_t value, size_t& roleMask, int& size);
//
//      int defineStructSizeEx(ref_t classReference, bool& variable, bool embeddableOnly = true);
//      int defineStructSize(ref_t classReference, bool embeddableOnly = true)
//      {
//         bool dummy;
//         return defineStructSizeEx(classReference, dummy, embeddableOnly);
//      }
//      int defineSubjectSizeEx(ref_t type_ref, bool& variable, bool embeddableOnly = true);
//      int defineSubjectSize(ref_t type_ref, bool embeddableOnly = true)
//      {
//         bool dummy;
//         return defineSubjectSizeEx(type_ref, dummy, embeddableOnly);
//      }
//
//      int checkMethod(ClassInfo& info, ref_t message, ref_t& outputType);
//      int checkMethod(ref_t reference, ref_t message, bool& found, ref_t& outputType);
//      int checkMethod(ref_t reference, ref_t message)
//      {
//         bool dummy;
//         ref_t dummyRef;
//         return checkMethod(reference, message, dummy, dummyRef);
//      }

      void loadAttributes(_Module* module);
//      void loadExtensions(TerminalInfo terminal, _Module* module);
      void loadActions(_Module* module);

      void saveAttribute(ref_t attrRef, ref_t classReference, bool internalType);
//      void saveTemplate(ref_t template_ref);
//      bool saveExtension(ref_t message, ref_t type, ref_t role);
//      void saveAction(ref_t message, ref_t reference);

      void validateReference(SNode terminal, ref_t reference);

//      ref_t getBaseLazyExpressionClass();
//
//      int getClassFlags(ref_t reference);
//      int getTypeFlags(ref_t subject)
//      {
//         return getClassFlags(subjectHints.get(subject));
//      }
//
//      bool checkIfCompatible(ref_t typeRef, ref_t classRef);
//      ref_t defineType(ref_t classRef);

      void importClassInfo(ClassInfo& copy, ClassInfo& target, _Module* exporter, bool headerOnly);

      void loadModuleInfo(_Module* extModule)
      {
         loadAttributes(extModule);
         //loadExtensions(TerminalInfo(), extModule);
         loadActions(extModule);
      }

      ref_t mapNestedExpression();
//      ref_t mapNestedTemplate();

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
//      void raiseError(const char* message, SyntaxTree::Node node)
//      {
//         SyntaxTree::Node row = SyntaxTree::findChild(node, lxRow);
//         SyntaxTree::Node col = SyntaxTree::findChild(node, lxCol);
//         SyntaxTree::Node terminal = SyntaxTree::findChild(node, lxTerminal);
//
//         moduleScope->raiseError(message, row.argument, col.argument, terminal.identifier());
//      }
//      void raiseWarning(int level, const char* message, SyntaxTree::Node node)
//      {
//         SyntaxTree::Node row = SyntaxTree::findChild(node, lxRow);
//         SyntaxTree::Node col = SyntaxTree::findChild(node, lxCol);
//         SyntaxTree::Node terminal = SyntaxTree::findChild(node, lxTerminal);
//
//         moduleScope->raiseWarning(level, message, row.argument, col.argument, terminal.identifier());
//      }

      ObjectInfo mapObject(SNode terminal)
      {
         ObjectInfo object = mapTerminal(terminal.findChild(lxTerminal).identifier());
         if (object.kind == okUnknown) {
            if (parent) {
               return parent->mapObject(terminal);
            }
            else return moduleScope->mapObject(terminal);
         }
         else return object;
      }

      virtual ObjectInfo mapTerminal(ident_t identifier)
      {
         return ObjectInfo();
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
         else return moduleScope->mapAttribute(terminal, output);
      }

//      virtual bool isVirtualSubject(TerminalInfo terminal)
//      {
//         if (parent) {
//            return parent->isVirtualSubject(terminal);
//         }
//         else return false;
//      }

      virtual ref_t mapSubject(SNode terminal, bool implicitOnly = true)
      {
         if (parent) {
            return parent->mapSubject(terminal, implicitOnly);
         }
         else return moduleScope->mapAttribute(terminal, implicitOnly);
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
//      SyntaxTree syntaxTree;
//      MemoryDump imported;

      CommandTape    tape;
      ref_t          reference;

      SourceScope(ModuleScope* parent, ref_t reference);
   };

   // - ClassScope -
   struct ClassScope : public SourceScope
   {
      ClassInfo   info;
//      ref_t       extensionMode;

      virtual ObjectInfo mapTerminal(ident_t identifier);

      void compileClassAttribute(SyntaxTree::Node hint);
//      //void compileFieldHints(DNode hints, int& size, ref_t& type);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slClass || level == slOwnerClass) {
            return this;
         }
         else return Scope::getScope(level);
      }

//      virtual bool validateTemplate(ref_t hintRef)
//      {
//         _Module* extModule = NULL;
//         return moduleScope->loadTemplateInfo(hintRef, extModule) != 0;
//      }

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
//      CommandTape* tape;

      ref_t        message;
      LocalMap     parameters;
      int          reserved;           // defines inter-frame stack buffer (excluded from GC frame chain)
      int          rootToFree;         // by default is 1, for open argument - contains the list of normal arguments as well
//      bool         withOpenArg;
//      bool         stackSafe;
//      bool         generic;
//      bool         sealed;

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slMethod) {
            return this;
         }
         else return parent->getScope(level);
      }

//      ref_t getReturningType() const
//      {
//         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);
//
//         return scope->info.methodHints.get(ClassInfo::Attribute(message, maType));
//      }
//
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
      ActionScope(ClassScope* parent);

      virtual ObjectInfo mapTerminal(ident_t identifier);
   };

   // - CodeScope -
   struct CodeScope : public Scope
   {
//      SyntaxWriter* writer;
////      int           rootBookmark;   // !! should be removed??

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
      void mapLocal(ident_t local, int level, ref_t type, size_t class_ref, int size)
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

//      ref_t getFieldType(int offset, bool ownerClass = true)
//      {
//         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);
//
//         return scope ? scope->info.fieldTypes.get(offset) : 0;
//      }
//
//      ref_t getClassFlags(bool ownerClass = true)
//      {
//         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);
//
//         return scope ? scope->info.header.flags : 0;
//      }

      CodeScope(SymbolScope* parent);
      CodeScope(MethodScope* parent);
//      CodeScope(CodeScope* parent);
   };

   // - InlineClassScope -
   struct InlineClassScope : public ClassScope
   {
      struct Outer
      {
         int        reference;
         bool       preserved;
         ObjectInfo outerObject;

         Outer()
         {
            reference = -1;
            preserved = false;
         }
         Outer(int reference, ObjectInfo outerObject)
         {
            this->reference = reference;
            this->outerObject = outerObject;
            this->preserved = false;
         }
      };

      //bool                    templateMode;
      //ref_t                   templateRef;
      Map<ident_t, Outer>     outers;
      ClassInfo::FieldTypeMap outerFieldTypes;

      Outer mapSelf();

      //bool markAsPresaved(ObjectInfo object);

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
//      enum Type
//      {
//         ttNone = 0,
//         ttClass,
//         ttField,
//         ttMethod
//      };
//
//      ref_t       templateRef;
//      Type        type;
//      ForwardMap  parameters;
//
//      // NOTE : reference is defined in subject namespace, so templateRef should be initialized and used
//      // proper reference is 0 in this case
//      TemplateScope(ModuleScope* parent, ref_t reference);
//
//      virtual ObjectInfo mapObject(TerminalInfo identifier);
//
//      virtual bool validateTemplate(ref_t reference);
//
//      virtual ref_t mapSubject(TerminalInfo terminal, IdentifierString& output)
//      {
//         ref_t parameter = parameters.get(terminal);
//         if (parameter != 0) {
//            int offset = output.Length();
//
//            output.append(TARGET_POSTFIX);
//            output.appendInt((int)parameter);            
//
//            return moduleScope->module->mapSubject(output + offset, false);
//         }
//         else return moduleScope->mapSubject(terminal, output);
//      }
//
//      virtual bool isVirtualSubject(TerminalInfo terminal)
//      {
//         return parameters.exist(terminal);
//      }
//
//      virtual ref_t mapSubject(TerminalInfo terminal, bool implicitOnly = true)
//      {
//         ref_t parameter = parameters.get(terminal);
//         if (parameter != 0) {
//            IdentifierString output;
//            output.copy(TARGET_POSTFIX);
//            output.appendInt((int)parameter);
//
//            return moduleScope->module->mapSubject(output, false);
//         }
//         else return Scope::mapSubject(terminal, implicitOnly);
//      }

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slTemplate) {
            return this;
         }
         else return parent->getScope(level);
      }

//      void save()
//      {
//         _Memory* section = moduleScope->module->mapSection(templateRef | mskSyntaxTreeRef, false);
//         section->trim(0);
//
//         syntaxTree.save(section);
//      }

      TemplateScope(ClassScope* parent)
         : ClassScope(parent->moduleScope, parent->reference)
      {
         this->parent = parent;
         this->info.header.flags = 0;
      }
   };

   _CompilerLogic*  _logic;

   ByteCodeWriter _writer;
   Parser         _parser;

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
   
   void insertDebugStep(SNode& node, int stepType)
   {
      node.insertNode(lxBreakpoint, stepType);
   }
   void appendDebugStep(SNode& node, int stepType)
   {
      node.appendNode(lxBreakpoint, stepType);
   }
   void setDebugStep(SNode& node, int stepType)
   {
      node.set(lxBreakpoint, stepType);
   }

//   void raiseWarning(ModuleScope& scope, SNode node, ident_t message, int warningLevel, int warningMask, bool triggered = true);
//
//   void appendObjectInfo(CodeScope& scope, ObjectInfo object);
   void insertMessage(SNode node, ModuleScope& scope, ref_t messageRef);
   ref_t mapAttribute(SNode attribute, int paramCounter, ModuleScope& scope, int& attrValue);

//   bool checkIfCompatible(ModuleScope& scope, ref_t typeRef, SyntaxTree::Node node);
//   bool checkIfImplicitBoxable(ModuleScope& scope, ref_t sourceClassRef, ClassInfo& targetInfo);
   ref_t resolveObjectReference(CodeScope& scope, ObjectInfo object);

//   ref_t mapNestedExpression(CodeScope& scope);
//   ref_t mapExtension(CodeScope& scope, ref_t messageRef, ObjectInfo target);

   void importCode(SNode node, ModuleScope& scope, ident_t reference, ref_t message);

   InheritResult inheritClass(ClassScope& scope, ref_t parentRef, bool ignoreSealed);

//   ref_t declareInlineTemplate(ModuleScope& scope, SNode node, TemplateInfo& templateInfo, ref_t inlineTemplateRef);

   void declareParameterDebugInfo(SNode node, MethodScope& scope, bool withThis, bool withSelf);

//   void readFieldTermplateHints(ModuleScope& scope, ref_t templateRef, ref_t& target, int& size);
//   bool readSymbolTermplateHints(SymbolScope& scope, ref_t templateRef);
//
//   bool declareAttribute(DNode hint, ClassScope& scope, SyntaxWriter& writer, ref_t hintRef, RoleMap* attributes = NULL);
//   bool declareMethodAttribute(DNode hint, MethodScope& scope, SyntaxWriter& writer, ref_t hintRef);
//   void declareTemplateParameters(DNode hint, ModuleScope& scope, RoleMap& parameters);
//   void updateMethodTemplateInfo(MethodScope& scope, size_t rollbackPosition);
//
//   void importTemplateInfo(SyntaxTree::Node node, ClassScope& scope, ref_t ownerRef, _Module* templateModule, TemplateInfo& info);
//   void copyTemplateDeclaration(ClassScope& scope, SyntaxTree::Node node, SyntaxTree::Writer& writer, _Module* templateModule, 
//                                 TemplateInfo& info, RoleMap* attributes);
//   bool copyTemplateDeclaration(ClassScope& scope, TemplateInfo& info, SyntaxTree::Writer& writer, RoleMap* attributes = NULL);
//   void copyTemplateInfo(TemplateInfo& info, SyntaxTree::Writer& writer);

   void compileParentDeclaration(SNode baseNode, ClassScope& scope, ref_t parentRef, bool ignoreSealed = false);
   void compileParentDeclaration(SNode node, ClassScope& scope);
   void compileFieldDeclarations(SNode member, ClassScope& scope); 
//   void compileTemplateFieldDeclaration(DNode& node, SyntaxWriter& writer, TemplateScope& scope);
//
//   void compileSymbolHints(DNode hints, SymbolScope& scope, bool silentMode);
//   bool compileClassHint(DNode hint, SyntaxWriter& writer, ClassScope& scope);
   void compileClassAttributes(SNode node, ClassScope& scope);
//   void compileSingletonHints(DNode hints, SyntaxWriter& writer, ClassScope& scope);
//
//   void compileTemplateHints(DNode hints, SyntaxWriter& writer, TemplateScope& scope);
   void compileLocalAttributes(SNode hints, CodeScope& scope, ObjectInfo& variable, int& size);
   void compileFieldAttributes(SNode hints, ClassScope& scope);
   void compileMethodAttributes(SNode hints, MethodScope& scope);
   void declareVMT(SNode member, ClassScope& scope);

//   bool importTemplateDeclarations(ClassScope& scope, SyntaxWriter& writer);
//   bool importTemplateDeclaration(ClassScope& scope, SyntaxWriter& writer, TemplateInfo& templateInfo);
//   void importTemplateImplementations(ClassScope& scope, SyntaxWriter& writerf);
//   void importTemplateImplementation(ClassScope& scope, SyntaxWriter& writer, TemplateInfo& templateInfo);
//   void importTemplateTree(ClassScope& scope, SyntaxWriter& writer, SyntaxTree::Node node, TemplateInfo& info, _Module* templateModule);
//   void copyNode(ClassScope& scope, SyntaxTree::Node node, SyntaxWriter& writer, _Module* templateModule, TemplateInfo& info);
//   void copyTree(ClassScope& scope, SyntaxTree::Node node, SyntaxWriter& writer, _Module* templateModule, TemplateInfo& info);
////   bool validateMethodTemplate(SyntaxTree::Node node, ref_t& targetMethod);

   ref_t mapMessage(SNode node, CodeScope& scope, size_t& count/*, bool& argsUnboxing*/);
//   ref_t mapMessage(DNode node, CodeScope& scope, size_t& count)
//   {
//      bool dummy = false;
//      return mapMessage(node, scope, count, dummy);
//   }
//
//   void compileSwitch(DNode node, CodeScope& scope, ObjectInfo switchValue);
   void compileVariable(SNode node, CodeScope& scope/*, DNode hints*/);

   ObjectInfo compileClosure(SNode node, CodeScope& ownerScope, int mode);
   ObjectInfo compileClosure(SNode node, CodeScope& ownerScope, InlineClassScope& scope, int mode);
//   ObjectInfo compileCollection(DNode objectNode, CodeScope& scope, int mode);
//   ObjectInfo compileCollection(DNode objectNode, CodeScope& scope, int mode, ref_t vmtReference);
//
//   ObjectInfo compileMessageReference(DNode objectNode, CodeScope& scope);
   void setTerminal(SNode& terminal, CodeScope& scope, ObjectInfo object, int mode);

   ObjectInfo compileTerminal(SNode node, CodeScope& scope, int mode);
   ObjectInfo compileObject(SNode objectNode, CodeScope& scope, int mode);

//   int mapOpArg(Compiler::ModuleScope& scope, SNode arg, ref_t& target);
//   int mapOpArg(Compiler::ModuleScope& scope, SNode arg);

   ObjectInfo compileOperator(SNode node, CodeScope& scope, /*ObjectInfo object, int mode, */int operator_id);
   ObjectInfo compileOperator(SNode node, CodeScope& scope/*, ObjectInfo object, int mode*/);
   ObjectInfo compileBranchingOperator(SNode& node, CodeScope& scope, /*ObjectInfo object, int mode, */int operator_id);

   ObjectInfo compileMessageParameters(SNode node, CodeScope& scope);   // returns an info of the first operand

   ObjectInfo compileMessage(SNode node, CodeScope& scope);
   ObjectInfo compileMessage(SNode node, CodeScope& scope, ObjectInfo target, int messageRef, int mode);
//   ObjectInfo compileExtensionMessage(DNode node, CodeScope& scope, ObjectInfo object, ObjectInfo role/*, int mode*/);
//
//   ObjectInfo compileNewOperator(DNode node, CodeScope& scope, int mode);
   ObjectInfo compileAssigning(SNode node, CodeScope& scope, int mode);
   ObjectInfo compileOperations(SNode node, CodeScope& scope, ObjectInfo target, int mode);   
//   ObjectInfo compileExtension(DNode& node, CodeScope& scope, ObjectInfo object, int mode);
   ObjectInfo compileExpression(SNode node, CodeScope& scope/*, ref_t targetType*/, int mode);
   ObjectInfo compileRetExpression(SNode node, CodeScope& scope, int mode);
   ObjectInfo compileAssigningExpression(SNode node, SNode assigning, CodeScope& scope, ObjectInfo target, int mode = 0);

//   ObjectInfo compileBranching(DNode thenNode, CodeScope& scope/*, ObjectInfo target, int verb, int subCodinteMode*/);
//
//   void compileLoop(DNode node, CodeScope& scope);
//   void compileThrow(DNode node, CodeScope& scope, int mode);
////   void compileTry(DNode node, CodeScope& scope);
//   void compileLock(DNode node, CodeScope& scope);
//
//   void compileExternalArguments(DNode node, CodeScope& scope/*, ExternalScope& externalScope*/);

   int allocateStructure(/*bool bytearray, */size_t& allocatedSize, int& reserved);
   int allocateStructure(/*ModuleScope& scope, */SNode node, size_t& size);
   bool allocateStructure(CodeScope& scope, size_t size, /*bool bytearray, */ObjectInfo& exprOperand);

//   ObjectInfo compileExternalCall(DNode node, CodeScope& scope, ident_t dllName, int mode);
//   ObjectInfo compileInternalCall(DNode node, CodeScope& scope, ObjectInfo info);
//
//   void compileConstructorResendExpression(DNode node, CodeScope& scope, ClassScope& classClassScope, bool& withFrame);
//   void compileConstructorDispatchExpression(DNode node, SyntaxWriter& writer, CodeScope& scope);
//   void compileResendExpression(DNode node, CodeScope& scope, CommandTape* tape);
//   void compileDispatchExpression(DNode node, CodeScope& scope, CommandTape* tape);

   ObjectInfo compileCode(SNode node, CodeScope& scope);

   void declareArgumentList(SNode node, MethodScope& scope);
//   ref_t declareInlineArgumentList(DNode node, MethodScope& scope);
   bool declareActionScope(SNode& node, ClassScope& scope/*, DNode argNode, SyntaxWriter& writer*/, ActionScope& methodScope, int mode, bool alreadyDeclared);

//   void declareSingletonClass(DNode member, DNode parentNode, ClassScope& scope, DNode hints);
//   void compileSingletonClass(DNode member, ClassScope& scope, DNode hints);
//
//   void declareSingletonAction(ClassScope& scope, DNode objNode, DNode expression, DNode hints);

   void compileActionMethod(SNode member, /*SyntaxWriter& writer, */MethodScope& scope);
//   void compileLazyExpressionMethod(DNode member, SyntaxWriter& writer, MethodScope& scope);
   void compileDispatcher(SNode node, MethodScope& scope/*, bool withGenericMethods = false*/);

   void compileMethod(SNode node, MethodScope& scope);
   void compileDefaultConstructor(SNode node, MethodScope& scope, ClassScope& classClassScope);
//   void compileDynamicDefaultConstructor(MethodScope& scope, SyntaxWriter& writer, ClassScope& classClassScope);
   void compileConstructor(SNode node, MethodScope& scope, ClassScope& classClassScope, ref_t embeddedMethodRef = 0);
//   void compileEmbeddableConstructor(DNode node, SyntaxWriter& writer, MethodScope& scope, ClassScope& classClassScope);

//   void compilePreloadedCode(SymbolScope& scope);
   void compileSymbolCode(ClassScope& scope);
//   void compileVirtualDispatchMethod(SyntaxWriter& writer, MethodScope& scope, LexicalType target, int argument = 0);

   void compileAction(SNode node, ClassScope& scope, /*SNode argNode, */int mode, bool alreadyDeclared = false);
//   void compileNestedVMT(DNode node, DNode parent, InlineClassScope& scope);

   void compileVMT(SNode member, /*SyntaxWriter& writer, */ClassScope& scope);

//   void declareVirtualMethods(ClassScope& scope);
//
//   ref_t generateTemplate(ModuleScope& scope, TemplateInfo& templateInfo, ref_t reference);

   void generateClassField(ClassScope& scope, SyntaxTree::Node node/*, bool singleField*/);
//   void generateClassStaticField(ClassScope& scope, SNode current);   

   void generateClassFlags(ClassScope& scope, SyntaxTree::Node root);
   void generateClassFields(ClassScope& scope, SyntaxTree::Node root);
   void generateMethodAttributes(ClassScope& scope, SyntaxTree::Node node, ref_t message);
   void generateMethodDeclarations(ClassScope& scope, SNode node, bool hideDuplicates, bool closed);
   void generateClassDeclaration(SNode node, ClassScope& scope, bool closed);
   void generateInlineClassDeclaration(SNode node, ClassScope& scope, bool closed);

   void generateClassImplementation(SNode node, ClassScope& scope);

   void compileClassDeclaration(SNode node, ClassScope& scope);
   void compileClassImplementation(SNode node, ClassScope& scope/*, DNode hints*/);
//   void compileTemplateDeclaration(DNode node, TemplateScope& scope, DNode hints);
   void compileClassClassDeclaration(SNode node, ClassScope& classClassScope, ClassScope& classScope);
   void compileClassClassImplementation(SNode node, ClassScope& classClassScope, ClassScope& classScope);
   void compileSymbolDeclaration(SNode node, SymbolScope& scope/*, DNode hints*/);
   void compileSymbolImplementation(SNode node, SymbolScope& scope/*, DNode hints, bool isStatic*/);
//   bool compileSymbolConstant(SymbolScope& scope, ObjectInfo retVal);
   void compileIncludeModule(SNode node, ModuleScope& scope/*, DNode hints*/);
   void declareSubject(SNode member, ModuleScope& scope);
//   void compileSubject(DNode& member, ModuleScope& scope, DNode hints);

   void compileDeclarations(SNode member, ModuleScope& scope);
   void compileImplementations(SNode member, ModuleScope& scope);
   void compileIncludeSection(SNode& node, ModuleScope& scope);

   bool validate(_ProjectManager& project, _Module* module, int reference);
   void validateUnresolved(Unresolveds& unresolveds, _ProjectManager& project);

////   void compileWarningHints(ModuleScope& scope, DNode hints, SyntaxWriter& writer);
//
//   int tryTypecasting(ModuleScope& scope, ref_t targetType, SNode& node, SNode& object, bool& typecasted, int mode);
   ObjectInfo typecastObject(SNode node, CodeScope& scope, ref_t subjectRef, ObjectInfo object);
   bool convertObject(SNode node, CodeScope& scope, ref_t targetRef, ref_t sourceRef);

   void optimizeAssigning(ModuleScope& scope, SyntaxTree::Node node/*, int warningLevel*/);
   ObjectInfo assignResult(CodeScope& scope, SNode& node, ref_t targetRef/*, int warningLevel, int mode, bool& variable*/);
//   bool boxPrimitive(ModuleScope& scope, SyntaxTree::Node& node, ref_t targetRef, int warningLevel, int mode)
//   {
//      bool dummy;
//      return boxPrimitive(scope, node, targetRef, warningLevel, mode, dummy);
//   }
//   void optimizeExtCall(ModuleScope& scope, SyntaxTree::Node node, int warningLevel, int mode);
//   void optimizeInternalCall(ModuleScope& scope, SyntaxTree::Node node, int warningLevel, int mode);
//   void optimizeDirectCall(ModuleScope& scope, SyntaxTree::Node node, int warningLevel);
//   void optimizeCall(ModuleScope& scope, SyntaxTree::Node node, int warningLevel);
//   void optimizeEmbeddableCall(ModuleScope& scope, SyntaxTree::Node& assignNode, SyntaxTree::Node& callNode);
   /*bool*/void optimizeOp(ModuleScope& scope, SyntaxTree::Node node, /*int warningLevel, */int mode);
//   void optimizeNewOp(ModuleScope& scope, SyntaxTree::Node node, int warningLevel, int mode);
//   void optimizeBoolOp(ModuleScope& scope, SyntaxTree::Node node, int warningLevel, int mode);
//
//   // NOTE : return true if the target is required unboxing
//   bool defineTargetSize(ModuleScope& scope, SNode& node);

   void optimizeBoxing(ModuleScope& scope, SNode node, /*int warningLevel, */int mode);
//   void optimizeTypecast(ModuleScope& scope, SyntaxTree::Node node, int warningLevel, int mode);
//   void optimizeArgUnboxing(ModuleScope& scope, SyntaxTree::Node node, int warningLevel);
//   void optimizeNestedExpression(ModuleScope& scope, SyntaxTree::Node node, int warningLevel, int mode = 0);
   void optimizeSyntaxNode(ModuleScope& scope, SNode node, /*int warningLevel, */int mode);
   void optimizeSyntaxExpression(ModuleScope& scope, SNode node, /*int warningLevel, */int mode = 0);
   void optimizeClassTree(SNode node, ClassScope& scope);
//   void optimizeSymbolTree(SourceScope& scope);
//
//   bool recognizeEmbeddableGet(ModuleScope& scope, SyntaxTree& tree, SyntaxTree::Node node, ref_t returningType, ref_t& subject);
//   bool recognizeEmbeddableIdle(SyntaxTree& tree, SyntaxTree::Node node);
//   void defineEmbeddableAttributes(ClassScope& scope, SyntaxTree::Node node);
//
//   void createPackageInfo(_Module* module, Project& project);

public:
   void loadRules(StreamReader* optimization);
   void turnOnOptimiation(int level)
   {
      _optFlag |= level;
   }

   void compileModule(SNode node, ModuleScope& scope);
   void compileModule(ident_t source, ModuleScope& scope);

   bool run(_ProjectManager& project, bool withDebugInfo);

   // _Compiler interface implementation
   virtual void injectVirtualReturningMethod(SNode node, ident_t variable);

   Compiler(StreamReader* syntax, _CompilerLogic* logic);
};

} // _ELENA_

#endif // compilerH
