/**
 * @file
 *
 * @author Lawrence Murray <lawrence.murray@csiro.au>
 * $Rev$
 * $Date$
 */
#ifndef BI_PDF_FUNCTOR_HPP
#define BI_PDF_FUNCTOR_HPP

#include "../math/scalar.hpp"
#include "../math/function.hpp"
#include "../math/misc.hpp"

namespace bi {

/**
 * @ingroup math_pdf
 *
 * Gamma log-density functor.
 */
template<class T>
struct gamma_log_density_functor: public std::unary_function<T,T> {
  const T alpha, beta, logZ;

  CUDA_FUNC_HOST
  gamma_log_density_functor(const T alpha, const T beta) :
      alpha(alpha), beta(beta), logZ(
          bi::lgamma(alpha) + alpha * bi::log(beta)) {
    //
  }

  CUDA_FUNC_BOTH
  T operator()(const T& x) const {
    return (alpha - static_cast<T>(1.0)) * bi::log(x) - x / beta - logZ;
  }
};

/**
 * @ingroup math_pdf
 *
 * Gamma density functor.
 */
template<class T>
struct gamma_density_functor: private gamma_log_density_functor<T> {
  CUDA_FUNC_HOST
  gamma_density_functor(const T alpha, const T beta) :
      gamma_log_density_functor<T>(alpha, beta) {
    //
  }

  CUDA_FUNC_BOTH
  T operator()(const T& x) const {
    return bi::exp(gamma_log_density_functor<T>::operator()(x));
  }
};

/**
 * @ingroup math_pdf
 *
 * Inverse gamma log-density functor.
 */
template<class T>
struct inverse_gamma_log_density_functor: private gamma_log_density_functor<T> {
  CUDA_FUNC_HOST
  inverse_gamma_log_density_functor(const T alpha, const T beta) :
      gamma_log_density_functor<T>(alpha, static_cast<T>(1.0) / beta) {
    //
  }

  CUDA_FUNC_BOTH
  T operator()(const T& x) const {
    return gamma_log_density_functor<T>::operator()(static_cast<T>(1.0) / x)
        - static_cast<T>(2.0) * bi::log(x);
  }
};

/**
 * @ingroup math_pdf
 *
 * Inverse gamma density functor.
 */
template<class T>
struct inverse_gamma_density_functor: private gamma_log_density_functor<T> {
  CUDA_FUNC_HOST
  inverse_gamma_density_functor(const T alpha, const T beta) :
      gamma_log_density_functor<T>(alpha, static_cast<T>(1.0) / beta) {
    //
  }

  CUDA_FUNC_BOTH
  T operator()(const T& x) const {
    return bi::exp(
        gamma_log_density_functor<T>::operator()(static_cast<T>(1.0) / x)
            - static_cast<T>(2.0) * bi::log(x));
  }
};

/**
 * Specialised functor for Gaussian density evaluations.
 *
 * @tparam T1 Scalar type.
 */
template<class T1>
struct gaussian_density_functor {
  /**
   * Constructor.
   *
   * @param logZ Log of normalising constant.
   */
  gaussian_density_functor(const T1 logZ) :
      logZ(logZ) {
    //
  }

  /**
   * Apply.
   */
  T1 operator()(const T1 p) {
    const T1 a = -0.5;

    return bi::exp(a * p - logZ);
  }

  /**
   * Log of normalising constant.
   */
  const T1 logZ;
};

/**
 * Specialised functor for Gaussian density evaluations.
 *
 * @tparam T1 Scalar type.
 */
template<class T1>
struct gaussian_density_update_functor {
  /**
   * Constructor.
   *
   * @param logZ Log of normalising constant.
   */
  gaussian_density_update_functor(const T1 logZ) :
      logZ(logZ) {
    //
  }

  /**
   * Apply.
   */
  T1 operator()(const T1 p1, const T1 p2) {
    const T1 a = -0.5;

    return p1 * bi::exp(a * p2 - logZ);
  }

  /**
   * Log of normalising constant.
   */
  const T1 logZ;
};

/**
 * Specialised functor for Gaussian log-density evaluations.
 *
 * @tparam T1 Scalar type.
 */
template<class T1>
struct gaussian_log_density_functor {
  /**
   * Constructor.
   *
   * @param logZ Log of normalising constant.
   */
  gaussian_log_density_functor(const T1 logZ) :
      logZ(logZ) {
    //
  }

  /**
   * Apply.
   */
  T1 operator()(const T1 p) {
    const T1 a = -0.5;

    return a * p - logZ;
  }

  /**
   * Log of normalising constant.
   */
  const T1 logZ;
};

/**
 * Specialised functor for Gaussian log-density evaluations.
 *
 * @tparam T1 Scalar type.
 */
template<class T1>
struct gaussian_log_density_update_functor {
  /**
   * Constructor.
   *
   * @param logZ Log of normalising constant.
   */
  gaussian_log_density_update_functor(const T1 logZ) :
      logZ(logZ) {
    //
  }

  /**
   * Apply.
   */
  T1 operator()(const T1 p1, const T1 p2) {
    const T1 a = -0.5;

    return p1 + a * p2 - logZ;
  }

  /**
   * Log of normalising constant.
   */
  const T1 logZ;
};

}

#endif
