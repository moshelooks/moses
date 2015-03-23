This page lists test problems useful for testing the evolution of programs involving programmatic constructs (i.e. going beyond Boolean and arithmetic operators).

## Test Problems from the AI literature generally ##

  * Eric Baum's version of the BlocksWorldProblem, which he solved using Hayek

## Test Problems From the "Evolution of Functional Programs" Literature ##

Looking through the literature on evolving functional programs today, I had a hard time finding nontrivial test problems that anyone had succeeded on!
I didn't do a really thorough search though....  (Ben, April 20 2007)

## Clack & Yu 97 ##

From

http://citeseer.ist.psu.edu/clack97performance.html

we find

```
The NTH-3 function takes two arguments, an integer N and a list L, 
and returns the Nth element of L. If N < 1, it returns the first element of L. If N > length(L), it 
returns the last element of L.

Output Type: The output has type G1 

Arguments: The argument N has typeint and the argument L has type [G1] 

Terminal Set: T = {L:: [G1],      N:: int,    1:: int} 

Function Set:
F = {HEAD:: [a]->a,IF-THEN-ELSE:: bool->a->a->a, 
TAIL:: [a]->[a],
LENGTH:: [a]->int, 
<= :: int->int->bool, 
> :: int->int->bool, 
- :: int->int->int,
NTH-3:: int->[G1]->G1 
```

and

```
The MAPCAR function takes two arguments, a functionF and a listL, 
and returns the list obtained by applyingF to each element ofL. 

Output Type: The output has type[G2] 
Arguments: The argumentF has typeG1->G2 and the argumentL has type[G1] 
Terminal Set:T = {L:: [G1],        NIL:: [a],         F:: (G1->G2)} 
Function Set:
F = {HEAD ::[a]->a, 
IF-THEN-ELSE ::bool->a->a->a, 
TAIL ::[a]->[a], CONS ::a->[a]->[a], 
NULL ::[a]->bool, F ::G1->G2, 
MAPCAR ::(G1->G2)->[G1]->[G2]}
```

## Lambda Calculus ##
In

http://citeseer.ist.psu.edu/32875.html

we find the problem of evolving the successor
function for Church numerals.

Recall the Church numeral n is defined as
` n = \f . \x . (f)^n x `
(where \ is lambda), so that the successor function is
` succ = \n \g \y ((n) g)(g)y `

## Programs Learned by Complete Enumeration ##
In these papers

http://www.cs.ioc.ee/tfp-icfp-gpce05/tfp-proc/14num.pdf

http://nautilus.cs.miyazaki-u.ac.jp/~skata/skataPRICAI04.pdf

we find a description of a program learning system that uses complete
enumeration to learn Haskell programs.  It outperforms PolyGP on
simple test problems.

Test problems used are

```
- nth :: Int -> [a ] -> a satisfying 
nth 5 ?widjfgwi ? == 
```