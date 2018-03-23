#include "bbox.h"

#include "GL/glew.h"

#include <algorithm>
#include <iostream>

namespace CGL {

bool BBox::intersect(const Ray& r, double& t0, double& t1) const {

  // TODO (Part 2.2):
  // Implement ray - bounding box intersection test
  // If the ray intersected the bouding box within the range given by
  // t0, t1, update t0 and t1 with the new intersection times.
  Vector3D submin = this->min - r.o;
  Vector3D submax = this->max - r.o;
  double tminx = std::min(submin.x / r.d.x, submax.x / r.d.x);
  double tmaxx = std::max(submin.x / r.d.x, submax.x / r.d.x);
  double tminy = std::min(submin.y / r.d.y, submax.y / r.d.y);
  double tmaxy = std::max(submin.y / r.d.y, submax.y / r.d.y);
  double tminz = std::min(submin.z / r.d.z, submax.z / r.d.z);
  double tmaxz = std::max(submin.z / r.d.z, submax.z / r.d.z);
  double tmind = std::max(tminx, std::max(tminy, tminz));
  double tmaxd = std::min(tmaxx, std::min(tmaxy, tmaxz));
  if (tmind > tmaxd || tmind > r.max_t || tmaxd < r.min_t) {
    return false;
  }
  t0 = std::min(tmind, tmaxd);
  t1 = std::max(tmind, tmaxd);
  return true;
}

void BBox::draw(Color c) const {

  glColor4f(c.r, c.g, c.b, c.a);

	// top
	glBegin(GL_LINE_STRIP);
	glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, max.y, min.z);
  glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, max.y, max.z);
  glVertex3d(max.x, max.y, max.z);
	glEnd();

	// bottom
	glBegin(GL_LINE_STRIP);
  glVertex3d(min.x, min.y, min.z);
  glVertex3d(min.x, min.y, max.z);
  glVertex3d(max.x, min.y, max.z);
  glVertex3d(max.x, min.y, min.z);
  glVertex3d(min.x, min.y, min.z);
	glEnd();

	// side
	glBegin(GL_LINES);
	glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, min.y, max.z);
	glVertex3d(max.x, max.y, min.z);
  glVertex3d(max.x, min.y, min.z);
	glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, min.y, min.z);
	glVertex3d(min.x, max.y, max.z);
  glVertex3d(min.x, min.y, max.z);
	glEnd();

}

std::ostream& operator<<(std::ostream& os, const BBox& b) {
  return os << "BBOX(" << b.min << ", " << b.max << ")";
}

} // namespace CGL
