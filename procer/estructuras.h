/*

procer.h

*/


#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include "commons/collections/queue.h"
#include "commons/collections/list.h"
#include "commons/config.h"
#include "commons/log.h"

#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

typedef struct t_variable{
	char nombre;
	int valor;
} variable;

typedef struct t_etiqueta{
	char *nombre;
	int posicion;
} etiqueta;

typedef struct t_funcion{
	char *nombre;
	int inicio;
} funcion;

typedef struct t_codigo{
	t_list* lineas;
} segmento_codigo;

typedef struct t_informacion_stack{
	char *nombre;	
	int pc_actual;
} informacion_stack;

typedef struct t_proceso{
	int id;
	int program_counter;
	int prioridad;
	int tiempo_io;
	int ultimo_q;
	int primera_estimacion_q; // esta bien usar esta variable? como la calculamos?
	float estimacion_anterior_q;
	float estimacion_q;
	t_list* funciones;
	t_list* etiquetas;
	t_list* datos;	
	segmento_codigo codigo;
	t_list* stack;
	int estado;
} proceso;

typedef struct t_cola{
	sem_t tamanio;
	pthread_mutex_t mutex;
	t_queue *procesos;
} cola;


typedef struct t_pi{
	int id;
	int prioridad;
	int estado;
	t_list *codigo;
} pi;

typedef struct t_lista{
	sem_t tamanio;
	pthread_mutex_t mutex;
	t_list * elementos;
} lista;

typedef struct t_posibleBusqueda{
  char *nombre;
  int entero;
} posibleBusqueda;

typedef	struct DATA_SISTEMA{
	int estadoSistema;
	int puertoEscucha;
	int procesos_en_sitema;
	int max_procesos_en_sitema;
	pthread_mutex_t mutex_actualizacion_algoritmo;
	int actualizacion_algoritmo;
	int algoritmo;
	pthread_mutex_t mutex_actualizacion_prioridad;
	int actualizacion_prioridad;
	int prioridad_nuevos;
	int prioridad_reanudados;
	int prioridad_fin_q;
	int prioridad_fin_io;
	pthread_mutex_t mutex_actualizacion_colas;
	pthread_mutex_t mutex_multiprogramacion;
	pthread_mutex_t mutex_sts;

	t_log* log;
	pthread_mutex_t mutex_log;

	int quantum;
	float alpha;
	int sleep;
	int sleep_io;
	int valor_multiprogramacion;
	int max_valor_multiprogramacion;
	int iot_sistema;
	pthread_mutex_t mutex_iot_ocupados;
	int iot_ocupados;
	cola reanudados;
	cola nuevos;
	cola fin_q;
	cola fin_io;
	cola listos;
	cola bloqueados;
	lista suspendidos;
} data;



#endif /*ESTRUCTURAS_H_*/
