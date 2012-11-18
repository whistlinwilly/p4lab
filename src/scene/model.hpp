/**
 * @file model.hpp
 * @brief Model class
 *
 * @author Eric Butler (edbutler)
 */

#ifndef _462_SCENE_MODEL_HPP_
#define _462_SCENE_MODEL_HPP_

#include "scene/scene.hpp"
#include "scene/mesh.hpp"

namespace _462 {

/**
 * A mesh of triangles.
 */
class Model : public Geometry
{
public:

    const Mesh* mesh;
    const Material* material;

    Model();
    virtual ~Model();

	Color3 ambient;
	Color3 diffuse;
	Color3 specular;
	Color3 texture;
	Vector3 normal;
	Vector3 intersection;

    virtual void render() const;
	virtual real_t intersect(ray_t myRay);
	virtual Color3 getAmbientColor();
	virtual Color3 getDiffuseColor();
	virtual Vector3 getNormal();
	virtual Color3 getSpecularColor();
	virtual Vector3 getIntersectionPt();
	virtual Color3 getTextureColor();

private:
	real_t beta;
	real_t gamma;
	real_t getTime(MeshVertex v0, MeshVertex v1, MeshVertex v2, ray_t myRay);

};


} /* _462 */

#endif /* _462_SCENE_MODEL_HPP_ */

