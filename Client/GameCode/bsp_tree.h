#pragma once

#include "../PlatformShared/platform_shared.h"

#include <vector>

struct Plane
{
	glm::vec3 normal;
	float distance;
};

struct Edge
{
	glm::vec3 vertices[2];
};

/*
// Arrarize everything
struct BSPFace
{
//	uint16 planeIndex;		
//	uint16 planeNormalSide;

	Plane plane;
	std::vector<Edge> edges;
};


struct BSPLeaf
{
	glm::vec3 bboxMin;
	glm::vec3 bboxMax;

	std::vector<BSPFace> faces;
};
*/

struct BspPolygon
{
	std::vector<glm::vec3> vertices;
	Plane plane;

	BspPolygon() {}
	BspPolygon(glm::vec3* frontVerts, int numVerts)
	{
		for (int i = 0; i < numVerts; i++)
		{
			vertices.push_back(frontVerts[i]);
		}
	}
};

struct Brush
{
	std::vector<BspPolygon> polygons;
};


struct BSPNode
{
	// node only
	Plane splitPlane;
//	std::vector<BSPFace> faces;
	BSPNode* frontChild;
	BSPNode* backChild;

	/*
	union
	{
		BSPNode* frontChild;
		BSPLeaf* frontLeaf;
	};
	union
	{
		BSPNode* backChild;
		BSPLeaf* backLeaf;
	};
	*/


	// leafs only
	std::vector<Brush> brushes;

	glm::vec3 bboxMin;
	glm::vec3 bboxMax;

	BSPNode() {}

	BSPNode(BSPNode* frontTree, BSPNode* backTree)
	{
		frontChild = frontTree;
		backChild = backTree;
	}
};





enum SplittingPlaneResult
{
	NONE,
	POLYGON_FRONT,
	POLYGON_BACK,
	POLYGON_BOTH,
	POLYGON_COPLANNAR,	// all vertices is coplanar with the plane

	POINT_FRONT,
	POINT_BACK,
	POINT_ON_PLANE,
};

SplittingPlaneResult ClassifyPointToPlane(glm::vec3 point, Plane splittingPlane)
{
	SplittingPlaneResult result = SplittingPlaneResult::NONE;
	return result;
}

SplittingPlaneResult ClassifyPolygonToPlane(BspPolygon* polygon, Plane splittingPlane)
{
	int numInFront = 0, numBehind = 0;

	for (int i = 0; i < polygon->vertices.size(); i++)
	{
		glm::vec3 v = polygon->vertices[i];
		switch (ClassifyPointToPlane(v, splittingPlane))
		{
			case SplittingPlaneResult::POINT_FRONT:
				numInFront++;
				break;
			case SplittingPlaneResult::POINT_BACK:
				numBehind++;
				break;
		}
	}

	if (numBehind != 0 && numInFront != 0)
		return SplittingPlaneResult::POLYGON_BOTH;
	if (numInFront != 0)
		return SplittingPlaneResult::POLYGON_FRONT;
	if (numBehind != 0 )
		return SplittingPlaneResult::POLYGON_BACK;

	return SplittingPlaneResult::POLYGON_COPLANNAR;
}


float EvaluateSplittingPlane(Plane splittingPlane, std::vector<Brush>& brushes)
{
	int numInFront = 0, numInBack = 0, numInBoth = 0;
	for (int i = 0; i < brushes.size(); i++)
	{
		for (int j = 0; j < brushes[i].polygons.size(); j++)
		{
			SplittingPlaneResult result = ClassifyPolygonToPlane(&brushes[i].polygons[j], splittingPlane);
			switch (result)
			{
				case SplittingPlaneResult::POLYGON_COPLANNAR:	// coplanar polygons treated as being in front of plane
				case SplittingPlaneResult::POLYGON_FRONT:
					numInFront++;
					break;
				case SplittingPlaneResult::POLYGON_BACK:
					numInBack++;
					break;
				case SplittingPlaneResult::POLYGON_BOTH:
					numInBoth++;
					break;
			}
		}
	}

	// pick planes so as to minimize splitting of geometry and to attempt
	// to balance the geometry equall on both sides of the splitting plane. 
	float score = numInBoth + abs(numInFront - numInBack);
	return score;
}


// pick planes so as to minimize splitting of geometry and to attempt
// to balance the geometry equall on both sides of the splitting plane. 
Plane PickSplittingPlane(std::vector<Brush>& brushes)
{
	const float k = 0;

	Plane bestPlane;
	float bestScore = FLT_MAX;

	for (int i = 0; i < brushes.size(); i++)
	{
		std::vector<BspPolygon>& polygons = brushes[i].polygons;
		for (int j = 0; j < polygons.size(); j++)
		{
			float score = EvaluateSplittingPlane(polygons[j].plane, brushes);

			if (score < bestScore)
			{
				bestScore = score;
				bestPlane = polygons[j].plane;
			}
		}
	}
	return bestPlane;
}


// Page 175 IntersectSegmentPlane
bool IntersectEdgeAgainstPlane(glm::vec3 p0, glm::vec3 p1, Plane plane, glm::vec3& q)
{
	float t = 0;
	glm::vec3 vec1_0 = p1 - p0;

	t = (plane.distance - glm::dot(plane.normal, p0)) / glm::dot(plane.normal, vec1_0);

	if (0.0f <= t && t <= 1.0f)
	{
		q = p0 + t * vec1_0;
		return true;
	}

	return false;
}

void SplitPolygon(BspPolygon& polygon, Plane plane, BspPolygon* frontPoly, BspPolygon* backPoly)
{
	int numFront = 0, numBack = 0;
	const int MAX_POINTS = 1024;

	glm::vec3 frontVerts[MAX_POINTS], backVerts[MAX_POINTS];

	int numVerts = polygon.vertices.size();

	// vector from v0 -----> v1
	glm::vec3 v0 = polygon.vertices[numVerts - 1];
	SplittingPlaneResult v0Side = ClassifyPointToPlane(v0, plane);

	for (int i = 0; i < numVerts; i++)
	{
		glm::vec3 v1 = polygon.vertices[i];

		SplittingPlaneResult v1Side = ClassifyPointToPlane(v1, plane);

		if (v1Side == SplittingPlaneResult::POINT_FRONT)
		{
			if (v0Side == SplittingPlaneResult::POINT_BACK)
			{
				glm::vec3 intersectionPoint;
				IntersectEdgeAgainstPlane(v0, v1, plane, intersectionPoint);
				assert(ClassifyPointToPlane(intersectionPoint, plane) == SplittingPlaneResult::POINT_ON_PLANE);
				frontVerts[numFront++] = backVerts[numBack++] = intersectionPoint;
			}
			frontVerts[numFront++] = v1;
		}
		else if (v1Side == SplittingPlaneResult::POINT_BACK)
		{
			if (v0Side == SplittingPlaneResult::POINT_FRONT)
			{
				glm::vec3 intersectionPoint;
				IntersectEdgeAgainstPlane(v0, v1, plane, intersectionPoint);
				assert(ClassifyPointToPlane(intersectionPoint, plane) == SplittingPlaneResult::POINT_ON_PLANE);
				frontVerts[numFront++] = backVerts[numBack++] = intersectionPoint;
			}
			else if (v0Side == SplittingPlaneResult::POINT_ON_PLANE)
			{
				backVerts[numBack++] = v0;
			}

			backVerts[numBack++] = v1;
		}
		else
		{
			frontVerts[numFront++] = v1;
			if (v0Side == SplittingPlaneResult::POINT_BACK)
			{
				backVerts[numBack++] = v1;
			}
		}
		v0 = v1;
		v0Side = v1Side;
	}

	frontPoly = new BspPolygon(frontVerts, numFront);
	backPoly = new BspPolygon(backVerts, numBack);
}

// test case: https://www.bluesnews.com/abrash/chap64.shtml

BSPNode* BuildBSPTree(std::vector<Brush> brushes, int depth)
{
	const int MAX_DEPTH = 10;
	const int MIN_LEAF_SIZE = 20;

	if (brushes.empty())
		return NULL;

//	int numPolygons = polygons.size();

	/*
	// Reconsider this
	if (depth >= MAX_DEPTH || numPolygons <= MIN_LEAF_SIZE)
	{
		return new BSPNode(polygons);
	}
	*/

	if (depth == 2)
	{
		BSPNode* node = new BSPNode();
		node->brushes = brushes;
		return node;
	}

	Plane splitPlane = PickSplittingPlane(brushes);

//	std::vector<BspPolygon*> frontList, backList;

	std::vector<Brush> frontBrushes, backBrushes;


	for (int i = 0; i < brushes.size(); i++)
	{
		Brush frontBrush, backBrush;
		for (int j = 0; j < brushes[i].polygons.size(); j++)
		{
			BspPolygon* polygon = &brushes[i].polygons[j];
			BspPolygon* frontPart = NULL;
			BspPolygon* backPart = NULL;

			SplittingPlaneResult result = ClassifyPolygonToPlane(polygon, splitPlane);

			switch (result)
			{
				case SplittingPlaneResult::POLYGON_FRONT:
					frontBrush.polygons.push_back(*polygon);
					break;
				case SplittingPlaneResult::POLYGON_BACK:
					backBrush.polygons.push_back(*polygon);
					break;
				case SplittingPlaneResult::POLYGON_BOTH:
					SplitPolygon(*polygon, splitPlane, frontPart, backPart);
					frontBrush.polygons.push_back(*frontPart);
					backBrush.polygons.push_back(*backPart);
					break;
			}
		}
		frontBrushes.push_back(frontBrush);
		backBrushes.push_back(backBrush);
	}

	BSPNode* frontTree = BuildBSPTree(frontBrushes, depth + 1);
	BSPNode* backTree = BuildBSPTree(backBrushes, depth + 1);
	return new BSPNode(frontTree, backTree);
}