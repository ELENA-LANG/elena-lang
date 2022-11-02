//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing doc generator code
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "ldoc.h"
#include "ldocconst.h"
#include "module.h"

using namespace elena_lang;

void writeNs(TextFileWriter& writer, ustr_t ns)
{
   for (int i = 0; i < getlength(ns); i++)
   {
      if (ns[i] == '\'') {
         writer.writeChar('-');
      }
      else writer.writeChar(ns[i]);
   }
}

void writeNs(IdentifierString& name, ApiModuleInfo* info)
{
   for (size_t i = 0; i < info->name.length(); i++)
   {
      if (info->name[i] == '\'') {
         name.append('-');
      }
      else name.append(info->name[i]);
   }
}

void writeHeader(TextFileWriter& writer, const char* package, const char* packageLink)
{
   writer.writeTextLine("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Frameset//EN\"\"http://www.w3.org/TR/REC-html40/frameset.dtd\">");
   writer.writeTextLine("<HTML>");
   writer.writeTextLine("<HEAD>");
   writer.writeTextLine("<TITLE>");
   writer.writeText(TITLE);
   writer.writeTextLine(package);
   writer.writeTextLine("</TITLE>");
   writer.writeTextLine("<meta name=\"collection\" content=\"api\">");
   writer.writeTextLine("<LINK REL =\"stylesheet\" TYPE=\"text/css\" HREF=\"stylesheet.css\" TITLE=\"Style\">");
   writer.writeTextLine("</HEAD>");
   writer.writeTextLine("<BODY BGCOLOR=\"white\">");

   writer.writeTextLine("<DIV CLASS=\"topNav\">");

   writer.writeTextLine("<UL CLASS=\"navList\">");

   writer.writeTextLine("<LI>");
   writer.writeTextLine("<A HREF=\"index.html\">Overview</A>");
   writer.writeTextLine("</LI>");

   writer.writeTextLine("<LI>");
   if (!emptystr(packageLink)) {
      writer.writeText("<A HREF=\"");
      writer.writeText(packageLink);
      writer.writeTextLine("\">Module</A>");
   }
   else writer.writeTextLine("Module");
   writer.writeTextLine("</LI>");

   writer.writeTextLine("</UL>");

   writer.writeTextLine("<DIV CLASS=\"aboutLanguage\">");
   writer.writeTextLine("<STRONG>");
   writer.writeTextLine(TITLE2);
   writer.writeTextLine("</STRONG>");
   writer.writeTextLine("</DIV>");

   writer.writeTextLine("</DIV>");
}

void writeSummaryHeader(TextFileWriter& writer, const char* name, const char* shortDescr)
{
   writer.writeTextLine("<DIV CLASS=\"header\">");
   writer.writeTextLine("<H1>");
   writer.writeText("Module ");
   writer.writeTextLine(name);
   writer.writeTextLine("</H1>");
   writer.writeTextLine("</DIV>");

   writer.writeTextLine("<DIV CLASS=\"docSummary\">");
   writer.writeText("<DIV CLASS=\"block\">");
   writer.writeText(shortDescr);
   writer.writeTextLine("</DIV>");
   writer.writeTextLine("</DIV>");

   writer.writeTextLine("<DIV CLASS=\"contentContainer\">");
   writer.writeTextLine("<UL CLASS=\"blockList\">");
}

void writeClassSummaryHeader(TextFileWriter& writer)
{
   writer.writeTextLine("<LI CLASS=\"blockList\">");
   writer.writeTextLine("<TABLE CLASS=\"typeSummary\" BORDER=\"0\" CELLPADDING=\"3\" CELLSPACING=\"0\">");
   writer.writeTextLine("<HEADER>");
   writer.writeTextLine("Class Summary");
   writer.writeTextLine("</HEADER>");
   writer.writeTextLine("<TR>");
   writer.writeTextLine("<TH CLASS=\"colFirst\" scope=\"col\">Class name</TH>");
   writer.writeTextLine("<TH CLASS=\"colLast\" scope=\"col\">Description</TH>");
   writer.writeTextLine("</TR>");
}

void parseNs(IdentifierString& ns, ustr_t root, ustr_t fullName)
{
   if (isWeakReference(fullName)) {
      ns.copy(root);
   }

   size_t ltIndex = fullName.findStr("&lt;");
   size_t last = 0;
   for (size_t i = 0; i < getlength(fullName); i++) {
      if (fullName[i] == '\'') {
         last = i;
      }
      else if (fullName[i] == '#' || (fullName[i] == '&' && i == ltIndex))
         break;
   }

   ns.append(fullName, last);
}

void writeRefName(TextFileWriter& writer, ustr_t name, bool allowResolvedTemplates)
{
   int paramIndex = 1;
   bool paramMode = false;
   for (int i = 0; i < getlength(name); i++)
   {
      if (!paramMode && name[i] == '\'') {
         writer.writeChar('-');
      }
      else if (name.compareSub("&lt;", i, 4)) {
         paramMode = true;
         writer.writeText("&lt;");

         i += 3;
      }
      else if (name.compareSub("&gt;", i, 4)) {
         if (!allowResolvedTemplates) {
            String<char, 5> tmp;
            tmp.copy("T");
            tmp.appendInt(paramIndex);
            writer.writeText(tmp.str());
         }
         writer.writeText("&gt;");
         paramMode = false;

         i += 3;
      }
      else if (name[i] == ',') {
         if (!allowResolvedTemplates) {
            String<char, 5> tmp;
            tmp.copy("T");
            tmp.appendInt(paramIndex);
            writer.writeText(tmp.str());

            paramIndex++;
         }
         writer.writeText(",");
      }
      else if (name[i] == '[' && name[i + 1] == ']') {
         // skip array brackets
         i++;
      }
      else if (!paramMode || allowResolvedTemplates)
         writer.writeChar(name[i]);
   }
}

void writeClassName(TextFileWriter& writer, ApiClassInfo* info)
{
   const char* descr = *info->shortDescr;
   if (!emptystr(descr)) {
      writer.writeText(descr);
   }
   else {
      if (info->prefix.length() > 0) {
         writer.writeText(*info->prefix);
      }

      writer.writeText("<SPAN CLASS=\"typeNameLabel\">");
      writer.writeText(*info->name);
      writer.writeText("</SPAN>");
   }
}

void writeType(TextFileWriter& writer, ustr_t type, bool fullReference = false)
{
   bool arrayMode = false;
   if (type.startsWith("params ")) {
      writer.writeText("<I>params</I>&nbsp;");
      type = type + 7;
   }
   else if (type.startsWith("ref ")) {
      writer.writeText("<I>ref</I>&nbsp;");
      type = type + 4;
   }
   else if (type.startsWith("arrayof ")) {
      type = type + 8;
      arrayMode = true;
   }

   writer.writeText("<SPAN CLASS=\"memberNameLink\">");
   if (type.find('\'') != NOTFOUND_POS) {
      writer.writeText("<A HREF=\"");

      pos_t index = type.findStr("&lt;");
      NamespaceString ns(type);
      if (index != NOTFOUND_POS) {
         for (pos_t i = index; i >= 0; i--) {
            if (type[i] == '\'') {
               ns.copy(type, i);
               break;
            }
         }
      }

      if (emptystr(*ns)) {
         writer.writeText("#");
      }
      else {
         writeNs(writer, *ns);
         writer.writeText(".html#");
      }
      writeRefName(writer, type + ns.length() + 1, false);

      writer.writeText("\">");
      if (fullReference) {
         writer.writeText(type);
      }
      else writer.writeText(type + ns.length() + 1);

      writer.writeText("</A>");
   }
   else writer.writeText(type);

   if (arrayMode)
      writer.writeText("<I>[]</I>");

   writer.writeText("</SPAN>");
}

void writeSummaryTable(TextFileWriter& writer, ApiClassInfo* info, const char* bodyFileName)
{
   writer.writeTextLine("<TD CLASS=\"colFirst\">");

   writer.writeText("<A HREF=\"");
   writer.writeText(bodyFileName);
   writer.writeText("#");
   writeRefName(writer, *info->name, false);
   writer.writeText("\">");
   writer.writeText(*info->name);
   writer.writeTextLine("</A>");
   writer.writeTextLine("</TD>");

   writer.writeTextLine("<TD CLASS=\"colLast\">");
   writer.writeTextLine("<DIV CLASS=\"block\">");
   writeClassName(writer, info);
   writer.writeTextLine("</DIV>");
   writer.writeTextLine("</TD>");
}

void writeClassBodyHeader(TextFileWriter& writer, ApiClassInfo* info, ustr_t moduleName)
{
   writer.writeText("<A NAME=\"");
   writer.writeText(*info->name);
   writer.writeTextLine("\">");
   writer.writeTextLine("</A>");

   writer.writeTextLine("<!-- ======== START OF CLASS DATA ======== -->");

   writer.writeTextLine("<DIV CLASS=\"header\">");

   writer.writeTextLine("<DIV CLASS=\"subTitle\">");
   writer.writeText(moduleName);
   writer.writeText("'");
   writer.writeTextLine("</DIV>");

   writer.writeText("<H2 title=\"");
   writer.writeText(*info->title);
   writer.writeText("\" class=\"title\">");
   writer.writeText(*info->title);
   writer.writeTextLine("</H2>");

   writer.writeTextLine("</DIV>");

   writer.writeTextLine("<DIV CLASS=\"contentContainer\">");

   writer.writeTextLine("<DIV CLASS=\"description\">");
   writer.writeTextLine("<BR>");
   writer.writeTextLine("<HR>");
   //const char* descr = nullptr;
   writer.writeTextLine("<PRE STYLE=\"padding-top: 15px;\">");
   writeClassName(writer, info);
   writer.writeTextLine("</PRE>");
   writer.writeTextLine("<BR>");
   writer.writeTextLine("</DIV>");
}

void writeParent(TextFileWriter& writer, StringList::Iterator& it, ApiClassInfo* info)
{
   writer.writeTextLine("<UL CLASS=\"inheritance\">");
   writer.writeTextLine("<LI>");

   if (!it.eof()) {
      writeType(writer, (*it), true);
      writer.writeTextLine("</LI>");

      it++;
      writer.writeTextLine("<LI>");
      writeParent(writer, it, info);
   }
   else {
      writer.writeText(*info->fullName);
   }

   writer.writeTextLine("</LI>");

   writer.writeTextLine("</UL>");
}

void writeParents(TextFileWriter& writer, ApiClassInfo* info, ustr_t moduleName)
{
   auto it = info->parents.start();
   writeParent(writer, it, info);
}

void writeClassBodyFooter(TextFileWriter& writer, ApiClassInfo* info, ustr_t moduleName)
{
   writer.writeTextLine("<HR>");

   writer.writeTextLine("</DIV>");
}

void writeClassSummaryFooter(TextFileWriter& writer)
{
   writer.writeTextLine("</TABLE>");
   writer.writeTextLine("</LI>");
}

void writeSummaryFooter(TextFileWriter& writer)
{
   writer.writeTextLine("</UL>");
   writer.writeTextLine("</DIV>");
}

void writeFooter(TextFileWriter& writer, const char* packageLink)
{
   writer.writeTextLine("<DIV CLASS=\"bottomNav\">");

   writer.writeTextLine("<UL CLASS=\"navList\">");

   writer.writeTextLine("<LI>");
   writer.writeTextLine("<A HREF=\"index.html\">Overview</A>");
   writer.writeTextLine("</LI>");

   writer.writeTextLine("<LI>");
   if (!emptystr(packageLink)) {
      writer.writeText("<A HREF=\"");
      writer.writeText(packageLink);
      writer.writeTextLine("\">Module</A>");
   }
   else writer.writeTextLine("Module");
   writer.writeTextLine("</LI>");

   writer.writeTextLine("</UL>");

   writer.writeTextLine("<DIV CLASS=\"aboutLanguage\">");
   writer.writeTextLine("<STRONG>");
   writer.writeTextLine(TITLE2);
   writer.writeTextLine("</STRONG>");
   writer.writeTextLine("</DIV>");

   writer.writeTextLine("</DIV>");

   writer.writeTextLine("</BODY>");
   writer.writeTextLine("</HTML>");
}

// --- DocGenerator ---

struct HelpStruct
{
   DocGenerator*      docGenerator;
   ApiModuleInfoList* list;
};

ApiModuleInfo* DocGenerator :: findModule(ApiModuleInfoList& modules, ustr_t ns)
{
   for (auto it = modules.start(); !it.eof(); ++it) {
      if ((*it)->name.compare(ns)) {
         return *it;
      }
   }
   return nullptr;
}


ApiClassInfo* DocGenerator :: findClass(ApiModuleInfo* module, ustr_t fullName)
{
   for (auto it = module->classes.start(); !it.eof(); ++it) {
      if ((*it)->fullName.compare(fullName)) {
         return *it;
      }
   }
   return nullptr;
}

bool DocGenerator :: loadClassInfo(ref_t reference, ClassInfo& info)
{
   if (!reference)
      return false;

   MemoryBase* vmt = _module->mapSection(reference | mskVMTRef, true);
   if (!vmt) {
      ustr_t refName = _module->resolveReference(reference);
      if (isTemplateWeakReference(refName)) {
         ref_t resolvedReference = _module->mapReference(refName + getlength(TEMPLATE_PREFIX_NS) - 1);

         vmt = _module->mapSection(resolvedReference | mskVMTRef, true);
         if (!vmt)
            return false;
      }
      else return false;
   }

   MemoryReader vmtReader(vmt);
   // read tape record size
   vmtReader.getDWord();

   // read VMT info
   vmtReader.read((void*)&info.header, sizeof(ClassHeader));

   return true;
}

void DocGenerator :: loadParents(ApiClassInfo* apiClassInfo, ref_t reference)
{
   if (!reference)
      return;

   // read VMT info
   ClassInfo info;
   if (loadClassInfo(reference, info)) {
      loadParents(apiClassInfo, info.header.parentRef);
   }

   ustr_t name = _module->resolveReference(reference);
   apiClassInfo->parents.add(name.clone());
}


void DocGenerator :: loadClassMembers(ApiClassInfo* apiClassInfo, ref_t reference)
{
   MemoryBase* vmt = _module->mapSection(reference | mskVMTRef, true);
   if (!vmt) {
      ustr_t refName = _module->resolveReference(reference);
      if (isTemplateWeakReference(refName)) {
         ref_t resolvedReference = _module->mapReference(refName + getlength(TEMPLATE_PREFIX_NS) - 1);

         vmt = _module->mapSection(resolvedReference | mskVMTRef, true);
         if (!vmt)
            return;
      }
      else return;
   }

   MemoryReader vmtReader(vmt);
   // read tape record size
   vmtReader.getDWord();

   // read VMT info
   ClassInfo info;
   vmtReader.read((void*)&info.header, sizeof(ClassHeader));

   loadParents(apiClassInfo, info.header.parentRef);
}

void DocGenerator :: loadMember(ApiModuleInfoList& modules, ref_t reference)
{
   auto referenceName = _module->resolveReference(reference);
   if (isWeakReference(referenceName)) {
      IdentifierString prefix("public ");
      if (referenceName.startsWith(INTERNAL_PREFIX)) {
         if (_publicOnly)
            return;

         referenceName += getlength(INTERNAL_PREFIX);
         prefix.copy("intern ");
      }
      else if (referenceName.startsWith(PRIVATE_PREFIX)) {
         if (_publicOnly)
            return;

         referenceName += getlength(PRIVATE_PREFIX);
         prefix.copy("private ");
      }
      else if (referenceName.startsWith(TEMPLATE_PREFIX)) {
         return;
      }

      NamespaceString ns(*_rootNs, referenceName);

      ReferenceProperName properName(referenceName);
      ReferenceName fullName(*_rootNs, *properName);

      ApiModuleInfo* moduleInfo = findModule(modules, *ns);
      if (!moduleInfo) {
         moduleInfo = new ApiModuleInfo();
         moduleInfo->name.copy(*ns);

         modules.add(moduleInfo);
      }

      if (_module->mapSection(reference | mskVMTRef, true)) {
         prefix.append("class ");

         ApiClassInfo* info = findClass(moduleInfo, *fullName);
         if (!info) {
            info = new ApiClassInfo();

            info->fullName.copy(*fullName);
            info->name.copy(*properName);
            info->title.copy(*properName);
            info->prefix.copy(*prefix);

            moduleInfo->classes.add(info);
         }

         loadClassMembers(info, reference);
      }
      else if (_module->mapSection(reference | mskSymbolRef, true)) {
      }
   }
}

void DocGenerator :: loadNestedModules(ApiModuleInfoList& modules)
{
   _presenter->print(LDOC_READING);

   HelpStruct arg = { this, &modules };

   _module->forEachReference(&arg, [](ModuleBase* module, ref_t reference, void* arg)
      {
         HelpStruct* dicArg = static_cast<HelpStruct*>(arg);

         dicArg->docGenerator->loadMember(*dicArg->list, reference);
      });
}

void DocGenerator :: generateClassDoc(TextFileWriter& summaryWriter, TextFileWriter& bodyWriter, ApiClassInfo* classInfo, ustr_t bodyName)
{
   IdentifierString moduleName;
   parseNs(moduleName, *_rootNs, *classInfo->fullName);

   writeSummaryTable(summaryWriter, classInfo, bodyName);

   writeClassBodyHeader(bodyWriter, classInfo, *moduleName);

   if (classInfo->parents.count() > 0) {
      writeParents(bodyWriter, classInfo, *_rootNs);
   }

   writeClassBodyFooter(bodyWriter, classInfo, *moduleName);
}

void DocGenerator :: generateModuleDoc(ApiModuleInfo* moduleInfo)
{
   _presenter->print(LDOC_GENERATING, *moduleInfo->name);

   IdentifierString name;
   writeNs(name, moduleInfo);
   name.append(".html");

   IdentifierString summaryname;
   writeNs(summaryname, moduleInfo);
   summaryname.append("-summary");
   summaryname.append(".html");

   PathString outPath;
   outPath.copy(*name);

   PathString outSumPath;
   outSumPath.copy(*summaryname);

   TextFileWriter bodyWriter(outPath.str(), FileEncoding::UTF8, false);
   TextFileWriter summaryWriter(outSumPath.str(), FileEncoding::UTF8, false);

   writeHeader(summaryWriter, *moduleInfo->name, nullptr);
   writeHeader(bodyWriter, *moduleInfo->name, *summaryname);

   writeSummaryHeader(summaryWriter, *moduleInfo->name, *moduleInfo->shortDescr);

   if (moduleInfo->classes.count() > 0) {
      // classes
      writeClassSummaryHeader(summaryWriter);

      bool alt = true;
      for (auto class_it = moduleInfo->classes.start(); !class_it.eof(); ++class_it) {
         ApiClassInfo* classInfo = *class_it;
         if (alt) {
            summaryWriter.writeTextLine("<TR CLASS=\"altColor\">");
         }
         else {
            summaryWriter.writeTextLine("<TR CLASS=\"rowColor\">");
         }
         alt = !alt;

         generateClassDoc(summaryWriter, bodyWriter, classInfo, *name);

         summaryWriter.writeTextLine("</TR>");
      }

      writeClassSummaryFooter(summaryWriter);
   }

   writeSummaryFooter(summaryWriter);

   writeFooter(summaryWriter, nullptr);
   writeFooter(bodyWriter, *summaryname);
}

bool DocGenerator :: load(path_t path)
{
   FileNameString fileName(path);
   IdentifierString name(*fileName);
   _rootNs.copy(*name);

   _module = new Module();

   FileReader reader(path, FileRBMode, FileEncoding::Raw, false);
   auto retVal = static_cast<Module*>(_module)->load(reader);

   if (retVal == LoadResult::Successful) {
      return true;
   }
   else return false;
}

bool DocGenerator :: loadByName(ustr_t name)
{
   _module = _provider->loadModule(name);

   _rootNs.copy(name);

   return _module != nullptr;
}

void DocGenerator :: generate()
{
   ApiModuleInfoList modules(nullptr);
   loadNestedModules(modules);

   for (auto it = modules.start(); !it.eof(); ++it) {
      generateModuleDoc(*it);
   }
}