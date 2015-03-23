# The _Reduct_ Library: Reducing Programs to Normal Form #
## Concepts ##
The general goal of reducing programs to normal form is to eliminate
redundancies and to be able to recognize syntactically when two programs or
program fragments are equivalent. This is uncomputable for Turing-complete
programs, and NP-hard for restricted languages such as Boolean
formulae. _Reduct_ is thus intended to be a heuristic system based on
iteratively applying a set of reduction rules that reduce some measure of
program complexity (hence certain to terminate), that is relatively efficient,
but not guaranteed to find the smallest possible program, or always identify
when two syntactically distinct programs are behaviorally equivalent.

This is the approach taken in the simplification methods for symbolic
expressions found in languages like Mathematica
(see [here](http://documents.wolfram.com/mathematica/functions/Simplify)).
For _Reduct_, an additional design goal is to use as small a set of basic
functions as possible while still maximizing expressiveness and, when possible,
using "smoother" functions. Fortunately, this means that there are fewer
possibilities to consider, and hence fewer rules to encode.

The first stage of _Reduct_ development will cover normal forms for Boolean
formulae, real-valued expressions, and mixed Boolean-real expressions involving
conditionals, described herein. Later extensions will also handle list
manipulation, iteration, procedural abstractions (![http://moses.googlecode.com/files/lambda.png](http://moses.googlecode.com/files/lambda.png) s), temporal and
spatial primitives and identities, etc.

**GeneralNormalForm**

**BooleanNormalForm**

**RealValuedNormalForm**

**MixedRealBooleanNormalForm**

**ExtendedMixedRealBoolNormalForm**

**ActionPerceptionNormalForm**

**ProgrammaticConstructNormalForm**

Some possible (advanced) test problems:

**BlocksWorldProblem**

## Equivalence Test ##

The systems should also include the option of using a more expensive method for
testing the semantic equivalence between expressions. One way to do this is via
random probing. The report [``Probabilistic Checks for the Equivalence of Mathematical Expressions'' ](http://www.cse.unl.edu/~sscott/students/tfisher.pdf), by T. Fisher,
describes a useful way of doing so for real-value formulae based on rounded
interval arithmetic (to correct for rounding errors in floating-point
calculations). Computing intervals for real-valued expressions is a useful
capability for other areas as well, so it is worth implementing according to an
efficient and general design (note that [Boost](http://www.boost.org) has an
interval library that may be used here if convenient).

Equivalence testing should always proceed through stages, doing the fastest
checks first, and only then moving on to more sophisticated methods, for
example:

  * Reduce both expressions to normal form and compare them for exact semantic equality (if they are, we can return "equal").

  * Compute the set of variables appearing in the normal forms of the two expressions: if these are not identical, we can (heuristically) return ``not equal''.

  * Randomly probe the expressions (assuming that the overall ranges of all arguments are known), as in Fisher's scheme. One complexity here is that rather than using a fixed maximum number of trials, the number of trials should be intelligently increased based on the arity (the number of variables appearing in the expressions) -- Fisher only deals with single-argument expressions). Intuitively, this need only grow with the logarithm of the arity, but experimentation is needed.

## Implementation ##
The implementation is in C++. It need not be obsessively coded for speed, but
obvious slowdowns (e.g., needlessly converting objects to and from strings)
should be avoided. Many parts have already been implemented, including the
complete Boolean normal form as described above. The basic representation is
based on [tree.h](http://www.aei.mpg.de/~peekas/tree/tree.h), a simple STL-like
class for representing <sub>n</sub>-ary trees. The basic interface should be something
like the following set of functions:
```
  void logical_reduce(vtree&);
  void contin_reduce(vtree&);
  void mixed_reduce(vtree&);

  vtree::iterator logical_reduce(vtree&,vtree::iterator);
  vtree::iterator contin_reduce(vtree&,vtree::iterator);
  vtree::iterator mixed_reduce(vtree&,vtree::iterator);
```

The first three functions reduce the entire tree. The next three are for doing
_local_  reduction. Each of these takes a tree and an iterator pointing to
some node in the tree, and does simplifications only relating to the subtree
rooted in the given node. These functions should return iterators pointing at
the nodes that they are called on, if the node still exists after reduction, or
whatever node takes its place (e.g., if it is replaced by one of its
children). If the entire subtree rooted in the node is removed, then an `end()`
iterator should be returned. I believe that it is possible to implement the
system in such a way that such calls to these functions will not invalidate the
node's parent (if it exists); if not, the the return value here will need to be
somewhat more complex.

It is critical that the asymptotic complexity of the local reduction calls be
independent of the total size of the tree they are called on. It should depend
only on the depth of the tree, the size of the given node's subtree, and the
numbers of neighbors that nodes have.

For example, given the tree:
```
and(or(x y foo) or(z q somebigtree)),
```
the cost of a call to:
```
logical_reduce(tree,foo_iter);
```
may depend on the size of _foo_ , the number of neighbors it has, and its
depth from the root (since the reduction to ENF takes into account paths to the
root). However, it should not depend at all on the size of _somebigtree_ .

There are also pieces of code to do some of the real-valued reductions
described above. However, these were written before a recent refactoring, and
will need to be lightly rewritten to work properly with the new code.

Functions should also be created for querying the equivalence of trees and
subtrees, and for computing intervals for expressions (if and when interval
arithmetic gets implemented).

All code should live in a namespace; the two top-level namespaces are currently
_reduct_  and _util_ . It is fine to do imports such as \empusing
std::set within these namespaces, but doing _using namespace std_  should
be avoided (outside of functions/subnamespaces).

## Links ##
  * [Compilation of the papers mention in this document that are not linked to](http://www.metacog.org/reducto_background.tgz)

  * [Basic intro to computer algebra systems, with nice examples](http://www.math.wpi.edu/IQP/BVCalcHist/calc5.htm)

  * [Comparison of computer algebra systems](http://en.wikipedia.org/wiki/Comparison_of_computer_algebra_systems)


-- Moshe Looks - 17 Apr 2007