/**
 * @file sphere.hpp
 * @brief Class defnition for Sphere.
 *
 * @author Kristin Siu (kasiu)
 * @author Eric Butler (edbutler)
 */

#ifndef _462_SCENE_SPHERE_HPP_
#define _462_SCENE_SPHERE_HPP_

#include "scene/scene.hpp"

namespace _462 {

/**
 * A sphere, centered on its position with a certain radius.
 */
class Sphere : public Geometry
{
public:

    real_t radius;
    const Material* material;

	Color3 color;
	Color3 diffuse;
	Color3 ambient;
	Color3 texture;

	Vector3 normal;
	Vector3 intersection;

    Sphere();
    virtual ~Sphere();
    virtual void render() const;
	virtual real_t intersect(ray_t myRay);
	virtual Color3 getAmbientColor();
	virtual Color3 getDiffuseColor();
	virtual Vector3 getNormal();
	virtual Color3 getSpecularColor();
	virtual Vector3 getIntersectionPt();
	virtual Color3 getTextureColor();

};

} /* _462 */

#endif /* _462_SCENE_SPHERE_HPP_ */

