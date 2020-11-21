# Max Flow on Bipartite Graphs

## About The Project

This implementation is an optimized version of Preflow-Push for Bipartite Graphs based on the algorithm 
described in the book **"Network Flows: Theory, Algorithms and Applications"** 
by James B. Orlin, Ravindra K. Ahuja, and Thomas L. Magnanti

The optimized algorithm runs in O(n1^2^m) time, where n1 = |N1|. 
The original algorithm has O((N^2^)m) time complexity.

## Libraries and Technologies

The programming language of choice is C++.
The libraries used are:
* [BOOST](https://www.boost.org/users/history/version_1_55_0.html) version 1.55.0
* LEDA version 6.3 (only for comparison)


## Getting Started

### Prerequisites

You need to download BOOST library in the designated version. 
LEDA is optional and is only used for the creation of the Graphs 
and the comparison with correspondent LEDA function **MAX_FLOW()**.

### Usage
To compile and run the project just use the make file that is included:
```
> make 
> make run
```

## Function inputs
If you want to use my implementation and make your own bipartite graph you will need:
- A bipartite bidirectional graph of type Boost graph (BGraph)
- The source node of your graph (int)
-  The residual capacity for each edge (vector`<int>`)