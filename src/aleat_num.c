#include "aleat_num.h"
#include "assert.h"


int aleat_num(int inf, int sup){
  int n, r;
  long end;

  if(inf<sup){
    n=sup-inf+1;
  }
  else if(sup<inf){
    n=inf-sup+1;
  }
  else{
    fprintf(stderr, "aleat_num: cant generate a random number between the same number\n");
    return -1;
  }

  if ((n - 1) == RAND_MAX) {
      return rand();
  }
  else {
      /* Chop off all of the values that would cause skew...*/

      end = RAND_MAX / n; /* truncate skew*/
      assert (end > 0L);
      end *= n;

      /* ... and ignore results from rand() that fall above that limit.
       (Worst case the loop condition should succeed 50% of the time,
       so we can expect to bail out of this loop pretty quickly.)*/

      while ((r = rand()) >= end);

      r = r % n;
      if(inf<sup){
        r=inf+r;
      }
      else{
        r=sup+r;
      }
      return r;
    }

}
