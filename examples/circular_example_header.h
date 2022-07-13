class B;

class A
{
public:
  A(int counter) : counter_(counter){

  };
  int counter(){return counter_;};
  void set_b(B* b){b_ = b;}
  void print_b_counter(B* b);
private:
  B* b_;
  int counter_;

};

class B
{
public:
  B(int counter) :counter_(counter){};
  int counter();
  void print_a_counter();
  void set_a(A* a){a_ = a;}
private:
  A* a_;
  int counter_;

};
void A::print_b_counter(B*b){b->counter();}
int B::counter(){return counter_;}
void B::print_a_counter(){a_->counter();}
