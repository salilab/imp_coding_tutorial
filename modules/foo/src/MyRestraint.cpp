#include <IMP/foo/MyRestraint.h>
#include <IMP/core/XYZ.h>

IMPFOO_BEGIN_NAMESPACE

MyRestraint::MyRestraint(Model *m, ParticleIndex p, double k)
    : Restraint(m, "MyRestraint%1%"), p_(p), k_(k) {}

void MyRestraint::do_add_score_and_derivatives(ScoreAccumulator sa) const {
  core::XYZ d(get_model(), p_);
  double score = .5 * k_ * square(d.get_z());
  if (sa.get_derivative_accumulator()) {
    double deriv = k_ * d.get_z();
    d.add_to_derivative(2, deriv, *sa.get_derivative_accumulator());
  }
  sa.add_score(score);
}

ModelObjectsTemp MyRestraint::do_get_inputs() const {
  return ModelObjectsTemp(1, get_model()->get_particle(p_));
}

IMPFOO_END_NAMESPACE
