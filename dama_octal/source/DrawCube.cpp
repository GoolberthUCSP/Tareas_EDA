#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkUnsignedCharArray.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkNamedColors.h>
#include<vector>
#include<iostream>
#include<array>
#include<fstream>
#include<cmath>
#include<algorithm>

using namespace std;

  // The ordering of the corner points on each face.
  std::array<std::array<vtkIdType, 4>, 6> ordering = {{{{0, 3, 2, 1}},
                                                       {{4, 5, 6, 7}},
                                                       {{0, 1, 5, 4}},
                                                       {{1, 2, 6, 5}},
                                                       {{2, 3, 7, 6}},
                                                       {{3, 0, 4, 7}}}};

vtkNew<vtkActor> drawCube( double x, double y, double z, double h){
	std::array<std::array<double, 3>, 8> pts;
	pts[0] = {x,y,z};       //0,0,0
	pts[1] = {x+h, y, z };  //1,0,0
	pts[2] = {x+h, y+h, z}; //1,1,0
	pts[3] = {x, y+h, z};   //0,1,0
	pts[4] = {x, y, z+h};   //0,0,1
	pts[5] = {x+h,y,z+h};   //1,0,1
	pts[6] = {x+h,y+h,z+h}; //1,1,1
	pts[7] = {x,y+h,z+h};   //0,1,1

	vtkNew<vtkPolyData> cube;
	vtkNew<vtkPoints> points;
	vtkNew<vtkCellArray> polys;
	vtkNew<vtkFloatArray> scalars;

	for (auto i = 0ul; i < pts.size(); ++i) {
		points->InsertPoint(i, pts[i].data());
		scalars->InsertTuple1(i, i);
	}
	for (auto&& i : ordering) {
		polys->InsertNextCell(vtkIdType(i.size()), i.data());
	}

	// We now assign the pieces to the vtkPolyData.
	cube->SetPoints(points);
	cube->SetPolys(polys);
	cube->GetPointData()->SetScalars(scalars);

	// Now we'll look at it.
	vtkNew<vtkPolyDataMapper> cubeMapper;
	cubeMapper->SetInputData(cube);
	cubeMapper->SetScalarRange(cube->GetScalarRange());
	vtkNew<vtkActor> cubeActor;
	cubeActor->SetMapper(cubeMapper);
	cubeActor->GetProperty()->SetRepresentationToWireframe();

	return cubeActor;
}

vtkNew<vtkActor> drawPoint(double x, double y, double z) {
	vtkNew<vtkNamedColors> namedColors;

	vtkNew<vtkPoints> points;
	points->InsertNextPoint(x, y, z);

	vtkNew<vtkPolyData> pointsPolydata;
	pointsPolydata->SetPoints(points);


	vtkNew<vtkPolyData> polyData;

    polyData->SetPoints(points);

    vtkNew<vtkVertexGlyphFilter> glyphFilter;
    glyphFilter->SetInputData(polyData);
    glyphFilter->Update();
	// Visualization
	vtkNew<vtkPolyDataMapper> mapper;
	mapper->SetInputConnection(glyphFilter->GetOutputPort());

	vtkNew<vtkActor> actor;
	actor->SetMapper(mapper);
	actor->GetProperty()->SetPointSize(10);
	actor->GetProperty()->SetColor(namedColors->GetColor3d("Olive").GetData());

	return actor;
}
///////////////////////////Octree///////////////////////////
struct Point {
    double x, y, z;
    Point(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}
    Point operator - (Point p) const {
        return Point(x - p.x, y - p.y, z - p.z);
    }
    Point operator + (Point p) const {
        return Point(x + p.x, y + p.y, z + p.z);
    }
    bool operator == (Point p) {
        return this->x == p.x && this->y == p.y && this->z == p.z;
    }
    double max_coord() {
        double x_abs = abs(x);
        double y_abs = abs(y);
        double z_abs = abs(z);
        return max(max(x_abs, y_abs), z_abs);
    }
    double norm() {
        return sqrt(x * x + y * y + z * z);
    }
};

Point getMinPoint(Point &pointA, Point pointB) {
    double x = pointA.x < pointB.x ? pointA.x : pointB.x;
    double y = pointA.y < pointB.y ? pointA.y : pointB.y;
    double z = pointA.z < pointB.z ? pointA.z : pointB.z;
    return Point(x, y, z);
}

Point getMaxPoint(Point &pointA, Point pointB){
    double x = pointA.x > pointB.x ? pointA.x : pointB.x;
    double y = pointA.y > pointB.y ? pointA.y : pointB.y;
    double z = pointA.z > pointB.z ? pointA.z : pointB.z;
    return Point(x, y, z);
}

struct Octree {
    Point *points;
    int K; //K granularidad
    int nPoints;
    Point leftBottom, rightTop;
    double h;
    bool isLeaf; //si es hoja
    Octree* children[8]{ NULL };
    Octree(int K) { 
        this->K = K;
        this->nPoints = 0;
        points = new Point[K];
        isLeaf = true;
    }

    void insert(Point &point) {
        if( isLeaf && nPoints == 0 ){
            points[nPoints++] = point;
            leftBottom = point;
            rightTop = point;
            h = 0;
        }
        else if (isLeaf && nPoints < K ) {
            points[nPoints++] = point;
            leftBottom = getMinPoint(point, leftBottom);
            rightTop = getMaxPoint(point, rightTop);
            h = (rightTop - leftBottom).max_coord();
        }
        else if (isLeaf && nPoints == K){
            isLeaf=false;
            for (int i = 0; i < 8; i++)
                children[i] = new Octree(K);
            // buscar el hijo que contiene el punto y repartir los puntos
            for (int i = 0; i < nPoints; i++)
            {
                insertTypeOctree(points[i]);
            }
            insertTypeOctree(point);
        }
        else {
            insertTypeOctree(point);
        }
    }

    void insertTypeOctree(Point &point) {
        Point mid= leftBottom + Point(h/2,h/2,h/2);
        if (point.x < mid.x) {
            if (point.y < mid.y) {
                if (point.z < mid.z) {
                    children[0]->insert(point);
                }
                else {
                    children[1]->insert(point);
                }
            }
            else {
                if (point.z < mid.z) {
                    children[2]->insert(point);
                }
                else {
                    children[3]->insert(point);
                }
            }
        }
        else {
            if (point.y < mid.y) {
                if (point.z < mid.z) {
                    children[4]->insert(point);
                }
                else {
                    children[5]->insert(point);
                }
            }
            else {
                if (point.z < mid.z) {
                    children[6]->insert(point);
                }
                else {
                    children[7]->insert(point);
                }
            }
        }
    }

    bool exist(Point &point) {
        if( isLeaf ){
            for (int i = 0; i < nPoints; i++)
            {
                if( points[i] == point )
                    return true;   
            }
            return false;
        } else {
            Point mid= leftBottom + Point(h/2,h/2,h/2);
            if (point.x < mid.x) {
                if (point.y < mid.y) {
                    if (point.z < mid.z) {
                        return children[0]->exist(point);
                    }
                    else {
                        return children[1]->exist(point);
                    }
                }
                else {
                    if (point.z < mid.z) {
                        return children[2]->exist(point);
                    }
                    else {
                        return children[3]->exist(point);
                    }
                }
            }
            else {
                if (point.y < mid.y) {
                    if (point.z < mid.z) {
                        return children[4]->exist(point);
                    }
                    else {
                        return children[5]->exist(point);
                    }
                }
                else {
                    if (point.z < mid.z) {
                        return children[6]->exist(point);
                    }
                    else {
                        return children[7]->exist(point);
                    }
                }
            }
        }
    }

    Point find_closest(Point &point){
        if (isLeaf) {
            if (nPoints == 0)
                return Point(0, 0, 0);
            Point closest = points[0];
            double minDist = (closest - point).norm();
            for (int i = 1; i < nPoints; i++)
            {
                double dist = (points[i] - point).norm();
                if (dist < minDist) {
                    minDist = dist;
                    closest = points[i];
                }
            }
            return closest;
        }
        else {
            Point mid = leftBottom + Point(h / 2, h / 2, h / 2);
            if (point.x < mid.x) {
                if (point.y < mid.y) {
                    if (point.z < mid.z) {
                        return children[0]->find_closest(point);
                    }
                    else {
                        return children[1]->find_closest(point);
                    }
                }
                else {
                    if (point.z < mid.z) {
                        return children[2]->find_closest(point);
                    }
                    else {
                        return children[3]->find_closest(point);
                    }
                }
            }
            else {
                if (point.y < mid.y) {
                    if (point.z < mid.z) {
                        return children[4]->find_closest(point);
                    }
                    else {
                        return children[5]->find_closest(point);
                    }
                }
                else {
                    if (point.z < mid.z) {
                        return children[6]->find_closest(point);
                    }
                    else {
                        return children[7]->find_closest(point);
                    }
                }
            }
        }
    }
};

struct Cube{
	Point leftBottom;
	double h;
	Cube(Point leftBottom, double h) : leftBottom(leftBottom), h(h) {}
};

void loadFile(Octree *octree) {
    double x, y, z;
    ifstream file("../dama_octal.txt");
    while (!file.eof()) {
        file >> x >> y >> z;
        Point a(x, y, z);
        octree->insert(a);
    }
}

void ocToCube(Octree *octree, vector<Cube> &cubes) {
    if (octree->isLeaf) {
        if (octree->nPoints > 1) {
            cubes.push_back(Cube(octree->leftBottom, octree->h));
        }
    }
    else {
        for (int i = 0; i < 8; i++)
        {
            ocToCube(octree->children[i], cubes);
        }
    }
}

int main(int argc, char **argv)
{
	Octree *octree = new Octree(100);
	loadFile(octree);
	vector<Cube> cubes;
	ocToCube(octree, cubes);

	vtkNew<vtkNamedColors> colors;

	vtkNew<vtkCamera> camera;
	camera->SetPosition(1, 1, 1);
	camera->SetFocalPoint(0, 0, 0);
    //Create renderer
	vtkNew<vtkRenderer> renderer;
	vtkNew<vtkRenderWindow> renWin;
	renWin->AddRenderer(renderer);
	renWin->SetWindowName("Octree");

    for (auto i:cubes){
        vtkNew<vtkActor> cube= drawCube(i.leftBottom.x, i.leftBottom.y, i.leftBottom.z, i.h);
        renderer->AddActor(cube);
    }

	vtkNew<vtkRenderWindowInteractor> iren;
	iren->SetRenderWindow(renWin);

	renderer->SetActiveCamera(camera);
	renderer->ResetCamera();
	renderer->SetBackground(colors->GetColor3d("Cornsilk").GetData());
	renWin->SetSize(600, 600);
	
	renWin->Render();
	iren->Start();

	return EXIT_SUCCESS;
}