#include "bsdf.h"

#include <iostream>
#include <algorithm>
#include <utility>

using std::min;
using std::max;
using std::swap;

namespace CGL {

void make_coord_space(Matrix3x3& o2w, const Vector3D& n) {

    Vector3D z = Vector3D(n.x, n.y, n.z);
    Vector3D h = z;
    if (fabs(h.x) <= fabs(h.y) && fabs(h.x) <= fabs(h.z)) h.x = 1.0;
    else if (fabs(h.y) <= fabs(h.x) && fabs(h.y) <= fabs(h.z)) h.y = 1.0;
    else h.z = 1.0;

    z.normalize();
    Vector3D y = cross(h, z);
    y.normalize();
    Vector3D x = cross(z, y);
    x.normalize();

    o2w[0] = x;
    o2w[1] = y;
    o2w[2] = z;
}

// Diffuse BSDF //

Spectrum DiffuseBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  // TODO (Part 3.1): 
  // This function takes in both wo and wi and returns the evaluation of
  // the BSDF for those two directions.
  
  return this->reflectance * abs_cos_theta(wi);
}

Spectrum DiffuseBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // TODO (Part 3.1): 
  // This function takes in only wo and provides pointers for wi and pdf,
  // which should be assigned by this function.
  // After sampling a value for wi, it returns the evaluation of the BSDF
  // at (wo, *wi).
  *wi = sampler.get_sample(pdf);
  *pdf *= 2 * PI;
  return this->reflectance * abs_cos_theta(*wi);
}


// Mirror BSDF //

Spectrum MirrorBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum(0.0f, 0.0f, 0.0f);
}

Spectrum MirrorBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // TODO:
  // Implement MirrorBSDF
  reflect(wo, wi);
  *pdf = 1.0;
  return this->reflectance;
}

// Microfacet BSDF //

double MicrofacetBSDF::G(const Vector3D& wo, const Vector3D& wi) {
    return 1.0 / (1.0 + Lambda(wi) + Lambda(wo));
}

double MicrofacetBSDF::D(const Vector3D& h) {
    // TODO: proj3-2, part 2
    // Compute Beckmann normal distribution function (NDF) here.
    // You will need the roughness alpha.
    double asq = double(this->alpha) * double(this->alpha);
    double cth = cos_theta(h);
    
    return exp( -sin_theta2(h) / (cth * cth * asq)) / (PI * asq * cth * cth * cth * cth);
}

Spectrum MicrofacetBSDF::F(const Vector3D& wi) {
    // TODO: proj3-2, part 2
    // Compute Fresnel term for reflection on dielectric-conductor interface.
    // You will need both eta and etaK, both of which are Spectrum.
    Spectrum sqsum = this->eta * this->eta + this->k * this->k;
    Spectrum ct(cos_theta(wi), cos_theta(wi), cos_theta(wi));
    Spectrum Rs = (sqsum - 2 * this->eta * ct + ct * ct) / (sqsum + 2 * this->eta * ct + ct * ct); 
    Spectrum Rp = (sqsum * ct * ct - 2 * this->eta * ct + Spectrum(1.0f, 1.0f, 1.0f)) / (sqsum * ct * ct + 2 * this->eta * ct + Spectrum(1.0f, 1.0f, 1.0f));
    return (Rs + Rp) * 0.5;
}

Spectrum MicrofacetBSDF::f(const Vector3D& wo, const Vector3D& wi) {
    // TODO: proj3-2, part 2
    // Implement microfacet model here.
    Vector3D h = wo + wi;
    h.normalize();
    return cos_theta(wo) <= 0.0f || cos_theta(wi) <= 0.0f ? Spectrum(0.0f, 0.0f, 0.0f) : F(wi) * G(wo, wi) * D(h) / (4 * cos_theta(wo) * cos_theta(wi));
}

Spectrum MicrofacetBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
    // TODO: proj3-2, part 2
    // *Importance* sample Beckmann normal distribution function (NDF) here.
    // Note: You should fill in the sampled direction *wi and the corresponding *pdf,
    //       and return the sampled BRDF value.
    Vector2D rands(this->sampler.get_sample());
    double r1 = rands.x, r2 = 2 * PI * rands.y;
    double r = sqrt(-this->alpha * this->alpha * log(1 - r1));
    Vector3D h(r * cos(r2), r * sin(r2), 1.0);
    h.normalize();
    *wi = - wo - 2 * dot(-wo, h) * h;
    (*wi).normalize();
    if (cos_theta(wo) <= 0.0f || cos_theta(*wi) <= 0.0f) {
      *pdf = 0;
      return Spectrum(0.0f, 0.0f, 0.0f);
    }
    *pdf = D(h) * cos_theta(h) / (4 * dot(*wi, h));
    return f(wo, *wi) * abs_cos_theta(*wi);
}

// Refraction BSDF //

Spectrum RefractionBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum RefractionBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {

  // TODO:
  // Implement RefractionBSDF
  return Spectrum();
}

// Glass BSDF //

Spectrum GlassBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum(0.0f, 0.0f, 0.0f);
}

Spectrum GlassBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // TODO: 3-2 Part 1 Task 4
  // Compute Fresnel coefficient and either reflect or refract based on it.
  if (!refract(wo, wi, this->ior)) {
    reflect(wo, wi);
    *pdf = 1.0;
    return this->reflectance;
  }
  double R = (1.0 - this->ior) / (1.0 + this->ior);
  double eta = wo.z > 0 ? 1.0 / this->ior : this->ior;
  R *= R;
  double tmp = 1 - abs_cos_theta(wo);
  R = R + (1 - R) * tmp * tmp * tmp * tmp * tmp;
  if (coin_flip(R)) {
    reflect(wo, wi);
    *pdf = R;
    return R * this->reflectance;
  }
  *pdf = 1 - R;
  return *pdf * this->transmittance / (eta * eta);
}


void BSDF::reflect(const Vector3D& wo, Vector3D* wi) {
  // TODO: 3-2 Part 1 Task 1
  // Implement reflection of wo about normal (0,0,1) and store result in wi.
  *wi = Vector3D(-wo.x, -wo.y, wo.z);
}

bool BSDF::refract(const Vector3D& wo, Vector3D* wi, float ior) {
  // TODO: 3-2 Part 1 Task 3
  // Use Snell's Law to refract wo surface and store result ray in wi.
  // Return false if refraction does not occur due to total internal reflection
  // and true otherwise. When wo.z is positive, then wo corresponds to a
  // ray entering the surface through vacuum.
  int sign = 1;
  double n = ior;
  if (wo.z > 0) {
    n = 1.0 / ior;
    sign = -1;
  }
  double tmp = 1 - n * n * sin_theta2(wo);
  if (tmp < 0) {
    return false;
  }
  *wi = Vector3D(-n * wo.x, -n * wo.y, sign * sqrt(tmp));
  return true;
}

// Emission BSDF //

Spectrum EmissionBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum EmissionBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  *pdf = 1.0 / PI;
  *wi  = sampler.get_sample(pdf);
  return Spectrum();
}

} // namespace CGL
