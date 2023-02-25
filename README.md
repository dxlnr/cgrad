# MARS 

For now its just a simple autograd engine written as a mix of C/C++ & Python.
Based on the implementation by Karpathy's [micrograd](https://github.com/karpathy/micrograd) in Python. 
Maybe in the future, additional stuff will be implemented.

Find more information about Pytorch internals at [ezyang’s blog](http://blog.ezyang.com/2019/05/pytorch-internals/).

## Get Started

```shell
# Building the package from source
python setup.py install bdist_wheel
# Installing for dev/locally
pip install -e .
#
# Running the tests
python -m unittest tests/test_value.py
```

