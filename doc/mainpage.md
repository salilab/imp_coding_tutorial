Introduction to writing IMP code {#mainpage}
================================

[TOC]

# Introduction {#introduction}

In this tutorial we will cover creating a new IMP module, and writing a
new [restraint](@ref IMP::Restraint) in C++.

For more information, see the
[section on developing IMP in the manual](@ref developing).

First, we need to build %IMP from source code. See the
[installation instructions in the manual](@ref installation_source) for
more details.

# Add a new module {#addmodule}

First, change into the top-level directory in the IMP source code. You
will see a `modules` directory that contains a subdirectory for each IMP
module. To add a new module called '`foo`', use the `tools/make-module.py`
script as follows (the `$` character here denotes the command prompt):

\code{.sh}
$ tools/make-module.py foo
\endcode

This will make a new subdirectory `modules/foo`; let's take a look at its
contents:

\code{.sh}
$ ls modules/foo
README.md        bin             examples           pyext   test
benchmark        dependencies.py include            src     utility
\endcode

# include directory (C++ headers) {#include}

The `include` directory in the new module contains C++ header files that
declare the *public* classes and other functions that are part of the module.
For classes that are not intended to be public (e.g. utility classes only used
by your module itself) put them instead in the `include/internal` subdirectory.

Let’s add a new class to our module, `MyRestraint`, a simple restraint that
restrains a particle to the xy plane (see the
[ExampleRestraint class](@ref IMP::example::ExampleRestraint) in
`modules/example/` for a similar class).

\note the convention in %IMP is for class names (and the files declaring
and defining them) to be [CamelCase](https://en.wikipedia.org/wiki/Camel_case).
See the [naming conventions section in the manual](@ref conventions_names)
for more details.

We do this by creating a file `MyRestraint.h` in the `modules/foo/include/`
subdirectory. We'll look at each section of this file in turn. The
first part of the file looks like

\code{.cpp}
#ifndef IMPFOO_MY_RESTRAINT_H
#define IMPFOO_MY_RESTRAINT_H

#include <IMP/foo/foo_config.h>
#include <IMP/Restraint.h>

IMPFOO_BEGIN_NAMESPACE
\endcode

The `ifndef`/`define` is a *header guard*, which prevents the file from being
included multiple times. The convention in %IMP for the header guard
name is to use upper case `%IMP<module name>_<file name>`.

All of our classes will exist in the `IMP::foo` namespace.
The `IMPFOO_BEGIN_NAMESPACE` macro ensures this. It is defined in the
`foo_config.h` header file.

We are going to declare a restraint, so the compiler needs the declaration
of the IMP::Restraint base class, which is in `IMP/Restraint.h`.

The next part of the header declares our new class:

\code{.cpp}
class IMPFOOEXPORT MyRestraint : public Restraint {
  ParticleIndex p_;
  double k_;

public:
  MyRestraint(Model *m, ParticleIndex p, double k);
  void do_add_score_and_derivatives(ScoreAccumulator sa) const IMP_OVERRIDE;
  ModelObjectsTemp do_get_inputs() const;
  IMP_OBJECT_METHODS(MyRestraint);
};
\endcode

`IMPFOOEXPORT` should be used for any class that has a `.cpp` implementation,
and ensures the class can be used outside of the module (e.g. in Python).

The `IMP_OBJECT_METHODS` macro adds standard methods that all %IMP objects
(like IMP::Restraint) are expected to provide.

Our constructor takes an IMP::Model, a particle in that model, and a force
constant. We also declare the necessary methods to return the score and
inputs for the restraint - we will define these later in the `.cpp` file. 

The final part of the file looks like:

\code{.cpp}
IMPFOO_END_NAMESPACE

#endif /* IMPFOO_MY_RESTRAINT_H */
\endcode

This just closes the namespace and header guard from the start of the file.

# src directory (C++ code) {#src}

Next, we need to provide a definition for the class. We do this by making a
corresponding file `MyRestraint.cpp` in the `modules/foo/src/` subdirectory.
The first part of this file looks like:

\code{.cpp}
#include <IMP/foo/MyRestraint.h>
#include <IMP/core/XYZ.h>

IMPFOO_BEGIN_NAMESPACE
\endcode

Similarly to the header file, we need to put everything in the `IMP::foo`
namespace and include any needed header files. Here we include the previous
declaration of the `MyRestraint` class. We also need the declaration of the
[XYZ decorator](@ref IMP::core::XYZ) from the IMP::core module since we are
going to be using the particle’s coordinates to calculate the score.

Next, we define the constructor of the class:

\code{.cpp}
MyRestraint::MyRestraint(Model *m, ParticleIndex p, double k)
    : Restraint(m, "MyRestraint%1%"), p_(p), k_(k) {}
\endcode

The constructor simply calls the IMP::Restraint base class constructor (which
takes the IMP::Model and a human-readable name) and stores the `p` and `k`
arguments in the class attributes `p_` and `k_` (%IMP convention is for class
attributes to end in an underscore).  `%1%` is replaced with a unique
number, so multiple restraints will be named MyRestraint1, MyRestraint2, etc.

Next, we implement the restraint's score and first derivatives:
\code{.cpp}
void MyRestraint::do_add_score_and_derivatives(ScoreAccumulator sa) const {
  core::XYZ d(get_model(), p_);
  double score = .5 * k_ * square(d.get_z());
  if (sa.get_derivative_accumulator()) {
    double deriv = k_ * d.get_z();
    d.add_to_derivative(2, deriv, *sa.get_derivative_accumulator());
  }
  sa.add_score(score);
}
\endcode

We apply a simple harmonic restraint to the z coordinate to keep the particle
in the xy plane; we use the IMP::core::XYZ decorator to treat the particle
as a coordinate.

The IMP::ScoreAccumulator class is given the score, and analytic first
derivatives as well if requested.

We also need to tell %IMP which particles our restraint acts on by
overriding the `do_get_inputs` method:

\code{.cpp}
ModelObjectsTemp MyRestraint::do_get_inputs() const {
  return ModelObjectsTemp(1, get_model()->get_particle(p_));
}
\endcode

Here we just have a single particle, `p_`.

This is used to order the evaluation of restraints and constraints
(a constraint which moves particle A must be evaluated before any restraint
with A as an input) and for parallelization.
See [the IMP manual](@ref internal_dependencies) for more details.

Finally, the file ends with:

\code{.cpp}
IMPFOO_END_NAMESPACE
\endcode

As before, we need to close the namespace.

# pyext directory (Python interface) {#pyext}

Next, we make the C++ class available in Python. In %IMP we use a tool called
[SWIG](http://www.swig.org/) to do this. We need to configure the SWIG
interface by modifying the `swig.i-in` file in the `modules/foo/pyext/`
subdirectory. First, we need to tell SWIG how to wrap the `MyRestraint` class
by adding this line to the file: 

\code{.cpp}
IMP_SWIG_OBJECT(IMP::foo, MyRestraint, MyRestraints);
\endcode

This tells SWIG that `MyRestraint` is an %IMP [Object](@ref IMP::Object).
Most %IMP classes are subclasses of IMP::Object. These are heavyweight objects
which are always passed by reference-counted pointers, and are generally not
copied. Some simple classes (e.g. IMP::algebra::Vector3D) are subclasses of
IMP::Value instead. These are lightweight objects which are generally passed
by value or reference, and can be trivially copied. See the
[IMP manual](@ref conventions_values) for more details.

Next, we tell SWIG to parse our C++ header file for the class by adding the
line:
\code{.cpp}
%include "IMP/foo/MyRestraint.h"
\endcode

With the SWIG interface complete, we will be able to use our class from
Python as `IMP.foo.MyRestraint`.

# Documentation {#documentation}

Documentation of our custom class is omitted here for clarity, but all C++
headers and `.cpp` files should contain comments!
All comments are parsed by the [doxygen](http://www.doxygen.nl/) tool, which
uses the special comment markers `//!` and `/** */`.
See the [IMP manual](@ref documenting) for more details.

You should also fill in `modules/foo/README.md` with a description of the
module and the license it is released under. We recommend an open source
license such as the [LGPL](https://www.gnu.org/licenses/lgpl-3.0.en.html).

# test directory (test cases) {#test}

Next we should write a test case in the `modules/foo/test/` directory,
by creating a new file `test_restraint.py`. Test cases periodically verify
that %IMP is working correctly. They can be written in C++, but are almost
always written in Python for flexibility.

%IMP convention is to name a test file starting with `test_`.

The first part of our test file looks like
\code{.py}
from __future__ import print_function, division
import IMP
import IMP.test
import IMP.algebra
import IMP.core
import IMP.foo

class Tests(IMP.test.TestCase):
\endcode

This imports the %IMP kernel, any other IMP modules used in the test, and our
own `IMP.foo` module. The imports from `__future__` help to ensure that our
test works in the same way in both Python 2 and Python 3.

All tests should be classes that use the IMP.test module, which adds some
%IMP-specific functionality to the standard Python
[unittest](https://docs.python.org/2/library/unittest.html) module.

Next, we add a test method to our class:

\code{.py}
    def test_my_restraint(self):
        m = IMP.Model()
        p = m.add_particle("p")
        d = IMP.core.XYZ.setup_particle(m, p, IMP.algebra.Vector3D(1,2,3))
        r = IMP.foo.MyRestraint(m, p, 10.)
        self.assertAlmostEqual(r.evaluate(True), 45.0, delta=1e-4)
        self.assertLess(IMP.algebra.get_distance(d.get_derivatives(),
                                                 IMP.algebra.Vector3D(0,0,30)),
                        1e-4)
        self.assertEqual(len(r.get_inputs()), 1)
\endcode

This creates a restraint object, requests its score and derivatives
(`evaluate`), and asks for inputs (`get_inputs`). Here we simply test by
comparing to known good values using the
[standard unittest methods](https://docs.python.org/2/library/unittest.html#assert-methods)
`assertAlmostEqual`, `assertLess`, and `assertEqual`. (The IMP.test.TestCase
class provides some additional methods helpful for %IMP tests.)

\note Always use `assertAlmostEqual` for floating point comparisons,
      never `assertEqual` (two floating point numbers which look identical
      to a human may not be represented identically by the computer).

Finally, we end the test script with

\code{.py}
if __name__ == '__main__':
    IMP.test.main()
\endcode

This simply runs all the tests in this file if the script is run directly
from the command line with
`python test_restraint.py`.

# Dependencies {#depends}

Finally we need to tell the %IMP build system which other modules and external
code the module depends on. This is done by editing the file
`modules/foo/dependencies.py` to read:

\code{.py}
required_modules = 'core:algebra'
required_dependencies = ''
optional_dependencies = ''
\endcode

Since we use the IMP::core and IMP::algebra modules, we need to declare them
as requirements for this module.

`required_dependencies` and `optional_dependencies` can also be used to make
use of 3rd party libraries. See [the IMP manual](@ref extdepends) for more
information.

# Source control {#sourcecontrol}

Now is a good time to store the module in source control so that it can be
easily shared with collaborators and users, and changes to it can be tracked.
This will also simplify the process of incorporating the module into the
main %IMP distribution later, if applicable.

Most %IMP modules are stored on [GitHub](https://github.com).
See https://github.com/salilab/pmi/ and
https://github.com/salilab/npctransport for examples.

# Build and test {#buildtest}

To build the custom module, build %IMP from source code
[in the usual way](@ref installation_compilation).
`cmake` should detect the new module and configure it, and then your build
tool (usually `make` or `ninja`) will build it.

Test the new code with something like
(in the %IMP build directory):
\code{.sh}
$ ./setup_environment.sh python ../imp/modules/foo/test/test_restraint.py
\endcode

You can also run all of your module's test cases using the `ctest` tool;
see the [IMP manual](@ref installation_testing) for more details.
