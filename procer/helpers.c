#include "helpers.h"
#include <errno.h>

void cola_push(struct t_cola * cola, struct t_proceso * proceso){
  pthread_mutex_lock(&cola->mutex);
  queue_push(cola->procesos, proceso);
  pthread_mutex_unlock(&cola->mutex);
  sem_post(&cola->tamanio);
}

struct t_proceso * cola_pop(struct t_cola * cola){
  sem_wait(&cola->tamanio);
  pthread_mutex_lock(&cola->mutex);
  struct t_proceso *proceso = queue_pop(cola->procesos);
  pthread_mutex_unlock(&cola->mutex);
  return proceso; 
}

struct t_proceso * cola_try_pop(struct t_cola * cola){
  if(sem_trywait(&cola->tamanio)==0){
    pthread_mutex_lock(&cola->mutex);
    struct t_proceso *proceso = queue_pop(cola->procesos);
    pthread_mutex_unlock(&cola->mutex);
    return proceso;   
  }else{
    return NULL;
  }
  
}

int cola_size(struct t_cola * cola){
  pthread_mutex_lock(&cola->mutex);
  int size = queue_size(cola->procesos);
  pthread_mutex_unlock(&cola->mutex);
  return size;
}

struct t_proceso * proceso_crear(pi *p){
  struct t_proceso* proceso = (struct t_proceso*) malloc(sizeof( struct t_proceso ));
  proceso->id = p->id;
  proceso->prioridad = p->prioridad;
  proceso->estado = H_PROCESO_INICIO;
  proceso->ultimo_q = 6;
  proceso->estimacion_anterior_q = 0;
  proceso->codigo.lineas = p->codigo;
  parsearLTS(proceso);
  return proceso;
}

struct t_pi * pi_crear(int id){
  struct t_pi* pi = (struct t_pi*) malloc(sizeof( struct t_pi ));
  pi->id = id;
  pi->estado = 0;
  pi->codigo = NULL;
  return pi;
}

cabecera * crear_cabecera(int tipo,int tamanio){
  cabecera* cabecera = (struct t_cabecera*) malloc(sizeof( struct t_cabecera ));
  cabecera->tipo = tipo;
  cabecera->tamanio = tamanio;
  return cabecera;
}
char * string_cabecera(struct t_cabecera * hh) {
        char *datos=  malloc(100 * sizeof (char));
        memset(datos,'\0',100);
        memcpy(datos,hh , sizeof(struct t_cabecera));
	return datos;
}

cabecera *struct_cabecera(char * s) {
        cabecera *hh = (struct t_cabecera*) malloc(sizeof( struct t_cabecera ));
        memcpy(hh,s, sizeof (struct t_cabecera ));
        return hh;
}

variable * sacarDeListaVariables(t_list* lista, char nombreABuscar){
  bool resultadoBusqueda(posibleBusqueda* busqueda){ return busqueda->nombre == nombreABuscar; };
  bool (* punteroresultadoBusqueda)();
  punteroresultadoBusqueda = resultadoBusqueda;
  posibleBusqueda * variableResultado = (posibleBusqueda *) list_remove_by_condition(lista, punteroresultadoBusqueda);
  return variableResultado;
}

variable * buscarDeListaVariables(t_list* lista, char nombreABuscar){
  bool resultadoBusqueda(variable* busqueda){ return busqueda->nombre == nombreABuscar; };
  bool (* punteroresultadoBusqueda)();
  punteroresultadoBusqueda = resultadoBusqueda;
  variable * variableResultado = list_find(lista, punteroresultadoBusqueda);
  return variableResultado;
}

etiqueta * buscarDeListaEtiquetas(t_list* lista, char * nombreABuscar){
  nombreABuscar[strlen(nombreABuscar)-1]='\0';  
  int resultadoBusqueda(etiqueta * busqueda){ return string_equals_ignore_case(busqueda->nombre, nombreABuscar); };
  etiqueta * variableResultado = list_find(lista, (void *) resultadoBusqueda);
  return variableResultado;
}

funcion * buscarDeListaFunciones(t_list* lista, char * nombreABuscar){
  funcion * primerfuncion = list_get(lista,0);
  int resultadoBusqueda(funcion * busqueda){ return string_equals_ignore_case(busqueda->nombre, nombreABuscar); };
  funcion * variableResultado = list_find(lista, (void *) resultadoBusqueda);
  return variableResultado;
}

informacion_stack * buscarDeListaStack(t_list* lista, char * nombreABuscar){
  nombreABuscar[strlen(nombreABuscar)-1]='\0';
  int resultadoBusqueda(etiqueta * busqueda){ return string_equals_ignore_case(busqueda->nombre, nombreABuscar); };
  etiqueta * variableResultado = list_find(lista, (void *) resultadoBusqueda);
  return variableResultado;
}

int finCadena(char *cadena, int posicion){
    if (posicion >= strlen(cadena)){
      return 1;
    }
  return 0;
}

int contiene(char *cadenaDondeBuscar, char *cadenaABuscar){
  int indiceCadenaDondeBuscar = 0;
  int indiceCadenaABuscar = 0;
  int unaCoincidenciaAlMenos = 0;
  char encontrada = 0;  
  for (indiceCadenaDondeBuscar=0, indiceCadenaABuscar=0; (!(encontrada == 1) && !(finCadena(cadenaDondeBuscar,indiceCadenaDondeBuscar))); indiceCadenaDondeBuscar++){
        if(cadenaDondeBuscar[indiceCadenaDondeBuscar] == cadenaABuscar[indiceCadenaABuscar]){
          indiceCadenaABuscar++;
          if (finCadena(cadenaABuscar,indiceCadenaABuscar)){
            encontrada = 1;
          }
          unaCoincidenciaAlMenos = 1;
        }else{
          if (unaCoincidenciaAlMenos==1){
            indiceCadenaABuscar=0;
          }
        }
  }
  if (encontrada == 1){
    return 1;
  }
  return 0;
}

void agregarSuspendido(struct t_lista *suspendidos , struct t_proceso * proceso){
    proceso->estado=H_PROCESO_SUSPENDIDO;
    pthread_mutex_lock(&suspendidos->mutex);
    list_add(suspendidos->elementos, proceso);
    pthread_mutex_unlock(&suspendidos->mutex);
    sem_post(&suspendidos->tamanio);
}

struct t_proceso * quitarSuspendido(struct t_lista *suspendidos , int id){
    pthread_mutex_lock(&suspendidos->mutex);
    bool _get_sock(pi *piget) {
      return(piget->id==id);
    }
    struct t_proceso *p=(struct t_proceso *) list_remove_by_condition(suspendidos->elementos, (void*) _get_sock );
    pthread_mutex_unlock(&suspendidos->mutex);
    return p;
}

char* log_de_suspendido(proceso * estructuraProceso){
  char * id_y_pc = (char*) malloc(100);
  char * codigo = (char*) malloc(150 * list_size(estructuraProceso->codigo.lineas));
  char * datos = (char*) malloc(1024);
  char * stack = (char*) malloc(1024);
  char * log_de_suspendido = (char *) malloc(2048);
  
  variable* dato;
  char * dato_string = (char*) malloc(100);
  char * info_stack_string = (char*) malloc(100);
  informacion_stack* info_stack; 
  int i;

  memset(id_y_pc, '\0', 100);
  memset(codigo, '\0', 150 * list_size(estructuraProceso->codigo.lineas));
  memset(datos, '\0', 1024);
  memset(stack, '\0', 1024);

  sprintf(id_y_pc, "ID=%d\nPC=%d", estructuraProceso->id, estructuraProceso->program_counter);

  for (i = 0; i < list_size(estructuraProceso->codigo.lineas); ++i){
    string_append(&codigo, (char *) list_get(estructuraProceso->codigo.lineas, i));
    string_append(&codigo, "\n");
  }

  
  for (i = 0; i < list_size(estructuraProceso->datos); ++i){
    memset(dato_string, '\0', 100);
    dato = (variable*) list_get(estructuraProceso->datos, i);
    sprintf(dato_string, "%c=%d\n", dato->nombre, dato->valor );
    string_append(&datos, dato_string);
  }

  for (i = 0; i < list_size(estructuraProceso->stack); ++i){
    memset(info_stack_string, '\0', 100);
    info_stack = (informacion_stack*) list_get(estructuraProceso->stack, i);
    sprintf(info_stack_string, "%d=%s\n", info_stack->pc_actual, info_stack->nombre );
    string_append(&stack, info_stack_string);
  }

  
  sprintf(log_de_suspendido,"%s\n\n%s\n\n- Estructura de codigo ---\n%s\n%s\n- Estructura de Datos ---\n%s\n%s\n- Estructura de Stack ---\n%s\n%s\n", string_repeat('-', 30), id_y_pc,
    codigo, string_repeat('-', 30), datos,string_repeat('-', 30), stack, string_repeat('-', 30));

  free(id_y_pc);
  free(codigo);
  free(datos);
  free(dato_string);
  free(info_stack_string);
  free(stack);

  return log_de_suspendido;
}

int get_config(char * key, t_config * config, int default_value){
  if(config_has_property(config, key)){
    return config_get_int_value(config, key);
  } else {
    return default_value;
  }
}

float get_float_config(char * key, t_config * config, float default_value){
  if(config_has_property(config, key)){
    return config_get_double_value(config, key);
  } else {
    return default_value;
  }
}

int get_algoritmo(t_config * config){
  if(config_has_property(config, "LPL")){
    char* lpl = config_get_string_value(config, "LPL");
    if( strcmp(lpl,"RR") == 0){
      return LPL_RR;
    } else if( strcmp(lpl,"FIFO") == 0){
      return LPL_FIFO;
    } else if( strcmp(lpl,"PRI") == 0){
      return LPL_PRI;
    } else if( strcmp(lpl,"SPN") == 0){
      return LPL_SPN;
    } 
  } else {
    return LPL_RR;
  }
}

void procer_log_info(data* data, char* nombre_hilo, char* mensaje){
  char* message[2];
  message[0] = nombre_hilo;
  message[1] = mensaje;
  pthread_mutex_lock(&data->mutex_log); 
  log_info(data->log, message);
  pthread_mutex_unlock(&data->mutex_log); 
}

void procer_log_debug(data* data, char* nombre_hilo, char* mensaje){
  char* message[2];
  message[0] = nombre_hilo;
  message[1] = mensaje;
  pthread_mutex_lock(&data->mutex_log); 
  log_debug(data->log, message);
  pthread_mutex_unlock(&data->mutex_log); 
}

void procer_log_error(data* data, char* nombre_hilo, char* mensaje){
  char* message[2];
  message[0] = nombre_hilo;
  message[1] = mensaje;
  pthread_mutex_lock(&data->mutex_log); 
  log_error(data->log, message);
  pthread_mutex_unlock(&data->mutex_log); 
}