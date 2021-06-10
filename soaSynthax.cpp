/*
 We look for a SoA (and then AoSoA or Sharded SoA) with a syntax as close as possible to
 the "natural" AoS equivalent.

 This test attempts to crete syntactic sugar to make SoA look like AoS.
*/

#include<stddef.h>
#include <ctype.h>
#include <stdio.h>

const size_t sz = 4;

struct AoSElement{
  int i;
  float f;
};

typedef AoSElement AoS[sz];

void hexdump(void *ptr, int buflen) {
  /* From https://stackoverflow.com/a/29865 with adaptations */
  unsigned char *buf = (unsigned char*)ptr;
  int i, j;
  for (i=0; i<buflen; i+=16) {
    printf("%06x: ", i);
    for (j=0; j<16; j++) {
      if (i+j < buflen)
        printf("%02x ", buf[i+j]);
      else
        printf("   ");
      if ((i+j) % 4 == 3) printf (" ");
    }
    printf(" ");
//  for (j=0; j<16; j++) 
//    if (i+j < buflen)
//      printf("%c", isprint(buf[i+j]) ? buf[i+j] : '.');
    printf("\n");
  }
}

class SoA {
public:
  template<typename T>
  class Element {
  public:
    Element(size_t i, T(& col)[sz]): idx_(i), col_(col) {}
    operator T&() { return col_[idx_]; }
    template <typename T2>
    T& operator= (const T2& v) { col_[idx_] = v; return col_[idx_]; }
  private:
    size_t idx_;
    T (&col_)[sz];
  };
  struct Row {
    Row(size_t idx, int(&i)[sz], float(&f)[sz]): i(idx, i), f(idx, f) {}
    Element<int> i;
    Element<float> f;
  };
  Row operator[](size_t idx) { return Row(idx, i_, f_); }
private:
  int i_[sz];
  float f_[sz];
};

int main() {
  AoS aos;
  SoA soa;
  int val = 1;
  for (size_t i=0; i<sz; ++i) {
    aos[i].f = aos[i].i = val;
    soa[i].f = soa[i].i = val;
    val*=2;
  }
  printf("AoS:\n");
  hexdump(&aos, sizeof(aos));
  printf("\nSoA\n");
  hexdump(&soa, sizeof(soa));
  return 0;
}

