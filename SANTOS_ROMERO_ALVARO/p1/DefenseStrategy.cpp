// ###### Config options ################

//#define PRINT_DEFENSE_STRATEGY 1    // generate map images

// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"

#ifdef PRINT_DEFENSE_STRATEGY
#include "ppm.h"
#endif

#ifdef CUSTOM_RAND_GENERATOR
RAND_TYPE SimpleRandomGenerator::a;
#endif

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

/*CellValue para el resto de defensas (despues de haber colocado el CEM)*/
float cellValue(int row, int col, int sRow, int sCol, bool **freeCells, int nCellsWidth, 
int nCellsHeight, float mapWidth, float mapHeight, List<Object *> obstacles, List<Defense *> defenses)
{

    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;
    Vector3 position = cellCenterToPosition(row, col, cellWidth, cellHeight);
    //Srow y sCol son los valores que se obtienen al ordenar las celdas en place defenses
    Vector3 bestPosition = cellCenterToPosition(sRow, sCol, cellWidth, cellHeight);
    float value;

    //Se obtiene el valor de la celda mediante esta formula (longitud de la mejor posicion - distancia entre las posiciones)
    value = bestPosition.length() - _distance(position, bestPosition);

    //Si la distancia es mayor que 3*radio del CEM, se reduce el valor.
    if (_distance(bestPosition, position) > 3 * (*(defenses.back())).radio)
        value = value * 0.1;

    return value;
}

/*Funcion para dar valor a la mejor celda del mapa, donde se colocará el centro de extracción*/
float cellValueCEM(int row, int col, bool **freeCells, int nCellsWidth, int nCellsHeight, 
float mapWidth, float mapHeight, List<Object *> obstacles)
{
    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;
    int bestRow = nCellsHeight / 2, bestCol = nCellsWidth / 2;
    Vector3 position = cellCenterToPosition(row, col, cellWidth, cellHeight);
    Vector3 bestPosition = cellCenterToPosition(bestRow, bestCol, cellWidth, cellHeight);
    float value;

    value = bestPosition.length() - _distance(position, bestPosition);

    if (row < 3 || row > nCellsHeight - 3 || col > nCellsWidth - 3 || col < 3)
        value = value * 0.01;

    return value;
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
};

Cell create_cell(Cell c){return c.row_, c.col_, c.value_;}   

/*
    CREAR UNA ESTRUTURA DONDE GUARDAR LOS VALORES DE LAS CELDAS
    COLOCAR CENTRO MINERALES
    ASIGNAR VALORES A LAS CELDAS PARA COLOCAR CEM EN LA MEJOR POSICION Y GUARDARLAS EN LA ESTRUCTURA CREADA ANTERIORMENTE
    TOMO LA PRIMERA DEFENSA -> MIENTRAS NO TENGA POSICION LA PRIMERA DEFENSA
    POSICION = SELECCIONAR EL CANDIDATO MAS PROMETEDOR
    SI ES FACTIBLE
        SE COLOCA EN ESA POSICION
    FIN MIENTRAS


    COLOCAR RESTO DEFENSAS
    ASIGNAR VALORES A LAS CELDAS PARA COLOCAR EL RESTO DE LAS DEFENSAS Y GUARDARLAS EN LA ESTRUCTURA CREADA ANTERIORMENTE
    TOMO LA SIGUIENTE DEFENSA -> MIENTRAS NO TENGA COLOCADAS TODAS LAS DEFENSAS
    POSICION = SELECCIONAR EL CANDIDATO MAS PROMETEDOR
    SI ES FACTIBLE
        SE COLOCA EN ESA POSICION
        TOMO LA SIGUIENTE DEFENSA
    FIN MIENTRAS
    */

void DEF_LIB_EXPORTED placeDefenses(bool **freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight, std::list<Object *> obstacles, std::list<Defense *> defenses)
{

    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;

    int maxAttemps = 1000;
    std::vector<Cell> Cells;
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
                Cells.push_back(Cell(i, j, cellValueCEM(i, j, freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles)));
        }

    //Se recorren las celdas y se hace un intercambio para situar la de mayor valor en la primera posicion
    for (i = 0; i < Cells.size(); i++)
        for (k = i + 1; k < Cells.size(); k++)
        {
            //Si el valor de la primera celda es el menor, no se hace ningun intercambio
            if (Cells[k].value_ < Cells[i].value_)
            std::swap(Cells[k],Cells[i]);
        }

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
                Cs.push_back(Cell(i, j, cellValue(i, j, S.row_, S.col_, freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses)));
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
}
#ifdef PRINT_DEFENSE_STRATEGY

    float** cellValues = new float* [nCellsHeight]; 
    for(int i = 0; i < nCellsHeight; ++i) {
       cellValues[i] = new float[nCellsWidth];
       for(int j = 0; j < nCellsWidth; ++j) {
           cellValues[i][j] = ((int)(cellValue(i, j))) % 256;
       }
    }
    dPrintMap("strategy.ppm", nCellsHeight, nCellsWidth, cellHeight, cellWidth, freeCells
                         , cellValues, std::list<Defense*>(), true);

    for(int i = 0; i < nCellsHeight ; ++i)
        delete [] cellValues[i];
	delete [] cellValues;
	cellValues = NULL;

#endif

