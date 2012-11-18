/**
 * @file triangle.cpp
 * @brief Function definitions for the Triangle class.
 *
 * @author Eric Butler (edbutler)
 */

#include "scene/triangle.hpp"
#include "application/opengl.hpp"

namespace _462 {

Triangle::Triangle()
{
    vertices[0].material = 0;
    vertices[1].material = 0;
    vertices[2].material = 0;
}

Triangle::~Triangle() { 

}

void Triangle::render() const
{
    bool materials_nonnull = true;
    for ( int i = 0; i < 3; ++i )
        materials_nonnull = materials_nonnull && vertices[i].material;

    // this doesn't interpolate materials. Ah well.
    if ( materials_nonnull )
        vertices[0].material->set_gl_state();

    glBegin(GL_TRIANGLES);

    glNormal3dv( &vertices[0].normal.x );
    glTexCoord2dv( &vertices[0].tex_coord.x );
    glVertex3dv( &vertices[0].position.x );

    glNormal3dv( &vertices[1].normal.x );
    glTexCoord2dv( &vertices[1].tex_coord.x );
    glVertex3dv( &vertices[1].position.x);

    glNormal3dv( &vertices[2].normal.x );
    glTexCoord2dv( &vertices[2].tex_coord.x );
    glVertex3dv( &vertices[2].position.x);

    glEnd();

    if ( materials_nonnull )
        vertices[0].material->reset_gl_state();
}

real_t Triangle::intersect(ray_t myRay){
	//variable names taken from shirley text
	//corresponding to equation 4.2

	real_t a = vertices[0].position.x - vertices[1].position.x; 
	real_t b = vertices[0].position.y - vertices[1].position.y;
	real_t c = vertices[0].position.z - vertices[1].position.z;
	real_t d = vertices[0].position.x - vertices[2].position.x; 
	real_t e = vertices[0].position.y - vertices[2].position.y;
	real_t f = vertices[0].position.z - vertices[2].position.z;
	real_t g = myRay.direction.x;
	real_t h = myRay.direction.y;
	real_t i = myRay.direction.z;
	real_t j = vertices[0].position.x - myRay.eye.x;
	real_t k = vertices[0].position.y - myRay.eye.y;
	real_t l = vertices[0].position.z - myRay.eye.z;
	
	real_t akMinusjb = a * k - j * b;
	real_t jcMinusal = j * c - a * l;
	real_t blMinuskc = b * l - k * c;
	real_t eiMinushf = e * i - h * f;
	real_t gfMinusdi = g * f - d * i;
	real_t dhMinuseg = d * h - e * g;

	real_t M = a * eiMinushf + b * gfMinusdi + c * dhMinuseg;
	real_t t = -1.0 * (f * akMinusjb + e * jcMinusal + d * blMinuskc)/M;

	if( (t < SLOP_FACTOR) || (t > 100) )
		return -1;

	real_t gamma = (i * akMinusjb + h * jcMinusal + g * blMinuskc)/M;

	if( (gamma < 0) || (gamma > 1) )
		return -1;

	real_t beta = (j * eiMinushf + k * gfMinusdi + l * dhMinuseg)/M;

	if( (beta < 0) || (beta > 1 - gamma) )
		return -1;

	Vector2 coords = (beta * vertices[1].tex_coord) + (gamma * vertices[2].tex_coord) + ((1-beta-gamma) * vertices[0].tex_coord);

	double scratch;

	coords.x = modf(coords.x, &scratch);
	coords.y = modf(coords.y, &scratch);

	int width;
	int height;
	int x;
	int y;

	vertices[1].material->get_texture_size(&width, &height);

	x = coords.x * width;
	y = coords.y * height;

	Color3 betaPixel = vertices[1].material->get_texture_pixel(x,y);

	vertices[2].material->get_texture_size(&width, &height);

	x = coords.x * width;
	y = coords.y * height;

	Color3 gammaPixel = vertices[2].material->get_texture_pixel(x, y);

	vertices[0].material->get_texture_size(&width, &height);

	x = coords.x * width;
	y = coords.y * height;

	Color3 betaGammaPixel = vertices[0].material->get_texture_pixel(x, y);

	texture = beta * betaPixel + gamma * gammaPixel + (1-beta-gamma) * betaGammaPixel;

	diffuse = (beta * vertices[1].material->diffuse) + (gamma * vertices[2].material->diffuse) + ((1-beta-gamma) * vertices[0].material->diffuse);

	ambient = (beta * vertices[1].material->ambient) + (gamma * vertices[2].material->ambient) + ((1-beta-gamma) * vertices[0].material->ambient);
	
	specular = (beta * vertices[1].material->specular) + (gamma * vertices[2].material->specular) + ((1-beta-gamma) * vertices[0].material->specular);
	
	normal = (beta * vertices[1].normal) + (gamma * vertices[2].normal) + ((1-beta-gamma) * vertices[0].normal);
	normal = normalize(normal);

	intersection = myRay.eye + (t * myRay.direction);

	return t;
}

Color3 Triangle::getAmbientColor(){
	return ambient;
}

Color3 Triangle::getDiffuseColor(){
	return diffuse;
}

Vector3 Triangle::getNormal(){
	return normal;
}

Color3 Triangle::getSpecularColor(){
	return specular;
}

Vector3 Triangle::getIntersectionPt(){
	return intersection;
}

Color3 Triangle::getTextureColor(){
	return texture;
}


} /* _462 */

