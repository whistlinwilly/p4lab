/**
 * @file model.cpp
 * @brief Model class
 *
 * @author Eric Butler (edbutler)
 * @author Zeyang Li (zeyangl)
 */

#include "scene/model.hpp"
#include "scene/material.hpp"
#include <GL/gl.h>
#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <sstream>


namespace _462 {

Model::Model() : mesh( 0 ), material( 0 ) { }
Model::~Model() { }

void Model::render() const
{
    if ( !mesh )
        return;
    if ( material )
        material->set_gl_state();
    mesh->render();
    if ( material )
        material->reset_gl_state();
}

real_t Model::getTime(MeshVertex v0, MeshVertex v1, MeshVertex v2, ray_t myRay){

	real_t a = v0.position.x - v1.position.x; 
	real_t b = v0.position.y - v1.position.y;
	real_t c = v0.position.z - v1.position.z;
	real_t d = v0.position.x - v2.position.x; 
	real_t e = v0.position.y - v2.position.y;
	real_t f = v0.position.z - v2.position.z;
	real_t g = myRay.direction.x;
	real_t h = myRay.direction.y;
	real_t i = myRay.direction.z;
	real_t j = v0.position.x - myRay.eye.x;
	real_t k = v0.position.y - myRay.eye.y;
	real_t l = v0.position.z - myRay.eye.z;
	
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

	this->gamma = (i * akMinusjb + h * jcMinusal + g * blMinuskc)/M;

	if( (this->gamma < 0) || (this->gamma > 1) )
		return -1;

	this->beta = (j * eiMinushf + k * gfMinusdi + l * dhMinuseg)/M;

	if( (this->beta < 0) || (this->beta > 1 - this->gamma) )
		return -1;

	Vector3 myNormal = (this->beta * v1.normal) + (this->gamma * v2.normal) + ((1 - this->beta - this->gamma) * v0.normal);
	myNormal = normalize(myNormal);

	if(dot(myNormal,myRay.direction) >= 0)
				t= -1;

	return t;
}

real_t Model::intersect(ray_t myRay){
	
	const MeshTriangle* myMesh = mesh->get_triangles();
	const MeshVertex* myVertices = mesh->get_vertices();
	real_t t = -1;
	real_t time = 100;

	for(int i = 0; i < mesh->num_triangles(); i++){

		MeshVertex v0 = myVertices[myMesh[i].vertices[0]];
		MeshVertex v1 = myVertices[myMesh[i].vertices[1]];
		MeshVertex v2 = myVertices[myMesh[i].vertices[2]];

		t = getTime(v0,v1,v2,myRay);

		if( (t > SLOP_FACTOR) && (t < time) ){

			time = t;

			real_t magic = 0.25;

			if(v1.tex_coord.x - v2.tex_coord.x > magic)
				std::cout<< "GOTEM";

			if(v2.tex_coord.x - v0.tex_coord.x > magic)
				std::cout << "GOTEM";

			if(v0.tex_coord.x - v1.tex_coord.x > magic)
				std::cout << "GOTEM";

			if(v1.tex_coord.y - v2.tex_coord.y > magic)
				std::cout<< "GOTEM";

			if(v2.tex_coord.y - v0.tex_coord.y > magic)
				std::cout << "GOTEM";

			if(v0.tex_coord.y - v1.tex_coord.y > magic)
				std::cout << "GOTEM";

			this->normal = (beta * v1.normal) + (gamma * v2.normal) + ((1-beta-gamma) * v0.normal);
			this->normal = normalize(normal);

			Vector2 coords = (this->beta * v1.tex_coord) + (this->gamma * v2.tex_coord) + ((1 - this->beta - this->gamma) * v0.tex_coord);

			int width;
			int height;

			material->get_texture_size(&width, &height);

			int x = coords.x * width;
			int y = coords.y * height;

			this->texture = material->get_texture_pixel(x, y);
	
			this->diffuse = material->diffuse;

			this->ambient = material->ambient;

			this->specular = material->specular;

			this->intersection = myRay.eye + (t * myRay.direction);
		}

	}

	if(time == 100)
		return -1;
	else
		return time;
}

Color3 Model::getAmbientColor(){
	return this->ambient;
}

Color3 Model::getDiffuseColor(){
	return this->diffuse;
}

Vector3 Model::getNormal(){
	return this->normal;
}

Color3 Model::getSpecularColor(){
	return this->specular;
}

Vector3 Model::getIntersectionPt(){
	return this->intersection;
}

Color3 Model::getTextureColor(){
	return this->texture;
}


} /* _462 */

