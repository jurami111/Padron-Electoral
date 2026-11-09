//Portada 
//Integrantes: Génesis Castrillo Ballestero, Mariana Lai Sánchez, Julián Ramírez Artavia. 
//Grupo 5 
//Proyecto 1B  

#include <iostream>
#include <iomanip> // Para mostrar tiempos con dos decimales
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>


// Datos que se guardan de cada persona del padron.
// Uso long long para la cedula porque es un dato numerico grande.
// Los demas campos se guardan en arreglos de caracteres, como vienen del archivo.
// La cedula sirve como clave para ordenar y buscar en los arboles.

using namespace std;
using namespace chrono;

struct Persona
{
    long long cedula;
    char codigoElectoral[10];
    char fechaCaducidad[10];
    char nombre[40];
    char apellido1[40];
    char apellido2[40];
};


// Esta estructura guarda los tiempos de las cuatro operaciones que se comparan.
// Carga, listado y destruccion se miden en microsegundos (us).
// La busqueda se mide en nanosegundos (ns) porque suele ser mas rapida.
struct TiempoOperacion
{
    double carga = 0;
    double listado = 0;
    double busqueda = 0;
    double destruccion = 0;
};
// Aqui se agrupan los resultados de cada estructura para mostrarlos juntos.
// Asi no se mezclan los tiempos de la lista con los de cada arbol.
struct ReporteGeneral
{
    TiempoOperacion lista;
    TiempoOperacion abb;
    TiempoOperacion avl;
    TiempoOperacion btree;
};



// Quita espacios, tabulaciones y saltos de linea que queden al final del texto.
// Empieza desde el ultimo caracter y coloca el fin de cadena (\0) cuando
// encuentra un caracter sobrante. No modifica los espacios del medio.
void LimpiarEspacios(char* str)
{
    int i = (int)strlen(str) - 1;
    while (i >= 0 && (str[i] == ' ' || str[i] == '\t' || str[i] == '\n' || str[i] == '\r'))
    {
        str[i] = '\0';
        i--;
    }
}


// Lista enlazada maestra.
// Cada nodo almacena una persona y la direccion del siguiente nodo.
// La cabeza apunta al primer registro, o a NULL si la lista esta vacia.
struct NodoLista
{
    Persona dato;
    NodoLista* siguiente;
};

// Agrega un registro al principio de la lista.
// Primero se crea el nodo y se enlaza con la cabeza anterior.
// Al final, cabeza queda apuntando al nuevo nodo.
// Se recibe por referencia para modificar la cabeza original.
void InsertarInicioLista(NodoLista*& cabeza, Persona p)
{
    NodoLista* nuevo = new NodoLista;
    nuevo->dato = p;
    nuevo->siguiente = cabeza;
    cabeza = nuevo;
}

// Busca una cedula recorriendo los nodos uno por uno.
// Si la encuentra, devuelve la direccion del registro Persona.
// Si llega al final sin encontrarla, devuelve NULL.
Persona* BuscarLista(NodoLista* cabeza, long long cedula)
{
    NodoLista* aux = cabeza;
    while (aux != NULL)
    {
        if (aux->dato.cedula == cedula)
        {
            return &(aux->dato);
        }
        aux = aux->siguiente;
    }
    return NULL;
}

// Recorre la lista e imprime cada cedula con su nombre completo.
// Como las inserciones se hacen al inicio, el orden puede ser inverso al de lectura.
void ListarLista(NodoLista* cabeza)
{
    NodoLista* aux = cabeza;
    while (aux != NULL)
    {
        cout << aux->dato.cedula << "\t" << aux->dato.nombre << " " << aux->dato.apellido1 << " " << aux->dato.apellido2 << "\n";
        aux = aux->siguiente;
    }
}

// Libera toda la memoria reservada para la lista.
// Guarda temporalmente el nodo actual, avanza la cabeza y lo elimina.
// Cuando termina, cabeza queda en NULL.
void DestruirLista(NodoLista*& cabeza)
{
    while (cabeza != NULL)
    {
        NodoLista* temp = cabeza;
        cabeza = cabeza->siguiente;
        delete temp;
    }
}

// Arbol binario de busqueda (ABB).
// Cada nodo tiene un registro y dos hijos: izquierdo y derecho.
// No se balancea automaticamente; su forma depende del orden de insercion.
struct NodoABB
{
    Persona dato;
    NodoABB* izq;
    NodoABB* der;
};

//Arbol ABB original

/*// Estructura de cada nodo del arbol
typedef struct NodoBB
{
    int llave;
    NodoBB* HijoIzquierdo;
    NodoBB* HijoDerecho;
} X;


// Inserta un numero en el arbol
bool Insertar(NodoBB*& Raiz, int numero)
{
    // Crea el nodo cuando encuentra una posicion vacia
    if (Raiz == NULL)
    {
        Raiz = new NodoBB;
        Raiz->llave = numero;
        Raiz->HijoIzquierdo = NULL;
        Raiz->HijoDerecho = NULL;
        return true;
    }

    if (numero == Raiz->llave)
        return false;

    if (numero < Raiz->llave)
        return Insertar(Raiz->HijoIzquierdo, numero);

    return Insertar(Raiz->HijoDerecho, numero);
}*/



// Inserta un registro en el ABB usando la cedula como clave.
// Si encuentra un espacio vacio, crea un nodo nuevo.
// Si la cedula es menor continua por la izquierda; si es mayor, por la derecha.
// Devuelve false cuando la cedula ya estaba registrada.
bool InsertarABB(NodoABB*& raiz, Persona p)
{
    if (raiz == NULL)
    {
        raiz = new NodoABB;
        raiz->dato = p;
        raiz->izq = NULL;
        raiz->der = NULL;
        return true;
    }
    if (p.cedula == raiz->dato.cedula) return false;

    if (p.cedula < raiz->dato.cedula)
        return InsertarABB(raiz->izq, p);
    else
        return InsertarABB(raiz->der, p);
}

// Busca una cedula en el ABB sin recorrer las ramas que no hacen falta.
// Compara con la raiz actual y continua solo por el lado correspondiente.
// Devuelve el registro si lo encuentra o NULL en caso contrario.
Persona* BuscarABB(NodoABB* raiz, long long cedula)
{
    if (raiz == NULL) return NULL;
    if (cedula == raiz->dato.cedula) return &(raiz->dato);

    if (cedula < raiz->dato.cedula)
        return BuscarABB(raiz->izq, cedula);
    else
        return BuscarABB(raiz->der, cedula);
}

// Hace un recorrido InOrden: hijo izquierdo, nodo actual e hijo derecho.
// Como el ABB ordena por cedula, los registros se muestran de menor a mayor.
void ListarABB(NodoABB* raiz)
{
    if (raiz != NULL)
    {
        ListarABB(raiz->izq);
        cout << raiz->dato.cedula << "\t" << raiz->dato.nombre << " " << raiz->dato.apellido1 << " " << raiz->dato.apellido2 << "\n";
        ListarABB(raiz->der);
    }
}

// Destruye el ABB con un recorrido PostOrden.
// Elimina primero los hijos para no perder sus direcciones y despues la raiz.
// Deja el puntero de cada nodo en NULL.
void PodarHojasABB(NodoABB*& raiz)
{
    if (raiz != NULL)
    {
        PodarHojasABB(raiz->izq);
        PodarHojasABB(raiz->der);
        delete raiz;
        raiz = NULL;
    }
}

// Arbol AVL.
// Funciona como un ABB, pero cada nodo tambien guarda su altura.
// Al insertar se comprueba el balance para evitar que una rama crezca demasiado.
struct NodoAVL
{
    Persona dato;
    NodoAVL* izq;
    NodoAVL* der;
    int altura;
};

//El código oringinal se encuentra adjunto al final, el fagmento de código es extenso. 

// Devuelve la altura del nodo; un puntero NULL tiene altura cero.
int AlturaNodo(NodoAVL* n) { return (n == NULL) ? 0 : n->altura; }
// Devuelve el mayor de dos numeros, necesario para calcular alturas.
int Maximo(int a, int b) { return (a > b) ? a : b; }
// Balance = altura izquierda - altura derecha.
// Si es mayor que 1 o menor que -1, se necesita una rotacion.
int ObtenerBalance(NodoAVL* n) { return (n == NULL) ? 0 : AlturaNodo(n->izq) - AlturaNodo(n->der); }

// Rotacion simple a la derecha.
// Se utiliza cuando el lado izquierdo tiene demasiada altura.
// El hijo izquierdo sube, el nodo anterior baja a la derecha y
// T2 conserva el subarbol que debe quedar entre ambos.
// Despues se actualizan las alturas de los nodos afectados.
NodoAVL* RotarDerecha(NodoAVL* y)
{
    NodoAVL* x = y->izq;
    NodoAVL* T2 = x->der;

    x->der = y;
    y->izq = T2;

    y->altura = Maximo(AlturaNodo(y->izq), AlturaNodo(y->der)) + 1;
    x->altura = Maximo(AlturaNodo(x->izq), AlturaNodo(x->der)) + 1;

    return x;
}

// Rotacion simple a la izquierda.
// Es el caso contrario: el hijo derecho pasa a ocupar la posicion superior.
// T2 se acomoda como hijo derecho del nodo que bajo.
// Se recalculan las alturas antes de devolver la nueva raiz.
NodoAVL* RotarIzquierda(NodoAVL* x)
{
    NodoAVL* y = x->der;
    NodoAVL* T2 = y->izq;

    y->izq = x;
    x->der = T2;

    x->altura = Maximo(AlturaNodo(x->izq), AlturaNodo(x->der)) + 1;
    y->altura = Maximo(AlturaNodo(y->izq), AlturaNodo(y->der)) + 1;

    return y;
}

// Inserta un registro en el AVL comparando la cedula, igual que en el ABB.
// Al regresar de la llamada recursiva, actualiza la altura y revisa el balance.
// Segun la direccion del desbalance, realiza una rotacion derecha, izquierda
// o una combinacion de las dos (casos izquierda-derecha y derecha-izquierda).
// La funcion devuelve la raiz actualizada del subarbol.
NodoAVL* InsertarAVL(NodoAVL* nodo, Persona p)
{
    if (nodo == NULL)
    {
        NodoAVL* nuevo = new NodoAVL;
        nuevo->dato = p;
        nuevo->izq = NULL;
        nuevo->der = NULL;
        nuevo->altura = 1;
        return nuevo;
    }

    if (p.cedula < nodo->dato.cedula)
        nodo->izq = InsertarAVL(nodo->izq, p);

    else if (p.cedula > nodo->dato.cedula)
        nodo->der = InsertarAVL(nodo->der, p);

    else
        return nodo;

    nodo->altura = 1 + Maximo(AlturaNodo(nodo->izq), AlturaNodo(nodo->der));
    int balance = ObtenerBalance(nodo);

    if (balance > 1 && p.cedula < nodo->izq->dato.cedula) return RotarDerecha(nodo);
    if (balance < -1 && p.cedula > nodo->der->dato.cedula) return RotarIzquierda(nodo);
    if (balance > 1 && p.cedula > nodo->izq->dato.cedula)
    {
        nodo->izq = RotarIzquierda(nodo->izq);
        return RotarDerecha(nodo);
    }
    if (balance < -1 && p.cedula < nodo->der->dato.cedula)
    {
        nodo->der = RotarDerecha(nodo->der);
        return RotarIzquierda(nodo);
    }
    return nodo;
}

// Busca una cedula en el AVL comparando con el nodo actual.
// El balance del arbol ayuda a que normalmente haya menos niveles por recorrer.
// Devuelve la direccion del registro o NULL si no existe.
Persona* BuscarAVL(NodoAVL* nodo, long long cedula)
{
    if (nodo == NULL) return NULL;
    if (cedula == nodo->dato.cedula) return &(nodo->dato);

    if (cedula < nodo->dato.cedula)
        return BuscarAVL(nodo->izq, cedula);
    else
        return BuscarAVL(nodo->der, cedula);
}

// Recorre el AVL en InOrden para mostrar las cedulas de menor a mayor.
void ListarAVL(NodoAVL* nodo)
{
    if (nodo != NULL)
    {
        ListarAVL(nodo->izq);
        cout << nodo->dato.cedula << "\t" << nodo->dato.nombre << " " << nodo->dato.apellido1 << " " << nodo->dato.apellido2 << "\n";
        ListarAVL(nodo->der);
    }
}

// Libera la memoria del AVL de abajo hacia arriba.
// Primero destruye ambos hijos y luego elimina el nodo actual.
void DestruirAVL(NodoAVL*& nodo)
{
    if (nodo != NULL)
    {
        DestruirAVL(nodo->izq);
        DestruirAVL(nodo->der);
        delete nodo;
        nodo = NULL;
    }
}

// B-Tree de orden 5.
// Un nodo puede guardar varias claves (registros) ordenadas y varios hijos.
// MAX indica la cantidad maxima de claves y MIN se usa al dividir nodos.
// Los arreglos dejan una posicion adicional para el manejo de inserciones.
// Se mantienen los #define originales para no cambiar el algoritmo.
#define MAX 4
#define MIN 2

struct NodoBtree
{
    Persona val[MAX + 1];
    int count;
    NodoBtree* link[MAX + 1];
};

//Btree original 
/*using namespace std;

#define MAX 4
#define MIN 2

struct NodoBtree {
    int val[MAX + 1], count;
    NodoBtree *link[MAX + 1];
};

NodoBtree *root;

//Crear nodo
NodoBtree * crearnodo(int val, NodoBtree *hijo) {
    NodoBtree *NuevoNodo = new NodoBtree;
    NuevoNodo->val[1] = val;
    NuevoNodo->count = 1;
    NuevoNodo->link[0] = root;
    NuevoNodo->link[1] = hijo;
    return NuevoNodo;
}*/


// Crea un nodo del arbol B con su primer registro.
// Guarda la clave en val[1], inicia el contador en 1 y conecta
// el hijo izquierdo (link[0]) y el derecho (link[1]).
NodoBtree* CrearNodoBTree(Persona val, NodoBtree* hijo, NodoBtree* root)
{
    NodoBtree* NuevoNodo = new NodoBtree;
    NuevoNodo->val[1] = val;
    NuevoNodo->count = 1;
    NuevoNodo->link[0] = root;
    NuevoNodo->link[1] = hijo;
    return NuevoNodo;
}

// Coloca una nueva clave dentro de un nodo que aun tiene espacio.
// Mueve hacia la derecha las claves y enlaces posteriores a la posicion.
// Inserta la nueva clave con su hijo y aumenta el contador.
void ColocarNodoBTree(Persona val, int pos, NodoBtree* node, NodoBtree* hijo)
{
    int j = node->count;
    while (j > pos)
    {
        node->val[j + 1] = node->val[j];
        node->link[j + 1] = node->link[j];
        j--;
    }
    node->val[j + 1] = val;
    node->link[j + 1] = hijo;
    node->count++;
}

// Divide un nodo B cuando ya no cabe otra clave.
// Selecciona la mediana, mueve las claves mayores al nodo nuevo
// y deja preparada la clave central para subir al nodo padre.
// Los enlaces de los hijos tambien se distribuyen entre los dos nodos.
void PartirNodoBTree(Persona val, Persona* pval, int pos, NodoBtree* node, NodoBtree* hijo, NodoBtree** NuevoNodo)
{
    int median = (pos > MIN) ? (MIN + 1) : MIN;
    *NuevoNodo = new NodoBtree;
    int j = median + 1;
    while (j <= MAX)
    {
        (*NuevoNodo)->val[j - median] = node->val[j];
        (*NuevoNodo)->link[j - median] = node->link[j];
        j++;
    }
    node->count = median;
    (*NuevoNodo)->count = MAX - median;

    if (pos <= MIN) ColocarNodoBTree(val, pos, node, hijo);
    else ColocarNodoBTree(val, pos - median, *NuevoNodo, hijo);

    *pval = node->val[node->count];
    (*NuevoNodo)->link[0] = node->link[node->count];
    node->count--;
}

// Busca de forma recursiva el nodo donde corresponde insertar la cedula.
// Si llega a un enlace vacio, devuelve la nueva clave al nivel anterior.
// Si hay espacio en el nodo, la coloca; si esta lleno, lo divide.
// El valor de retorno indica si debe propagarse una clave hacia arriba.
int SetValorNodoBTree(Persona val, Persona* pval, NodoBtree* node, NodoBtree** hijo)
{
    int pos;
    if (!node)
    {
        *pval = val;
        *hijo = NULL;
        return 1;
    }

    if (val.cedula < node->val[1].cedula)
    {
        pos = 0;
    }
    else
    {
        for (pos = node->count; (val.cedula < node->val[pos].cedula && pos > 1); pos--);
        if (val.cedula == node->val[pos].cedula) return 0;
    }

    if (SetValorNodoBTree(val, pval, node->link[pos], hijo))
    {
        if (node->count < MAX) ColocarNodoBTree(*pval, pos, node, *hijo);
        else
        {
            PartirNodoBTree(*pval, pval, pos, node, *hijo, hijo);
            return 1;
        }
    }
    return 0;
}

// Inicia la insercion en el arbol B.
// Si la insercion provoca que una clave suba por encima de la raiz,
// se crea una raiz nueva para mantener la estructura del arbol.
void InsertarBTree(NodoBtree*& root, Persona p)
{
    Persona i;
    NodoBtree* hijo;
    int flag = SetValorNodoBTree(p, &i, root, &hijo);
    if (flag) root = CrearNodoBTree(i, hijo, root);
}

// Busca una cedula dentro del arbol B.
// Revisa las claves del nodo actual en orden; si no coincide ninguna,
// baja por el enlace que corresponde al intervalo de la cedula.
// Cuando i es la primera clave mayor, se toma link[i-1].
// Si ninguna es mayor, se toma el ultimo hijo (link[count]).
// Devuelve el registro cuando lo encuentra o NULL si no esta.
Persona* BuscarBTree(NodoBtree* root, long long cedula)
{
    NodoBtree* actual = root;
    while (actual != NULL)
    {
        int i;
        for (i = 1; i <= actual->count; i++)
        {
            if (actual->val[i].cedula == cedula) return &(actual->val[i]);
            if (cedula < actual->val[i].cedula) break;
        }
        actual = actual->link[i - 1];
    }
    return NULL;
}

// Muestra los registros del arbol B ordenados por cedula.
// Para cada clave se recorre primero el hijo de la izquierda,
// despues se imprime el registro; al final se visita el ultimo hijo.
void ListarBTree(NodoBtree* root)
{
    if (root)
    {
        int i;
        for (i = 0; i < root->count; i++)
        {
            ListarBTree(root->link[i]);
            cout << root->val[i + 1].cedula << "\t" << root->val[i + 1].nombre << " " << root->val[i + 1].apellido1 << " " << root->val[i + 1].apellido2 << "\n";
        }
        ListarBTree(root->link[i]);
    }
}

// Elimina todos los nodos del arbol B.
// Como cada nodo puede tener varios hijos, los libera en un ciclo
// y despues elimina el nodo actual.
void DestruirBTree(NodoBtree*& root)
{
    if (root != NULL)
    {
        for (int i = 0; i <= root->count; i++)
        {
            DestruirBTree(root->link[i]);
        }
        delete root;
        root = NULL;
    }
}

// Lectura de archivos del padron.
// Abre el archivo de texto y lee una linea por cada registro.
// sscanf_s separa las columnas por comas y guarda cada campo en su arreglo.
// Luego se limpian los espacios finales, se forma una Persona y se inserta
// en la lista. La lectura se detiene al alcanzar el limite indicado.
int CargarListaMaestraASCII(const char* ruta, NodoLista*& lista, int maxRegistros)
{
    if (maxRegistros <= 0)
    {
        cout << "La cantidad de registros debe ser mayor que cero.\n";
        return 0;
    }

    FILE* archivo = NULL;
    fopen_s(&archivo, ruta, "r");

    if (archivo == NULL)
    {
        cout << "Error: No se pudo abrir el archivo de texto " << ruta << "\n";
        return 0;
    }

    char linea[400];
    int contador = 0;

    while (fgets(linea, sizeof(linea), archivo) != NULL && contador < maxRegistros)
    {
        char sCedula[30] = "", codElec[30] = "", vacio[30] = "", fCad[30] = "";
        char otro[30] = "", nom[50] = "", ap1[50] = "", ap2[50] = "";

        int leidos = sscanf_s(linea, "%29[^,],%29[^,],%29[^,],%29[^,],%29[^,],%49[^,],%49[^,],%49[^\n\r]",
            sCedula, (unsigned)sizeof(sCedula),
            codElec, (unsigned)sizeof(codElec),
            vacio, (unsigned)sizeof(vacio),
            fCad, (unsigned)sizeof(fCad),
            otro, (unsigned)sizeof(otro),
            nom, (unsigned)sizeof(nom),
            ap1, (unsigned)sizeof(ap1),
            ap2, (unsigned)sizeof(ap2));

        // Solo se usan lineas con los ocho campos y datos que caben en Persona.
        if (leidos == 8 && strlen(codElec) < sizeof(Persona::codigoElectoral)
            && strlen(fCad) < sizeof(Persona::fechaCaducidad)
            && strlen(nom) < sizeof(Persona::nombre)
            && strlen(ap1) < sizeof(Persona::apellido1)
            && strlen(ap2) < sizeof(Persona::apellido2))
        {
            LimpiarEspacios(nom);
            LimpiarEspacios(ap1);
            LimpiarEspacios(ap2);
            LimpiarEspacios(codElec);
            LimpiarEspacios(fCad);

            Persona p;
            p.cedula = _atoi64(sCedula);
            strcpy_s(p.codigoElectoral, sizeof(p.codigoElectoral), codElec);
            strcpy_s(p.fechaCaducidad, sizeof(p.fechaCaducidad), fCad);
            strcpy_s(p.nombre, sizeof(p.nombre), nom);
            strcpy_s(p.apellido1, sizeof(p.apellido1), ap1);
            strcpy_s(p.apellido2, sizeof(p.apellido2), ap2);

            InsertarInicioLista(lista, p);
            contador++;
        }
    }
    fclose(archivo);
    cout << "Registros cargados en la lista: " << contador << "\n";
    return contador;
}

// Genera un archivo binario a partir de los registros del texto.
// Primero carga una lista temporal, despues escribe cada Persona con fwrite.
// El modo "wb" crea el archivo binario, y al terminar se libera la lista.
bool ConvertirTextoABinario(const char* rutaTexto, const char* rutaBinario, int maxRegistros)
{
    NodoLista* tempLista = NULL;
    int cantidad = CargarListaMaestraASCII(rutaTexto, tempLista, maxRegistros);
    if (cantidad == 0)
    {
        cout << "No hay registros para generar el archivo binario.\n";
        return false;
    }

    FILE* archivoBin = NULL;
    fopen_s(&archivoBin, rutaBinario, "wb");

    if (archivoBin != NULL)
    {
        NodoLista* aux = tempLista;
        while (aux != NULL)
        {
            fwrite(&(aux->dato), sizeof(Persona), 1, archivoBin);
            aux = aux->siguiente;
        }
        fclose(archivoBin);
        cout << "Archivo binario creado: " << rutaBinario << "\n";
    }

    bool creado = (archivoBin != NULL);
    DestruirLista(tempLista);
    return creado;
}

// Recupera los registros guardados en el archivo binario.
// Con fread lee una Persona completa por vez y la agrega a la lista.
// El modo "rb" permite abrir el archivo para lectura binaria.
void CargarListaMaestraBinario(const char* rutaBinario, NodoLista*& lista)
{
    FILE* archivoBin = NULL;
    fopen_s(&archivoBin, rutaBinario, "rb");

    if (archivoBin == NULL)
    {
        cout << "Error: No se pudo abrir el archivo binario " << rutaBinario << "\n";
        return;
    }

    Persona p;
    while (fread(&p, sizeof(Persona), 1, archivoBin) == 1)
    {
        InsertarInicioLista(lista, p);
    }
    fclose(archivoBin);
}

// Muestra el resultado de una busqueda sin repetir codigo en el menu.
// Se llama despues de medir el tiempo para no incluir la impresion.
void MostrarResultadoBusqueda(const char* estructura, Persona* p)
{
    cout << "\n" << estructura << ":\n";
    if (p == NULL)
    {
        cout << "  Cedula no encontrada.\n";
        return;
    }

    cout << "  Cedula: " << p->cedula << "\n";
    cout << "  Nombre: " << p->nombre << " " << p->apellido1 << " " << p->apellido2 << "\n";
    cout << "  Codigo electoral: " << p->codigoElectoral << "\n";
    cout << "  Fecha de caducidad: " << p->fechaCaducidad << "\n";
}

// Muestra los tiempos de carga, listado, busqueda y liberacion de memoria.
// Cada fila corresponde a una estructura de datos.
// Se conservan las unidades usadas en las mediciones originales.
void MostrarReporte(ReporteGeneral reporte)
{
    cout << "\n--------------------------------------------------------------------\n";
    cout << "                      Tiempos de ejecucion\n";
    cout << "--------------------------------------------------------------------\n";
    cout << "Unidades: carga, listado y liberacion en microsegundos; busqueda en nanosegundos.\n";
    cout << "Estructura\t\tCarga\t\tListado\t\tBusqueda\tLiberar\n";
    cout << "--------------------------------------------------------------------\n";

    // Se usan dos decimales para mostrar los tiempos medidos.
    cout << fixed << setprecision(2);
    cout << "Lista Enlazada\t\t" << reporte.lista.carga << "\t\t" << reporte.lista.listado << "\t\t" << reporte.lista.busqueda << "\t\t" << reporte.lista.destruccion << "\n";

    cout << "Arbol ABB\t\t" << reporte.abb.carga << "\t\t" << reporte.abb.listado << "\t\t" << reporte.abb.busqueda << "\t\t" << reporte.abb.destruccion << "\n";

    cout << "Arbol AVL\t\t" << reporte.avl.carga << "\t\t" << reporte.avl.listado << "\t\t" << reporte.avl.busqueda << "\t\t" << reporte.avl.destruccion << "\n";

    cout << "Arbol BTree\t\t" << reporte.btree.carga << "\t\t" << reporte.btree.listado << "\t\t" << reporte.btree.busqueda << "\t\t" << reporte.btree.destruccion << "\n";

    cout << "--------------------------------------------------------------------\n";
}

// Programa principal.
// Mantiene los punteros de las cuatro estructuras y un menu de opciones.
// La variable cargado evita cargar los mismos datos dos veces sin liberar.
// chrono toma la hora antes y despues de cada operacion para calcular su duracion.
// El reporte guarda el ultimo tiempo medido de cada operacion.
int main()
{
    NodoLista* listaMaestra = NULL;
    NodoABB* arbolABB = NULL;
    NodoAVL* arbolAVL = NULL;
    NodoBtree* arbolBTree = NULL;

    ReporteGeneral reporte;
    bool cargado = false;
    int opcion = -1;
    int limiteRegistros = 50000;

    do
    {
        cout << "\n------------------------------\n";
        cout << " Padron electoral - Proyecto 1B \n";
        cout << "------------------------------\n";
        cout << "1. Cargar registros\n";
        cout << "2. Mostrar registros\n";
        cout << "3. Buscar por cedula\n";
        cout << "4. Liberar memoria\n";
        cout << "5. Ver tiempos\n";
        cout << "6. Probar carga desde archivo .DAT\n";
        cout << "0. Salir\n";
        cout << "Opcion: ";

        if (scanf_s("%d", &opcion) != 1) break;

        switch (opcion)
        {
            // Opcion 1: carga la lista y construye los tres arboles.
        case 1:
        {
            if (cargado)
            {
                cout << "\nLos registros ya estan cargados.\n";
                break;
            }

            cout << "\nCantidad de registros a cargar: ";
            if (scanf_s("%d", &limiteRegistros) != 1 || limiteRegistros <= 0)
            {
                cout << "Cantidad no valida.\n";
                return 0;
            }

            cout << "\nCargando " << limiteRegistros << " registros desde PADRON_COMPLETO.txt...\n";
            auto inicioL = high_resolution_clock::now();
            int cantidadCargada = CargarListaMaestraASCII("PADRON_COMPLETO.txt", listaMaestra, limiteRegistros);
            auto finL = high_resolution_clock::now();
            if (cantidadCargada == 0)
            {
                cout << "No se cargaron registros. Revise el archivo de entrada.\n";
                break;
            }
            reporte.lista.carga = duration_cast<microseconds>(finL - inicioL).count();

            cout << "Cargando arbol ABB...\n";
            auto inicioABB = high_resolution_clock::now();
            NodoLista* aux = listaMaestra;
            while (aux != NULL)
            {
                InsertarABB(arbolABB, aux->dato);
                aux = aux->siguiente;
            }
            auto finABB = high_resolution_clock::now();
            reporte.abb.carga = duration_cast<microseconds>(finABB - inicioABB).count();

            cout << "Cargando arbol AVL...\n";
            auto inicioAVL = high_resolution_clock::now();
            aux = listaMaestra;
            while (aux != NULL)
            {
                arbolAVL = InsertarAVL(arbolAVL, aux->dato);
                aux = aux->siguiente;
            }
            auto finAVL = high_resolution_clock::now();
            reporte.avl.carga = duration_cast<microseconds>(finAVL - inicioAVL).count();

            cout << "Cargando arbol B...\n";
            auto inicioBTree = high_resolution_clock::now();
            aux = listaMaestra;
            while (aux != NULL)
            {
                InsertarBTree(arbolBTree, aux->dato);
                aux = aux->siguiente;
            }
            auto finBTree = high_resolution_clock::now();
            reporte.btree.carga = duration_cast<microseconds>(finBTree - inicioBTree).count();

            cargado = true;
            cout << "\nCarga terminada. Registros leidos: " << cantidadCargada << "\n";
            break;
        }
        // Opcion 2: muestra cada estructura y mide cuanto tarda.
        // La impresion en consola tambien forma parte de ese tiempo.
        case 2:
        {
            if (!cargado) { cout << "\nPrimero cargue los registros con la opcion 1.\n"; break; }

            cout << "\nLista enlazada\n";
            auto t1 = high_resolution_clock::now();
            ListarLista(listaMaestra);
            auto t2 = high_resolution_clock::now();
            reporte.lista.listado = duration_cast<microseconds>(t2 - t1).count();

            cout << "\nArbol ABB\n";
            auto t3 = high_resolution_clock::now();
            ListarABB(arbolABB);
            auto t4 = high_resolution_clock::now();
            reporte.abb.listado = duration_cast<microseconds>(t4 - t3).count();

            cout << "\nArbol AVL\n";
            auto t5 = high_resolution_clock::now();
            ListarAVL(arbolAVL);
            auto t6 = high_resolution_clock::now();
            reporte.avl.listado = duration_cast<microseconds>(t6 - t5).count();

            cout << "\nArbol B\n";
            auto t7 = high_resolution_clock::now();
            ListarBTree(arbolBTree);
            auto t8 = high_resolution_clock::now();
            reporte.btree.listado = duration_cast<microseconds>(t8 - t7).count();

            break;
        }
        // Opcion 3: busca la misma cedula en las cuatro estructuras.
        // Se registra el tiempo por separado y se muestra el resultado de cada estructura.
        case 3:
        {
            if (!cargado) { cout << "\nPrimero cargue los registros con la opcion 1.\n"; break; }

            long long cedula;
            cout << "\nDigite el numero de cedula a buscar: ";
            if (scanf_s("%lld", &cedula) != 1)
            {
                cout << "Cedula no valida.\n";
                return 0;
            }

            auto t1 = high_resolution_clock::now();
            Persona* pL = BuscarLista(listaMaestra, cedula);
            auto t2 = high_resolution_clock::now();
            reporte.lista.busqueda = duration_cast<nanoseconds>(t2 - t1).count();

            auto t3 = high_resolution_clock::now();
            Persona* pABB = BuscarABB(arbolABB, cedula);
            auto t4 = high_resolution_clock::now();
            reporte.abb.busqueda = duration_cast<nanoseconds>(t4 - t3).count();

            auto t5 = high_resolution_clock::now();
            Persona* pAVL = BuscarAVL(arbolAVL, cedula);
            auto t6 = high_resolution_clock::now();
            reporte.avl.busqueda = duration_cast<nanoseconds>(t6 - t5).count();

            auto t7 = high_resolution_clock::now();
            Persona* pBT = BuscarBTree(arbolBTree, cedula);
            auto t8 = high_resolution_clock::now();
            reporte.btree.busqueda = duration_cast<nanoseconds>(t8 - t7).count();

            // Se muestra el resultado de cada estructura por separado.
            cout << "\nResultados de la busqueda:\n";
            MostrarResultadoBusqueda("Lista enlazada", pL);
            MostrarResultadoBusqueda("Arbol ABB", pABB);
            MostrarResultadoBusqueda("Arbol AVL", pAVL);
            MostrarResultadoBusqueda("Arbol B", pBT);
            break;
        }
        // Opcion 4: mide la liberacion de memoria de cada estructura.
        case 4:
        {
            if (!cargado) { cout << "\nNo hay estructuras cargadas.\n"; break; }

            cout << "\nLiberando memoria...\n";

            auto t1 = high_resolution_clock::now();
            DestruirLista(listaMaestra);
            auto t2 = high_resolution_clock::now();
            reporte.lista.destruccion = duration_cast<microseconds>(t2 - t1).count();

            auto t3 = high_resolution_clock::now();
            PodarHojasABB(arbolABB);
            auto t4 = high_resolution_clock::now();
            reporte.abb.destruccion = duration_cast<microseconds>(t4 - t3).count();

            auto t5 = high_resolution_clock::now();
            DestruirAVL(arbolAVL);
            auto t6 = high_resolution_clock::now();
            reporte.avl.destruccion = duration_cast<microseconds>(t6 - t5).count();

            auto t7 = high_resolution_clock::now();
            DestruirBTree(arbolBTree);
            auto t8 = high_resolution_clock::now();
            reporte.btree.destruccion = duration_cast<microseconds>(t8 - t7).count();

            cargado = false;
            cout << "Memoria liberada.\n";
            break;
        }
        // Opcion 5: presenta los ultimos tiempos guardados.
        case 5:
            MostrarReporte(reporte);
            break;

            // Opcion 6: compara la lectura del archivo de texto con el .DAT.
        case 6:
        {
            cout << "\nPrueba con archivo binario (.DAT)\n";
            if (!ConvertirTextoABinario("PADRON_COMPLETO.txt", "PADRON_COMPLETO.DAT", limiteRegistros))
            {
                cout << "No se pudo preparar la prueba binaria.\n";
                break;
            }

            NodoLista* listaBin = NULL;
            auto inicioB = high_resolution_clock::now();
            CargarListaMaestraBinario("PADRON_COMPLETO.DAT", listaBin);
            auto finB = high_resolution_clock::now();
            double tBin = duration_cast<microseconds>(finB - inicioB).count();

            if (cargado)
                cout << "Tiempo de carga archivo Texto (ASCII): " << reporte.lista.carga << " microsegundos\n";
            else
                cout << "Para comparar el tiempo ASCII, primero use la opcion 1.\n";
            cout << "Tiempo de carga archivo Binario (.DAT): " << tBin << " microsegundos\n";

            DestruirLista(listaBin);
            break;
        }
        // Opcion 0: termina y libera las estructuras si aun estan cargadas.
        case 0:
            if (cargado)
            {
                DestruirLista(listaMaestra);
                PodarHojasABB(arbolABB);
                DestruirAVL(arbolAVL);
                DestruirBTree(arbolBTree);
            }
            cout << "\nFin del programa.\n";
            break;

        default:
            cout << "\nOpcion incorrecta.\n";
            break;
        }

    } while (opcion != 0);

    return 0;
}


//Arbol AVL Original 
/*
struct Nodo
{
    int llave;
    struct Nodo *izq;
    struct Nodo *derecha;
    int altura;
};

//obtiene el maximo de dos numeros
int max(int a, int b);

//obtiene la altura del arbol
int altura(struct Nodo *N)
{
    if (N == NULL)
        return 0;
    return N->altura;
}


int max(int a, int b)
{
    return (a > b) ? a : b;
}


struct Nodo* nuevoNodo(int llave)
{
    struct Nodo* Nodo = (struct Nodo*)
        malloc(sizeof(struct Nodo));
    Nodo->llave = llave;
    Nodo->izq = NULL;
    Nodo->derecha = NULL;
    Nodo->altura = 1;  //nuevonodo es agregado como hoja
    return(Nodo);
}


struct Nodo *RotarDerecha(struct Nodo *y)
{
    struct Nodo *x = y->izq;
    struct Nodo *T2 = x->derecha;

    // hace rotacion
    x->derecha = y;
    y->izq = T2;

    // actualiza alturas
    y->altura = max(altura(y->izq), altura(y->derecha)) + 1;
    x->altura = max(altura(x->izq), altura(x->derecha)) + 1;

    // retorna nueva raiz
    return x;
}


struct Nodo *RotarIzquierda(struct Nodo *x)
{
    struct Nodo *y = x->derecha;
    struct Nodo *T2 = y->izq;

    // hace rotacion
    y->izq = x;
    x->derecha = T2;

    //  actualiza alturas
    x->altura = max(altura(x->izq), altura(x->derecha)) + 1;
    y->altura = max(altura(y->izq), altura(y->derecha)) + 1;

    // retorna nueva raiz
    return y;
}

// obtiene balance
int obtenerbalance(struct Nodo *N)
{
    if (N == NULL)
        return 0;
    return altura(N->izq) - altura(N->derecha);
}


struct Nodo* insertarAVL(struct Nodo* Nodo, int llave)
{
/*	 1. Insercion normal
    if (Nodo == NULL)
        return(nuevoNodo(llave));

    if (llave < Nodo->llave)
        Nodo->izq = insertarAVL(Nodo->izq, llave);
    else if (llave > Nodo->llave)
        Nodo->derecha = insertarAVL(Nodo->derecha, llave);
    else // no se permiten llaves iguales
        return Nodo;

     2. actualiza altura
    Nodo->altura = 1 + max(altura(Nodo->izq),
        altura(Nodo->derecha));

     3. verifica si el arbol se desbalancea
    int balance = obtenerbalance(Nodo);

    // 4 casos para un arbol desbalanceado

    // izq izq
    if (balance > 1 && llave < Nodo->izq->llave)
        return RotarDerecha(Nodo);

    // derecha derecha
    if (balance < -1 && llave > Nodo->derecha->llave)
        return RotarIzquierda(Nodo);

    // izq derecha
    if (balance > 1 && llave > Nodo->izq->llave)
    {
        Nodo->izq = RotarIzquierda(Nodo->izq);
        return RotarDerecha(Nodo);
    }

    // derecha izq
    if (balance < -1 && llave < Nodo->derecha->llave)
    {
        Nodo->derecha = RotarDerecha(Nodo->derecha);
        return RotarIzquierda(Nodo);
    }


    return Nodo;
}
*/



