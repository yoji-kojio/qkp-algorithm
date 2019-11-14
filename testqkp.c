
/* ======================================================================
        TEST SCHEDULER  Alberto Caprara, David Pisinger, Polo Toth
   ====================================================================== */

/* This code generates and runs a number of test instances
 * for the quadratic knapsack problem:
 *
 *   maximize   \sum_{i=1}^{n} \sum_{j=1}^{n} p_{i,j} x_{i} x_{j}
 *   subject to \sum_{j=1}^{n} w_{j} x_{j} \leq c
 *              x_{j} \in \{0,1\}, j = 1,\ldots,n
 *
 * It is assumed that all coefficients are nonnegative integers.
 * A description of the test instances generated is found in:
 *
 *   A. Caprara, D. Pisinger, P. Toth,
 *   "Exact solution of the Quadratic Knapsack Problem",
 *   INFORMS Journal on Computing, 11, 125-137 (1999).
 *
 * The present code is written in ANSI-C, and has been compiled with
 * the GNU-C compiler using option "-ansi -pedantic". It may however
 * be necessary to customize the timing routines for other operating
 * systems. To obtain an exacutable code with the "quadknap" algorithm
 * compile with
 *
 *   gcc -O4 -o quadknap quadknap.c testqkp.c
 *
 * The program reads three arguments
 *
 *   n     number of items
 *   r     range of coefficients i.e. profits weights are in [1,r]
 *   pct   density of instance, i.e. expected frequency of nonzero
 *         profits
 *
 * having read the parameters, 10 instances are constructed and
 * solved using the "quadknap" algorithm. Average values of the
 * obtained solutions, capacities, and solution times are printed
 * out to a file named "trace.c".
 *
 * (c) Copyright, August 2000,
 *   David Pisinger                     Alberto Caprara, Paolo Toth
 *   DIKU, University of Copenhagen     DEIS, University of Bolgona
 *   Universitetsparken 1               Viale Risorgimento 2
 *   Copenhagen, Denmark                Bologna, Italy
 *
 * This code can be used free of charge for research and academic purposes
 * only.

/* This algorithm solves the quadratic knapsack problem */

#define TESTS       10   /* number of test to run in each series */
#define MSIZE       400  /* maximum number of 0-1 variables */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include "values.h"
#include <string.h>
#include <math.h>
#include <sys/times.h>
#include <unistd.h>


/* ======================================================================
      macros
   ====================================================================== */

#define srand(x)     srand48(x)
#define randm(x)    (lrand48() % (long) (x))


/* ======================================================================
      global variables
   ====================================================================== */

int n, c, z;
int p[MSIZE][MSIZE];
int w[MSIZE];
int x[MSIZE];
FILE *trace;


/* ======================================================================
      cpu_time
   ====================================================================== */

/* The following routine is used for measuring the time used
 *   cpu_time(NULL)    - start of time measuring
 *   cpu_time(&t)      - returns the elapsed cpu-time in variable t (double).
 */

void cpu_time(double *t) {
  static struct tms start, stop;

  if (t == NULL) {
    times(&start);
  } else {
    times(&stop);
    *t = (double) (stop.tms_utime - start.tms_utime) / sysconf(_SC_CLK_TCK);
  }
}


/* =======================================================================
      terminate
   ======================================================================= */

void terminate(char *str) {
  printf("%s\n", str);
  printf("PROGRAM IS TERMINATED !!!\n\n");
  exit(-1);
}



/* ======================================================================
			printitems
   ====================================================================== */

void printitems(void) {
  /* print instance to file for further handling */
  int i, j;
  FILE *out;

  out = fopen("save.out", "a");
  fprintf(out, "----------\nsize %d\n", n);
  for (j = 0; j < n; j++) fprintf(out," %3d", w[j]);
  fprintf(out, "\n\n");
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) fprintf(out," %3d", p[i][j]);
    fprintf(out, "\n");
  }
  for (j = 0; j < n; j++) fprintf(out," %3d", x[j]);
  fprintf(out, "\nc %d\n", c);
  fclose(out);
}


/* ======================================================================
			maketest
   ====================================================================== */

void maketest(int n1, int r, int pct) {
  int i, j;
  int psum, wsum;
  int row, columns;

  n = n1;

  for (i = 0; i < n; i++) {
    for (j = 0; j <= i; j++) {
      p[i][j] = p[j][i] = (randm(100) >= pct ? 0 : randm(r)+1);
    }

    w[i] = randm(r/2)+1;
  }

  // print weight array
  // for(i=0; i<n; i++) {
  //   printf("%d ", w[i]);
  // }

  // print instance matrix
  // for (row = 0; row < MSIZE; row++) {
  //   for (columns = 0; columns < MSIZE; columns++) {
  //     printf("%d     ", p[row][columns]);
  //   }

  //   printf("\n");
  // }

  psum = 0; wsum = 0;

  for (i = 0; i < n; i++) {
    x[i] = 0; wsum += w[i];
    for (j = 0; j < n; j++) psum += p[i][j];
  }

  if (wsum - 50 <= 0) terminate("too small weight sum");

  c = randm(wsum-50) + 50;
}

/* ======================================================================
      sumdata
   ====================================================================== */

void sumdata(int n1, int r1, int pct1, long z, long c, double tottime) {
  static long n;
  static long r;
  static long pct;
  static double time = 0.0;
  static long ztot   = 0;
  static long zsum   = 0;
  static long csum   = 0;

  if (n1 == 0) {
    fprintf(trace,"n          = %ld\n", n);
    fprintf(trace,"r          = %ld\n", r);
    fprintf(trace,"pct        = %ld\n", pct);
    fprintf(trace,"time       = %.2lf\n", time        / (double) TESTS);
    fprintf(trace,"ztot       = %.1lf\n", ztot        / (double) TESTS);
    fprintf(trace,"zsum       = %.0lf\n", zsum        / (double)     1);
    fprintf(trace,"csum       = %.0lf\n", csum        / (double)     1);
  } else {
    n       = n1;
    r       = r1;
    pct     = pct1;
    ztot   += z;                  /* sum of optimal solutions       */
    time   += tottime;            /* total computational time       */
    zsum    = ((zsum+z) % 1000);  /* controle sum of all solutions  */
    csum    = ((csum+c) % 1000);  /* controle sum of all capacities */
  }
}


/* ======================================================================
				checksolution
   ====================================================================== */

void checksolution(int c, int z) {
  int i, j;
  int psum, wsum;

  psum = wsum = 0;

  for (i = 0; i < n; i++) {
    if (!x[i]) continue;
    wsum += w[i];
    for (j = 0; j < n; j++) {
      if (x[j]) psum += p[i][j];
    }
  }

  /* printf("CHECK %d: psum %d z %d wsum %d c %d\n", n, psum,z,wsum,c); */
  if (wsum > c) terminate("excess weight");
  if (psum != z) terminate("bad solution");
}


/* ======================================================================
				main
   ====================================================================== */

int main(int argc, char *argv[]) {
  int n, r, pct, v, z;
  double time;

  if (argc == 4) {
    n = atoi(argv[1]);
    r = atoi(argv[2]);
    pct = atoi(argv[3]);
    printf("\nQUADKNAP %d, %d, %d\n", n, r, pct);
  } else {
    printf("quadknap\n");
    printf("n = ");
    scanf("%d", &n);
    printf("r = ");
    scanf("%d", &r);
    printf("pct = ");
    scanf("%d", &pct);
  }

  trace = fopen("trace.c", "a");
  fprintf(trace,"\nQUADKNAP: n: %d, r: %d, pct: %d\n", n, r, pct);

  if (n > MSIZE) terminate("table too small");

  for (v = 1; v <= TESTS; v++) {
    srand(v+n+r+pct);
    maketest(n, r, pct);
    cpu_time(NULL);
    z = quadknap(n, c, p, w, x);
    cpu_time(&time);
    printf("%d: c %d z %d time %.2lf\n", v, c, z, time);
    fprintf(trace, "%d: c %d z %d time %.2lf\n", v, c, z, time);
    checksolution(c, z);
    sumdata(n, r, pct, z, c, time);
  }

  sumdata(0, 0, 0, 0, 0, 0);
  fclose(trace);

  return 0;
}
