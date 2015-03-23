## Real-valued Normal Form ##

Moving from Boolean normal form to a normal form for real-valued expressions is
fairly straightforward. Analogous to alternating levels of _AND_ s and _OR_ s,
there should be alternating levels of weighted linear sums and nonlinear
functions (see Figure, taken from the paper 'Canonical Form Functions as a Simple Means for Genetic Programming to Evolve Human-Interpretable Functions' by McConaghy and Gielen, which is useful background). A possible function set to use here would be `+, *, /, log, exp, sin`. Addition and multiplication may have any `arity >= 2`, division must have arity <sub>2</sub> , natural logarithm, exponentiation ( e<sup>2</sup>), and sine all have arity one. Note that subtraction may be simulated by
multiplication by <sub>-1</sub> and addition.

A normal form for representing real-valued expressions:

![http://moses.googlecode.com/files/real.png](http://moses.googlecode.com/files/real.png)

Real-valued normal form should have at least the following identities, where _x, y_ , etc. may be arbitrary expressions, and _c, c<sub>1</sub> , c<sub>2</sub>_ etc.  are numeric constants:

![http://moses.googlecode.com/files/real_01.png](http://moses.googlecode.com/files/real_01.png)

There are many rules that emerge implicitly by iteratively applying these
(e.g., _x-x = 0_ may be derived from _c<sub>1</sub> `*` x + c<sub>2</sub> `*` x --> ( c<sub>1</sub> +c<sub>2</sub> ) `*` x followed by partial evaluation, followed by_0 `*` x --> 0_Furthermore, the rules should be implemented to function effectively with addition and multiplication taking
more than two arguments, to exploit the commutativity of these operations, and
to know that_x_is the same as_1 `*` x_.  Similarly partial evaluation needs to
know how to extract subexpressions such as_(6`*`x)/(2`*`y) --> (3`*`x)/y_. Here are
some more examples:_

  * using `log(exp(x)*y) --> x+log(y)` in deriving `log(42*exp(x)*y) --> 3.738+log(x)+y`
  * using `x*y+x*z --> x*(y+z)` in deriving `y*x + x --> x*(y+1)`
  * using `(x*y)/(x*z) --> y/z` in deriving `x/x --> 1`.

The system should also be _easily extensible_ ; for example, adding general exponentiation and the associated rules, if desired, should be fairly easy.



-- Moshe Looks