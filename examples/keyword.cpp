#include "circuit/keyword.h"
#include  <iostream>
#include <exception>

int main(){
  using namespace circuit;
  using namespace std;
  Keyword keyword("func");
  try{
    Keyword not_a_keyword("abcdefg");
  }
  catch(const std::exception& e){
    cout << "exception caught: {}" << e.what()  << endl;
  }
  return 0;
}