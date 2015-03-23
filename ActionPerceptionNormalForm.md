## Action-Perception Normal Form ##
The action-perception language and rules are intended to make it possible to
plug different simulation worlds into the interpreter, ranging from the very
simple (e.g., the world of the artificial ant problem) to the complex (e.g.,
AGISim or Second Life). To achieve this, a rather general typology is defined
which we should be able to keep constant, just swapping in and out different
instances and configurations.

For now, I am only describing a simple sequential execution model without any
interrupts. That is, the system can represent a program for walking and chewing
gum at the same time - it would need to learn separate walking and chewing gum
programs, and then use inference or some other process that could determine
that these two programs were not mutually exclusive, and then orchestrate their
execution...

Even this requires a bit of care, however.  Specifically, actions have **side
effects**, and hence cannot appear within normal Boolean and real expressions
where evaluation order is undefined. E.g., evaluating x followed by y to
compute AND(x y) is assumed always equivalent to evaluating y followed by x,
which is clearly incorrect if x and y involve taking action on an external
world. Also because of side-effects, an action-perception sequence, even one
without variables, may not be evaluated in the course of reduction.

## Types ##
**physical\_object\_type** - a "natural category" relevant to the system such as "human", "food", "pet", "obstacle", etc.

**physical\_object\_instance** - a unique ID referencing some instance of a physical object (it is assumed for now that the system has no difficult identifying and distinguishing between object instances)

**action\_result** - a type indicating the success/failure of an attempted action

**actuator** - this is actually a parameterized family (think templates in C++)
of _function types_ mapping to action results, since different actuators may
take different number/type of arguments (while each actuator is assumed to have
exactly one valid type signature). This type signature may be described as
[t<sub>1</sub> , t<sub>2</sub> ..., t<sub>N</sub> ], 0 <= N, where the t<sub>i</sub> s are either "discrete" (d),
"continuous" (c), or "object" (o) and either "modulo" (`\%`) or "non-modulo" (`+`)
(not a relevant option for "object"). If discrete there is an associated range
`[N,M]`, and if continuous there an associated range [t<sub>min</sub> ,t<sub>max</sub>]. Having a maximum/range is optional for the non-modulo case, and required for the modulo case. For the non-modulo case, one-sided intervals may also be supported.

Some examples:

  * a "gun" which may be fired at any angle: ![http://moses.googlecode.com/files/apn_01.png](http://moses.googlecode.com/files/apn_01.png)

  * the same gun, in 3D: ![http://moses.googlecode.com/files/apn_02.png](http://moses.googlecode.com/files/apn_02.png)

  * a movement command to move one step forward: `[d + [0, 1]]`

  * a grabber/dropper for blocksworld with three stacks of blocks to grab/drop on: `[d + [1, 3], d + [-1, 1]]` - the first dimension is the number of the stack (1, 2, or 3), and the second is grabbing (e.g., 1) or dropping (-1), with zero being neutral (we shall see the advantages of in discussing reduction rules), for example
    * grabber(1,-1) - drops the block currently being held onto stack 1
    * grabber(2,1) - grabs the top block off of stack two
    * grabber(3,0) - does nothing (becuase the second argument is zero)
  * a movement command to move to a particular object with a continuous speed parameter `[o, c + [0, 1]]`

  * a "tail wag" with three possible speeds and an adjustable width: ![http://moses.googlecode.com/files/apn_02.png](http://moses.googlecode.com/files/apn_02.png)

## Other Functions ##

`distance : physical_object_instance --> real`
the distance between two objects

`nearest : physical_object_type --> physical_object_instance`
the nearest object-instance of a given type (may return a special null identifier if no objects of the given type are in proximity)

_AND<sub>seq</sub>_ : _action\_result<sup>n</sup> --> action\_result_ Executes a sequence of actions sequentially from left to right until one fails and returns success if all of its children succeed.

_OR<sub>seq</sub>_ : _action\_result<sup>n</sup> --> action\_result_ Executes a sequence of actions sequentially from left to right until one succeeds and returns success if at least one of its children succeed.

_EXEC<sub>seq</sub> : action\_result<sup>n</sup> --> action\_result_ Executes a sequence of actions sequentially from left to right - always returns success.

_if : action\_result T T -> T_ Branching based on whether an action is successful or not. `T` may be real, bool, or action\_result.

## Example ##

```
if (distance(nearest(human),me) < distance(nearest(dog),me))
	EXECseq(bark,wag_tail(1,0.5))
else
	EXECseq(yap,wag_tail(2,1.1))
```

In this example, bark and yap are actuators that take no arguments (i.e., map directly to action\_result when evalauted as children of EXECseq), and wag\_tail is an actuator that takes two arguments. Note that percepts inside a single Boolean or continuous expression (such as the "nearest" and "distance" calls in this example) have no specified evaluation order, and are assumed to be evaluated simultaneiously wrt the simulation world (or nearly so).

## Reduction Rules ##

Some of the new reduction rules are obvious (e.g., for the new "if" statement, and for nested sequential execution). Once we get non-trivial perception (not included for now), there will be some more interesting rules for this. At the moment, the most interesting new reduction rules appear to involve actuators. I believe that a simple abstract formulation of the required reduction may be expressed in terms of four properties:

**additive** ANDseq(f(x1),f(x2)) -> ANDseq(f(x1+x2))

**positive additive** ANDseq(f(x1),f(x2)) -> ANDseq(f(x1+x2)), if (x1>=0 and x2>=0) or (x1<=0 and x2<=0)

**nullary** ANDseq(f(x1),f(x1)) -> ANDseq(f(x1))

**0-inert** ANDseq(f(0)) -> ANDseq()

For example, rotation and movement are positive-additive, and under some circumstances additive. Grasping is nullary (two grasps or two drops with the same argument are the same as one, because the second one will never have any effect). Etc... Interval arithmetic may be worth implementing in order to be able to exploit positive additivity for non-constant expressions...

## A Note On Memory and Object Instances ##

Within the framework above, there are some severe limitations to what agents
can represent as action/perception programs (and hence learn). For example,
let's say that we want to target and then follow a particular object instance
(e.g., some human). There is no mechanism for "memory" across execution cycles,
so the closest we could come would be to continually call "nearest". Thus, if
another person interposed him/herself, the agent would begin following them
instead of the actual target. A simple hack around this (in a Petaverese
context) would be to have "get/set target" functions. Or perhaps very stupid
pets are more endearing? I believe that the correct solution (from an NCE
perspective) involves integrative AI rather than just MOSES...

## Implementational Concerns ##

First of all, the interpreter will need to be extended (again) to handle these. Since the interpreter is going to continue to be extended and tweaked a fair bit, it might be worth moving to a nicer (i.e. more modular and efficient) design, after the mixed Boolean-real reductions are all done (I want these available quickly to do development and tests with MOSES which will probably take quite some time - the perception/action and higher-order stuff are less urgent).

An older (and somewhat different) version of this document is archived at OldActionPerceptionNormalForm , for those curious.


-- Moshe Looks