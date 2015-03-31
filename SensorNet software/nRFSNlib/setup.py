import numpy
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

setup(
    cmdclass = {'build_ext': build_ext},
    ext_modules=[Extension(
             name="py_nRFSN",
             sources=["py_nRFSN.pyx", "nRFSN.c"],
             libraries=["bcm2835", "m"],
             language="c",
             extra_link_args=['-std=c99']
             ),
            ]
)