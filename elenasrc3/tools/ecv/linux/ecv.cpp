//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing ecode viewer code
//
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "ecvconst.h"
#include "ecviewer.h"

using namespace elena_lang;

class Presenter : public PresenterBase
{
public:
   void readLine(char* buffer, size_t length) override
   {
      // !! fgets is used instead of fgetws, because there is a strange bug in fgetws implementation
      fgets(buffer, LINE_LEN, stdin);
   }

   void print(ustr_t message) override
   {
      printf("%s", message.str());
   }

   void print(ustr_t message, ustr_t arg) override
   {
      printf(message.str(), arg.str());
   }

   void printPath(ustr_t message, path_t arg) override
   {
      printf(message.str(), arg.str());
   }
};

int main(int argc, char* argv[])
{
   Presenter presenter;
   ByteCodeViewer viewer(&presenter);

   if (argc < 2) {
      presenter.print("ecv <module name> | ecv -p<module path>");
      return 0;
   }

   if (ustr_t(argv[1]).endsWith(".nl")) {
      // if direct path is provided

      PathString path(argv[1]);
      if(!viewer.load(*path)) {
         presenter.printPath(ECV_MODULE_NOTLOADED, path.str());

         return -1;
      }
   }

   viewer.runSession();

   return 0;
}
