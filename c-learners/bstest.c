/*
 * Copyright (C) 2002 Bart Massey and Mick Thomure.
 * All Rights Reserved.  See the file COPYING in this directory
 * for licensing information.
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitset.h"

void bs_print(bitset b) {
  int i,count;
  if(b==NULL) return;
  
  // print out number of bits and sign of set,
  // and for each bit print either '0' or '1'
  count=b->count<<5;
  printf("%d:",count);
  if(!b->neg) printf("%d:",bs_popcount(b));
  putchar(b->neg?'-':'+');
  for(i=count-1; i>=0; --i)
    putchar(bs_isset(b,i)?'1':'0');
}

int main(void) {
  bitset b[4],res,mask;
  int i,j;

  // initialize bitsets to test on, b[2]==~b[0] and b[3]==~b[1]
  b[0]=bs_new(63);
  b[1]=bs_new(8);
  srand(time(NULL));
  for(i=0; i<b[0]->count; ++i) b[0]->bits[i]=rand();
  for(i=0; i<b[1]->count; ++i) b[1]->bits[i]=rand();
  b[2]=bs_not(b[0]);
  b[3]=bs_not(b[1]);

  // print out sets we are working with
  for(i=0; i<4; ++i) {
    printf("b[%d]:",i);
    bs_print(b[i]);
    putchar('\n');
  }

  // test setting and clearing
  bs_lclear(b[3],2);
  printf("clr(b[3],2):"); bs_print(b[3]); putchar('\n');
  bs_lset(b[3],2);
  printf("set(b[3],2):"); bs_print(b[3]); putchar('\n');
  bs_lclear(b[3],2);
  printf("clr(b[3],2):"); bs_print(b[3]); putchar('\n');


  // test masking
  mask=bs_mask(17);
  res=bs_and(b[0],mask);
  printf("b[0]&mask:"); bs_print(res); putchar('\n');
  bs_free(res);

  res=bs_and(b[1],mask);
  printf("b[1]&mask:"); bs_print(res); putchar('\n');
  bs_free(res);
		  
  // combine all possible possible combinations
  for(i=0; i<4; ++i) {
    for(j=i+1; j<4; ++j) {
      res=bs_and(b[i],b[j]);
      printf("b[%d]&b[%d]:",i,j); bs_print(res); putchar('\n');
      bs_free(res);

      res=bs_or(b[i],b[j]);
      printf("b[%d]|b[%d]:",i,j); bs_print(res); putchar('\n');
      bs_free(res);
    }
  }
  
  // free and return
  for(i=0; i<4; ++i) bs_free(b[i]);
  return 0;
}
