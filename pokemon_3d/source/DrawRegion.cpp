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

#include <vtkTexture.h>
#include <vtkImageViewer.h>
#include <vtkJPEGReader.h>
#include <vtkPNGReader.h>
#include <vtkRegressionTestImage.h>
#include <vtkSmartPointer.h>
#include <vtkPlaneSource.h>

#include<string>
#include<vector>
#include<cmath>
#include<fstream>
#include<sstream>
#include<string>

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
    float coords[ndim]{0};
    Point(void){}
    Point(float coords_[ndim]){
        for (int i = 0; i < ndim; i++){
            coords[i] = coords_[i];
        }
    }

    //El operador [] está sobrecargado en el objeto vector
    float& operator[](int index){ return coords[index]; }
    
    Point<ndim> getCoords(Point<ndim> other, float function(float, float)){
        //getCoords retorna el punto con las coordenadas maximas o minimas de ambos puntos
        float coords_[ndim];
        for (int i = 0; i < ndim; i++){
            coords_[i] = function(coords[i], other[i]);
        }
        return Point<ndim>(coords_);
    }
    float euclideanDistance(Point<ndim> &point){
        float distance= 0;
        for (int i=0; i<ndim; i++){
            distance+= pow(coords[i]-point[i], 2);
        }
        return sqrt(distance);
    }

    friend ostream& operator<<(ostream &os, Point<ndim> &point){
        os << "(";
        for (int i = 0; i < ndim; i++){
            os << point[i];
            if (i != ndim-1){
                os << ", ";
            }
        }
        os << ")";
        return os;
    }

};

//id name attack defense speed height hp weight
class Pokemon{
    int id;
    string name;
    float height, hp, weight;
public:
    Pokemon(int id, string name, float height, float hp, float weight) : id(id), name(name), height(height), hp(hp), weight(weight) {}
    int getId(){ return id; }
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
    T &getObject(){ return object; }
    Point<ndim> &getPoint(){ return this->coords; }
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
    int nRecords;
    bool isLeaf;
    //Puntero al padre
    Node<T,ndim>* parent;

public:
    Node(int maxRecords_, Node<T,ndim>* parent_=NULL, bool isLeaf_=true) : maxRecords(maxRecords_), nRecords(0), isLeaf(isLeaf_), parent(parent_) {}
    bool insert(Record<T,ndim>  &record){
        //Inserta un record en el nodo hoja, si éste necesita dividirse, retorna false.
        records.push_back(record);
        nRecords++;
        if (nRecords == 1){
            bottomLeft = record.getPoint();
            upperRight = record.getPoint();
            return true;
        }
        else{
            bottomLeft = bottomLeft.getCoords(record.getPoint(), min);
            upperRight = upperRight.getCoords(record.getPoint(), max);
            return nRecords <= maxRecords;
        }
    }

    Node<T, ndim>* chooseLeaf(Record<T,ndim> &record){
        //Retorna el mejor nodo para insertar, si es hoja retorna el mismo nodo
        //Si no es hoja, llama recursivamente a chooseLeaf en el mejor hijo
        if (isLeaf){
            return this;
        }
        //Actualizar el rango del mejor hijo para insertar
        float minExpansion = children[0]->getExpansion(record);
        int bestIndex = 0;
        for (int i = 1; i < (int)children.size(); i++){
            float expansion = children[i]->getExpansion(record);
            if (expansion < minExpansion){
                minExpansion = expansion;
                bestIndex = i;
            }
            else if (expansion == minExpansion){
                if (children[i]->getSpace() < children[bestIndex]->getSpace()){
                    bestIndex = i;
                }
            }
        }
        children[bestIndex]->getBottomLeft() = children[bestIndex]->getBottomLeft().getCoords(record.getPoint(), min);
        children[bestIndex]->getUpperRight() = children[bestIndex]->getUpperRight().getCoords(record.getPoint(), max);
        return children[bestIndex]->chooseLeaf(record);
    }

    Node<T,ndim>* adjustTree(){
        /*
        - Si el nodo se divide, se crea un nuevo nodo y se inserta en el padre
        - Si el nodo actual necesita dividirse, se llama a splitXxxx
        - Si el nodo es hoja, se llama a splitLeaf, si no, se llama a split
        - Si el nodo actual es la raiz y necesita dividirse, se crea un nuevo nodo y se inserta en el padre, luego se retorna la nueva raiz
        El root llamará a chooseLeaf, luego insertará el record en el nodo hoja retornado por chooseLeaf
        Luego si el nodo hoja se divide, se llamará a adjustTree en el padre del nodo hoja
        */
        if ( parent == NULL ){ 
            //Si es la raiz y necesita dividirse, se crea un nuevo nodo y se inserta en el padre, luego se retorna la nueva raiz, sino se retorna el mismo nodo
            if ( isLeaf ){
                if (nRecords > maxRecords){
                    Node<T,ndim>* node = splitLeaf();
                    parent = new Node<T,ndim>(maxRecords, NULL, false);
                    parent->insert(node);
                    parent->insert(this);
                    return parent;
                }
                return this;
            }
            else{
                if ( (int)children.size() > maxRecords ){
                    Node<T,ndim>* node = split();
                    parent = new Node<T,ndim>(maxRecords, NULL, false);
                    parent->insert(node);
                    parent->insert(this);
                    return parent;
                }
                return this;
            }
        }
        else{
            if ( isLeaf ){
                //Nodo hoja que no es la raiz, solo se llama a esta función si necesita dividirse
                //No es necesario saber si necesita dividirse
                Node<T,ndim>* node = splitLeaf();
                parent->insert(node);
            }
            else{//Nodo intermedio
                if ( (int)children.size() > maxRecords ){
                    //Si necesita dividirse, se llama a split
                    Node<T,ndim>* node = split();
                    parent->insert(node);
                }
            }
            return parent->adjustTree();
        }
    }

    Point<ndim> getPoint(){
        //Retorna el punto medio del nodo
        float coords[ndim];
        for (int i = 0; i < ndim; i++){
            coords[i] = (bottomLeft[i] + upperRight[i])/2;
        }
        return Point<ndim>(coords);
    }

    Point<ndim> &getBottomLeft(){ return bottomLeft; }
    Point<ndim> &getUpperRight(){ return upperRight; }
    Point<ndim> getUpRight(){ return upperRight; }
    Point<ndim> getBotLeft(){ return bottomLeft; }
    bool getIsLeaf(){ return isLeaf; }
    vector< Node<T,ndim>* > &getChildren(){ return children; }
    int getNRecords(){ return nRecords; }

    float getExpansion(Record<T,ndim>  &record){
        //Retorna el espacio que se expandiria si se insertara el record, 
        //Se calcula como el espacio que ocuparia el nodo si se insertara el record menos el espacio que ocupa actualmente
        float expansion = 1;
        Point<ndim> botLeft= bottomLeft.getCoords(record.getPoint(), min);
        Point<ndim> upRight = upperRight.getCoords(record.getPoint(), max);
        for (int i = 0; i < ndim; i++){
            expansion *= (upRight[i]-botLeft[i]);
        }
        return expansion - getSpace();
    }

    float getSpace(){
        //Retorna el espacio que ocupa en la dimensión ndim
        float space = 1;
        for (int i = 0; i < ndim; i++){
            space *= (upperRight[i] - bottomLeft[i]);
        }
        return space;
    }

    //Es necesario para actualizar el padre de los nodos hijos que se crean al dividir un nodo
    void setParent(Node<T,ndim>* parent_){ parent = parent_; }

    bool insert(Node<T,ndim>* node){
        //Funcion para insertar nodos dentro de un nodo intermedio, se usa cuando algún hijo se divide
        children.push_back(node);
        node->setParent(this);
        if (children.size() == 1){
            bottomLeft = node->getBotLeft();
            upperRight = node->getUpRight();
        }
        else if (children.size() >= 2){
            bottomLeft = bottomLeft.getCoords(node->getPoint(), min);
            upperRight = upperRight.getCoords(node->getPoint(), max);
        }
        return children.size()<=maxRecords;
    }

    Node<T,ndim>* splitLeaf(){
        //Reparte los records en 2 nodos, sobreescribe el actual y crea un nuevo nodo y lo retorna
        //Se inicializa el nuevo nodo hoja con el mismo padre que el nodo actual
        Node<T,ndim>* node = new Node<T,ndim>(maxRecords, parent); //Insertar la mitad de records más cercanos a botLeft
        vector< Record<T,ndim> > recCopy(records); //Copia de los records original
        Point<ndim> botLeft= bottomLeft, upRight= upperRight; //Copia de los rangos
        records.clear(); //Se reinician los records del nodo actual
        nRecords = 0;
        //Se insertará los records más cercanos a los extremos de los nodos bottomLeft(node) y upperRight(this)
        int middle= (maxRecords+1)/2;
        //Insertar a los más cercanos a los extremos de los nodos bottomLeft y upperRight
        //Si un nodo alcanza la mitad de los records, insertar el resto en el otro nodo
        for (auto record: recCopy){
            if (this->nRecords < middle && node->nRecords < middle){
                if (record.getPoint().euclideanDistance(botLeft) < record.getPoint().euclideanDistance(upRight)){
                    node->insert(record);

                }
                else this->insert(record);
            }
			else if (node->nRecords == middle){
                this->insert(record);
            }
            else{
                node->insert(record);
            }
        }
        return node;
    }

    Node<T, ndim>* split(){
        //Reparte los nodos en 2 nodos, sobreescribe el actual y crea un nuevo nodo y lo retorna
        Node<T,ndim>* node= new Node<T, ndim>(maxRecords, parent, false);//Se crea un nodo intermedio en el mismo nivel que el nodo actual
        vector< Node<T,ndim>* > childCopy(children); //Copia de la lista de records original
        Point<ndim> botLeft= bottomLeft, upRight= upperRight; //Copia de los rangos
        children.clear() //Se reinician los records del nodo actual
        //upperRight= this
        int middle= (maxRecords+1)/2;
        //Insertar a los más cercanos a los extremos de los nodos bottomLeft y upperRight
        //Si un nodo alcanza la mitad de los records, insertar el resto en el otro nodo
        for (auto child: childCopy){
            //VERIFICAR SI SE ACTUALIZA EL PARENT DE FORMA CORRECTA!!!!!!!!!!!!!!!!!!!
            if (this->nRecords < middle && node->nRecords < middle){
                //Se calcula la distancia euclidiana de los puntos medios de los nodos a los extremos del nodo original
                if (child->getPoint().euclideanDistance(botLeft) < child->getPoint().euclideanDistance(upRight)){
                    node->insert(child);
                }
                else this->insert(child);
            }
            else if (node->nRecords == middle){
                this->insert(child);
            }
            else{
                node->insert(child);
            }
        }
        return node;
    }
};

template< typename T, int ndim >
class RTree {
private:
    Node<T,ndim> *root;
    int maxRecords;
public:
    RTree(int maxRecords) : root(new Node<T,ndim>(maxRecords)), maxRecords(maxRecords) {}
    Node<T,ndim>* getRoot(){ return root; }
    void insert( Record<T,ndim> record ){
        /*
        - Llamar a chooseLeaf en la raiz
        - Insertar el record en el nodo hoja retornado por chooseLeaf
        - Si el nodo hoja se divide, llamar a adjustTree en el nodo hoja 
        */
        Node<T,ndim>* node = root->chooseLeaf(record);
        if (!node->insert(record)){
            //Se necesita dividir, se iguala la raiz al nodo retornado por adjustTree
            root = node->adjustTree();
        }
    }
};

struct Rect3D{
    Point<3> bottomLeft;
    float hx, hy, hz;
    Rect3D(Point<3> bottomLeft, Point<3> upperRight) : bottomLeft(bottomLeft){
        hx = upperRight[0]-bottomLeft[0];
        hy = upperRight[1]-bottomLeft[1];
        hz = upperRight[2]-bottomLeft[2];
    }
};

void preOrder(Node<Pokemon, 3>* node, vector<Rect3D> &rects, int level=0){
    rects.push_back(Rect3D(node->getBottomLeft(), node->getUpperRight()));
    //Imprimir bottomLeft y upperRight
    /*
    cout << node->getBottomLeft() << " " << node->getUpperRight() << " " << level;
    if (node->getIsLeaf()){
        cout << " Leaf " << node->getNRecords() << endl;
    }
    else{
        cout << " Mid " << node->getChildren().size() << endl;
    }
    */
    if (node->getIsLeaf()){
        return;
    }
    else{
        for (auto child: node->getChildren()){
            preOrder(child, rects, level+1);
        }
    }
}

void loadData(RTree<Pokemon, 3> &rtree, vector<Record<Pokemon,3>> &pokemons){
    ifstream file("pokemon.csv");
    string line, name;
    int id;
    float attack, defense, speed, height, hp, weight;
    while (getline(file, line)){
        stringstream linestream(line);
        linestream >> id >> name >> attack >> defense >> speed >> height >> hp >> weight;
        Pokemon* pokemon= new Pokemon(id, name, height, hp, weight);
        float coords[3] = {attack, defense, speed};
        Point<3>* point= new Point<3>(coords);
        Record<Pokemon, 3> *record= new Record<Pokemon, 3>(*pokemon, *point);
        rtree.insert(*record);
        pokemons.push_back(*record);
    }
    file.close();
}

vector<Rect3D> rtreeToRect(RTree<Pokemon, 3> &rtree){
    //Recorrido preorder
    vector<Rect3D> rects;
    Node<Pokemon, 3>* node = rtree.getRoot();
    preOrder(node, rects);
    return rects;
}

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

vtkNew<vtkActor> drawFigure( const string &filename, double x, double y, double z, double hx, double hy){
    vtkNew<vtkPNGReader> PNGReader;
    PNGReader->SetFileName(filename.c_str());
    PNGReader->Update();
    // objeto textura
    vtkNew<vtkTexture> texture;
    texture->SetInputConnection(PNGReader->GetOutputPort());
    // objeto plano
    vtkNew<vtkPlaneSource> planeSource;
    planeSource->SetOrigin(0, 0, 0);
    planeSource->SetPoint1(hx,0,0);
    planeSource->SetPoint2(0,hy,0);
    planeSource->SetCenter(x, y, z);
    planeSource->Update();
    // mapea y asigna textura
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(planeSource->GetOutputPort());
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->SetTexture(texture);
    return actor;
}

int main(int argc, char **argv)
{
    RTree <Pokemon, 3> rtree(22);//21 minimo
    vector<Record<Pokemon, 3>> pokemons;
    loadData(rtree, pokemons);
    vector<Rect3D> rects = rtreeToRect(rtree);
    
    vtkNew<vtkNamedColors> colors;

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

    for (auto rect : rects){
        vtkNew<vtkActor> actor = drawRegion(rect.bottomLeft[0], rect.bottomLeft[1], rect.bottomLeft[2], rect.hx, rect.hy, rect.hz);
        renderer->AddActor(actor);
    }

    for (auto pokemon : pokemons){
        vtkNew<vtkActor> image = drawFigure( "pokemon_png/" + to_string(pokemon.getObject().getId())+".png", 
                                              pokemon.getPoint()[0], pokemon.getPoint()[1], pokemon.getPoint()[2], 
                                              pokemon.getObject().getHeight(), pokemon.getObject().getWeight() );
        renderer->AddActor(image); 
    }

    renderer->SetActiveCamera(camera);
    renderer->ResetCamera();
    renderer->SetBackground(colors->GetColor3d("Cornsilk").GetData());

    renWin->SetSize(600, 600);

    // interact with data
    renWin->Render();
    iren->Start();

    return EXIT_SUCCESS;
}
