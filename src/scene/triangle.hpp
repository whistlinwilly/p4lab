/**
 * @file triangle.hpp
 * @brief Class definition for Triangle.
 *
 * @author Eric Butler (edbutler)
 */

#ifndef _462_SCENE_TRIANGLE_HPP_
#define _462_SCENE_TRIANGLE_HPP_

#include "scene/scene.hpp"

namespace _462 {

/**
 * a triangle geometry.
 * Triangles consist of 3 vertices. Each vertex has its own position, normal,
 * texture coordinate, and material. These should all be interpolated to get
 * values in the middle of the triangle.
 * These values are all in local space, so it must still be transformed by
 * the Geometry's position, orientation, and scale.
 */
class Triangle : public Geometry
{
public:

    struct Vertex
    {
        // note that position and normal are in local space
        Vector3 position;
        Vector3 normal;
        Vector2 tex_coord;
        const Material* material;
    };

    // the triangle's vertices, in CCW order
    Vertex vertices[3];

	Color3 color;
	Color3 diffuse;
	Color3 ambient;
	Color3 specular;
	Color3 texture;

	Vector3 intersection;

	Vector3 normal;

    Triangle();
    virtual ~Triangle();
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

#endif /* _462_SCENE_TRIANGLE_HPP_ */

