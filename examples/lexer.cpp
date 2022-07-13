#include "circuit/lexer.h"
#include <fstream>


int main(){
  using namespace circuit;
  using std::ifstream;\
  auto keyword_filter = KeywordFilter();
  ifstream ifs;
  auto lexer = Lexer(ifs);
}