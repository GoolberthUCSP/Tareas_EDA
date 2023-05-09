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
#include<string>
#include<vector>

using namespace std;

float min(float a, float b){
    if (a < b){
        return a;
    }
    return b;
}
float max(float a, float b){
    if (a > b){
        return a;
    }
    return b;
}

template<int ndim>
struct Point{
    vector<float>coords;
    Point(void){
        this->coords.reserve(ndim);
        for (int i = 0; i < ndim; i++){
            coords.push_back(0);
        }
    }
    Point(float coords[ndim]){
        this->coords.reserve(ndim);
        for (int i = 0; i < ndim; i++){
            this->coords.push_back(coords[i]);
        }
    }
    Point(vector<float> coords){
        this->coords.reserve(ndim);
        for (int i = 0; i < ndim; i++){
            this->coords.push_back(coords[i]);
        }
    }
    //El operador [] está sobrecargado en el objeto vector
    float& operator[](int index){ return coords[index]; }
    ~Point(void){ delete &coords; }
    //getCoords retorna el punto con las coordenadas maximas o minimas de ambos puntos
    Point<ndim> getCoords(Point<ndim> other, float function(float, float)){
        float tmp[ndim];
        for (int i = 0; i < ndim; i++){
            tmp[i] = function(this->coords[i], other[i]);
        }
        return Point<ndim>(tmp);
    }
};

//id name attack defense speed height hp weight
class Pokemon{
    int id;
    string name;
    float height, hp, weight;
public:
    Pokemon(int id, string name, float height, float hp, float weight) : id(id), name(name), height(height), hp(hp), weight(weight) {}
    float getId(){ return id; }
    float getHeight(){ return height; }
    float getHp(){ return hp; }
    float getWeight(){ return weight; }
};

template< typename T, int ndim >
class Record {
private:
    T object; //aqui el pokemon (aqui está su tamaño)
    Point<ndim> coords; //coordenada donde está el pokemon
public:
    Record(T object, Point<ndim> coords) : object(object), coords(coords) {}
    T getObject(){ return object; }
    Point<ndim> getTuple(){ return this->coords; }
};

template< typename T, int ndim >
class Node { //Region //Rectangle //MBR
private:
    vector< Record<T,ndim> > records;
    vector< Node<T,ndim>* > children;
    //bottomLeft tiene las coordenadas minimas de todos los records
    //upperRight tiene las coordenadas maximas de todos los records
    Point<ndim> bottomLeft;
    Point<ndim> upperRight; 
    int maxRecords;
    int countRecords;
    bool isLeaf;  

public:
    Node(int maxRecords) : maxRecords(maxRecords), countRecords(0), isLeaf(true) {}
    bool insert(Record<T,ndim>  record){
        if (isLeaf){
			records.push_back(record);
            if (countRecords == 0){
                countRecords++;
                bottomLeft = upperRight = record.getTuple();
                return true;
            }
            else if (countRecords < maxRecords){
                countRecords++;
                bottomLeft = bottomLeft.getCoords(record.getTuple(), min);
                upperRight = upperRight.getCoords(record.getTuple(), max);
                return true;
            }
            else{
				bottomLeft = bottomLeft.getCoords(record.getTuple(), min);
                upperRight = upperRight.getCoords(record.getTuple(), max);
                return false;
            }
        }
        return insertToBest(record);
    }

    //insertToBest inserta en el nodo que se expanda menos de la lista de nodos pasados por parametro
    //si hay empate, inserta en el que tenga menor area
    bool insertToBest(Record<T,ndim>  record){
        float minExpansion = children[0]->getExpansion(record);
        int index = 0;
        for (int i = 1; i < children.size(); i++){
            float expansion = children[i]->getExpansion(record);
            if (expansion < minExpansion){
                minExpansion = expansion;
                index = i;
            }
            else if (expansion == minExpansion){
                if (children[i]->getArea() < children[index]->getArea()){
                    index = i;
                }
            }
        }
        return children[index]->insert(record);
    }

    int getCountRecords(){
        return countRecords;
    }

    int getCountChildren(){
        return children.size();
    }

    //getExpansion retorna el area que se expandiria si se insertara el record
    float getExpansion(Record<T,ndim>  record){
        float expansion = 1;
        Point<ndim> botLeft= bottomLeft.getCoords(record.getTuple(), min);
        Point<ndim> upRight = upperRight.getCoords(record.getTuple(), max);
        for (int i = 0; i < ndim; i++){
            expansion *= (upRight[i]-botLeft[i]);
        }
        return expansion - getArea();
    }

    float getArea(){
        float area = 1;
        for (int i = 0; i < ndim; i++){
            area *= (upperRight[i] - bottomLeft[i]);
        }
        return area;
    }

    //insert utilizado principalmente por RTree
    bool insert(Node<T,ndim>* node1, Node<T,ndim>* node2= NULL){
        //Insertar los nodos en el nodo padre
        isLeaf= false;
		children.push_back(node1);
        if (node2 != NULL){
            children.push_back(node2);
        }
        return true;
    }

    pair< Node<T,ndim>*, Node<T,ndim>*> split(){
        pair< Node<T,ndim>*, Node<T,ndim>*> nodes;
        nodes.first = new Node<T,ndim>(maxRecords);
        nodes.second = new Node<T,ndim>(maxRecords);
        //Insertar a los más cercanos a los extremos de los nodos bottomLeft y upperRight
        //Si un nodo alcanza la mitad de los records, insertar el resto en el otro nodo
        for (auto record: records){
            if (nodes.first->countRecords < maxRecords/2 && nodes.second->countRecords < maxRecords/2){
                if (nodes.first->getExpansion(record) < nodes.second->getExpansion(record)){
                    nodes.first->insert(record);
                }
                else{
                    nodes.second->insert(record);
                }
            }
			else if (nodes.first->countRecords < maxRecords/2){
                nodes.first->insert(record);
            }
            else (){
                nodes.second->insert(record);
            }
        }
        return nodes;
    }
};

template< typename T, int ndim >
class RTree {
private:
    Node<T,ndim> *root;
    int maxRecords;
public:
    RTree(int maxRecords) : root(new Node<T,ndim>(maxRecords)), maxRecords(maxRecords) {}

    bool insert( Record<T,ndim> record ){
        //Si el nodo se divide, se crea un nuevo nodo y se inserta en el root
        if (!root->insert(record)){
            pair< Node<T,ndim>*, Node<T,ndim>*> tmp= root->split();
			root = new Node<T,ndim>(maxRecords);
            root->insert(tmp.first, tmp.second);
            return root->insertToBest(record);
        }
        return true;
    }
	
    int getNumberOfNodes(){
        return root->getCountChildren();
    }
};

  // The ordering of the corner points on each face.
  std::array<std::array<vtkIdType, 4>, 6> ordering = {{{{0, 3, 2, 1}},
                                                       {{4, 5, 6, 7}},
                                                       {{0, 1, 5, 4}},
                                                       {{1, 2, 6, 5}},
                                                       {{2, 3, 7, 6}},
                                                       {{3, 0, 4, 7}}}};

vtkNew<vtkActor> drawRegion( double x, double y, double z, double hx, double hy, double hz){
	std::array<std::array<double, 3>, 8> pts;
	pts[0] = {x,y,z};       //0,0,0
	pts[1] = {x+hx, y, z };  //1,0,0
	pts[2] = {x+hx, y+hy, z}; //1,1,0
	pts[3] = {x, y+hy, z};   //0,1,0
	pts[4] = {x, y, z+hz};   //0,0,1
	pts[5] = {x+hx,y,z+hz};   //1,0,1
	pts[6] = {x+hx,y+hy,z+hz}; //1,1,1
	pts[7] = {x,y+hy,z+hz};   //0,1,1

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


int main(int argc, char **argv)
{
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkActor> cubeActor1 = drawRegion(0, 0, 0, 
		                           1, 1, 1);
  vtkNew<vtkActor> cubeActor2 = drawRegion(1, 1, 1,
		                           2, 8, 2);

  // The usual rendering stuff.
  vtkNew<vtkCamera> camera;
  camera->SetPosition(1, 1, 1);
  camera->SetFocalPoint(0, 0, 0);

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(renderer);
  renWin->SetWindowName("Region");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  renderer->AddActor(cubeActor1);
  renderer->AddActor(cubeActor2);


  renderer->AddActor(drawPoint(0.5,0.5,0.5));
  renderer->SetActiveCamera(camera);
  renderer->ResetCamera();
  renderer->SetBackground(colors->GetColor3d("Cornsilk").GetData());

  renWin->SetSize(600, 600);

  // interact with data
  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}
