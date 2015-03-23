## Boolean Normal Form ##

Holman has developed an "elegant normal form" (ENF) for simplification that is both efficient to derive and heuristically effective (Holman has taken down the full description of ENF formerly on his website, but I have a saved local copy). He reports for instance on experiments involving randomly generated Boolean formulae with hundreds of literals, where 99% of the formulae required fewer than 10,000 atomic operations to reduce to ENF, and retained fewer than 2% of their original literals. Formulae in ENF use the basis _AND_ ,
_OR_ , _NOT_. Formulae can also contain variables, and the primitives _true_  and _false_ . In contrast to conjunctive normal form, ENF preserves formulae's hierarchical structure. To define ENF, let's introduce some terminology: \footnotFor clarity, the definitions are slightly different than those given by Holman

  * The _guard set_ of an internal node is all of its children that are literals, and the guard set of a literal is itself.

  * A _branch set_ is the union of all of the guard sets of conjunctions and literals on the shortest path between some leaf and the root.

  * The _dominant set_ of a node is the union of all of the guard sets of nodes on the shortest path between the node and the root, excluding the node itself.

A redundant Boolean formula (left) and its equivalent in normal form (right):

> ![http://moses.googlecode.com/files/enf.png](http://moses.googlecode.com/files/enf.png)

Consider the formula on the left. The _AND_ node in the lower right's guard set
is x<sub>3</sub> , x<sub>6</sub> and its dominant set is x<sub>1</sub> , x<sub>2</sub> , x<sub>3</sub> , x<sub>7</sub> . The branch set for the literal x<sub>5</sub> (in the center) is x<sub>1</sub> , x<sub>2</sub> , x<sub>5</sub>.  A formula is in
ENF if:

  * Negation appears only in literals.
  * Levels of conjunction and disjunction alternate.
  * No conjunction or disjunction has both a literal and its negation, or multiple copies of the same literal, as children.
  * No branch set contains a literal and its negation.
  * The intersection of all of the children of any disjunction's guard sets is empty.
  * The intersection of any conjunction's guard set and dominant set is empty.

Thus, the formula on the left in the figure is not in ENF, because the intersection of the _OR_  node in the lower right's children's guard sets is non-empty -- it contains x<sub>3</sub> (condition 5). Holman also gives an efficient procedure for reducing any formula to ENF consisting of a set of eight reduction rules that are executed iteratively over the entire formula until no further reductions are possible. I have extended the definition ENF
and the corresponding reduction procedure to obey the following additional constraints:

  * The intersection of the guard sets of the children of a conjunction is empty -- this corresponds to item 5 above, for a conjunction-of-disjunctions rather than a
disjunction-of-conjunctions.

  * No node's guard set is a subset of any of its siblings' guard sets.

  * For any pair of siblings' guard sets having the form ![http://moses.googlecode.com/files/bnf1.png](http://moses.googlecode.com/files/bnf1.png) and ![http://moses.googlecode.com/files/bnf2.png](http://moses.googlecode.com/files/bnf2.png)
, where S<sub>1</sub> and S<sub>2</sub> are sets of literals, no _third_  sibling's guard set is a subset of ![http://moses.googlecode.com/files/bnf3.png](http://moses.googlecode.com/files/bnf3.png) .


-- Moshe Looks