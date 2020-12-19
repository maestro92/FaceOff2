#pragma once

#include <assert.h> 

#include "../PlatformShared/platform_shared.h"
#include "../FaceOff2/memory.h"

#include "bsp_tree.h"

// each face is a quad
struct Face
{
	// Assume this is a quad
	// p0 p1 p2 p3 in clock wise order
	std::vector<glm::vec3> vertices;
};



struct Entity
{
	glm::vec3 pos;
	glm::vec3 dim;


	// For AABB physics, in object space
	glm::vec3 min;
	glm::vec3 max;

	// For Rendering
	// TODO: change this model index
	std::vector<Face> model;
};

struct World
{
	MemoryArena memoryArena;

	BSPNode* tree;

	Entity entities[1024];
	int numEntities;
	int maxEntityCount;
};


void initEntity(Entity* entity, glm::vec3 pos, std::vector<Face> faces)
{
	entity->pos = pos;
	entity->model = faces;
}


std::vector<glm::vec3> GetCubeVertices(glm::vec3 min, glm::vec3 max)
{
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

	return {p0, p1, p2, p3, p4, p5, p6, p7};
}

std::vector<glm::vec3> ToVector(glm::vec3* data)
{
	std::vector<glm::vec3> result;
	for (int i = 0; i < ArrayCount(data); i++)
	{
		result.push_back(data[i]);
	}
	return result;
}

enum RampRiseDirection
{
	POS_X,
	NEG_X,
	POS_Z,
	NEG_Z,
};

// min max as a volume
std::vector<Face> CreateRampMinMax(glm::vec3 min, glm::vec3 max, RampRiseDirection rampRiseDirection)
{
	std::vector<Face> result;
	std::vector<std::vector<glm::vec3>> temp;

	std::vector<glm::vec3> vertices = GetCubeVertices(min, max);

	// 4 points on front face 
	glm::vec3 p0 = vertices[0];
	glm::vec3 p1 = vertices[1];
	glm::vec3 p2 = vertices[2];
	glm::vec3 p3 = vertices[3];

	// 4 points on back face 
	glm::vec3 p4 = vertices[4];
	glm::vec3 p5 = vertices[5];
	glm::vec3 p6 = vertices[6];
	glm::vec3 p7 = vertices[7];

	if (rampRiseDirection == POS_Z)
	{
		p4 = p6;
		p5 = p7;
	}
	else if (rampRiseDirection == NEG_Z)
	{
		p0 = p2;
		p1 = p3;
	}
	else if (rampRiseDirection == POS_X)
	{
		p0 = p2;
		p4 = p6;
	}
	else if (rampRiseDirection == NEG_X)
	{
		p5 = p7;
		p1 = p3;
	}

	temp.push_back({ p0, p2, p3, p1 });		// front
	temp.push_back({ p4, p0, p1, p5 });		// top
	temp.push_back({ p4, p6, p2, p0 });		// left 
	temp.push_back({ p2, p6, p7, p3 });		// bottom
	temp.push_back({ p1, p3, p7, p5 });		// right 
	temp.push_back({ p5, p7, p6, p4 });		// back 
	

	for (int i = 0; i < temp.size(); i++)
	{
		Face face = { temp[i] };
		result.push_back(face);
	}

	return result;
}





std::vector<Face> CreatePlaneMinMax(glm::vec3 min, glm::vec3 max)
{
	std::vector<Face> result;

	std::vector<glm::vec3> vertices = GetCubeVertices(min, max);

	// 4 points on front face 
	glm::vec3 p0 = vertices[0];
	glm::vec3 p1 = vertices[1];
	glm::vec3 p2 = vertices[2];
	glm::vec3 p3 = vertices[3];

	// 4 points on back face 
	glm::vec3 p4 = vertices[4];
	glm::vec3 p5 = vertices[5];
	glm::vec3 p6 = vertices[6];
	glm::vec3 p7 = vertices[7];


	std::vector<std::vector<glm::vec3>> temp;
	temp.push_back({ p0, p2, p3, p1 });		// front
	temp.push_back({ p4, p0, p1, p5 });		// top
	temp.push_back({ p4, p6, p2, p0 });		// left 
	temp.push_back({ p2, p6, p7, p3 });		// bottom
	temp.push_back({ p1, p3, p7, p5 });		// right 
	temp.push_back({ p5, p7, p6, p4 });		// back 

	for (int i = 0; i < temp.size(); i++)
	{
		Face face = { temp[i] };
		result.push_back(face);
	}

	return result;
}



std::vector<Face> CreateCubeFaceMinMax(glm::vec3 min, glm::vec3 max)
{
	std::vector<Face> result;

	std::vector<glm::vec3> vertices = GetCubeVertices(min, max);

	// 4 points on front face 
	glm::vec3 p0 = vertices[0];
	glm::vec3 p1 = vertices[1];
	glm::vec3 p2 = vertices[2];
	glm::vec3 p3 = vertices[3];

	// 4 points on back face 
	glm::vec3 p4 = vertices[4];
	glm::vec3 p5 = vertices[5];
	glm::vec3 p6 = vertices[6];
	glm::vec3 p7 = vertices[7];


	std::vector<std::vector<glm::vec3>> temp;
	temp.push_back({ p0, p2, p3, p1 });		// front
	temp.push_back({ p4, p0, p1, p5 });		// top
	temp.push_back({ p4, p6, p2, p0 });		// left 
	temp.push_back({ p2, p6, p7, p3 });		// bottom
	temp.push_back({ p1, p3, p7, p5 });		// right 
	temp.push_back({ p5, p7, p6, p4 });		// back 

	for (int i = 0; i < temp.size(); i++)
	{
		Face face = { temp[i] };
		result.push_back(face);
	}

	return result;
}


std::vector<Face> CreateCubeFaceCentered(glm::vec3 pos, glm::vec3 dim)
{
	glm::vec3 min = pos - dim;
	glm::vec3 max = pos + dim;
	return CreateCubeFaceMinMax(min, max);
}


void AddPolygonToBrush(Brush* brush, std::vector<glm::vec3> verticesIn)
{
	const int arraySize = verticesIn.size();
	glm::vec3* vertices = new glm::vec3[arraySize];

	for (int i = 0; i < arraySize; i++)
	{
		vertices[i] = verticesIn[i];
	}

	assert(arraySize == 4);
	// attemp to build normal in all dimensions
	glm::vec3 normal;
	for (int i = 0; i < 2; i++)
	{
		glm::vec3 v0 = vertices[i+1] - vertices[i];
		glm::vec3 v1 = vertices[i+2] - vertices[i+1];
		normal = glm::normalize(glm::cross(v0, v1));

		if (!isnan(normal.x) && !isnan(normal.y) && !isnan(normal.z))
		{
			break;
		}
	}


	if (!isnan(normal.x) && !isnan(normal.y) && !isnan(normal.z))
	{
		std::cout << "normal " << normal << std::endl;
		BspPolygon polygon(vertices, 4);
		float dist = glm::dot(normal, vertices[0]);
		polygon.plane = { normal, dist };
		brush->polygons.push_back(polygon);
		brush->used.push_back(false);
	}


}


Brush ConvertFaceToBrush(std::vector<Face> faces)
{
	Brush brush;

	for (int i = 0; i < faces.size(); i++)
	{
		AddPolygonToBrush(&brush, faces[i].vertices);
	}
	std::cout << std::endl;
	return brush;
}

/*
Brush CreateBrushForEntity(Entity* entity)
{
	Brush brush;
	glm::vec3 min = entity->pos - entity->dim;
	glm::vec3 max = entity->pos + entity->dim;

	std::vector<glm::vec3> vertices = GetCubeVertices(min, max);

	// 4 points on front face 
	glm::vec3 p0 = vertices[0];
	glm::vec3 p1 = vertices[1];
	glm::vec3 p2 = vertices[2];
	glm::vec3 p3 = vertices[3];

	// 4 points on back face 
	glm::vec3 p4 = vertices[4];
	glm::vec3 p5 = vertices[5];
	glm::vec3 p6 = vertices[6];
	glm::vec3 p7 = vertices[7];

	// counter clock wise
	// front
	glm::vec3 vertices0[4] = {p0, p2, p3, p1};
	AddPolygonToBrush(&brush, vertices0, glm::vec3(0, 0, 1));

	// top
	glm::vec3 vertices1[4] = { p4, p0, p1, p5 };
	AddPolygonToBrush(&brush, vertices1, glm::vec3(0, 1, 0));

	// left 
	glm::vec3 vertices2[4] = { p4, p6, p2, p0 };
	AddPolygonToBrush(&brush, vertices2, glm::vec3(-1, 0, 0));

	// bottom
	glm::vec3 vertices3[4] = { p2, p6, p7, p3 };
	AddPolygonToBrush(&brush, vertices3, glm::vec3(0, -1, 0));

	// right 
	glm::vec3 vertices4[4] = { p1, p3, p7, p5 };
	AddPolygonToBrush(&brush, vertices4, glm::vec3(1, 0, 0));

	// back
	glm::vec3 vertices5[4] = { p5, p7, p6, p4 };
	AddPolygonToBrush(&brush, vertices5, glm::vec3(0, 0, -1));

	brush.PrintDebug();

	return brush;
}
*/

void CreateAreaA(World* world, std::vector<Brush>& brushes)
{
	Entity* entity = NULL;
	std::vector<Face> faces;

	// lower level
	// Box
	entity = &world->entities[world->numEntities++];
	glm::vec3 pos;
	glm::vec3 dim;
	glm::vec3 min;
	glm::vec3 max;


	
	// plane 1
	entity = &world->entities[world->numEntities++];
	pos = glm::vec3(0, 0, 0);
	dim = glm::vec3(200, 1, 50);

	min = glm::vec3(-200, 0, -200);
	max = glm::vec3(200, 1, 0);

	faces = CreateCubeFaceMinMax(min, max);
	brushes.push_back(ConvertFaceToBrush(faces));
	initEntity(entity, pos, faces);

	// plane 1 wall 1
	entity = &world->entities[world->numEntities++];
	pos = glm::vec3(0, 0, 0);

	min = glm::vec3(-200, 0, 0);
	max = glm::vec3(200, 100, 1);

	faces = CreateCubeFaceMinMax(min, max);
	brushes.push_back(ConvertFaceToBrush(faces));
	initEntity(entity, pos, faces);


	// plane 1 wall 2
	entity = &world->entities[world->numEntities++];
	pos = glm::vec3(0, 0, 0);
	dim = glm::vec3(200, 1, 50);

	min = glm::vec3(-201, 0, -200);
	max = glm::vec3(-200, 100, 0);

	faces = CreateCubeFaceMinMax(min, max);
	brushes.push_back(ConvertFaceToBrush(faces));
	initEntity(entity, pos, faces);


	// plane 1 wall 3
	std::cout << "plane 1 wall 3" << std::endl;
	entity = &world->entities[world->numEntities++];
	pos = glm::vec3(0, 0, 0);
	dim = glm::vec3(200, 1, 50);

	min = glm::vec3(200, 0, -200);
	max = glm::vec3(201, 100, 0);

	faces = CreateCubeFaceMinMax(min, max);
	brushes.push_back(ConvertFaceToBrush(faces));
	initEntity(entity, pos, faces);



	// plane 1 wall 4
	std::cout << "plane 1 wall 4" << std::endl;
	entity = &world->entities[world->numEntities++];
	pos = glm::vec3(50, 0, 0);
	dim = glm::vec3(100, 1, 100);

	min = glm::vec3(-200, 0, -201);
	max = glm::vec3(-100, 100, -200);

	faces = CreateCubeFaceMinMax(min, max);
	brushes.push_back(ConvertFaceToBrush(faces));
	initEntity(entity, pos, faces);
	



	// plane 2
	std::cout << "plane 2" << std::endl;
	entity = &world->entities[world->numEntities++];
	pos = glm::vec3(50, 0, 0);
	dim = glm::vec3(100, 1, 100);

	min = glm::vec3(0, 0, -400);
	max = glm::vec3(200, 1, -200);

	faces = CreateCubeFaceMinMax(min, max);
	brushes.push_back(ConvertFaceToBrush(faces));
	initEntity(entity, pos, faces);
		

	// ramp, doing it as a hack
	std::cout << "ramp" << std::endl;
	entity = &world->entities[world->numEntities++];
	pos = glm::vec3(50, 0, 0);
	dim = glm::vec3(100, 1, 100);

	min = glm::vec3(-100, -50, -400);
	max = glm::vec3(0, 1, -200);

	faces = CreateRampMinMax(min, max, POS_Z);
	brushes.push_back(ConvertFaceToBrush(faces));
	initEntity(entity, pos, faces);

	
	// walls for the ramp
	std::cout << "walls for ramp" << std::endl;
	entity = &world->entities[world->numEntities++];
	pos = glm::vec3(50, 0, 0);
	dim = glm::vec3(100, 1, 100);

	min = glm::vec3(0, -50, -400);
	max = glm::vec3(1, 1, -200);

	faces = CreateCubeFaceMinMax(min, max);
	brushes.push_back(ConvertFaceToBrush(faces));
	initEntity(entity, pos, faces);


	// wall 3
	std::cout << "wall 3" << std::endl;
	entity = &world->entities[world->numEntities++];
	pos = glm::vec3(50, 0, 0);
	dim = glm::vec3(100, 1, 100);

	min = glm::vec3(0, -50, -401);
	max = glm::vec3(200, 0, -400);

	faces = CreateCubeFaceMinMax(min, max);
	brushes.push_back(ConvertFaceToBrush(faces));
	initEntity(entity, pos, faces);


	// plane 4
	std::cout << "plane 4" << std::endl;
	entity = &world->entities[world->numEntities++];
	pos = glm::vec3(50, 0, 0);
	dim = glm::vec3(100, 1, 100);

	min = glm::vec3(-100, -51, -600);
	max = glm::vec3(200, -50, -400);

	faces = CreateCubeFaceMinMax(min, max);
	brushes.push_back(ConvertFaceToBrush(faces));
	initEntity(entity, pos, faces);



	// plane 4 wall 4
	std::cout << "plane 4 wall 4" << std::endl;
	entity = &world->entities[world->numEntities++];
	pos = glm::vec3(50, 0, 0);
	dim = glm::vec3(100, 1, 100);

	min = glm::vec3(-101, -50, -600);
	max = glm::vec3(-100, 100, -200);

	faces = CreateCubeFaceMinMax(min, max);
	brushes.push_back(ConvertFaceToBrush(faces));
	initEntity(entity, pos, faces);


	// plane 4 wall 5
	std::cout << "plane 4 wall 5" << std::endl;
	entity = &world->entities[world->numEntities++];
	pos = glm::vec3(50, 0, 0);
	dim = glm::vec3(100, 1, 100);

	min = glm::vec3(200, -50, -600);
	max = glm::vec3(201, 100, -200);

	faces = CreateCubeFaceMinMax(min, max);
	brushes.push_back(ConvertFaceToBrush(faces));
	initEntity(entity, pos, faces);



	// plane 4 door 5
	std::cout << "plane 4 door 5" << std::endl;
	entity = &world->entities[world->numEntities++];
	pos = glm::vec3(50, 0, 0);
	min = glm::vec3(-100, -50, -601);
	max = glm::vec3(0, 100, -600);

	faces = CreateCubeFaceMinMax(min, max);
	brushes.push_back(ConvertFaceToBrush(faces));
	initEntity(entity, pos, faces);


	entity = &world->entities[world->numEntities++];
	pos = glm::vec3(50, 0, 0);
	min = glm::vec3(100, -50, -601);
	max = glm::vec3(200, 100, -600);

	faces = CreateCubeFaceMinMax(min, max);
	brushes.push_back(ConvertFaceToBrush(faces));
	initEntity(entity, pos, faces);

	
	entity = &world->entities[world->numEntities++];
	pos = glm::vec3(50, 0, 0);
	min = glm::vec3(0, 25, -601);
	max = glm::vec3(100, 100, -600);

	faces = CreateCubeFaceMinMax(min, max);
	brushes.push_back(ConvertFaceToBrush(faces));
	initEntity(entity, pos, faces);
	
}



/*
void CreateAreaB(World* world, glm::vec3 position, glm::vec3 dimensions, std::vector<Brush>& brushes)
{
	float wallHeight = dimensions.y;

	glm::vec3 siteASize = glm::vec3(dimensions.x, wallHeight, dimensions.z);
	Entity* entity = NULL;

	// middle triangle
	entity = &world->entities[world->numEntities++];
	initEntity(entity, position + glm::vec3(0, 0, 0), glm::vec3(5, 40, 5));
	brushes.push_back(CreateBrushForEntity(entity));

	// the ground and the walls
	entity = &world->entities[world->numEntities++];
	initEntity(entity, position + glm::vec3(0, 0, 0), glm::vec3(siteASize.x, 1, siteASize.z));
	brushes.push_back(CreateBrushForEntity(entity));

	entity = &world->entities[world->numEntities++];
	initEntity(entity, position + glm::vec3(siteASize.x, wallHeight / 2, 0), glm::vec3(1, siteASize.y, siteASize.z));
	brushes.push_back(CreateBrushForEntity(entity));

	entity = &world->entities[world->numEntities++];
	initEntity(entity, position + glm::vec3(-siteASize.x, wallHeight / 2, 0), glm::vec3(1, siteASize.y, siteASize.z));
	brushes.push_back(CreateBrushForEntity(entity));

	entity = &world->entities[world->numEntities++];
	initEntity(entity, position + glm::vec3(0, wallHeight / 2, siteASize.z), glm::vec3(siteASize.x, siteASize.y, 1));
	brushes.push_back(CreateBrushForEntity(entity));

	entity = &world->entities[world->numEntities++];
	initEntity(entity, position + glm::vec3(0, wallHeight / 2, -siteASize.z), glm::vec3(siteASize.x, siteASize.y, 1));
	brushes.push_back(CreateBrushForEntity(entity));

}
*/


// Essentially recreating a simplified version of dust2
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



	std::vector<Brush> brushes;


	float wallHeight = 50;

	CreateAreaA(world, brushes);

	// glm::vec3 siteBSize = glm::vec3(200, wallHeight, 200);
	//CreateAreaB(world, glm::vec3(500, 0, 0), siteBSize, brushes);
	

	/*
	glm::vec3 siteBSize = glm::vec3(200, wallHeight, 200);
	AddRoom(world, glm::vec3(250, 0, 250), siteBSize, brushes);
	*/

	/*
	Entity* entity = NULL;

	// middle triangle
	entity = &world->entities[world->numEntities++];
	initEntity(entity, glm::vec3(0, 0, 0), glm::vec3(5, 40, 5));
	brushes.push_back(CreateBrushForEntity(entity));


	// the ground and the walls
	entity = &world->entities[world->numEntities++];
	initEntity(entity, glm::vec3(0, 0, 0), glm::vec3(siteASize.x, 1, siteASize.z));
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
	*/



	std::cout << "############# BuildBSPTree" << std::endl;
	world->tree = BuildBSPTree(brushes, 0);

}