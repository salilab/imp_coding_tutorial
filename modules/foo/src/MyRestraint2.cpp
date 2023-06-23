#include <IMP/foo/MyRestraint2.h>
#include <IMP/core/XYZ.h>

IMPFOO_BEGIN_NAMESPACE

MyRestraint2::MyRestraint2(Model *m, ParticleIndex p, double k)
    : Restraint(m, "MyRestraint%1%"), p_(p), k_(k) {}

void MyRestraint2::do_add_score_and_derivatives(ScoreAccumulator sa) const {
  core::XYZ d(get_model(), p_);
  double score = .5 * k_ * square(d.get_z());
  if (sa.get_derivative_accumulator()) {
    double deriv = k_ * d.get_z();
    d.add_to_derivative(2, deriv, *sa.get_derivative_accumulator());
  }
  sa.add_score(score);
}

ModelObjectsTemp MyRestraint2::do_get_inputs() const {
  return ModelObjectsTemp(1, get_model()->get_particle(p_));
}

// RMF output support
RestraintInfo *MyRestraint2::get_static_info() const {
  IMP_NEW(RestraintInfo, ri, ());
  ri->add_string("type", "IMP.foo.MyRestraint2");
  ri->add_float("force constant", k_);
  return ri.release();
}

// Serialization support
IMP_OBJECT_SERIALIZE_IMPL(IMP::foo::MyRestraint2);

IMPFOO_END_NAMESPACE
