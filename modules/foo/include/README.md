Place the public header files in this directory. They will be
available to your code (and other modules) with

     #include <IMP/foo/myheader.h>

All headers should include `IMP/foo/foo_config.h` as their
first include and surround all code with `IMPFOO_BEGIN_NAMESPACE`
and `IMPFOO_END_NAMESPACE` to put it in the IMP::foo namespace
and manage compiler warnings.

Headers should also be exposed to SWIG in the `pyext/swig.i-in` file.
