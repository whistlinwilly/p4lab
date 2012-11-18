/**
 * @file sphere.cpp
 * @brief Function defnitions for the Sphere class.
 *
 * @author Kristin Siu (kasiu)
 * @author Eric Butler (edbutler)
 */

#include "scene/sphere.hpp"
#include "application/opengl.hpp"

namespace _462 {

#define SPHERE_NUM_LAT 80
#define SPHERE_NUM_LON 100

#define SPHERE_NUM_VERTICES ( ( SPHERE_NUM_LAT + 1 ) * ( SPHERE_NUM_LON + 1 ) )
#define SPHERE_NUM_INDICES ( 6 * SPHERE_NUM_LAT * SPHERE_NUM_LON )
// index of the x,y sphere where x is lat and y is lon
#define SINDEX(x,y) ((x) * (SPHERE_NUM_LON + 1) + (y))
#define VERTEX_SIZE 8
#define TCOORD_OFFSET 0
#define NORMAL_OFFSET 2
#define VERTEX_OFFSET 5

static unsigned int Indices[SPHERE_NUM_INDICES];
static float Vertices[VERTEX_SIZE * SPHERE_NUM_VERTICES];

static void init_sphere()
{
    static bool initialized = false;
    if ( initialized )
        return;

    for ( int i = 0; i <= SPHERE_NUM_LAT; i++ ) {
        for ( int j = 0; j <= SPHERE_NUM_LON; j++ ) {
            real_t lat = real_t( i ) / SPHERE_NUM_LAT;
            real_t lon = real_t( j ) / SPHERE_NUM_LON;
            float* vptr = &Vertices[VERTEX_SIZE * SINDEX(i,j)];

            vptr[TCOORD_OFFSET + 0] = lon;
            vptr[TCOORD_OFFSET + 1] = 1-lat;

            lat *= PI;
            lon *= 2 * PI;
            real_t sinlat = sin( lat );

            vptr[NORMAL_OFFSET + 0] = vptr[VERTEX_OFFSET + 0] = sinlat * sin( lon );
            vptr[NORMAL_OFFSET + 1] = vptr[VERTEX_OFFSET + 1] = cos( lat ),
            vptr[NORMAL_OFFSET + 2] = vptr[VERTEX_OFFSET + 2] = sinlat * cos( lon );
        }
    }

    for ( int i = 0; i < SPHERE_NUM_LAT; i++ ) {
        for ( int j = 0; j < SPHERE_NUM_LON; j++ ) {
            unsigned int* iptr = &Indices[6 * ( SPHERE_NUM_LON * i + j )];

            unsigned int i00 = SINDEX(i,  j  );
            unsigned int i10 = SINDEX(i+1,j  );
            unsigned int i11 = SINDEX(i+1,j+1);
            unsigned int i01 = SINDEX(i,  j+1);

            iptr[0] = i00;
            iptr[1] = i10;
            iptr[2] = i11;
            iptr[3] = i11;
            iptr[4] = i01;
            iptr[5] = i00;
        }
    }

    initialized = true;
}

Sphere::Sphere()
    : radius(0), material(0) {
}

Sphere::~Sphere() {
}

void Sphere::render() const
{
    // create geometry if we haven't already
    init_sphere();

    if ( material )
        material->set_gl_state();

    // just scale by radius and draw unit sphere
    glPushMatrix();
    glScaled( radius, radius, radius );
    glInterleavedArrays( GL_T2F_N3F_V3F, VERTEX_SIZE * sizeof Vertices[0], Vertices );
    glDrawElements( GL_TRIANGLES, SPHERE_NUM_INDICES, GL_UNSIGNED_INT, Indices );
    glPopMatrix();

    if ( material )
        material->reset_gl_state();
}

real_t Sphere::intersect(ray_t myRay){

	//equations taken from shirley

	Vector3 e = myRay.eye;
	Vector3 c = Vector3(0,0,0);
	Vector3 d = myRay.direction;

	if(length(myRay.eye - c) < radius)
		return -1;

	Vector3 eMinusc = e - c;

	real_t bSquared = pow(dot(d, eMinusc),2);
	real_t twoC = dot(eMinusc, eMinusc) - pow(radius,2);
	real_t twoA = dot(d,d);
	real_t bSquaredMinus4AC = bSquared - (twoC * twoA);
	real_t sqrt = std::sqrt(bSquaredMinus4AC);
	real_t time;

	if(bSquaredMinus4AC >= 0){
		real_t negativeB = -1.0 * dot(d,eMinusc);
		real_t t1 = (negativeB + sqrt) / twoA;
		real_t t2 = (negativeB - sqrt) / twoA;
		color = this->material->ambient;
		if( (t1 > SLOP_FACTOR) ){
			if( (t1 < t2) )
				time = t1;
			else{
				if( t2 > SLOP_FACTOR )
					time =  t2;
				else
					time = t1;
			}
			intersection = myRay.eye + (time * myRay.direction);

			real_t lat = acos(intersection.y);
			real_t sinlat = sin(lat);
			real_t lon = acos(intersection.z / sinlat);

			lat = lat - floor(lat);
			lon = lon - floor(lon);

			int width;
			int height;

			material->get_texture_size(&width, &height);

			width = width * lat;
			height = height * lon;

			texture = material->get_texture_pixel(width, height);

			normal = intersection - c;
			return time;
		}
	}

	return -1;
}

Color3 Sphere::getAmbientColor(){
	return this->material->ambient;
}

Color3 Sphere::getDiffuseColor(){
	return this->material->diffuse;
}

Vector3 Sphere::getNormal(){
	return normal;
}

Color3 Sphere::getSpecularColor(){
	return this->material->specular;
}

Vector3 Sphere::getIntersectionPt(){
	return intersection;
}

Color3 Sphere::getTextureColor(){
	return Color3::White;
}



} /* _462 */

