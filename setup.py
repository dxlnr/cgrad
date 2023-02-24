import os 
from distutils.core import setup, Extension

# Project root
cwd = os.path.abspath(os.path.dirname(__file__))
with open(os.path.join(cwd, 'README.md'), encoding='utf-8') as f:
    long_description = f.read()


setup(name='mars',
      version='0.1.0',
      description='Autograd Engine.',
      author='Daniel Illner',
      license='MIT',
      long_description=long_description,
      url='https://github.com/dxlnr/mars',
      packages = ['mars'],
      ext_modules = [Extension('mars.engine', sources = ['mars/csrc/engine.c'])],
      classifiers=[
          "Programming Language :: Python :: 3",
          "License :: OSI Approved :: MIT License",
          "Operating System :: OS Independent",
      ],
      python_requires='>=3.6',
)
