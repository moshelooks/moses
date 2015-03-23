## Extended Rules for Mixed Real-Boolean Normal Form ##

This section exposes a set of additional mixed-rules that has been implemented along with their complexities.


### Recursive Rules to Deal with a Single Inequality ###

The idea is to use the reductor recursively, for instance to check if `x+y>0 --> true`, the following expressions 'x>0' and 'y>0' are reduced, if 'x>0' and 'y>0' were both reduced to true then one can conclude that 'x+y>0' reduces to true as well. The computational complexity is not very good because it uses the reduction in a recursive way, however by doing like that a lot of cases can be covered with a small set of rules.

### Addition ###
  * `0 < x+y --> true`       if `0 < x --> true and 0 < y --> true`
  * `0 < x+y --> false`      if `0 < x --> false and 0 < y --> false`

Complexity considerations :

Let cl(s) be the complexity of this reduction rule alone of an expression of size s. Let co(s) be the complexity of the complete reductor applied on an expression of size s apart from this reduction rule, thus :

cl(|0<x+y|) = co(|0<x|)+cl(|0<x|) + co(|0<y|)+cl(|0<y|)

Assuming that |x|=|y|=s we can say that cl(|0<x+y|) is approximated by :

`cl(2*s) = 2*co(s)+2*cl(s)`

which is a recursive expression equivalent to :

`cl(s) = sum_i=1..log(s) exp(i)*co(exp(i-1)) + s`

where log and exp are in base 2 and cl(1)=1 and co(1)=1.

Assuming that co is linear (which is probably not the case) :

`cl(s) = sum_i=1..log(s) exp(i)*exp(i-1) + s`

The biggest term is `exp(log(s))*exp(log(s)-1)` which is approximatively `exp(log(s))^2 = s^2`

So if co(s) is linear cl(s) is exponential. Assuming co(s) is exponential (due to other rules of that type) then cl(s) is probably exponential of exponential...

Nevertheless, in practice numerous nested inequalities would probably not be that necessary (so the recursion might stop after a certain depth) thus the average complexity might remain sub-exponential. So far in practice such a set of rules was run instantaneously.

Extension of this rule :

One can see that in many cases this rule is not gonna work, for instance it does not capture the following reduction :

`0 < x+y --> true if 5 < x --> true and -4 > y --> true`

To deal with that, one has to extend this method so that assertion like 5<x are induced upwards instead of downwards. Extending that method in such a way would be an elegant hack to deal with inequality-expressions using intervals.

### Multiplication ###

  * `0 < x*y --> 0< x   if 0 < y --> true`

  * `0 < x*y --> 0< -x   if 0 < -y --> true`

Same complexity as for Addition.

### Division ###

  * `0 < x/y --> 0< x   if 0 < y --> true`

  * `0 < x/y --> 0< y   if 0 < x --> true`

  * `0 < x/y --> 0< -x   if 0 < -y --> true`

  * `0 < x/y --> 0< -y   if 0 < -x --> true`

Same complexity as for Addition and Multiplication.

### Logarithm ###

  * `0 < log(x) --> true   if 0 < x-1 --> true`

  * `0 < log(x) --> false   if 0 < x-1 --> false`

Complexity quadratic assuming that co(s) is linear (using the same kind of reasoning).

### Addition of sin ###

  * `0 < x+sin(y) --> true   if 0 < x-1 --> true`

  * `0 < x+sin(y) --> false   if 0 < x+1 --> false`

Same complexity as Logarithm

## Recursive Rules to Deal with Conjunctions or Disjunctions of Inequalities ##

Here a way is presented to encompass a lot of cases concerning conjunction (or disjunction) of inequalities.

Let's illustrate the main idea with the following example :

`0 < x and 0 < x*y --> 0 < x and 0 < y`

To deal with that the reductor is going to try reducing 0<x\*y assuming 0<x (and alternatively try reducing 0<x assuming 0<x\*y, which is not going to work in this example).

Generally, for the expression 'x and y and z', the reductor is going to try the 3 following reductions separately :

- reducing x assuming y, z

- reducing y assuming x, z

- reducing z assuming x, y

and retains the one that shorten the most the expression.


Solving a disjunction (as 'x or y or z') merely consists to apply the above method after placing a double negation in front of the expression and propagating the first one before the reduction and the second one after the reduction, I mean applying the reduction over 'not(x) and not(y) and not(z)', then propagating the second not and reducing, that is reducing 'not(reduced(not(x) and not(y) and not(z)))'.

After coding these 2 rules only one more is needed and that's it!

Here it is :

Let's say the reductor is trying to reduce 0<x. So this new rule looks at the table of assumptions of the current reduction, and if :

1) 0<x is found then it reduces to true

2) not(0<x) is found then it reduces to false


More generally this can be applied with any boolean expression not only inequalities and permits to reduce this kind of expression as well :

b and 0<if(b x y) -> b and 0<x

If there are both b and not(b) in the set of assumptions (which is rather unlikely to happen because the boolean reductor of Moshe should have taken care of them) it works too because in one moment the reductor is going to try reducing b assuming not(b) to false and choose it because it is the shortest one.

Now, let's refine the rules above 1) and 2) as :

1) if some assumption logically implies the conclusion c, then c is reduced to true

2) is the negation of some assumption is logically implied by the conclusion c, then c is reduced to false

That is done, it becomes easy to handle for instance subtler reduction concerning intervals by defining an 'implies' function :

for instance :

3<x implies 2<x

This technique permits to encompass elegantly more cases, for instance it does encompass the rule :

`c1 < x\ and\ c2 < x --> max(c1, c2) < x`


A hack is suggested to get things easier to code :
Exploiting the fact that vtree is a forest rather than a single tree and putting the set assumptions in place of the other trees.
It makes things easier to code because there is no change at all in the code to code the fact that a set of assumptions is passed from rule to rule (which is not so hard to code of course, but just less direct).

Note that if the reduction was applied on a tree with no assumption at start, once the reduction is done the tree will have no assumption more (I mean that the reduction process is not going to add assumptions to the tree) because in the process the assumptions are added on other trees, which are partial copies of the original tree but not the original tree itself.

Complexity considerations :

Let be the following conjunction :

b1 and b2 and ... and bn

let's consider the complexity of reducing b1 assuming b2 and ... and bn.

b2 to bn must first be added in the set of assumptions A which is currently O(n) but should be O(n\*log(n)) (A being implemented as a ordered set instead of a vector).

cl(s) denote the complexity of this rule on an expression of size s and co(s) the complexity of the complete reductor apart from this rule.

Let assume |b1|=|b2|=...=|bn|=s, so cl(b1 and b2 and ... and bn) can be approched by :

`cl(n*s) = n*(cl(s)+co(s)+O(n*log(n)))`

In the right hand side expression n comes from the fact that there is n ways of considering the assumptions, b1 assuming b2..bn, b2 assuming b1...bn (without b2), etc.

We know that co(s) must include the complexity of checking if the expression currently being reduced belongs to the assumptions but its complexity is only s\*log(|A|) (looking for an expression of size s in an order set of |A| elements).

So making the same reasoning as for the recursive Addition-Rule, assuming that co is linear cl is exponential around s^n. If co is exponential (which is strongly assumed at least in the worse case due to recursive Addition-Rules, etc) then cl is probably exponential of exponential.

In practice the reduction was performed instantaneously though but has not been tested on big trees.

### Reduction of n-dimension inequalities ###

the idea is to check if an inequality is a positive linear combination of other (in the assumption set) positive inequalities (note that one can always see an inequality as positive (strict or not) that's why it covers all linear combinations, x<0==0<-x...):

for example :
0<2x+2 and 0<y+x+1 and 0<6x+2y+7 -> 0<2x+2 and 0<y+x+1
because 6x+2y+7 == 2(2x+2)+2(y+x+1)+1
to check that one needs to check if that kind of equations 6x+2y+7 == a(2x+2)+b(y+x+1)+c have solutions. For that a Gaussian elimination has been coded.

Since 0>=x is represented by not(0<x) we need only to deal with the case when the main term (to check if it is a linear combination) is a strict positive inequality (if 0<x reduct to true then not(0<x) will reduct to false).
There is a precaution when combining the terms of the assumptions which is to have for all coefficient a, b, c, ... :

- none of them is negative

- at least one is strictly positive that is multiplying a strictly positive inequality


if the condition above is satisfied then the main term is replace by true.

if the condition is not satisfied and that none of the coefficients are strict positive then the main term is replaced by false.

### contin\_if Rules ###

  * `contin_if(true x y) --> x`

  * `contin_if(false x y) --> y`

Complexity : O(1)

  * `op(contin_if(x y1 z1) contin_if(x y2 z2)) --> contin_if(x op(y1 z1)\ op(y2 z2))`

Complexity : O(|x|)

  * `contin_if(x y y) --> y`

Complexity : O(|y|)

  * `contin_if(x op(y z) op(y w)) --> op(y contin_if(x z w))`

When op in `{+,*,/}`. If op is `/` the order of the arguments must be kept.

Complexity : O(|y|+|z|)

  * `contin_if(x exp1[x] exp2[x]) --> contin_if(x exp1[true] exp2[false])`

This rule works only if x does not have side effects.

Complexity : O(|x|**(|exp1|+|exp2|))**

### impulse Rules ###

  * `impulse(x)*impulse(y)*z --> impulse(x and y)*z`

  * `0 < impulse(x)+impulse(y)) --> or(x y)`

  * `contin_if(x 1 0) --> impulse(x)`

  * `contin_if(x 0 1) --> impulse(NOT(x))`

All these rules have complexity O(1).

### Miscellaneous ###

Some ideas :

1) develop then reduce and check if better



-- Nil Geisweiller