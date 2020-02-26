/**
* @file types.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2020/01/08	(eus)
* @brief Define types
*
* @copyright Copyright (C) 2020 DigiPen I n s t i t u t e of Technology .
*/

#ifndef TYPES_H
#define TYPES_H


using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long int;

using ivec2 = glm::ivec2;
using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat3 = glm::mat3;
using mat4 = glm::mat4;

// compare... lengths???
//bool operator<(const vec3&l, const vec3&r) { return glm::length2(l) < glm::length2(r); }
//bool operator<(const vec2&l, const vec2&r) { return glm::length2(l) < glm::length2(r); }

#endif