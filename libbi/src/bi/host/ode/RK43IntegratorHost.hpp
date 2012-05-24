/**
 * @file
 *
 * @author Lawrence Murray <lawrence.murray@csiro.au>
 * $Rev$
 * $Date$
 */
#ifndef BI_HOST_ODE_RK43INTEGRATORHOST_HPP
#define BI_HOST_ODE_RK43INTEGRATORHOST_HPP

namespace bi {
/**
 * RK4(3)5[2R+]C low-storage Runge-Kutta integrator.
 *
 * @ingroup math_ode
 *
 * @tparam B Model type.
 * @tparam S Action type list.
 * @tparam T1 Scalar type.
 *
 * Implements the RK4(3)5[2R+]C method as described in @ref Kennedy2000
 * "Kennedy et. al. (2000)". Implementation described in @ref Murray2011
 * "Murray (2011)".
 */
template<class B, class S, class T1>
class RK43IntegratorHost {
public:
  /**
   * Integrate.
   *
   * @param t1 Start of time interval.
   * @param t2 End of time interval.
   * @param[in,out] s State.
   */
  static void update(const T1 t1, const T1 t2, State<B,ON_HOST>& s);
};
}

#include "RK43VisitorHost.hpp"
#include "IntegratorConstants.hpp"
#include "../host.hpp"
#include "../shared_host.hpp"
#include "../../state/Pa.hpp"
#include "../../typelist/front.hpp"
#include "../../typelist/pop_front.hpp"
#include "../../traits/block_traits.hpp"
#include "../../math/view.hpp"
#include "../../math/temp_vector.hpp"

template<class B, class S, class T1>
void bi::RK43IntegratorHost<B,S,T1>::update(const T1 t1, const T1 t2,
    State<B,ON_HOST>& s) {
  /* pre-condition */
  assert (t1 < t2);

  typedef typename temp_host_vector<real>::type vector_type;
  typedef Pa<ON_HOST,B,real,const_host,host,host,shared_host<S> > PX;
  typedef Ox<ON_HOST,B,real,host> OX;
  typedef RK43VisitorHost<B,S,S,real,PX,real> Visitor;
  static const int N = block_size<S>::value;

  bind(s);

  #pragma omp parallel
  {
    vector_type r1(N), r2(N), err(N), old(N);
    real t, h, e, e2, logfacold, logfac11, fac;
    int n, id, p, P = hostDState.size1();
    PX pax;
    OX x;

    #pragma omp for
    for (p = 0; p < P; ++p) {
      /* initialise shared memory from global memory */
      shared_host_init<B,S>(p);

      t = t1;
      h = h_h0;
      logfacold = BI_MATH_LOG(BI_REAL(1.0e-4));
      n = 0;
      old = *sharedHostState;
      r1 = old;

      /* integrate */
      while (t < t2 && n < h_nsteps) {
        if (BI_REAL(0.1)*BI_MATH_FABS(h) <= BI_MATH_FABS(t)*h_uround) {
          // step size too small
        }
        if (t + BI_REAL(1.01)*h - t2 > BI_REAL(0.0)) {
          h = t2 - t;
          if (h <= BI_REAL(0.0)) {
            t = t2;
            break;
          }
        }

        /* stages */
        Visitor::stage1(t, h, p, pax, r1.buf(), r2.buf(), err.buf());
        *sharedHostState = r1;

        Visitor::stage2(t, h, p, pax, r1.buf(), r2.buf(), err.buf());
        *sharedHostState = r2;

        Visitor::stage3(t, h, p, pax, r1.buf(), r2.buf(), err.buf());
        *sharedHostState = r1;

        Visitor::stage4(t, h, p, pax, r1.buf(), r2.buf(), err.buf());
        *sharedHostState = r2;

        Visitor::stage5(t, h, p, pax, r1.buf(), r2.buf(), err.buf());
        *sharedHostState = r1;

        e2 = BI_REAL(0.0);
        for (id = 0; id < N; ++id) {
          e = err(id)*h/(h_atoler + h_rtoler*BI_MATH_MAX(BI_MATH_FABS(old(id)), BI_MATH_FABS(r1(id))));
          e2 += e*e;
        }
        e2 /= N;

        if (e2 <= BI_REAL(1.0)) {
          /* accept */
          t += h;
          if (t < t2) {
            old = r1;
          }
        } else {
          /* reject */
          r1 = old;
          *sharedHostState = old;
        }

        /* compute next step size */
        if (t < t2) {
          logfac11 = h_expo*BI_MATH_LOG(e2);
          if (e2 > BI_REAL(1.0)) {
            /* step was rejected */
            h *= BI_MATH_MAX(h_facl, BI_MATH_EXP(h_logsafe - logfac11));
          } else {
            /* step was accepted */
            fac = BI_MATH_EXP(h_beta*logfacold + h_logsafe - logfac11); // Lund-stabilization
            fac = BI_MATH_MIN(h_facr, BI_MATH_MAX(h_facl, fac)); // bound
            h *= fac;
            logfacold = BI_REAL(0.5)*BI_MATH_LOG(BI_MATH_MAX(e2, BI_REAL(1.0e-8)));
          }
        }

        ++n;
      }

      /* write from shared back to global memory */
      shared_host_commit<B,S>(p);
    }
  }

  unbind(s);
}

#endif