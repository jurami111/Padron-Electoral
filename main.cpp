#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>

using namespace std;
using namespace chrono;

// ============================================================================
// ESTRUCTURA PRINCIPAL DE DATOS (PERSONA)
// ============================================================================
struct Persona
{
    long long cedula;
    char codigoElectoral[10];
    char fechaCaducidad[10];
    char nombre[40];
    char apellido1[40];
    char apellido2[40];
};

// ============================================================================
// ESTRUCTURAS DE TIEMPOS Y REPORTE GENERAL
// ============================================================================
struct TiempoOperacion
{
    double carga = 0;
    double listado = 0;
    double busqueda = 0;
    double destruccion = 0;
};

struct ReporteGeneral
{
    TiempoOperacion lista;
    TiempoOperacion abb;
    TiempoOperacion avl;
    TiempoOperacion btree;
};

// ============================================================================
// FUNCION AUXILIAR DE LIMPIEZA DE ESPACIOS EN BLANCO (TRIM)
// ============================================================================
void LimpiarEspacios(char* str)
{
    int i = (int)strlen(str) - 1;
    while (i >= 0 && (str[i] == ' ' || str[i] == '\t' || str[i] == '\n' || str[i] == '\r'))
    {
        str[i] = '\0';
        i--;
    }
}

// ============================================================================
// 1. LISTA ENLAZADA MAESTRA
// ============================================================================
struct NodoLista
{
    Persona dato;
    NodoLista* siguiente;
};

// Inserta una persona al inicio de la lista
void InsertarInicioLista(NodoLista*& cabeza, Persona p)
{
    NodoLista* nuevo = new NodoLista;
    nuevo->dato = p;
    nuevo->siguiente = cabeza;
    cabeza = nuevo;
}

// Busca un registro por cedula en la lista secuencial
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

// Imprime los elementos de la lista en consola
void ListarLista(NodoLista* cabeza)
{
    NodoLista* aux = cabeza;
    while (aux != NULL)
    {
        printf("%lld\t%s %s %s\n",
            aux->dato.cedula, aux->dato.nombre, aux->dato.apellido1, aux->dato.apellido2);
        aux = aux->siguiente;
    }
}

// Libera toda la memoria de la lista
void DestruirLista(NodoLista*& cabeza)
{
    while (cabeza != NULL)
    {
        NodoLista* temp = cabeza;
        cabeza = cabeza->siguiente;
        delete temp;
    }
}

// ============================================================================
// 2. ARBOL BINARIO DE BUSQUEDA (ABB)
// ============================================================================
struct NodoABB
{
    Persona dato;
    NodoABB* izq;
    NodoABB* der;
};

// Insercion recursiva en arbol binario comun
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

// Busqueda recursiva en ABB
Persona* BuscarABB(NodoABB* raiz, long long cedula)
{
    if (raiz == NULL) return NULL;
    if (cedula == raiz->dato.cedula) return &(raiz->dato);

    if (cedula < raiz->dato.cedula)
        return BuscarABB(raiz->izq, cedula);
    else
        return BuscarABB(raiz->der, cedula);
}

// Recorrido InOrden para mostrar los datos ordenados
void ListarABB(NodoABB* raiz)
{
    if (raiz != NULL)
    {
        ListarABB(raiz->izq);
        printf("%lld\t%s %s %s\n",
            raiz->dato.cedula, raiz->dato.nombre, raiz->dato.apellido1, raiz->dato.apellido2);
        ListarABB(raiz->der);
    }
}

// Libera memoria en orden PostOrden
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

// ============================================================================
// 3. ARBOL AVL
// ============================================================================
struct NodoAVL
{
    Persona dato;
    NodoAVL* izq;
    NodoAVL* der;
    int altura;
};

int AlturaNodo(NodoAVL* n) { return (n == NULL) ? 0 : n->altura; }
int Maximo(int a, int b) { return (a > b) ? a : b; }
int ObtenerBalance(NodoAVL* n) { return (n == NULL) ? 0 : AlturaNodo(n->izq) - AlturaNodo(n->der); }

// Rotacion simple a la derecha
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

// Rotacion simple a la izquierda
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

// Insercion con autorebalanceo AVL
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

// Busqueda en AVL
Persona* BuscarAVL(NodoAVL* nodo, long long cedula)
{
    if (nodo == NULL) return NULL;
    if (cedula == nodo->dato.cedula) return &(nodo->dato);

    if (cedula < nodo->dato.cedula)
        return BuscarAVL(nodo->izq, cedula);
    else
        return BuscarAVL(nodo->der, cedula);
}

// Recorrido InOrden en AVL
void ListarAVL(NodoAVL* nodo)
{
    if (nodo != NULL)
    {
        ListarAVL(nodo->izq);
        printf("%lld\t%s %s %s\n",
            nodo->dato.cedula, nodo->dato.nombre, nodo->dato.apellido1, nodo->dato.apellido2);
        ListarAVL(nodo->der);
    }
}

// Destruccion recursiva del AVL
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

// ============================================================================
// 4. ARBOL BTREE (ORDEN 5: MAX 4, MIN 2)
// ============================================================================
#define MAX 4
#define MIN 2

struct NodoBtree
{
    Persona val[MAX + 1];
    int count;
    NodoBtree* link[MAX + 1];
};

NodoBtree* CrearNodoBTree(Persona val, NodoBtree* hijo, NodoBtree* root)
{
    NodoBtree* NuevoNodo = new NodoBtree;
    NuevoNodo->val[1] = val;
    NuevoNodo->count = 1;
    NuevoNodo->link[0] = root;
    NuevoNodo->link[1] = hijo;
    return NuevoNodo;
}

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

void InsertarBTree(NodoBtree*& root, Persona p)
{
    Persona i;
    NodoBtree* hijo;
    int flag = SetValorNodoBTree(p, &i, root, &hijo);
    if (flag) root = CrearNodoBTree(i, hijo, root);
}

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

void ListarBTree(NodoBtree* root)
{
    if (root)
    {
        int i;
        for (i = 0; i < root->count; i++)
        {
            ListarBTree(root->link[i]);
            printf("%lld\t%s %s %s\n",
                root->val[i + 1].cedula, root->val[i + 1].nombre, root->val[i + 1].apellido1, root->val[i + 1].apellido2);
        }
        ListarBTree(root->link[i]);
    }
}

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

// ============================================================================
// LECTURA DE PADRON DE TEXTO Y MANEJO BINARIO (.DAT)
// ============================================================================
void CargarListaMaestraASCII(const char* ruta, NodoLista*& lista, int maxRegistros)
{
    FILE* archivo = NULL;
    fopen_s(&archivo, ruta, "r");

    if (archivo == NULL)
    {
        printf("Error: No se pudo abrir el archivo de texto %s\n", ruta);
        return;
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

        if (leidos >= 7)
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
    printf("Se cargaron exitosamente %d registros en la Lista Maestra.\n", contador);
}

void ConvertirTextoABinario(const char* rutaTexto, const char* rutaBinario, int maxRegistros)
{
    NodoLista* tempLista = NULL;
    CargarListaMaestraASCII(rutaTexto, tempLista, maxRegistros);

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
        printf("Archivo binario generado correctamente (%s).\n", rutaBinario);
    }

    DestruirLista(tempLista);
}

void CargarListaMaestraBinario(const char* rutaBinario, NodoLista*& lista)
{
    FILE* archivoBin = NULL;
    fopen_s(&archivoBin, rutaBinario, "rb");

    if (archivoBin == NULL)
    {
        printf("Error: No se pudo abrir el archivo binario %s\n", rutaBinario);
        return;
    }

    Persona p;
    while (fread(&p, sizeof(Persona), 1, archivoBin) == 1)
    {
        InsertarInicioLista(lista, p);
    }
    fclose(archivoBin);
}

// ============================================================================
// DESPLIEGUE DEL REPORTE DE TIEMPOS
// ============================================================================
void MostrarReporte(ReporteGeneral reporte)
{
    printf("\n========================================================================================\n");
    printf("                               REPORTE GENERAL DE TIEMPOS                               \n");
    printf("========================================================================================\n");
    printf("Estructura\t\tCarga (us)\tListado (us)\tBusqueda (ns)\tDestruccion (us)\n");
    printf("----------------------------------------------------------------------------------------\n");

    printf("Lista Enlazada\t\t%.2f\t\t%.2f\t\t%.2f\t\t%.2f\n",
        reporte.lista.carga, reporte.lista.listado, reporte.lista.busqueda, reporte.lista.destruccion);

    printf("Arbol ABB\t\t%.2f\t\t%.2f\t\t%.2f\t\t%.2f\n",
        reporte.abb.carga, reporte.abb.listado, reporte.abb.busqueda, reporte.abb.destruccion);

    printf("Arbol AVL\t\t%.2f\t\t%.2f\t\t%.2f\t\t%.2f\n",
        reporte.avl.carga, reporte.avl.listado, reporte.avl.busqueda, reporte.avl.destruccion);

    printf("Arbol BTree\t\t%.2f\t\t%.2f\t\t%.2f\t\t%.2f\n",
        reporte.btree.carga, reporte.btree.listado, reporte.btree.busqueda, reporte.btree.destruccion);

    printf("========================================================================================\n");
}

// ============================================================================
// PROGRAMA PRINCIPAL
// ============================================================================
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
        printf("\n=================================\n");
        printf(" PADRON ELECTORAL - PROYECTO 1B \n");
        printf("=================================\n");
        printf("1. Cargar Datos y Estructuras (con Limite N de Registros)\n");
        printf("2. Listar Estructuras (Tomar Tiempos)\n");
        printf("3. Buscar Cedula (Tomar Tiempos)\n");
        printf("4. Destruir / Liberar Memoria (Tomar Tiempos)\n");
        printf("5. Mostrar Reporte General de Tiempos\n");
        printf("6. Prueba Extra: Carga desde Archivo Binario (.DAT)\n");
        printf("0. Salir\n");
        printf("Opcion: ");

        if (scanf_s("%d", &opcion) != 1) break;

        switch (opcion)
        {
        case 1:
        {
            if (cargado)
            {
                printf("\nEl padron ya se encuentra cargado en memoria.\n");
                break;
            }

            printf("\nDigite la cantidad maxima de registros a cargar (ej. 10000, 50000, 100000): ");
            scanf_s("%d", &limiteRegistros);

            printf("\na. Cargando %d registros de PADRON_COMPLETO.txt en Lista Maestra...\n", limiteRegistros);
            auto inicioL = high_resolution_clock::now();
            CargarListaMaestraASCII("PADRON_COMPLETO.txt", listaMaestra, limiteRegistros);
            auto finL = high_resolution_clock::now();
            reporte.lista.carga = duration_cast<microseconds>(finL - inicioL).count();

            printf("b.i. Cargando en Arbol Binario de Busqueda (ABB)...\n");
            auto inicioABB = high_resolution_clock::now();
            NodoLista* aux = listaMaestra;
            while (aux != NULL)
            {
                InsertarABB(arbolABB, aux->dato);
                aux = aux->siguiente;
            }
            auto finABB = high_resolution_clock::now();
            reporte.abb.carga = duration_cast<microseconds>(finABB - inicioABB).count();

            printf("b.ii. Cargando en Arbol AVL...\n");
            auto inicioAVL = high_resolution_clock::now();
            aux = listaMaestra;
            while (aux != NULL)
            {
                arbolAVL = InsertarAVL(arbolAVL, aux->dato);
                aux = aux->siguiente;
            }
            auto finAVL = high_resolution_clock::now();
            reporte.avl.carga = duration_cast<microseconds>(finAVL - inicioAVL).count();

            printf("b.iii. Cargando en Arbol B-Tree...\n");
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
            printf("\nCarga masiva completada con exito para N = %d elementos.\n", limiteRegistros);
            break;
        }
        case 2:
        {
            if (!cargado) { printf("\nPrimero debe cargar los datos (Opcion 1).\n"); break; }

            printf("\n--- LISTADO LISTA MAESTRA ---\n");
            auto t1 = high_resolution_clock::now();
            ListarLista(listaMaestra);
            auto t2 = high_resolution_clock::now();
            reporte.lista.listado = duration_cast<microseconds>(t2 - t1).count();

            printf("\n--- LISTADO ABB ---\n");
            auto t3 = high_resolution_clock::now();
            ListarABB(arbolABB);
            auto t4 = high_resolution_clock::now();
            reporte.abb.listado = duration_cast<microseconds>(t4 - t3).count();

            printf("\n--- LISTADO AVL ---\n");
            auto t5 = high_resolution_clock::now();
            ListarAVL(arbolAVL);
            auto t6 = high_resolution_clock::now();
            reporte.avl.listado = duration_cast<microseconds>(t6 - t5).count();

            printf("\n--- LISTADO BTREE ---\n");
            auto t7 = high_resolution_clock::now();
            ListarBTree(arbolBTree);
            auto t8 = high_resolution_clock::now();
            reporte.btree.listado = duration_cast<microseconds>(t8 - t7).count();

            break;
        }
        case 3:
        {
            if (!cargado) { printf("\nPrimero debe cargar los datos (Opcion 1).\n"); break; }

            long long cedula;
            printf("\nDigite el numero de cedula a buscar: ");
            scanf_s("%lld", &cedula);

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

            if (pL != NULL)
            {
                printf("\n--- PERSONA ENCONTRADA ---\n");
                printf("Cedula: %lld\n", pL->cedula);
                printf("Nombre: %s %s %s\n", pL->nombre, pL->apellido1, pL->apellido2);
                printf("Codigo Electoral: %s\n", pL->codigoElectoral);
                printf("Fecha Caducidad: %s\n", pL->fechaCaducidad);
            }
            else
            {
                printf("\nNo se encontro la cedula solicitada.\n");
            }
            break;
        }
        case 4:
        {
            if (!cargado) { printf("\nLas estructuras ya estan vacias.\n"); break; }

            printf("\nLiberando memoria de las 4 estructuras...\n");

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
            printf("Memoria liberada exitosamente.\n");
            break;
        }
        case 5:
            MostrarReporte(reporte);
            break;

        case 6:
        {
            printf("\n--- DEMOSTRACION DE EXTRA: MANEJO BINARIO (.DAT) ---\n");
            ConvertirTextoABinario("PADRON_COMPLETO.txt", "PADRON_COMPLETO.DAT", limiteRegistros);

            NodoLista* listaBin = NULL;
            auto inicioB = high_resolution_clock::now();
            CargarListaMaestraBinario("PADRON_COMPLETO.DAT", listaBin);
            auto finB = high_resolution_clock::now();
            double tBin = duration_cast<microseconds>(finB - inicioB).count();

            printf("Tiempo de carga archivo Texto (ASCII): %.2f us\n", reporte.lista.carga);
            printf("Tiempo de carga archivo Binario (.DAT): %.2f us\n", tBin);

            DestruirLista(listaBin);
            break;
        }
        case 0:
            if (cargado)
            {
                DestruirLista(listaMaestra);
                PodarHojasABB(arbolABB);
                DestruirAVL(arbolAVL);
                DestruirBTree(arbolBTree);
            }
            printf("\nSaliendo del programa...\n");
            break;

        default:
            printf("\nOpcion no valida.\n");
            break;
        }

    } while (opcion != 0);

    return 0;
}
