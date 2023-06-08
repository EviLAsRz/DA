#PROBLEMA DE LAS MONEDAS

Sea M un conjunto de monedas y c una cantidad a devolver. Por cada tipo de moneda de valor v se dispone de un *suministro limitado* de k unidades.

Se desea hallar la composición del *cambio de menor número de monedas*.
Una estrategia posible de selección de monedas consiste en elegir, de las que quedan, la de mayor valor.

Esto se hace a diario en máquinas expendedoras, cajeros automáticos, etc.
Si es posible devolver el cambio con las monedas disponibles, se actualiza el conjunto para su próximo empleo.

Conviene seleccionar todas las monedas del mismo valor de una vez: puede tratarse M como *multiconjunto* o como un *conjunto de pares*.

Esta estrategia no produce, en general, una solución óptima.
Se demuestra que si se dispone de un número suficiente o *infinito*, de monedas de 1,5,10,25,50,100,200,500, el algoritmo siempre encuentra la solución óptima.

#ELEMENTOS PERTENECIENTES A LOS VORACES

- Conjunto inicial de candidatos: las monedas.
- Función solución: ¿Se ha devuelto ya el cambio?
- Función de selección: elige las monedas de mayor valor.
- Función de factibilidad: ¿Se pueden devolver las monedas sin exceder el cambio?
- Función objetivo: el número de monedas devueltas.
- Objetivo: Minimizar.

A cada paso, las monedas seleccionadas tienen el mayor valor. Se devuelven tantas monedas de dicho valor como se puedan suministrar sin exceder el cambio a devolver.

#ALGORITMO

cambio(M,c) -> S
C <- M
S <- ∅
mientras c != 0 ^ C != ∅
	(v,k) <- selecciona-monedas(C)
	C <- C - {(v,k)}
	k <- min(k,c div v)
	si k > 0
		S <- S U {(v,k)}
		c <- c - v.k

selecciona-monedas(C) -> (v,k)
v <- -∞
para todo (a,b) ∈ C
	si a > v
		(v,k) <- (a,b)


