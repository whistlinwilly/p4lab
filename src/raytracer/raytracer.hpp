/**
 * @file raytacer.hpp
 * @brief Raytracer class
 *
 * Implement these functions for project 2.
 *
 * @author H. Q. Bovik (hqbovik)
 * @bug Unimplemented
 */

#ifndef _462_RAYTRACER_HPP_
#define _462_RAYTRACER_HPP_

#include "math/color.hpp"
#include "math/vector.hpp"

#define SLOP_FACTOR (0.000001)
#define MAX_DEPTH (20)

namespace _462 {

class Scene;

	typedef struct{
		Vector3 eye;
		Vector3 direction;
		Vector3 end;
	}ray_t;

class Raytracer
{

public:

    Raytracer();

    ~Raytracer();

    bool initialize( Scene* scene, size_t width, size_t height );

    bool raytrace( unsigned char* buffer, real_t* max_time );

private:

    // the scene to trace
    Scene* scene;

    // the dimensions of the image to trace
    size_t width, height;

    // the next row to raytrace
    size_t current_row;
};

} /* _462 */

#endif /* _462_RAYTRACER_HPP_ */

