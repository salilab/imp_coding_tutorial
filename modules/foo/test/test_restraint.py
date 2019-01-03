from __future__ import print_function, division
import IMP
import IMP.test
import IMP.algebra
import IMP.core
import IMP.foo

class Tests(IMP.test.TestCase):

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

if __name__ == '__main__':
    IMP.test.main()
