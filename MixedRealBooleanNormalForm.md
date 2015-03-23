## Mixed Real-Boolean Normal Form ##

Combining the capabilities of the previous two sections, it is fairly easy to
describe a normal form for functions mapping from a mixture of Boolean and real
arguments to either a Boolean or a real output. This will involve the
incorporation of a number of new functions that can be used to compose Boolean
and real arguments, as follows:

![http://moses.googlecode.com/files/mrb_01.png](http://moses.googlecode.com/files/mrb_01.png)

Reduction rules for these functions should be at least as follows:

![http://moses.googlecode.com/files/mrb_02.png](http://moses.googlecode.com/files/mrb_02.png)

A ![http://moses.googlecode.com/files/mrb_03c.png](http://moses.googlecode.com/files/mrb_03c.png) is a note that indicates the use of protected operators. Protection is used in evolutionary computation (particularly genetic programming) when an the result of an operators is not defined for the whole desired range. For example, `x/0` is defined as `1` for all `x` (`1` is chosen so that `x/x` will always be `1`). Similarly ![http://moses.googlecode.com/files/mrb_03a.png](http://moses.googlecode.com/files/mrb_03a.png) is redefined
as ![http://moses.googlecode.com/files/mrb_03b.png](http://moses.googlecode.com/files/mrb_03b.png) , etc.

would render a rule problematic - this is not an immediate concern, however, as
protected operators may be simply be avoided.

The paper [A Normal Form for Function Rings of Piecewise Functions](http://www.cas.mcmaster.ca/~mohrens/JSC.pdf) contains additional transformation rules and techniques that should be useful (what they call _step_  is called _impulse_  here).

It should be possible to improve simplification of mixed expressions by integrating interval arithmetic to propagate constraints. For example, if some input variable `foo` is known to lie in `[-3,5]` and `sin(x)` is known to lie in `[-1, 1]` for all `x`, and `x*x` is known to lie in [x<sub>absmin</sub>`*` x<sub>absmin</sub> , x<sub>absmax</sub>`*` x<sub>absmax</sub>] for all `x` (and an analogous rule is encoded for general multiplication) we can infer that `foo*foo*sin(x)` must lie in `[-25,25]`, and therefore that `0 < 26+foo*foo*sin(x)` is in always true. This is not an urgent priority, but it should not be an extremely difficult extension to add later (see below for more on interval arithmetic).


-- Moshe Looks