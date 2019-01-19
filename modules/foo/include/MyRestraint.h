#ifndef IMPFOO_MY_RESTRAINT_H
#define IMPFOO_MY_RESTRAINT_H

#include <IMP/foo/foo_config.h>
#include <IMP/Restraint.h>

IMPFOO_BEGIN_NAMESPACE

class IMPFOOEXPORT MyRestraint : public Restraint {
  ParticleIndex p_;
  double k_;

public:
  MyRestraint(Model *m, ParticleIndex p, double k);
  void do_add_score_and_derivatives(ScoreAccumulator sa) const IMP_OVERRIDE;
  ModelObjectsTemp do_get_inputs() const IMP_OVERRIDE;
  IMP_OBJECT_METHODS(MyRestraint);
};

IMPFOO_END_NAMESPACE

#endif /* IMPFOO_MY_RESTRAINT_H */
