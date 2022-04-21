#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

bool prueba_vector_estatico();
bool prueba_vector_dinamico();
int main(void){
    bool ordenado1 = false, ordenado2 = false;
    //ordenado1 = prueba_vector_estatico();
    ordenado2 = prueba_vector_dinamico();
    //std::cout<<"Estado del vector estatico: "<<ordenado1<<std::endl;
    std::cout<<"Estado del vector dinamico: "<<ordenado2<<std::endl;
}

/*................................ORDENACION RAPIDA...........................................*/

template <class t>
int partition(std::vector<t>& array, int low, int high) {

  t pivot = array[high];
  
  int i = (low - 1);

  for (int j = low; j < high; j++) {
    if (array[j] <= pivot) {
      i++;  
      std::swap(array[i], array[j]);
    }
  }

  std::swap(array[i + 1], array[high]);
  
  return (i + 1);
}

template <class t>
void quickSort(std::vector<t>& array, int low, int high) {
  if (low < high) {
      
    int pi = partition(array, low, high);

    quickSort(array, low, pi - 1);

    quickSort(array, pi + 1, high);
  }
}

/*............................................FUSION...................................*/

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

bool prueba_vector_estatico(){
std::vector<int>v = {5,4,3,2,1};
std::vector<int>v2 = {1,2,3,4,5};
unsigned op;
bool ordenado = true;
std::cout<<"1-.Ordenacion por fusion\n2-.Ordenacion rapida"<<std::endl;
std::cin>>op;
switch(op){
    case 1: mergeSort(v,0,v.size()-1);
    break;
    case 2: quickSort(v,0,v.size()-1);
        break;
    }
     for(size_t i=0; i<v.size();i++){
        if(v[i]!= v2[i]){
            ordenado = false;
        }
    }

    return ordenado;
}

bool prueba_vector_dinamico(){
    std::vector<int>v,u;
    bool ordenado = true;
    unsigned op;
    size_t t = 100; //tamaño inicial del vector
    
    while(t < 10000 && ordenado){
        //se inicializa el vector con el tamaño
        v = std::vector<int>(t);
        for(size_t i = 0; i < t; i++)
        v[i] = i+1;
    
    u = v;  //a priori, u ya está ordenado de menor a mayor
    std::shuffle(v.begin(),v.end(), std::random_device());

    //mergeSort(v,0,v.size()-1);   
    quickSort(v,0,v.size()-1);
    
    for(size_t i=0; i<v.size();i++){
        if(v[i]!= u[i]){
            ordenado = false;
        }
    }
        t+=100;
    }
    return ordenado;
}