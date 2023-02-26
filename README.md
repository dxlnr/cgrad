# MARS 

For now its just a simple autograd engine written as a mix of C/C++ & Python.
Based on the implementation by Karpathy's [micrograd](https://github.com/karpathy/micrograd) in Python. 
Maybe in the future, additional stuff will be implemented.

Find more information about Pytorch internals at [ezyang’s blog](http://blog.ezyang.com/2019/05/pytorch-internals/).

## Get Started

```bash
# Building the package from source
python setup.py install bdist_wheel
# Installing for dev/locally
pip install -e .
#
# Running the tests
python -m unittest tests/test_value.py
```

### Notes

An interesting aspect of autograd is the usage of [Topological Ordering](https://en.wikipedia.org/wiki/Topological_sorting). A topological ordering is an ordering of the nodes in a directed graph where for each directed edge from node A to node B, node A appears before node B in the ordering. 

This is useful when constructing the backward pass in this case.

#### Topological Sort Algorithm

```
(1) Pick an unvisted node (self)
(2) Beginning with the selected node, do a Depth First Search (DFS) exploring only unvisited nodes. 
(3) On the recursive callback of the DFS, 
add the current node to the topological ordering in reverse order.
```

Implementation in C is done using [doubly linked list](https://www.geeksforgeeks.org/introduction-and-insertion-in-a-doubly-linked-list/).
