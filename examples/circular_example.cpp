#include "circular_example_header.h"

int main(){
  auto a = new A(1);
  auto b = new B(1);
  a->set_b(b);
  a->print_b_counter(b);
  b->set_a(a);
  b->print_a_counter();
  return 0;
}

