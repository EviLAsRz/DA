// ###### Config options ################

#define PRINT_PATHS 1

// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"

#ifdef PRINT_PATHS
#include "ppm.h"
#endif

using namespace Asedio;

struct sort_monticulo
{
    //Sobrecargamos el operador () para que el monticulo pueda ordenarlos correctamente  
    bool operator()(AStarNode* a, AStarNode* b)
    {
        return a->F > b->F;
    }
};


std::pair<int,int> getValues(const Vector3 pos, float cellWidth, float cellHeight)
{

    return std::make_pair((int)(pos.x * 1.0f / cellWidth), (int)(pos.y /cellHeight));

}

Vector3 cellCenterToPosition(int i, int j, float cellWidth, float cellHeight){ 
    return Vector3((j * cellWidth) + cellWidth * 0.5f, (i * cellHeight) + cellHeight * 0.5f, 0); 
}

float bestRelation(List<Defense*> defenses)
{
    float best = 0;
    List<Defense*>::iterator it = defenses.begin();
    
    while(it != defenses.end())
    {
        if((*it)->damage / (*it)->health > best)
            best = (*it)->damage / (*it)->health;
        it++;
    }
    return best;
}

void DEF_LIB_EXPORTED calculateAdditionalCost(float **additionalCost, int cellsWidth, int cellsHeight, float mapWidth, float mapHeight, List<Object *> obstacles, List<Defense *> defenses){

    float cellWidth = mapWidth / cellsWidth;
    float cellHeight = mapHeight / cellsHeight;
    List<Defense*>::iterator it = defenses.begin();
    float bestDef = bestRelation(defenses), diff;

    for (int i = 0; i < cellsHeight; ++i)
    {
        for (int j = 0; j < cellsWidth; ++j)
        {
            Vector3 cellPosition = cellCenterToPosition(i, j, cellWidth, cellHeight);
            float cost = 0;
            while(it != defenses.end())
            {
                cost += (1/_distance(cellPosition, (*it)->position))*1000;
                diff = bestDef - (bestDef - (*it)->damage / (*it)->health);
                cost *= diff*100;
                it++;
            }
            it = defenses.begin();
            additionalCost[i][j] = cost;
        }
    }
}


int heuristicaManhattan(AStarNode *begin, AStarNode *target, float cellWidth, float cellHeight)
{
    int ogX, ogY, deX, deY, distancia = 0;
    std::pair<int, int> sol;
    
    //Coordenadas origen
    sol = getValues(begin->position, cellWidth, cellHeight);
    ogX = sol.first; ogY = sol.second;

    //Coordenadas destino
    sol = getValues(target->position, cellWidth, cellHeight);
    deX = sol.first; deY = sol.second;

    distancia = abs(ogX - deY) + abs(ogY - deY);
    return distancia;
}

std::list<Vector3> recupera_camino(AStarNode *begin, AStarNode *destino)
{
    AStarNode *actual = destino;
    std::list<Vector3> camino;

    //Insertamos el final ya que se recorre a revés
    camino.push_back(destino->position);

    //for inverso para introducir la posicion
    for(;actual->parent != begin && destino->parent != NULL;actual=actual->parent)
        camino.push_front(actual->position);

    return camino;
}

bool existe_nodo(AStarNode *nodo, std::vector<AStarNode *> nodos)
{
    bool existe = false;
    
    for(auto i = nodos.begin(); i != nodos.end() && !existe; ++i){
        
        if (nodo == *i)
            existe = true;
        
    }
    
    return existe;
}

void DEF_LIB_EXPORTED calculatePath(AStarNode *originNode, AStarNode *targetNode, int cellsWidth, int cellsHeight, float mapWidth, float mapHeight, float **additionalCost, std::list<Vector3> &camino)
{
    std::pair<int, int> p;
    int maxIter = 100; 
    AStarNode *actual = originNode;
    std::vector<AStarNode *> abiertos, cerrados;
    bool token = false; //bool de control (ver si estamos en el nodo objetivo)
    std::list<AStarNode *>::iterator it;
    float distancia=0;
    float cellWidth = mapWidth / cellsWidth;
    float cellHeight = mapHeight / cellsHeight;
    
    targetNode->parent = NULL;

    actual->G = 0;
    //calculamos la heuristica
    actual->H = heuristicaManhattan(actual, targetNode, cellWidth, cellHeight);
    //Obtener posiciones dentro de la matriz
    p = getValues(actual->position,cellWidth, cellHeight);
    //actual f guarda el valor de A* (actual = nodo + heuristica + coste de llegada al nodo)
    actual->F = actual->G + actual->H + additionalCost[p.first][p.second];
    
    abiertos.push_back(actual); //metemos en abiertos el nodo actual

    while (!token && !abiertos.empty()) //mientras abiertos no este vacio, se ejecuta el algoritmo
    {
        actual = abiertos.front(); //sacamos el primero
        //funcion pop_heap compara usando monticulo y elimina el menos prometedor
        std::pop_heap(abiertos.begin(), abiertos.end(), sort_monticulo());

        abiertos.pop_back();
        cerrados.push_back(actual);
        
        if (actual == targetNode) //si nodo objetivo, se termina el algoritmo
        {
            token = true;
        }
        else
        {   //it guarda el primer adyacente de actual
            it = actual->adjacents.begin();
            while (it != actual->adjacents.end()) //mientras que it no sea igual al ultimo adyacente de actual
            {
                if (!existe_nodo((*it), cerrados)) //si it no se encuentra en cerrados
                {
                    if (!existe_nodo((*it), abiertos)) //si it no se encuentra en abiertos, se calcula el camino de este
                    {
                        (*it)->parent = actual;
                        (*it)->G = actual->G + _distance(actual->position, (*it)->position);
                        (*it)->H = heuristicaManhattan((*it), targetNode, cellWidth, cellHeight);
                        
                        p = getValues((*it)->position,cellWidth, cellHeight);
                        (*it)->F = (*it)->G + (*it)->H + additionalCost[p.first][p.second];
                        
                        abiertos.push_back((*it));
                        std::push_heap(abiertos.begin(), abiertos.end(), sort_monticulo());   
                    }
                    else
                    {
                        distancia = _distance(actual->position, (*it)->position);
                        if ((*it)->G > actual->G + distancia)
                        {
                            (*it)->parent = actual;
                            (*it)->G += distancia;
                            (*it)->F = (*it)->G + (*it)->H + + additionalCost[p.first][p.second];
                            p = getValues((*it)->position,cellWidth, cellHeight);
                            std::make_heap(abiertos.begin(), abiertos.end(), sort_monticulo());
                        }
                    }
                }
                it++;
            }
        }
    }
    camino = recupera_camino(originNode, targetNode); //recupera el camino hasta ese nodo
}