/**
 * @file raytacer.cpp
 * @brief Raytracer class
 *
 * Implement these functions for project 2.
 *
 * @author H. Q. Bovik (hqbovik)
 * @bug Unimplemented
 */

#include "raytracer.hpp"
#include "scene/scene.hpp"

#include <SDL/SDL_timer.h>
#include <iostream>


namespace _462 {

Color3 calcColor(real_t time, ray_t ray, int bestGeom, const Scene* scene, int depth);
real_t lastLength;

Raytracer::Raytracer()
    : scene( 0 ), width( 0 ), height( 0 ) { }

Raytracer::~Raytracer() { }

/**
 * Initializes the raytracer for the given scene. Overrides any previous
 * initializations. May be invoked before a previous raytrace completes.
 * @param scene The scene to raytrace.
 * @param width The width of the image being raytraced.
 * @param height The height of the image being raytraced.
 * @return true on success, false on error. The raytrace will abort if
 *  false is returned.
 */
bool Raytracer::initialize( Scene* scene, size_t width, size_t height )
{
    this->scene = scene;
    this->width = width;
    this->height = height;

    current_row = 0;

    // TODO any initialization or precompuation before the trace

    return true;
}

ray_t transform(ray_t curRay, const Geometry& geom){

	Matrix4 transform;
	Vector4 eye;
	Vector4 direction;
	Vector4 end;

	eye = Vector4(curRay.eye.x, curRay.eye.y, curRay.eye.z, 1.0);
	end = Vector4(curRay.end.x, curRay.end.y, curRay.end.z, 1.0);

	make_inverse_transformation_matrix(&transform, geom.position, geom.orientation, geom.scale);

	eye = transform * eye;
	end = transform * end;

	curRay.eye = Vector3(eye.x, eye.y, eye.z);
	direction = end - eye;
	lastLength = length(direction);
	direction = normalize(direction);
	curRay.end = Vector3(end.x, end.y, end.z);
	curRay.direction = Vector3(direction.x, direction.y, direction.z);

	return curRay;
}


ray_t getRay( const Scene* scene, size_t x, size_t y, size_t width, size_t height){

	ray_t returnRay;
	real_t alpha;	//near plane height / 2
	real_t beta;	//near plane width / 2
	real_t hPercent; // horizontal position from -1.0 to 1.0
	real_t vPercent; // vertical position from -1.0 to 1.0
	Vector3 s;
	Vector3 e;
	Vector3 npc; //near plane center point
	Vector3 leftVector;

	Camera cam = scene->camera;
	
	hPercent = ((1.0 * x / width) - 0.5) * 2.0;
	vPercent = ((1.0 * y / height) - 0.5) * 2.0;

	alpha = fabs(cam.get_near_clip()) * tan( cam.get_fov_radians() / 2 );
	beta = cam.get_aspect_ratio() * alpha;

	e = cam.get_position();
	npc = e + (cam.get_direction() * fabs(cam.get_near_clip()));

	leftVector = normalize(cross(cam.get_up(),cam.get_direction()));

	s = npc + (alpha * vPercent * cam.get_up()) - (beta * hPercent * leftVector);

	returnRay.eye = e;
	returnRay.direction = normalize(s - e);
	returnRay.end = s;

	return returnRay;
}

bool hitLight(ray_t shadowRay, PointLight light, const Scene* scene, int thisGeom){

	ray_t tRay;
	real_t time;

	Geometry* const* geometries = scene->get_geometries();
	Material* const* materials = scene->get_materials();

	real_t maxTime = length(light.position - shadowRay.eye);

    for ( size_t i = 0; i < scene->num_geometries(); i++ ) {	
        Geometry& geom = *geometries[i];
		tRay = transform(shadowRay, geom);
		time = geom.intersect(tRay);
		if( (time > SLOP_FACTOR) && (time < maxTime) && (i != thisGeom) )
			return false;
	}

	return true;

}

Color3 traceSpecularColor(ray_t reflectedRay, const Scene* scene, int depth, int thisGeom){

	ray_t tRay;
	real_t time;
	real_t bestTime = 100;
	int bestGeom;

	Geometry* const* geometries = scene->get_geometries();
	Material* const* materials = scene->get_materials();

    for ( size_t i = 0; i < scene->num_geometries(); i++ ) {	
        Geometry& geom = *geometries[i];
		tRay = transform(reflectedRay, geom);
		time = geom.intersect(tRay);
		if( (time > SLOP_FACTOR) && (time < bestTime) && (i != thisGeom) ){
			bestTime = time;
			bestGeom = i;
		}
	}

	if(bestTime < 100)
		return calcColor(bestTime, reflectedRay, bestGeom, scene, depth);
	//	return (*geometries[bestGeom]).getAmbientColor();
	else
		return scene->background_color;

}

Color3 calcColor(real_t time, ray_t ray, int bestGeom, const Scene* scene, int depth){

	Geometry* const* geometries = scene->get_geometries();
	Geometry& geom = *geometries[bestGeom];
	Vector3 ptIntersection;
	Vector3 normal;
	Vector4 temp;
	Matrix4 transform;
	Color3 color = geom.getAmbientColor() * scene->ambient_light;
	Color3 k = geom.getDiffuseColor();
	Matrix3 normalMatrix;

	make_transformation_matrix(&transform, geom.position, geom.orientation, geom.scale);

	make_normal_matrix(&normalMatrix,transform);

	normal = geom.getNormal();
	normal = normalMatrix * normal;
	normal = normalize(normal);

	Vector3 testIntersect = geom.getIntersectionPt();
	Vector4 test = Vector4(testIntersect.x, testIntersect.y, testIntersect.z, 1.0);
	test = transform * test;
	ptIntersection = Vector3(test.x, test.y, test.z);
	//ptIntersection = ray.eye + (time * ray.direction);

	const PointLight* lights = scene->get_lights();

	for( size_t i = 0; i < scene->num_lights(); i++){
		PointLight light = lights[i];
		Vector3 lPos = light.position;
		Vector3 vLight = lPos - ptIntersection;
		real_t d = length(vLight);
		//normalized vLight at one point...
		// matters?
		ray_t shadowRay;
		shadowRay.eye = ptIntersection;
		shadowRay.direction = normalize(vLight);
		shadowRay.end = lPos;
		if(hitLight(shadowRay, light, scene, bestGeom)){
			real_t a = dot(normal,vLight);
			real_t b = 0;

			real_t max = (a > b) ? a : b;

			real_t atten = light.attenuation.constant + (light.attenuation.linear * d) + (light.attenuation.constant * d * d);
			Color3 c = Color3(light.color.r / atten, light.color.g / atten, light.color.b / atten);
		//Color3 c = light.color;

			color += c * k * max;
		}
	}

	color = color * geom.getTextureColor();

	if(depth > 0){
		real_t dDotn = dot(ray.direction, normal);
		ray_t reflectedRay;
		reflectedRay.eye = ptIntersection;
		reflectedRay.direction = ray.direction - (2 * dDotn * normal);
		reflectedRay.end = ptIntersection + reflectedRay.direction;
		color += geom.getTextureColor() * geom.getSpecularColor() * traceSpecularColor(reflectedRay, scene, depth - 1, bestGeom);
	}
	return color;
}

/**
 * Performs a raytrace on the given pixel on the current scene.
 * The pixel is relative to the bottom-left corner of the image.
 * @param scene The scene to trace.
 * @param x The x-coordinate of the pixel to trace.
 * @param y The y-coordinate of the pixel to trace.
 * @param width The width of the screen in pixels.
 * @param height The height of the screen in pixels.
 * @return The color of that pixel in the final image.
 */
static Color3 trace_pixel( const Scene* scene, size_t x, size_t y, size_t width, size_t height )
{
	ray_t tRay;
	real_t time;
	real_t bestTime = 100000;
	ray_t bestRay;
	int bestGeom;

    assert( 0 <= x && x < width );
    assert( 0 <= y && y < height );
    
	ray_t curRay = getRay(scene, x, y, width, height);

	Geometry* const* geometries = scene->get_geometries();
	Material* const* materials = scene->get_materials();

    for ( size_t i = 0; i < scene->num_geometries(); i++ ) {	
        Geometry& geom = *geometries[i];
		tRay = transform(curRay, geom);
		time = geom.intersect(tRay);
		real_t lastTime = time;
		time = time / lastLength;
		if( (time > 0) && (time < bestTime) ){
			bestTime = time;
			bestGeom = i;
		}
	}

	if(bestTime < 100000)
		return calcColor(bestTime, curRay, bestGeom, scene, MAX_DEPTH);
	else
		return scene->background_color;
}


/**
 * Raytraces some portion of the scene. Should raytrace for about
 * max_time duration and then return, even if the raytrace is not copmlete.
 * The results should be placed in the given buffer.
 * @param buffer The buffer into which to place the color data. It is
 *  32-bit RGBA (4 bytes per pixel), in row-major order.
 * @param max_time, If non-null, the maximum suggested time this
 *  function raytrace before returning, in seconds. If null, the raytrace
 *  should run to completion.
 * @return true if the raytrace is complete, false if there is more
 *  work to be done.
 */
bool Raytracer::raytrace( unsigned char *buffer, real_t* max_time )
{
    // TODO Add any modifications to this algorithm, if needed.

    static const size_t PRINT_INTERVAL = 64;

    // the time in milliseconds that we should stop
    unsigned int end_time = 0;
    bool is_done;

    if ( max_time ) {
        // convert duration to milliseconds
        unsigned int duration = (unsigned int) ( *max_time * 1000 );
        end_time = SDL_GetTicks() + duration;
    }

    // until time is up, run the raytrace. we render an entire row at once
    // for simplicity and efficiency.
    for ( ; !max_time || end_time > SDL_GetTicks(); ++current_row ) {

        if ( current_row % PRINT_INTERVAL == 0 ) {
            printf( "Raytracing (row %u)...\n", current_row );
        }

        // we're done if we finish the last row
        is_done = current_row == height;
        // break if we finish
        if ( is_done )
            break;

        for ( size_t x = 0; x < width; ++x ) {
            // trace a pixel
            Color3 color = trace_pixel( scene, x, current_row, width, height );
            // write the result to the buffer, always use 1.0 as the alpha
            color.to_array( &buffer[4 * ( current_row * width + x )] );
        }
    }

    if ( is_done ) {
        printf( "Done raytracing!\n" );
    }

    return is_done;
}

} /* _462 */

