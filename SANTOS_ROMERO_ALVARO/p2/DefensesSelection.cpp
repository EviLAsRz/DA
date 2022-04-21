// ###### Config options ################
// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"
/*simulador -l 3324 -ases 1000 -v*/
using namespace Asedio;

/*
CREAR UNA ESTRUCTURA PARA ALMACENAR LA TSP (matriz tal cual no parece que funcione, se contemplará hacer un registro)
CODIGO PARA RELLENAR LOS VALORES DE LA TSP
PROGRAMAR EL ALGORITMO DE RECUPERACION Y CUMPLIMENTAR ADECUADAMENTE LA LISTA selectedIDs
*/

float defvalue(const Defense &def){
float value;
//se ha supuesto que la dispersión es el valor determinante de las defensas.
value = (def.damage*def.attacksPerSecond*def.health*def.range)/(def.dispersion);
return value;
}

void bestStrat(std::vector<std::vector<float>>& tsp, std::list<Defense*> defenses, unsigned int ases){

    //se crean las filas y columnas de la tsp.
    float values[defenses.size() - 1];
    int costs[defenses.size() - 1];
    int i,j;
    //se empieza la lista con el 2do elemento puesto que el primero es el CEM.
    List<Defense*>::iterator it = defenses.begin();
    it++;
    
    for(i = 0;i < defenses.size() - 1; i++){
        costs[i] = (*it)->cost;
        values[i] = defvalue((**it));
        it++;
    }

    //Codigo de relleno de la tsp
    ases -= (*defenses.begin())->cost;
    for(j = 0; j < ases + 1; j++){
        if (j < costs[0])
            tsp[0][j] = 0;
        else
            tsp[0][j] = values[0];
    }
    for(i = 1; i < defenses.size()-1; i++)
        for(j = 0; j < ases+1; j++)
        {
            if(j < costs[i])
                tsp[i][j] = tsp[i-1][j];
            else
                //Se toma el maximo de ambos.
                tsp[i][j] = std::max(tsp[i-1][j], tsp [i-1][j-costs[i]] + values[i]);
        }
        //El beneficio maximo estará en tsp[defenses.size()-1][ases];
}

void bestDefSelec(std::list<Defense*> defenses, unsigned int ases, std::list<int>& selectedIDs,
const std::vector<std::vector<float>>& tsp){

    int i = defenses.size() - 2, j = ases - (*defenses.begin())->cost;
    List<Defense*>::iterator it = defenses.end();
    it--;
    //mientras no se llegue al final de la tabla
    while (i > 0){
        if(tsp[i][j] != tsp[i-1][j])
        {
            j -= (*it)->cost;
            selectedIDs.push_back((*it)->id);
        }
        i--;
        it--;
    }
    if(tsp[0][j] != 0)
        selectedIDs.push_back((*it)->id);
}

void DEF_LIB_EXPORTED selectDefenses(std::list<Defense*> defenses, unsigned int ases, std::list<int> &selectedIDs
            , float mapWidth, float mapHeight, std::list<Object*> obstacles) {

unsigned int cost;
std::list<Defense*>::iterator it = defenses.begin();
//La tsp se almacena en un vector de vectores de la biblioteca estandar, te tal manera que no hace falta
//la creación de un registro para almacenar los valores.
std::vector<std::vector<float>> tsp(defenses.size()-1, std::vector<float>(ases-(*it)->cost+1));
selectedIDs.push_back((*it)->id);
//Rellena la tsp
bestStrat(tsp, defenses, ases);
//recupera la tsp
bestDefSelec(defenses, ases, selectedIDs, tsp);
}