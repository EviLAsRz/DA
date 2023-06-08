Problema de la Mochila

Elementos:
* Conjunto inicial de candidatos: objetos de la mochila.
* Función solución: ¿Se ha llenado la mochila?
* Función de selección: Elige un objeto con máxima relación valor/peso.
* Función de factibilidad: ¿Se puede introducir el nuevo objeto sin exceder la capacidad?
* Función objetivo: El valor total de los objetos devueltos.
* Objetivo: Maximizar.

Algoritmo estandar:

mochila(O,c) -> S
C <- O
S <- ∅

mientras c != 0 ^ C != ∅
	(v,p) <- selecciona-objeto(C)
	C <- C - {(v,p)}
	si p <= C
		S <- S U {(v,p)}
		c <- c - p
	si no
		S <- S U {(v.c)/p,c}
		c <- 0

selecciona-objeto(C) -> (v,p)
	r <- -∞
	para todo (a,b) ∈ C
		si a/b > r
			r <- a/b
			(v,p) <- (a,b)


;donde:
O -> Conjunto de candidatos pasados por valor.
C -> Conjunto de candidatos.
S -> Conjunto solución.
(v,p) -> tupla de valor/peso de un objeto.



Preordenando los elementos previo a introducirlos al algomritmo, conseguimos una mejora de tiempos, desde O(n2) hasta obtener el tiempo domiante del algoritmo de seleccion con t(n) ∈ Θ (nlogn).

Algoritmo mejorado (mejor tiempo)

mochila(O,c) -> S
C <- ordena(O)
S <- ∅

mientras c != 0 ^ C != ∅
	(v,p) <- extrae-primero(C)
	si p <= c
		S <- S U {(v,p)}
		c <- c - p
	si no
		S <- S U {(v.c)/p,c}
		C <- 0

Suponemos que la función extrae-primero es de orden Θ(1) y se encarga de eliminar el elemento extraído de la lista C.
