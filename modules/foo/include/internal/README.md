Place the private header files in this directory. They will be
available to your code with

     #include <IMP/foo/internal/myheader.h>

All headers should include `IMP/foo/foo_config.h` as their
first include and surround all code with `IMPFOO_BEGIN_INTERNAL_NAMESPACE`
and `IMPFOO_END_INTERNAL_NAMESPACE` to put it in the
IMP::foo::internal namespace and manage compiler warnings.
