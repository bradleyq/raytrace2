#include "sphere.h"

#include <cmath>

#include  "../bsdf.h"
#include "../misc/sphere_drawing.h"

namespace CGL { namespace StaticScene {

bool Sphere::test(const Ray& r, double& t1, double& t2) const {

  // TODO (Part 1.4):
  // Implement ray - sphere intersection test.
  // Return true if there are intersections and writing the
  // smaller of the two intersection times in t1 and the larger in t2.
  double a = dot(r.d, r.d);
  Vector3D omc = r.o - this->o;
  double nb = -2 * dot(omc, r.d);
  double c = dot(omc, omc) - this->r2;
  double disc = nb * nb - 4 * a * c;
  if (disc >= 0.0) {
    disc = sqrt(disc);
    double t1tmp = (nb - disc) / (2 * a);
    double t2tmp = (nb + disc) / (2 * a);
    if (t1tmp > t2tmp) {
      double tmp = t1tmp;
      t1tmp = t2tmp;
      t2tmp = tmp;
    }
    if (t1tmp <= r.max_t && t2tmp >= r.min_t) {
      t1 = t1tmp;
      t2 = t2tmp;
      r.max_t = t1tmp > 0 ? t1tmp : t2tmp;
      return true;
    }
  }
  return false;
}

bool Sphere::intersect(const Ray& r) const {

  // TODO (Part 1.4):
  // Implement ray - sphere intersection.
  // Note that you might want to use the the Sphere::test helper here.
  double tmp;
  return test(r, tmp, tmp);
}

bool Sphere::intersect(const Ray& r, Intersection *i) const {

  // TODO (Part 1.4):
  // Implement ray - sphere intersection.
  // Note again that you might want to use the the Sphere::test helper here.
  // When an intersection takes place, the Intersection data should be updated
  // correspondingly.
  double t1;
  double t2;
  if (test(r, t1, t2)) {
    i->t = t1 > 0 ? t1 : t2;
    Vector3D norm = (i->t * r.d + r.o) - this->o;
    norm.normalize();
    i->n = norm;
    i->primitive = this;
    i->bsdf = get_bsdf();
    return true;
  }
  return false;

}

void Sphere::draw(const Color& c) const {
  Misc::draw_sphere_opengl(o, r, c);
}

void Sphere::drawOutline(const Color& c) const {
    //Misc::draw_sphere_opengl(o, r, c);
}


} // namespace StaticScene
} // namespace CGL
