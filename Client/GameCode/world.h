#pragma once

#include "../PlatformShared/platform_shared.h"
#include "../FaceOff2/memory.h"

#include "bsp_tree.h"

struct Entity
{
	glm::vec3 pos;
	glm::vec3 dim;
};

struct World
{
	MemoryArena memoryArena;

	BSPNode* tree;

	Entity entities[1024];
	int numEntities;
	int maxEntityCount;
};

void initEntity(Entity* entity, glm::vec3 pos, glm::vec3 dim)
{
	entity->pos = pos;
	entity->dim = dim;
}



void AddPolygonToBrush(Brush* brush, glm::vec3* vertices, glm::vec3 normal)
{
	BspPolygon polygon(vertices, 4);
	brush->polygons.push_back(polygon);	
	float dist = glm::dot(normal, vertices[0]);
	polygon.plane = { normal, dist };
}


Brush CreateBrushForEntity(Entity* entity)
{
	Brush brush;

	// push the 6 sides
	glm::vec3 min = entity->pos - entity->dim;
	glm::vec3 max = entity->pos + entity->dim;


	/*
		y
		^
	  (-x,y,-z) p4 ------------ p5 (x,y,-z)
		|		|\              |\
		|		| \             | \
		|		| (-x,y,z)      |  \
		|		|	p0 ------------ p1 (x,y,z)
		|	    p6 -|----------	p7	|
	   (-x,-y,-z)\  |	(x,-y,-z)\	|
		|		  \	|		      \ |
		|		   \|			   \|
		|			p2 ------------ p3 (x,-y,z)
		|         (-x,-y,z)
		|
		------------------------------------------> x
		\
		 \
		  \
		   V z
	*/

	// 4 points on front face 
	glm::vec3 p0 = glm::vec3(min.x, max.y, max.z);
	glm::vec3 p1 = glm::vec3(max.x, max.y, max.z);
	glm::vec3 p2 = glm::vec3(min.x, min.y, max.z);
	glm::vec3 p3 = glm::vec3(max.x, min.y, max.z);

	// 4 points on back face 
	glm::vec3 p4 = glm::vec3(min.x, max.y, min.z);
	glm::vec3 p5 = glm::vec3(max.x, max.y, min.z);
	glm::vec3 p6 = glm::vec3(min.x, min.y, min.z);
	glm::vec3 p7 = glm::vec3(max.x, min.y, min.z);

	// front
	glm::vec3 vertices0[4] = {p0, p1, p3, p2};
	AddPolygonToBrush(&brush, vertices0, glm::vec3(0, 0, 1));

	// top
	glm::vec3 vertices1[4] = { p4, p5, p1, p0 };
	AddPolygonToBrush(&brush, vertices1, glm::vec3(0, 1, 0));

	// left 
	glm::vec3 vertices2[4] = { p4, p0, p2, p6 };
	AddPolygonToBrush(&brush, vertices2, glm::vec3(-1, 0, 0));

	// bottom
	glm::vec3 vertices3[4] = { p2, p3, p7, p6 };
	AddPolygonToBrush(&brush, vertices3, glm::vec3(0, -1, 0));

	// right 
	glm::vec3 vertices4[4] = { p1, p5, p7, p3 };
	AddPolygonToBrush(&brush, vertices4, glm::vec3(1, 0, 0));

	// back
	glm::vec3 vertices5[4] = { p5, p4, p6, p7 };
	AddPolygonToBrush(&brush, vertices5, glm::vec3(0, 0, -1));

	return brush;
}



// https://rpgcodex.net/forums/threads/level-design.110555/

void initWorld(World* world)
{
	// initlaize the game state  
	world->numEntities = 0;
	world->maxEntityCount = 1024;
	/*
	for (int i = 0; i < world->entityCount; i++)
	{
		world->entities[i].pos = glm::vec3(5 - i, 5 - i, 5 - i);
		world->entities[i].dim = glm::vec3(1);
		//			cout << i << ": " << gameState->entities[i].pos << std::endl;
	}
	*/


	// voxel generation of ice world
	// Site A
	
	std::vector<Brush> brushes;


	float wallHeight = 20;

	glm::vec3 siteASize = glm::vec3(50, wallHeight, 50);
	Entity* entity = &world->entities[world->numEntities++];
	initEntity(entity, glm::vec3(0, 0, 0), glm::vec3(siteASize.x, 0, siteASize.z));
	brushes.push_back(CreateBrushForEntity(entity));


	entity = &world->entities[world->numEntities++];
	initEntity(entity, glm::vec3(siteASize.x, wallHeight/2, 0), glm::vec3(1, siteASize.y, siteASize.z));
	brushes.push_back(CreateBrushForEntity(entity));

	entity = &world->entities[world->numEntities++];
	initEntity(entity, glm::vec3(-siteASize.x, wallHeight / 2, 0), glm::vec3(1, siteASize.y, siteASize.z));
	brushes.push_back(CreateBrushForEntity(entity));

	entity = &world->entities[world->numEntities++];
	initEntity(entity, glm::vec3(0, wallHeight / 2, siteASize.z), glm::vec3(siteASize.x, siteASize.y, 1));
	brushes.push_back(CreateBrushForEntity(entity));

	entity = &world->entities[world->numEntities++];
	initEntity(entity, glm::vec3(0, wallHeight / 2, -siteASize.z), glm::vec3(siteASize.x, siteASize.y, 1));
	brushes.push_back(CreateBrushForEntity(entity));

	
	// middle triangle
	entity = &world->entities[world->numEntities++];
	initEntity(entity, glm::vec3(0, 0, 0), glm::vec3(5, 5, 5));
	brushes.push_back(CreateBrushForEntity(entity));
	


	// world->tree = BuildBSPTree(brushes, 0);

	// middle aile





	// Site B


}