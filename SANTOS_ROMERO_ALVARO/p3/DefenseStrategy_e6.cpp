// ###### Config options ################



// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"
#include "cronometro.h"

using namespace Asedio;         

Vector3 cellCenterToPosition(int i, int j, float cellWidth, float cellHeight)
{
    return Vector3((j * cellWidth) + cellWidth * 0.5f, (i * cellHeight) + cellHeight * 0.5f, 0);
}

void positionToCell(const Vector3 pos, int &i_out, int &j_out, float cellWidth, float cellHeight)
{
    i_out = (int)(pos.y * 1.0f / cellHeight);
    j_out = (int)(pos.x * 1.0f / cellWidth);
}


float defaultCellValue(int row, int col, bool** freeCells, int nCellsWidth, int nCellsHeight
    , float mapWidth, float mapHeight, List<Object*> obstacles, List<Defense*> defenses) {
    	
    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;

    Vector3 cellPosition((col * cellWidth) + cellWidth * 0.5f, (row * cellHeight) + cellHeight * 0.5f, 0);
    	
    float val = 0;
    for (List<Object*>::iterator it=obstacles.begin(); it != obstacles.end(); ++it) {
	    val += _distance(cellPosition, (*it)->position);
    }

    return val;
}

bool factible(List<Defense *> defenses, const Defense &currentDef, List<Object *> obstacles, float mapHeight,
              float cellWidth, float cellHeight, float mapWidth, int row, int col, bool **freeCells)
{
    bool fact = true;

    Vector3 posicion = cellCenterToPosition(row, col, cellWidth, cellHeight);

    if (posicion.x + currentDef.radio > mapWidth ||  //Se sale por la derecha
        posicion.x - currentDef.radio < 0 ||         //Se sale por la izquierda
        posicion.y + currentDef.radio > mapHeight || //Se sale por abajo
        posicion.y - currentDef.radio < 0)           //Se sale por arriba
        fact = false;
   
    List<Object *>::iterator currentObstacle;

    for (currentObstacle = obstacles.begin(); currentObstacle != obstacles.end(); currentObstacle++)
    {
        //Si la distancia entre los centros es menor a la suma de los radios de la defensa y el obstáculo, colisionan
        if (_distance(posicion, (*currentObstacle)->position) < (currentDef.radio + (*currentObstacle)->radio))
            fact = false;
    }

    List<Defense *>::iterator currentDefense;

    for (currentDefense = defenses.begin(); currentDefense != defenses.end(); currentDefense++)
    {
        //Si la distancia entre los centros es menor a la suma de los radios de la defensa actual y la que está colocada, colisionan
        if (_distance(posicion, (*currentDefense)->position) < (currentDef.radio + (*currentDefense)->radio))
            fact = false;
    }

    return fact;
}


/*estructura para almacenar los valores a las celdas*/
struct Cell{

    int row_,col_;
    float value_;
    Cell(int r=0, int c=0, float v=0):row_(r), col_(c), value_(v){}
    Cell operator = (const Cell &c){
        this->row_ = c.row_;
        this->col_ = c.col_;
        return *this;
    }
    bool operator < (const Cell &c){
        return value_ < c.value_;
    }
    bool operator <= (const Cell &c){
        return value_ <= c.value_;
    }
};

Cell create_cell(Cell c){return c.row_, c.col_, c.value_;}   


template <class t>
int partition(std::vector<t>& array, int n, int n0) {
    
  t pivot = array[n0];
  
  int i = (n - 1);

  for (int j = n; j < n0; j++) {
    if (array[j] <= pivot) {
      i++;  
      std::swap(array[i], array[j]);
    }
  }
  
  std::swap(array[i + 1], array[n0]);
  
  return (i + 1);
}

template <class t>
void quickSort(std::vector<t>& array, int n, int n0) {
  if (n < n0) {
      
    int pi = partition(array, n, n0);

    quickSort(array, n, pi - 1);

    quickSort(array, pi + 1, n0);
  }
}  


/* -------------------- MERGESORT / ORDENACION POR FUSION ------------------- */
/* -------------------------------------------------------------------------- */
/*                                DOCUMENTACION                               */
/* -------------------------------------------------------------------------- */
/*
Implementacion del algoritmo mergesort / ordenacion por fusion
recibira una std::vector<t> &arr, size_t l, size_t m, size_t r
Precondiciones==> Los elementos de la lista deben tener sobrecargado el operador <
Postcondiciones ==> Devuelve la lista de los elementos t ordenados
Tipo de Algoritmo: Divide y Vencerás(Equilibrado)
El parámetro i representa la posición a partir de la cual pretende ordenarse el vector v. 
De forma análoga, el parámetro j representa posición del vector v hasta donde pretende 
realizarse la ordenación. El parámetro k representa la posición intermedia que divide 
el vector original en los dos subvectores que van a ser fusionados.
Created by @Mry3ll0w, under © GPL2.0
*/
template <class t>
void merge(std::vector<t> &arr, size_t l, size_t m, size_t r)
{
    size_t n1 = m - l + 1;
    size_t n2 = r - m;
        
    std::vector<t>L(n1),R(n2);
   
    for (int i = 0; i < n1; i++){//copia la parte izquierda al centro
        L[i] = arr[l + i];
    }
        
    for (int j = 0; j < n2; j++){//copia la parte derecha al centro
         R[j] = arr[m + 1 + j];
    }

    int i = 0;
    int j = 0;
    int k = l;
 
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
 
  
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
 
   
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }

}
 
template <class t>
void mergeSort(std::vector<t> &arr,size_t l,size_t r){
    if(l>=r){
        return;
    }
    size_t m = (l+r-1)/2;
    mergeSort(arr,l,m);
    mergeSort(arr,m+1,r);
    merge(arr,l,m,r);
}

//Algoritmo necesario para realizar las llamadas a los diferentes algoritmos de ordenacion en la misma
//ejecucion (recomendación de profesor de practicas)
void algorithm_selector(std::vector<Cell> &cells, int it){

    if(it == 0){
        std::list<Cell>auxList(cells.begin(), cells.end());
        auxList.sort();
        cells = std::vector<Cell>(cells.begin(), cells.end());
    }
    else if (it == 1){
        quickSort(cells,0,cells.size()-1);

    }
    else if(it == 2){
        mergeSort(cells,0,cells.size()-1);
    }
}

void DEF_LIB_EXPORTED placeDefenses3(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , List<Object*> obstacles, List<Defense*> defenses) {

    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight; 
	cronometro c;
    long int r = 0;
    for(size_t it_for = 0; it_for < 3; it_for ++){
    c.activar();
    //do {	
	
    int maxAttemps = 1000;
    std::vector<Cell> Cells;
    Cell auxCells;
    int i, j, k;
    float aux;
    bool placed = false;
    Cell C,S;

    //Colocación del CEM
    for (i = 0; i < nCellsHeight; i++)
        for (j = 0; j < nCellsWidth; j++)
        {
            if (freeCells[i][j])
            //se añade al vector la nueva celda, obteniendo el valor de esta mediante la funcion cellvalueCEM
                Cells.push_back(Cell(i, j, defaultCellValue(i, j, freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses)));
        }

    //llamada a la función para utilizar las ordenaciones
    algorithm_selector(Cells,it_for);

    List<Defense *>::iterator currentDefense = defenses.begin();

    //Devorador para CEM
    while (!placed && !Cells.empty())
    {
        //Se guarda en S la última celda (la de mayor valor) y se elimina del vector.
        S = Cells.back();
        Cells.pop_back();
        //Se llama a factible para no colocar la defensa en una celda inválida.
        if (factible(defenses, *(*currentDefense), obstacles, mapHeight,
                     cellWidth, cellHeight, mapWidth, S.row_, S.col_, freeCells))
        {
            //Una vez colocada, se actualizan los booleanos y se posiciona la defensa en el centro de la celda.
            placed = true;
            freeCells[S.row_][S.col_] = false;
            (*currentDefense)->position = cellCenterToPosition(S.row_, S.col_, cellWidth, cellHeight);
        }
    }
    currentDefense++;

    //Copia de los candidatos restantes para utilizarlos en la colocación de las demás defensas
    std::vector<Cell> Cs;

    for (i = 0; i < nCellsHeight; i++)
        for (j = 0; j < nCellsWidth; j++)
        {
            if (freeCells[i][j])
                Cs.push_back(Cell(i, j, defaultCellValue(i, j, freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses)));
        }

    k = 0;

    for (i = 0; i < Cs.size(); i++)
        for (k = i + 1; k < Cs.size(); k++)
        {
            if (Cs[k].value_ < Cs[i].value_)
                std::swap(Cs[k],Cs[i]);
        }

    Cells.clear();
    Cells = Cs;

    //Devorador para el resto de defensas

    std::vector<Cell>::iterator currentCell;
    int it;
    

    while (currentDefense != defenses.end())
    {
        it = 0;
        placed = false;
        currentCell = Cs.end();
        while (!placed && !Cells.empty())
        {
            it++;
            S = Cells.back();
            Cells.pop_back();
            if (factible(defenses, *(*currentDefense), obstacles, mapHeight, cellWidth, cellHeight, mapWidth, S.row_, S.col_, freeCells))
            {
                placed = true;
                (*currentDefense)->position = cellCenterToPosition(S.row_, S.col_, cellWidth, cellHeight);
                while (it > 0)
                {
                    currentCell--;
                    it--;
                }
                Cs.erase(currentCell);
            }
        }
        Cells.clear();
        Cells = Cs;
        currentDefense++;
    }

		//while(c.tiempo() < 1.0);
        ++r;
        c.parar();
        std::cout << (nCellsWidth * nCellsHeight) << '\t' << c.tiempo() / r << '\t' << c.tiempo()*2 / r << '\t' << c.tiempo()*3 / r << '\t' << c.tiempo()*4 / r << std::endl;
    }  
}
