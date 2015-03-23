## General Rules ##

There are a few general rules for the system, applying across all types, as
follows:

  * Any subtrees involving only constants should be evaluated and replaced with the result (i.e., partial evaluation).

  * Unknown functions may appear in program trees; they should have their children reduced, and then be treated as distinct symbols, e.g.,

> ![http://moses.googlecode.com/files/general_rules_01.png](http://moses.googlecode.com/files/general_rules_01.png)

  * Any commutative function should (after reduction) have its arguments sorted lexicographically (e.g., smallest to largest, dictionary order). For example, ![http://moses.googlecode.com/files/general_rules_02.png](http://moses.googlecode.com/files/general_rules_02.png) . The precise comparison function used is not important here, merely that a consistent ordering be applied.


-- Moshe Looks - 17 Apr 2007