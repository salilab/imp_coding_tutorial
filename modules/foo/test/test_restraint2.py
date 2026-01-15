import IMP
import IMP.test
import IMP.algebra
import IMP.core
import IMP.foo
import pickle


class Tests(IMP.test.TestCase):

    def test_my_restraint(self):
        """Test scoring of MyRestraint2"""
        m = IMP.Model()
        p = m.add_particle("p")
        d = IMP.core.XYZ.setup_particle(m, p, IMP.algebra.Vector3D(1,2,3))
        r = IMP.foo.MyRestraint2(m, p, 10.)
        self.assertAlmostEqual(r.evaluate(True), 45.0, delta=1e-4)
        self.assertLess(IMP.algebra.get_distance(d.get_derivatives(),
                                                 IMP.algebra.Vector3D(0,0,30)),
                        1e-4)
        self.assertEqual(len(r.get_inputs()), 1)

    def test_static_info(self):
        """Test static info of MyRestraint2"""
        m = IMP.Model()
        p = m.add_particle("p")
        d = IMP.core.XYZ.setup_particle(m, p, IMP.algebra.Vector3D(1,2,3))
        r = IMP.foo.MyRestraint2(m, p, 10.)
        info = r.get_static_info()
        self.assertEqual(info.get_number_of_string(), 1)
        self.assertEqual(info.get_string_key(0), "type")
        self.assertEqual(info.get_string_value(0), "IMP.foo.MyRestraint2")

        self.assertEqual(info.get_number_of_float(), 1)
        self.assertEqual(info.get_float_key(0), "force constant")
        self.assertAlmostEqual(info.get_float_value(0), 10.0, delta=0.001)

    def test_serialize(self):
        """Test (un-)serialize of MyRestraint2"""
        m = IMP.Model()
        p = m.add_particle("p")
        d = IMP.core.XYZ.setup_particle(m, p, IMP.algebra.Vector3D(1,2,3))
        r = IMP.foo.MyRestraint2(m, p, 10.)
        self.assertAlmostEqual(r.evaluate(False), 45.0, delta=1e-3)
        dump = pickle.dumps(r)
        newr = pickle.loads(dump)
        self.assertAlmostEqual(newr.evaluate(False), 45.0, delta=1e-3)
    
    def test_serialize_polymorphic(self):
        """Test (un-)serialize of MyRestraint2 via polymorphic pointer"""
        m = IMP.Model()
        p = m.add_particle("p")
        d = IMP.core.XYZ.setup_particle(m, p, IMP.algebra.Vector3D(1,2,3))
        r = IMP.foo.MyRestraint2(m, p, 10.)
        sf = IMP.core.RestraintsScoringFunction([r])
        self.assertAlmostEqual(sf.evaluate(False), 45.0, delta=1e-3)
        dump = pickle.dumps(sf)
        newsf = pickle.loads(dump)
        self.assertAlmostEqual(newsf.evaluate(False), 45.0, delta=1e-3)


if __name__ == '__main__':
    IMP.test.main()
