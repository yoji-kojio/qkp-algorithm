# QUADRATIC KNAPSACK PROBLEM

This code solves the quadratic knapsack problem, which asks to maximize a quadratic objective function subject to a single weight constraint.

A description of the code is found in the following paper:
A. Caprara, D. Pisinger, P. Toth, "Exact solution of the Quadratic Knapsack Problem", INFORMS Journal on Computing, 11, 125-137 (1999).

## Running the algorithm:
- install GCC/G++;
- compile: `gcc -O4 -o quadknap quadknap.c testqkp.c`;
- run executable: `./quadknap`;

## Algorithm parameters
This QKP algorithm will call for three parameters:
- `n`: number of items
- `r`: range of coefficients i.e. profits weights are in [1,r]
- `pct`: density of instance, i.e. expected frequency of nonzero profits
