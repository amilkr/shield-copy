#include "estructuras.h"
#include "constantes.h"
#include "lts/parser_lts.h"
#include "lts/descriptor.h"
#include <stdio.h>
#include <string.h>

#ifndef HELPERS_H_
#define HELPERS_H_

void cola_push(struct t_cola * cola, struct t_proceso * proceso);
struct t_proceso * cola_pop(struct t_cola * cola);
struct t_proceso * cola_try_pop(struct t_cola * cola);
int cola_size(struct t_cola * cola);
struct t_proceso * proceso_crear(pi *p);
struct t_pi * pi_crear(int id);
cabecera * crear_cabecera(int tipo,int tamanio);
char * string_cabecera(struct t_cabecera * hh);
cabecera * struct_cabecera(char *s);
variable * sacarDeListaVariables(t_list* lista, char nombreABuscar);
variable * buscarDeListaVariables(t_list* lista, char nombreABuscar);
etiqueta * buscarDeListaEtiquetas(t_list* lista, char *nombreABuscar);
funcion * buscarDeListaFunciones(t_list* lista, char *nombreABuscar);
informacion_stack * buscarDeListaStack(t_list* lista, char *nombreABuscar);
int finCadena(char *cadena, int posicion);
int contiene(char *cadenaDondeBuscar, char *cadenaABuscar);
void agregarSuspendido(struct t_lista *suspendidos , struct t_proceso * proceso);
struct t_proceso * quitarSuspendido(struct t_lista *suspendidos , int id);
char* log_de_suspendido(proceso * estructuraProceso);
int get_config(char * key, t_config * config, int default_value);
float get_float_config(char * key, t_config * config, float default_value);
int get_algoritmo(t_config * config);
void procer_log_info(data* data, char* nombre_hilo, char* mensaje);
void procer_log_debug(data* data, char* nombre_hilo, char* mensaje);
void procer_log_error(data* data, char* nombre_hilo, char* mensaje);
#endif /*HELPERS_H_*/
