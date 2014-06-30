//
//  vector.h
//  Renderer
//
//  Created by Benjamin Anderson on 6/24/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#ifndef Renderer_vector_h
#define Renderer_vector_h

#ifdef __cplusplus
extern "C" {
#endif

// A Vector is floating point array with either 3 or 4 components
// functions with the vec4 prefix require 4 elements in the array
// functions with vec3 prefix require only 3 elements in the array

// Subtracts one 4D vector to another
void vec4Add(float* vec, const float* lhs, const float* rhs);

// Subtracts one 4D vector from another
void vec4Subtract(float* vec, const float* lhs, const float* rhs);

// Multiplys one 4D vector by another
void vec4Multiply(float* vec, const float* lhs, const float* rhs);

// Divides one 4D vector by another
void vec4Divide(float* vec, const float* lhs, const float* rhs);

// Subtracts one 4D vector to another
void vec3Add(float* vec, const float* lhs, const float* rhs);

// Subtracts one 4D vector from another
void vec3Subtract(float* vec, const float* lhs, const float* rhs);

// Multiplys one 4D vector by another
void vec3Multiply(float* vec, const float* lhs, const float* rhs);

// Divides one 4D vector by another
void vec3Divide(float* vec, const float* lhs, const float* rhs);

// Calculates the Cross Product of a 3D vector
void vec3CrossProduct(float* vec, const float* lhs, const float* rhs);

// Normalizes a 3D vector
void vec3Normalize(float* vec, const float* src);

// Returns the Dot Product of 2 3D vectors
float vec3DotProduct(const float* lhs, const float* rhs);

// Returns the Dot Product of 2 4D vectors
float vec4DotProduct(const float* lhs, const float* rhs);

// Returns the length of a 3D vector
// (i.e the distance of a point from the origin)
float vec3Length(const float* vec);

// Returns the distance between two 3D points
float vec3Distance(const float* pointA, const float* pointB);

#ifdef __cplusplus
}
#endif

#endif // Renderer_vector_h
