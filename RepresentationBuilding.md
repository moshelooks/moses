# Representation-Building in MOSES #

I was thinking about the deficiencies of representation-building in MOSES and came up with an idea for an improved approach. The basic problem that representation-building attempts to solve is given some promising program `p`, to construct an `n`-dimensional space consisting of `n` possible independent axes for transformation of `p` (e.g., adding a literal to a clause, changing the value of a numerical constant, etc.) that minimizes the size of the space while maximizing semantic expressiveness (we want the space to contain as many semantically unique programs as possible) and semantic-syntactic (i.e., fitness-distance) correlation.

Note that the "dimensions" here are discrete and finite (i.e., integral values in `[0,d)` for some smallish `d`).

In practice, any given program `p` will have a very large space of possible independent transformations (the "syntactic neighborhood space"), so the problem of representation-building amounts to intelligently structuring and pruning this set of possible transformations, based on prior knowledge.

The current approach groups transformations together in a non-adaptive fashion into knobs based on prior knowledge (e.g., in the Boolean case, the two transformations of adding a literal to a clause and adding its negation are grouped together because we never want to do both at the same time). Then, each knob is evaluated independently based on the program p for "semantic utility" (by applying reduction to normal form after trying the knob together with its various settings).

The problem with doing things this way is that it doesn't consider potential interactions between knobs:

1) there may be some transformations that are only useful when combined with other transformations that get (incorrectly) excluded from the search space

2) there may be some transformations that should **never** be combined with certain other transformations, and these combinations get incorrectly included in the search space

Of course interactions between transformations are exactly what the BOA probabilistic modeling is supposed to discover, but doing this a priori based on knowledge when possible, rather than doing a bunch of fitness evaluations to gather statistics, is a huge savings (when it is possible to do so ;->).

Of course we can't consider all possible combinations of transformations, but it would be nice to at least consider pairwise interactions. My idea for doing this is actually similar to the old approach to doing dimensional embedding... The approach is as follows:

1) If there are no candidate knobs available, break.

2) Randomly select a candidate knob to serve as a "pivot" for the first dimension. Consider all pairwise interactions between this knob and other candidate knobs, (crudely) classifying all interactions as "interacts well with" or "interacts poorly with". This can be done based on reduction, the same way that individual knobs are now evaluated.

3) If the knob "pivot" interacts poorly with all other knobs (or above a certain % of them), throw it out and goto 1.

4) Otherwise, create a dimension based onthe the pivot knob, **combined with all of the knobs that it interacts poorly with**. This is based on the idea that we want to exclude combinations of knobs that interact poorly from consideration, and the heuristic that if knob `x` interacts poorly with knob `y` and with knob `z`, then `y` and `z` probably interact poorly too. So for example if knob `x` is our pivot and has arity 4, and interacts poorly with `y` that has arity 3 and `z` that has arity 2, then the first dimension of our reprsentational space will have arity `4+3+2`, with the first 4 settings corresponding to settings for `x`, the next three to settings of `y`, and the last two to combinations of `z`. This way, combinations of `x`, `y`, and `z` with other knobs will be expressible in the space, but not combination with each-other.

5) Remove the pivot knobs and all of the knobs that interacted poorly with it from consideration (they are now encoded in the representation as a dimension), and goto 1.

This would definitely be slower than the current approach, but would hopefully make up for it with faster/better optimization. I don't have a good intuition for whether the resultant spaces would be smaller or larger - it would probably depend on the exact parameters chosen.. Another possibility to consider is to keep the current approach to
representation-building, but to compute afterwards which knobs interacted poorly with other knobs, and use this information to guide the initial sampling.

## Acknowledgments ##

Ben has complained repeatedly that the current approach to representation-building is inadequate and that interactions should be considered. He was right, of course ;->. Eric Baum also thought that this aspect of MOSES was problematic, IIRC.