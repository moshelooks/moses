# Continuous and Hierarchical Optimization with Estimation of Distributions and Occam Bias #

## Background ##

A number of schemes have been developed for incorporating continuous parameters into the framework of discrete estimation-of-distribution algorithms (EDAs) such as the Bayesian optimization algorithm (BOA). One way to proceed is to simply encode each continuous value as a fix set of binary variables according to some prespecified scheme, providing fixed precision. Alternatively, one can work directly with continuous values, encoding each one as a single parameter to be modeled. For example, Ocenasek and Schwartz
([''Estimation Distribution Algorithm for Mixed Continuous-Discrete Optimization Problems''](http://jiri.ocenasek.com/papers/eisci02.pdf)) extend the hierarchical BOA, which learns Bayesian networks with local structure (decision trees) by adding decision tree models and splits corresponding to continuous values. Specifically, splits on a continuous variable `x` are binary, and take the form `x<c`, where `c` is a constant. To find good values for `c` hillclimbing on the set of values for `x` present in the population is performed.

Three primary concerns have motivated me to develop a new scheme for continuous optimization with discrete EDAs:

  * **Speed**: a discrete representation of a continuous parameter with `n` bits of precision will introduce `n` probabilistic variables into the modeling process (which is quadratic in the number of variables), while searching for splits via hillclimbing introduces an undesirable dependency on the population size. Ideally, each continuous variable should introduce a constant complexity independent of its precision and of the population size.

  * **Variance Adaptation**: it has been observed by Bosman and Grahl ([http://minner.bwl.uni-mannheim.de/hp/\_files/forschung/reports/tr-2005-03.pdf ''Matching Inductive Search Bias and Problem Structure in Continuous Estimation-of-Distribution Algorithms''](.md)) that continuous EDAs are often inefficient in requiring large uniform samples to minimize even simple functions, due to premature convergence and an inability exploit gradients to take larger steps. As a countermeasure, variance-adaptation schemes may be incorporated. It would be nice for such adaptation to derive naturally from an optimization approach however, rather then added on as an afterthought.

  * **Occam Bias**: in the domain of neural networks, Hochreiter and Schmidhuber ([''Simplifying Neural Nets by Discovering Flat Minima''](http://www.idsia.ch/~juergen/nipsfm/)) have developed methods for avoiding overfitting by exploiting a principled Occam bias in favor of simpler continuous-value models (those that can be represented with fewer bits). To my knowledge, no prior approach to continuous optimization with EDAs has attempted this. Such a bias is quite useful, however, for some application areas of interest (e.g., learning numerical constants for supervised classification programs).

## New Representation ##

The basic idea is to represent continuous quantities, conceptually, as
variable-length sequences of bits, with a simplicity prior towards shorter
sequences. Bit-sequences are mapped to continuous values using a simple
information-theoretic approach. For example, consider representing a value in
the range `(-1,1)` with a uniform prior:

![http://moses.googlecode.com/files/mas_01.png](http://moses.googlecode.com/files/mas_01.png)

This may be visualized as follows (showing the variable-length encoding of a real value in `(-1,1)` up to a length of 2 - leftward arrows indicate zeros, rightward arrows indicate ones):

![http://moses.googlecode.com/files/mas_02.png](http://moses.googlecode.com/files/mas_02.png)

To take another example, let's say we have a noise continuous variable (e.g., a microarray expression level or a quantity from a financial timeseries), and are learning a constant `c` for comparison with values drawn from this distribution. The prior here would be for `c` to be the median value of `x`, so this would be encoded by the null string (`''`). A `c` that is just greater than `3/4`ths of the data will be encoded as `'1'` whereas one just greater than `1/4`th of the data will be `'0'`. Additional bits are further correction terms, e.g., `'10'` is greater than `5/8ths` of the data, etc.

What about learning with an unbounded value? I will require that the mean and
variance be known. The first thing is to convert the variance into a ''step size'' parameter whose use will be described below (I haven't worked out theexact math for this yet, but it should be straightforward). Assume a mean `m` and a step size `s` - the null string encodes `m`, `'0'` encodes `m-s`, and `'1'` encodes `m+s`, as above. However, addition steps left or right _multiply_  the step size. E.g., `'11'` encodes `m+s+2s`, `'111'` encodes `m+s+2s+4s`, etc. For any value that is not all ones or all zeros, the
successive step sizes are computed by bisecting the distance between the upper
and lower bounds. E.g., `'101'` encodes `m+s-0.5s+0.25s`.

Given some real-valued constant `c` at an arbitrary program location in a
symbolic regression problem, the scheme above may be used with a mean of zero
and a step size of one. So `'1'` will actually encode `1`, while `'0'` will
encode `-1`, `'10'` will encoded `0.5`, and `'11'` will encode 2 (divide this
difference by two in the first case, multiply by two in the second case). This
allows us to exploit gradients through variable step size. E.g., we can reach
2<sup>n</sup> in only n steps through successive doublings.

More generally, we can consider an additional parameter, the ''expansion factor'', to determine step sizes. The scheme above uses an expansion factor of `2`, corresponding to successive doublings and exponential scaling. An expansion factor of `1` corresponds to uniform scaling, while `0.5` corresponds to geometric scaling -- all values will be bounded to `(m-2s,m+2s)`.

A disadvantage of this encoding is that numbers that are close together may end up being represented very differently in long strings (because early bits are exponentially more important than later bits: e.g., `100000 -> 0.03125`, `011111 -> -0.3125`. Each `0` added to the first number and `1` added to the second number will halve the distance between them, even as the Hamming distance increases by one. If this turns out to be a problems in
practice, I propose implementing "tunneling" (between distant encodings of similar numbers) by having nearby values on the real number line share some of their probability density with each other.

## Generalizing to Hierarchies ##

![http://moses.googlecode.com/files/mas_03.png](http://moses.googlecode.com/files/mas_03.png)

The scheme described above may be extended to model general hierarchical variables. For example, let's say that we have a very simple language ontology with top-level categories "noun", "verb", "adjective", with noun subdivided into "place" and "person", and "place" subdivided into "DC" and "St. Louis". Then, assuming the hierarchy is ordered and children are zero-indexed from left to right (see Figure), "St. Louis" will be encoded as `'001'`, "adjective" will be encoded as `'2'`, etc.

What desirable features should these hierarchies possess, and where may we derive from and how?

```
   animal
   /   \
  cat  dog
```

vs.

```
   {cat, dog}
   /        \
  cat       dog
```

to be completed soon

## Modeling ##

An interesting question to consider is what kind of model (generalized Bayes net) to use for these kinds of value? Considering continuous variables first, it seems that for depth `d`, a set of `d` ternary variables are best used, with the additional symbol meaning "stop" (e.g., the null string would be represented as a sequence of `d` stops). For the topology, making each variable dependent on it's superordinate is sufficient to ensure that only valid strings are generated (i.e., that all stops are follow by stops). One might also consider imposing additional structure (making variables dependent on their
grandparents, great-grandparent, etc.) as well to improve model accuracy. This is worth experimenting with.

For ontological variables, it seems that this additional structure will be necessary cases of variable branching factors (i.e., for tree with depth `d` and `n` variables we will effectively have somewhere in between `d` and `n` variables to model). Without this additional structure, we run the risk of specifying nodes that don't exist, e.g., taking the third child of an internal node with only two children.

When learning model structure, we can exploit our bias to first only consider splits based on the most-significant-bit / top-level-of-the-hierarchy. Finer splits need only be considered after we have already split on all superordinate levels.

## Acknowledgments ##

Thanks to Ben Goertzel, Cassio Pennachin, and Nil Geisweiller for helpful comments and discussion.


-- Moshe Looks