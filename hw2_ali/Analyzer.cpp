
#include "clang/Basic/SourceManager.h"
#include "clang/Driver/Options.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include<fstream>


using namespace std;
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;

static llvm::cl::OptionCategory MyToolCategory("Function Analyzer options");

unsigned numFunctions = 0;
unsigned numLoops = 0;
unsigned numCalled = 0;
unsigned stmtLine = 0;
unsigned comStmt = 0;



class FuncAnalysisVisitor : public RecursiveASTVisitor<FuncAnalysisVisitor> {
private:
  ASTContext *astContext; 
  
public:
  explicit FuncAnalysisVisitor(CompilerInstance *CI) 
      : astContext(&(CI->getASTContext())) {


      }


  bool VisitFunctionDecl(FunctionDecl *func) {

if(func->isThisDeclarationADefinition()==1)
{
    string funcName = func->getQualifiedNameAsString();
    llvm::outs() << "Function name: " << funcName << "\n";
    
    unsigned numArg=func->getNumParams();
   llvm::outs() << "Number of Arguments: " << numArg << "\n";


Stmt *FuncBody = func->getBody();
string u = FuncBody->getStmtClassName();
llvm::outs() << u;

llvm::outs() << funcName<<"  "<< "Number of loops: "<< numLoops<<"\n";
llvm::outs() << funcName<<"  "<< "Number of Statements: "<< stmtLine<<"\n";
llvm::outs() << funcName<<"  "<< "function called inside "<< comStmt<<"\n";




numLoops=0;
stmtLine=0;
comStmt=0;
numFunctions++;
}

      return true;
  } 



  bool VisitStmt(Stmt *s) {
   

    string m = s->getStmtClassName();
    if(m=="ForStmt" ||m=="DoStmt" || m=="WhileStmt")
    {
    numLoops++;

    }
/*    
    if(m=="CompoundStmt")
    {
      comStmt++;
    }
*/

    stmtLine++;
      return true;
    }
    

};


class FuncAnalysisASTConsumer : public ASTConsumer {
private:
    FuncAnalysisVisitor *visitor; 

public:

  explicit FuncAnalysisASTConsumer(CompilerInstance *CI)
    : visitor(new FuncAnalysisVisitor(CI)) { }

  virtual void HandleTranslationUnit(ASTContext &Context) {

    SourceManager& SM = Context.getSourceManager();
    auto Decls = Context.getTranslationUnitDecl()->decls();
    for (auto &Decl : Decls) {
      const auto& FileID = SM.getFileID(Decl->getLocation());
      if (FileID != SM.getMainFileID())
     	continue;
      visitor->TraverseDecl(Decl);
    }
  }
};


class FuncAnalysisFrontendAction : public ASTFrontendAction {
public:
  virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI,
							 llvm::StringRef file) {
    return std::unique_ptr<ASTConsumer>(new FuncAnalysisASTConsumer(&CI)); 
  }
};

int main(int argc, const char **argv) {

  CommonOptionsParser op(argc, argv, MyToolCategory);        
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());
  int result = Tool.run(newFrontendActionFactory<FuncAnalysisFrontendAction>().get());
  llvm::outs() << "Number of functions encountered " << numFunctions << "\n";
  return result;

}


