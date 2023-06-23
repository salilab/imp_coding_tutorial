#ifndef IMPFOO_MY_RESTRAINT2_H
#define IMPFOO_MY_RESTRAINT2_H

#include <IMP/foo/foo_config.h>
#include <IMP/Restraint.h>
#include <cereal/access.hpp> 

IMPFOO_BEGIN_NAMESPACE

class IMPFOOEXPORT MyRestraint2 : public Restraint {
  ParticleIndex p_;
  double k_;

public:
  MyRestraint2(Model *m, ParticleIndex p, double k);
  void do_add_score_and_derivatives(ScoreAccumulator sa) const override;
  ModelObjectsTemp do_get_inputs() const override;
  IMP_OBJECT_METHODS(MyRestraint2);

  // RMF output support
  RestraintInfo *get_static_info() const override;

  // Serialization support
  MyRestraint2() {}
private:
  friend class cereal::access;
  template<class Archive> void serialize(Archive &ar) {
    ar(cereal::base_class<Restraint>(this), p_, k_);
  }
  IMP_OBJECT_SERIALIZE_DECL(MyRestraint2);

};

IMPFOO_END_NAMESPACE

#endif /* IMPFOO_MY_RESTRAINT2_H */
