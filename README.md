# Assignment 1: Superoptimizer by Christopher Peterson

## What I did

For this assignment, I decided to implement something resembling Massalin's original superoptimizer in C++. I generally think this is a bit more interesting and helps me learn better than spending all my time trying to get someone else's code to work. I focused entirely on the "Multiplication by Constants" section of Massalin's work, as it seemed to be the most straightforward, easily reproducable, and used the fewest operators. Given the 4-hour timeframe and my decision to completely re-implment a solid chunk of code, I did not add optimizations into my search.

My goal was to reproduce some of Massalin's results and draw a few interesting conclusions.

## Results

I was able to confirm all of the multiplication results presented by Massalin's paper, except for *= 625. Without any optimizations, my code ran too slowly to calculate programs longer than 6 instructions.

```
d0 *= 29

move    d0,d1
lsl     #3,d0    // d0 *= 8
sub     d1,d0    // d0 *= 7
lsl     #2,d0    // d0 *= 28
add     d1,d0    // d0 *= 29
```

Although it's the same length as more straightforward solutions, I liked how this solution utilized both registers to arrive at the solution:

```
d0 *= 39

move    d0,d1
lsl     #5,d1    // d1 *= 32
sub     d0,d1    // d1 *= 31
lsl     #3,d0    // d0 *= 8
add     d1,d0    // d0 *= 39
```

The result for *= 156 was similar, using two powers of two. Subtracing a negative makes the process a bit more confusing, but still interesting:

```
d0 *= 156

move    d0,d1
lsl     #5,d0    // d0 *= 32
sub     d0,d1    // d1 *= -32
lsl     #2,d1    // d1 *= -128
sub     d1,d0    // d0 *= 156
```

My last goal was to validate the general claims that Massalin made about the amount of instructions required to compute constant multiplication:

```
The average size of programs that multiply by small numbers (less
than 40) is 5 instructions, most programs that multiply by numbers in
the hundreds are 6 to 7 instructions long, and programs that multiply
by thousands are between 7 and 8 instructions long.
```

Although runtime began to become prohibitive for longer and longer programs, I was able to create the following results for values 0-150, with the program limited to 2 registers. Anything that could not be solved using 6 instructions is labeled as 7 for this graph:

![results](https://github.com/cal-poly-csc530-2214/superoptimizer-Chris4a4/blob/main/Multiplication%20Constant%20vs%20Instructions%20Required.png?raw=true)

This more or less matches Massalin's work. The periodic nature of this data is very cool to me, and it's very easy to see the patterns in program length. I suspect that these trends would continue for larger values. This is definitely more precise than Massalin's conclusion, as the difficulty in representing larger numbers clearly comes from their distance from a power of two, not from their raw size.

## Further Work

If I had more time, the next thing I would have implemented are several search-space optimizations. There's a LOT of ways to improve on this approach, and they all seem pretty interesting to me:

1. Lookup table-based redundant sequence purning ala Massalin. This optimization could also be done in the opposite way; lookup valid next instructions based on one or more previous ones. Although this is an excellent optimization, it makes the search space a lot more complex than just a simple pow(possible instructions, program_length) brute-force iteration. Generating this table automatically seems like a really interesting and fun problem and I would have liked to tackle it given more time.
2. First instruction must use ``d0``, last instruction must modify ``d0``. Although these do get rid of valid programs, any removed programs were either not optimal OR a rewording of an optimal program.
3. More intelligent search. This didn't seem appropriate, as the purpose of this assigment was to investigate a brute-force type approach to program generation.
4. Caching instruction results as a program is being built. For example, the result of the initial instruction ``move d0, d1`` should be cached so that any program built off of this starting instruction won't need to run the instruction itself. Likewise, this can be applied to other instructions and even combinations of instructions to greatly save on execution time.

## Questions

The idea behind Massalin's "Probabilistic Test" optimization is a little strange to me. Massalin states that program validation is done in a two step process, which he outlines for the signum function:

1. Test on the inputs -1000, 0, 456.
2. If a program passes the first step, then test on all inputs from -1024 to 1024.

Fundamentally, I don't see why this is necessarily split into two distinct steps. The way this is worded also makes it seem like Massalin is running all three test inputs, even if the program fails the first one. It seems like program validation should be done like:

1. Sort the inputs in order of most likely to cause failure.
2. Check each input in order. If the output is correct, move on to the next input. If the output is incorrect, the program is thrown out and the superoptimizer immediately moves on.

Without access to Massalin's code, it's difficult to tell the exact implmentation of this part, but this detail was interesting to me. I suspect this was just a writing choice, an assembly limitation, or some other performance optimization.
