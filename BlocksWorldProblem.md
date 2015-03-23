## Solving the Block Stacking Problem with Action and Lists ##

The Block Stacking Problem is a toy problem from machine learning and AI planning research. In [Evolution of Cooperative Problem-Solving in an Artificial Economy](http://whatisthought.com/hayek32000.pdf) and [An Evolutionary Post Production System](http://whatisthought.com/ptech.pdf), Eric Baum & Igor Durdanovic describe two systems, Hayek3 and Hayek4, which use economies of agents to solve formulations of the task in terms of S-expressions with a carefully restricted vocabulary (Hayek3, solves problems with hundreds of blocks) and a more expressive variant of post-production systems (Hayek4, learns a general solution).

Basically, the problem is to rearrange some randomly scattered colored blocks to be stacked according to a particular specified arrangement of colors. The figure below taken from Baum's paper is quite descriptive:

![http://moses.googlecode.com/files/bwp.png](http://moses.googlecode.com/files/bwp.png)

To solve this problem with MOSES we have to decide how actions and perceptions are going to be represented, and what the control flow is going to look like.

For control flow the simplest thing to do is emulate Baum, and have a 'done' action (with the main program executed as a loop until done is called or the system exhausts its resources). It would be interesting to compare this approach against separately evolving a "main loop" branch (performing actions) and a termination criterion branch (returning a Boolean deciding whether or not to continue execution. Possibly the later is a more effectively constrained search space...

For perception, it seems to me that the stacks are best conceptualized as lists (e.g., looking at stack0 returns a list of color-symbols representing the current contents of the leftmost stack, ordered from top to bottom). Whatever general vocabulary of list-manipulation functions we decide on could then be used to manipulate & compare blocks and stacks. Note that this is a more general and costly approach than Baum takes in either of the Hayek systems (i.e., it might not work ;->).

For action, the simplest scheme to follow is to have six primitive actions: grab1, grab2, grab3, drop1, drop2, drop3. There should be reduction rules for these, specifically with two drops in a row you can ignore the second one, so too for two grabs to the same stack (for two grabs to different stacks you can't do this, because the first stack might have been empty). These reductions are not as ad hoc as they might seem at first - there seem to be a few very basic "algebras of equivalence" governing the reduction of various kinds of actions (also incl. rotations and steps) which I will articulate in ActionPerceptionNormalForm .

We could reduce the search space further by only allowing compound "grab and drop" actions (i.e., a single grab followed by a single drop)...

After a bit of fiddling around, I've come up with the following solution to the blocksworld problem expressed in these terms, which is algorithmically equivalent to the one evolved by Hayek4:
```
if (list-equal(stack0,stack1))     //1 below
	done
else if (list-equal(dorepeat(tail,stack0,length(stack0)-length(stack1)),stack1))         //2 below
	if (symbol-equal(head(stack2),nth(stack0,length(stack0)-length(stack1)-1)))  //3 below
		grab2
		drop1
	else if (contains(stack3,nth(stack0,length(stack0)-length(stack1)-1)))   //4 below
		grab3
		drop2
	else
		grab2
		drop3
else
	grab1
	drop3
```

With a few minor tweaks to improve readability, this is the language that I propose for MOSES to evolve programs in. See Baum for a walkthrough of how this works. Some notes wrt MOSES  are as follows:

1) If the target stack (stack1) is equal to the goal stack (stack0) say done. An equality operator on lists should be generally useful across many problems.

2) Else if there are no wrong symbols on stack1... This is tricky - we need to test stack1 for equality with the length(stack1)-long suffix of stack0. How to get this suffix? We can iterate the tail (i.e. cdr) operation length(stack0)-length(stack1) times as I do here, or include a sublist extraction primitive in the language, or iteratively test for element-wise equality, or ... This raises the general question of which list primitives incorporate. The [Caml Module 'List'](http://caml.inria.fr/pub/docs/manual-ocaml/libref/List.html) might be a good start (ignoring the functions added for efficiency). As a side note, the language used by Hayek4 is heavily biased to do subsequence comparisons (moreso than I think is warranted for general program evolution).

3) Here we need to compare the top element of the second stack to the "next needed color" - nth seems acceptable for this (another use of the integer type, for its second argument).

4) Here I invoke a "contains" function to check if stack3 contains a block of same color as the "next needed color". The Caml module doesn't have a contains function  [- it does, called "mem"](correction.md) - the closest is "exists" which takes a predicate as its argument (contains(list,elem) is equivalent to exists(list,lambda(x) { symbol-equal(x,elem) }), or exists(list,@(symbol-equal,elem)) if we use combinatory-logic-style currying ;->). Also note that the expression for "next needed color" needs to be reused - hence solving blocksworld should benefit from a simple procedural abstraction bias (discussed but not yet implemented or even designed in complete detail for MOSES).

In conclusion, the block stacking problem as formulated above appears quite challenging, but not impossible, for standalone MOSES (once all of the required elements are in place). It is worth considering to help craft approaches to list manipulation, action, and procedural abstraction.


-- Moshe Looks