from distutils.core import setup, Extension


engine = Extension('demo', sources = ['demo.c'])


setup (name = 'cgrad',
       version = '0.1.0',
       description = 'Neural Engine',
       ext_modules = [engine]
)
