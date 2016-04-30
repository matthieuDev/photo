#ifndef POINTC
#define POINTC
#include <math.h>
class Point
{
public:
	Point() {};
	Point(int xx, int yy) { x = xx; y = yy; };
	Point minus(Point p){ return Point(x - p.x, y - p.y); };
	float ll(){ return sqrt((double)(x*x + y*y)); };

	int x, y;
};

class Point3D {
public:
	Point3D() {};
	Point3D(float xx, float yy, float ww){ x = xx; y = yy; w = ww; };
	Point3D cross(Point3D p) { return Point3D((p.y - y) / w, (x - p.x) / w, (p.x*y - p.y*x) / (w*w)); };
	Point3D minus(Point3D p){ return Point3D(x - p.x, y - p.y, w - p.w); };
	bool equals(Point3D p){ return (x == p.x && y == p.y && w == p.w); };
	float ll(){ return sqrt((double)(x*x + y*y + w*w)); };
	float dist(Point3D p){ return minus(p).ll(); };
	float time(Point3D p){ return x*p.x + y*p.y + w*p.w; };
	
	float x, y, w;
};

class doublePoint {
public:
	doublePoint(){};
	doublePoint(Point pp){ p2d = pp; p3d = Point3D(0, 0, 0); };
	doublePoint(Point p2, Point3D p3){ p2d = p2; p3d = p3; };

	Point p2d;
	Point3D p3d;
};


#endif