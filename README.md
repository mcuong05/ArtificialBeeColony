# ArtificialBeeColony

## Overview

This repository contains a straightforward implementation of the Artificial Bee Colony (ABC) metaheuristic in C language. ABC is inspired by the foraging behavior of honey bees and is commonly used for solving continuous and discrete optimization problems. The implementation aims to be easy to compile and run, and suitable as a baseline for experiments and comparisons.

## Features

- Core ABC algorithm implemented in `C`.
- Configurable algorithm parameters: population size, number of employed / onlooker bees, limit (scout threshold), number of iterations.
- Example build and run instructions.

## Algorithm Summary

ABC models three kinds of bees in a search for food sources (candidate solutions):

- Employed bees: explore the neighborhood of their assigned food source and update it if a better solution is found.
- Onlooker bees: choose food sources to exploit based on the quality (fitness) information shared by employed bees.
- Scout bees: replace abandoned or stagnated food sources with new random solutions.

Typical flow: initialize population → iterate (employed phase → onlooker phase → scout phase) → stop when max iterations or stopping condition is met.

## References

- Derviş Karaboga, "An idea based on honey bee swarm for numerical optimization", Technical Report TR06, 2005.
- General literature on metaheuristics and population-based optimization.


---
