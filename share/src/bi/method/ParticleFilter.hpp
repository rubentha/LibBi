/**
 * @file
 *
 * @author Lawrence Murray <lawrence.murray@csiro.au>
 * $Rev$
 * $Date$
 */
#ifndef BI_METHOD_PARTICLEFILTER_HPP
#define BI_METHOD_PARTICLEFILTER_HPP

#include "Simulator.hpp"
#include "../cache/ParticleFilterCache.hpp"

namespace bi {
/**
 * Particle filter.
 *
 * @ingroup method
 *
 * @tparam B Model type.
 * @tparam S Simulator type.
 * @tparam R #concept::Resampler type.
 * @tparam IO1 Output type.
 *
 * @section Concepts
 *
 * #concept::Filter
 */
template<class B, class S, class R, class IO1>
class ParticleFilter {
public:
  /**
   * Constructor.
   *
   * @param m Model.
   * @param sim Simulator.
   * @param resam Resampler.
   * @param out Output.
   */
  ParticleFilter(B& m, S* sim = NULL, R* resam = NULL, IO1* out = NULL);

  /**
   * @name High-level interface.
   *
   * An easier interface for common usage.
   */
  //@{
  /**
   * Get simulator.
   *
   * @return Simulator.
   */
  S* getSim();

  /**
   * Set simulator.
   *
   * @param sim Simulator.
   */
  void setSim(S* sim);

  /**
   * Get resampler.
   *
   * @return Resampler.
   */
  R* getResam();

  /**
   * Set resampler.
   *
   * @param resam Resampler.
   */
  void setResam(R* resam);

  /**
   * Get output.
   *
   * @return Output.
   */
  IO1* getOutput();

  /**
   * Set output.
   *
   * @param out Output.
   */
  void setOutput(IO1* out);

  /**
   * %Filter forward.
   *
   * @tparam L Location.
   * @tparam IO2 Input type.
   *
   * @param[in,out] rng Random number generator.
   * @param first Start of time schedule.
   * @param last End of time schedule.
   * @param[out] s State.
   * @param inInit Initialisation file.
   *
   * @return Estimate of the marginal log-likelihood.
   */
  template<Location L, class IO2>
  real filter(Random& rng, const ScheduleIterator first,
      const ScheduleIterator last, State<B,L>& s, IO2* inInit);

  /**
   * %Filter forward, with fixed parameters.
   *
   * @tparam L Location.
   * @tparam V1 Vector type.
   *
   * @param[in,out] rng Random number generator.
   * @param first Start of time schedule.
   * @param last End of time schedule.
   * @param theta Parameters.
   * @param[out] s State.
   *
   * @return Estimate of the marginal log-likelihood.
   */
  template<Location L, class V1>
  real filter(Random& rng, const ScheduleIterator first,
      const ScheduleIterator last, const V1 theta, State<B,L>& s);

  /**
   * Filter forward conditioned on a single particle.
   *
   * @tparam L Location.
   * @tparam M1 Matrix type.
   *
   * @param[in,out] rng Random number generator.
   * @param first Start of time schedule.
   * @param last End of time schedule.
   * @param[out] s State.
   * @param X Path on which to condition. Rows index variables, columns
   * index times.
   *
   * This is the <em>conditional</em> particle filter of
   * @ref Andrieu2010 "Andrieu, Doucet \& Holenstein (2010)".
   */
  template<bi::Location L, class V1, class M1>
  real filter(Random& rng, const ScheduleIterator first,
      const ScheduleIterator last, const V1 theta, State<B,L>& s, const M1 X);

  /**
   * Sample single particle trajectory.
   *
   * @tparam M1 Matrix type.
   *
   * @param[in,out] rng Random number generator.
   * @param[out] X Trajectory.
   *
   * Sample a single particle trajectory from the smooth distribution.
   *
   * On output, @p X is arranged such that rows index variables and columns
   * index times.
   */
  template<class M1>
  void sampleTrajectory(Random& rng, M1 X);
  //@}

  /**
   * @name Low-level interface.
   *
   * Largely used by other features of the library or for finer control over
   * performance and behaviour.
   */
  //@{
  /**
   * Initialise.
   *
   * @tparam L Location.
   * @tparam V1 Vector type.
   * @tparam V2 Vector type.
   * @tparam IO2 Input type.
   *
   * @param[in,out] rng Random number generator.
   * @param now Current step in time schedule.
   * @param[out] s State.
   * @param[out] lws Log-weights.
   * @param[out] as Ancestry.
   * @param inInit Initialisation file.
   */
  template<Location L, class V1, class V2, class IO2>
  void init(Random& rng, const ScheduleElement now, State<B,L>& s, V1 lws,
      V2 as, IO2* inInit);

  /**
   * Initialise, with fixed parameters.
   *
   * @tparam L Location.
   * @tparam V1 Vector type.
   * @tparam V2 Vector type.
   * @tparam V3 Vector type.
   *
   * @param[in,out] rng Random number generator.
   * @param now Current step in time schedule.
   * @param theta Parameters.
   * @param[out] s State.
   * @param[out] lws Log-weights.
   * @param[out] as Ancestry.
   * @param inInit Initialisation file.
   */
  template<Location L, class V1, class V2, class V3>
  void init(Random& rng, const V1 theta, const ScheduleElement now,
      State<B,L>& s, V2 lws, V3 as);

  /**
   * Resample, predict and correct.
   *
   * @tparam L Location.
   * @tparam V1 Vector type.
   * @tparam V2 Vector type.
   *
   * @param[in,out] rng Random number generator.
   * @param[in,out] iter Current position in time schedule. Advanced on
   * return.
   * @param last End of time schedule.
   * @param[in,out] s State.
   * @param[in,out] lws Log-weights.
   * @param[out] as Ancestry after resampling.
   *
   * @return Estimate of the incremental log-likelihood.
   */
  template<bi::Location L, class V1, class V2>
  real step(Random& rng, ScheduleIterator& iter, const ScheduleIterator last,
      State<B,L>& s, V1 lws, V2 as);

  /**
   * Resample, predict and correct, conditionally.
   *
   * @tparam L Location.
   * @tparam M1 Matrix type.
   * @tparam V1 Vector type.
   * @tparam V2 Vector type.
   *
   * @param[in,out] rng Random number generator.
   * @param[in,out] iter Current position in time schedule. Advanced on
   * return.
   * @param last End of time schedule.
   * @param[in,out] s State.
   * @param X Path on which to condition. Rows index variables, columns
   * index times.
   * @param[in,out] lws Log-weights.
   * @param[out] as Ancestry after resampling.
   *
   * @return Estimate of the incremental log-likelihood.
   */
  template<bi::Location L, class M1, class V1, class V2>
  real step(Random& rng, ScheduleIterator& iter, const ScheduleIterator last,
      State<B,L>& s, const M1 X, V1 lws, V2 as);

  /**
   * Predict.
   *
   * @tparam L Location.
   *
   * @param[in,out] rng Random number generator.
   * @param next Next step in time schedule.
   * @param[in,out] s State.
   */
  template<Location L>
  void predict(Random& rng, const ScheduleElement next, State<B,L>& s);

  /**
   * Update particle weights using observations at the current time.
   *
   * @tparam L Location.
   * @tparam V1 Vector type.
   *
   * @param now Current step in time schedule.
   * @param s State.
   * @param lws Log-weights.
   *
   * @return Estimate of the incremental log-likelihood.
   */
  template<Location L, class V1>
  real correct(const ScheduleElement now, State<B,L>& s, V1 lws);

  /**
   * Resample.
   *
   * @tparam L Location.
   * @tparam V1 Vector type.
   * @tparam V2 Vector type.
   *
   * @param[in,out] rng Random number generator.
   * @param now Current step in time schedule.
   * @param[in,out] s State.
   * @param[in,out] lws Log-weights.
   * @param[out] as Ancestry after resampling.
   *
   * @return True if resampling was performed, false otherwise.
   */
  template<Location L, class V1, class V2>
  bool resample(Random& rng, const ScheduleElement now, State<B,L>& s, V1 lws,
      V2 as);

  /**
   * Resample with conditioned outcome for first particle.
   *
   * @tparam L Location.
   * @tparam V1 Vector type.
   * @tparam V2 Vector type.
   * @tparam R #concept::Resampler type.
   *
   * @param[in,out] rng Random number generator.
   * @param now Current step in time schedule.
   * @param[in,out] s State.
   * @param a Conditioned ancestor of first particle.
   * @param[in,out] lws Log-weights.
   * @param[out] as Ancestry after resampling.
   *
   * @return True if resampling was performed, false otherwise.
   */
  template<Location L, class V1, class V2>
  bool resample(Random& rng, const ScheduleElement now, State<B,L>& s,
      const int a, V1 lws, V2 as);

  /**
   * Output static variables.
   *
   * @param L Location.
   *
   * @param s State.
   */
  template<Location L>
  void output0(const State<B,L>& s);

  /**
   * Output dynamic variables.
   *
   * @tparam L Location.
   * @tparam V1 Vector type.
   * @tparam V2 Vector type.
   *
   * @param now Current step in time schedule.
   * @param s State.
   * @param r Was resampling performed?
   * @param lws Log-weights.
   * @param as Ancestry.
   */
  template<Location L, class V1, class V2>
  void output(const ScheduleElement now, const State<B,L>& s, const bool r,
      const V1 lws, const V2 as);

  /**
   * Output marginal log-likelihood estimate.
   *
   * @param ll Estimate of the marginal log-likelihood.
   */
  void outputT(const real ll);

  /**
   * Clean up.
   */
  void term();
  //@}

protected:
  /**
   * Model.
   */
  B& m;

  /**
   * Simulator.
   */
  S* sim;

  /**
   * Resampler.
   */
  R* resam;

  /**
   * Output.
   */
  IO1* out;
};

/**
 * Factory for creating ParticleFilter objects.
 *
 * @ingroup method
 *
 * @see ParticleFilter
 */
struct ParticleFilterFactory {
  /**
   * Create particle filter.
   *
   * @return ParticleFilter object. Caller has ownership.
   *
   * @see ParticleFilter::ParticleFilter()
   */
  template<class B, class S, class R, class IO1>
  static ParticleFilter<B,S,R,IO1>* create(B& m, S* sim = NULL, R* resam =
      NULL, IO1* out = NULL) {
    return new ParticleFilter<B,S,R,IO1>(m, sim, resam, out);
  }

  /**
   * Create particle filter.
   *
   * @return ParticleFilter object. Caller has ownership.
   *
   * @see ParticleFilter::ParticleFilter()
   */
  template<class B, class S, class R>
  static ParticleFilter<B,S,R,ParticleFilterCache<> >* create(B& m, S* sim =
      NULL, R* resam = NULL) {
    return new ParticleFilter<B,S,R,ParticleFilterCache<> >(m, sim, resam);
  }
};
}

#include "../primitive/vector_primitive.hpp"
#include "../primitive/matrix_primitive.hpp"
#include "../traits/resampler_traits.hpp"

template<class B, class S, class R, class IO1>
bi::ParticleFilter<B,S,R,IO1>::ParticleFilter(B& m, S* sim, R* resam,
    IO1* out) :
    m(m), sim(sim), resam(resam), out(out) {
  /* pre-conditions */
  BI_ASSERT(sim != NULL);

  //
}

template<class B, class S, class R, class IO1>
inline S* bi::ParticleFilter<B,S,R,IO1>::getSim() {
  return sim;
}

template<class B, class S, class R, class IO1>
inline void bi::ParticleFilter<B,S,R,IO1>::setSim(S* sim) {
  this->sim = sim;
}

template<class B, class S, class R, class IO1>
inline R* bi::ParticleFilter<B,S,R,IO1>::getResam() {
  return resam;
}

template<class B, class S, class R, class IO1>
inline void bi::ParticleFilter<B,S,R,IO1>::setResam(R* resam) {
  this->resam = resam;
}

template<class B, class S, class R, class IO1>
inline IO1* bi::ParticleFilter<B,S,R,IO1>::getOutput() {
  return out;
}

template<class B, class S, class R, class IO1>
inline void bi::ParticleFilter<B,S,R,IO1>::setOutput(IO1* out) {
  this->out = out;
}

template<class B, class S, class R, class IO1>
template<bi::Location L, class IO2>
real bi::ParticleFilter<B,S,R,IO1>::filter(Random& rng,
    const ScheduleIterator first, const ScheduleIterator last, State<B,L>& s,
    IO2* inInit) {
  const int P = s.size();
  bool r = false;
  real ll = 0.0;

  typename loc_temp_vector<L,real>::type lws(P);
  typename loc_temp_vector<L,int>::type as(P);

  ScheduleIterator iter = first;
  init(rng, *iter, s, lws, as, inInit);
  output0(s);
  ll = correct(*iter, s, lws);
  output(*iter, s, r, lws, as);
  while (iter + 1 != last) {
    ll += step(rng, iter, last, s, lws, as);
  }
  term();
  outputT(ll);

  return ll;
}

template<class B, class S, class R, class IO1>
template<bi::Location L, class V1>
real bi::ParticleFilter<B,S,R,IO1>::filter(Random& rng,
    const ScheduleIterator first, const ScheduleIterator last, const V1 theta,
    State<B,L>& s) {
  // this implementation is (should be) the same as filter() above, but with
  // a different init() call

  const int P = s.size();
  bool r = false;
  real ll = 0.0;

  typename loc_temp_vector<L,real>::type lws(P);
  typename loc_temp_vector<L,int>::type as(P);

  ScheduleIterator iter = first;
  init(rng, theta, *iter, s, lws, as);
  output0(s);
  ll = correct(*iter, s, lws);
  output(*iter, s, r, lws, as);
  while (iter + 1 != last) {
    ll += step(rng, iter, last, s, lws, as);
  }
  term();
  outputT(ll);

  return ll;
}

template<class B, class S, class R, class IO1>
template<bi::Location L, class V1, class M1>
real bi::ParticleFilter<B,S,R,IO1>::filter(Random& rng,
    const ScheduleIterator first, const ScheduleIterator last, const V1 theta,
    State<B,L>& s, M1 X) {
  // this implementation is (should be) the same as filter() above, but with
  // a different step() call

  const int P = s.size();
  bool r = false;
  real ll = 0.0;

  typename loc_temp_vector<L,real>::type lws(P);
  typename loc_temp_vector<L,int>::type as(P);

  ScheduleIterator iter = first;
  init(rng, theta, *iter, s, lws, as);
  row(s.getDyn(), 0) = column(X, 0);
  output0(s);
  ll = correct(*iter, s, lws);
  output(*iter, s, r, lws, as);
  while (iter + 1 != last) {
    ll += step(rng, iter, last, s, X, lws, as);
  }
  term();
  outputT(ll);

  return ll;
}

template<class B, class S, class R, class IO1>
template<class M1>
void bi::ParticleFilter<B,S,R,IO1>::sampleTrajectory(Random& rng, M1 X) {
  /* pre-condition */
  BI_ASSERT(out != NULL);

  /* pre-condition */
  int p = rng.multinomial(out->getLogWeights());
  out->readTrajectory(p, X);
}

template<class B, class S, class R, class IO1>
template<bi::Location L, class V1, class V2, class IO2>
void bi::ParticleFilter<B,S,R,IO1>::init(Random& rng,
    const ScheduleElement now, State<B,L>& s, V1 lws, V2 as, IO2* inInit) {
  /* pre-condition */
  BI_ASSERT(s.size() == as.size());
  BI_ASSERT(s.size() == lws.size());

  sim->init(rng, now, s, inInit);
  lws.clear();
  seq_elements(as, 0);
  if (out != NULL) {
    out->clear();
  }
}

template<class B, class S, class R, class IO1>
template<bi::Location L, class V1, class V2, class V3>
void bi::ParticleFilter<B,S,R,IO1>::init(Random& rng, const V1 theta,
    const ScheduleElement now, State<B,L>& s, V2 lws, V3 as) {
  /* pre-condition */
  BI_ASSERT(s.size() == as.size());
  BI_ASSERT(s.size() == lws.size());

  sim->init(rng, theta, now, s);
  lws.clear();
  seq_elements(as, 0);
  if (out != NULL) {
    out->clear();
  }
}

template<class B, class S, class R, class IO1>
template<bi::Location L, class V1, class V2>
real bi::ParticleFilter<B,S,R,IO1>::step(Random& rng, ScheduleIterator& iter,
    const ScheduleIterator last, State<B,L>& s, V1 lws, V2 as) {
  bool r = resample(rng, *iter, s, lws, as);
  do {
    ++iter;
    predict(rng, *iter, s);
  } while (iter + 1 != last && !iter->hasOutput());
  real ll = correct(*iter, s, lws);
  output(*iter, s, r, lws, as);

  return ll;
}

template<class B, class S, class R, class IO1>
template<bi::Location L, class M1, class V1, class V2>
real bi::ParticleFilter<B,S,R,IO1>::step(Random& rng, ScheduleIterator& iter,
    const ScheduleIterator last, State<B,L>& s, const M1 X, V1 lws, V2 as) {
  bool r = resample(rng, *iter, s, lws, as);
  do {
    ++iter;
    predict(rng, *iter, s);
  } while (iter + 1 != last && !iter->hasOutput());
  row(s.getDyn(), 0) = column(X, iter->indexOutput());
  real ll = correct(*iter, s, lws);
  output(*iter, s, r, lws, as);

  return ll;
}

template<class B, class S, class R, class IO1>
template<bi::Location L>
void bi::ParticleFilter<B,S,R,IO1>::predict(Random& rng,
    const ScheduleElement next, State<B,L>& s) {
  sim->advance(rng, next, s);
}

template<class B, class S, class R, class IO1>
template<bi::Location L, class V1>
real bi::ParticleFilter<B,S,R,IO1>::correct(const ScheduleElement now,
    State<B,L>& s, V1 lws) {
  /* pre-condition */
  BI_ASSERT(s.size() == lws.size());

  real ll = 0.0;
  if (now.hasObs()) {
    m.observationLogDensities(s, sim->getObs()->getMask(now.indexObs()), lws);
    ll = logsumexp_reduce(lws) - bi::log(static_cast<real>(s.size()));
  }
  return ll;
}

template<class B, class S, class R, class IO1>
template<bi::Location L, class V1, class V2>
bool bi::ParticleFilter<B,S,R,IO1>::resample(Random& rng,
    const ScheduleElement now, State<B,L>& s, V1 lws, V2 as) {
  /* pre-condition */
  BI_ASSERT(s.size() == lws.size());

  bool r = now.hasObs() && resam != NULL && resam->isTriggered(lws);
  if (r) {
    if (resampler_needs_max<R>::value) {
      resam->setMaxLogWeight(
          m.observationMaxLogDensity(s,
              sim->getObs()->getMask(now.indexObs())));
    }
    resam->resample(rng, lws, as, s);
  } else {
    seq_elements(as, 0);
    Resampler::normalise(lws);
  }
  return r;
}

template<class B, class S, class R, class IO1>
template<bi::Location L, class V1, class V2>
bool bi::ParticleFilter<B,S,R,IO1>::resample(Random& rng,
    const ScheduleElement now, State<B,L>& s, const int a, V1 lws, V2 as) {
  /* pre-condition */
  BI_ASSERT(s.size() == lws.size());
  BI_ASSERT(a == 0);

  bool r = now.hasObs() && resam != NULL && resam->isTriggered(lws);
  if (r) {
    if (resampler_needs_max<R>::value) {
      resam->setMaxLogWeight(
          m.observationMaxLogDensity(s,
              sim->getObs()->getMask(now.indexObs())));
    }
    resam->cond_resample(rng, a, a, lws, as, s);
  } else {
    seq_elements(as, 0);
    Resampler::normalise(lws);
  }
  return r;
}

template<class B, class S, class R, class IO1>
template<bi::Location L>
void bi::ParticleFilter<B,S,R,IO1>::output0(const State<B,L>& s) {
  if (out != NULL) {
    out->writeParameters(s.get(P_VAR));
  }
}

template<class B, class S, class R, class IO1>
template<bi::Location L, class V1, class V2>
void bi::ParticleFilter<B,S,R,IO1>::output(const ScheduleElement now,
    const State<B,L>& s, const bool r, const V1 lws, const V2 as) {
  if (out != NULL && now.hasOutput()) {
    const int k = now.indexOutput();
    out->writeTime(k, now.getTime());
    out->writeState(k, s.getDyn(), as, r);
    out->writeLogWeights(k, lws);
  }
}

template<class B, class S, class R, class IO1>
void bi::ParticleFilter<B,S,R,IO1>::outputT(const real ll) {
  if (out != NULL) {
    out->writeLL(ll);
  }
}

template<class B, class S, class R, class IO1>
void bi::ParticleFilter<B,S,R,IO1>::term() {
  sim->term();
}

#endif
