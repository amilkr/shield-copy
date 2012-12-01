/*

	sts.c

*/

#include "sts.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

int iniciarSTS(struct DATA_SISTEMA* data){
	pthread_t main_thr;
	pthread_attr_t attr;
	int rc;
	void *status;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	rc = pthread_create(&main_thr, &attr, sts_inicio, (void *) data);
	
	if(rc){
		printf("T_STS_CREADO");
		printf("\n");
	}
}


void * sts_inicio(void * arg){
	data * data_sistema = (data *) arg;
	int aux_posicion=0;
	proceso * proceso_nuevo;
	cola* colas[4];
	
  procer_log_error(data_sistema,"STS","iniciando");

	actualizar_prioridad_de_colas(colas, data_sistema);
		
	while(1){
		proceso_nuevo = NULL;
		pthread_mutex_lock(&data_sistema->mutex_sts);
    
		printf("STS: Me desbloquearon\n");

		// Actualizo las prioridades de las colas de entrada
		// si fueron modificadas
		if (data_sistema->actualizacion_prioridad == 1)
		{
			pthread_mutex_lock(&data_sistema->mutex_actualizacion_prioridad);
		  data_sistema->actualizacion_prioridad = 0;
		  pthread_mutex_unlock(&data_sistema->mutex_actualizacion_prioridad);

			actualizar_prioridad_de_colas(colas, data_sistema);
		}

		// Reordeno la cola de listos si se modifico el algoritmo
		if (data_sistema->actualizacion_algoritmo == 1)
		{
			pthread_mutex_lock(&data_sistema->mutex_actualizacion_algoritmo);
  		data_sistema->actualizacion_algoritmo = 0;
  		pthread_mutex_unlock(&data_sistema->mutex_actualizacion_algoritmo);

			reordenar_listos(data_sistema);
		}

		// Recorro las colas y agrego los procesos a la cola de listos
		for(aux_posicion=0; aux_posicion<4; aux_posicion++){
			// printf("STS: cantidad %d \n", cola_size(&colas[aux_posicion]));
			do {					
				proceso_nuevo = (proceso*) cola_try_pop(colas[aux_posicion]);
				if(proceso_nuevo != NULL){
          printf("STS: Agrego al listo %d\n",proceso_nuevo->id);
					agregar_a_listos(proceso_nuevo, data_sistema);
				}
			}while(proceso_nuevo != NULL);	
		}
    
	}

}

void actualizar_prioridad_de_colas(cola ** colas, data * data_sistema){
  printf("STS: Actualizacion de prioridad de colas\n");
  colas[data_sistema->prioridad_nuevos - 1]     = &data_sistema->nuevos;
  colas[data_sistema->prioridad_reanudados - 1] = &data_sistema->reanudados;
  colas[data_sistema->prioridad_fin_q -1]       = &data_sistema->fin_q;
  colas[data_sistema->prioridad_fin_io -1]      = &data_sistema->fin_io;
}

void reordenar_listos(data * data_sistema){
	printf("STS: Reordenando cola de listos\n");
	int algoritmo = data_sistema->algoritmo;

  switch ( algoritmo )
  {
    case LPL_RR:
      printf("round robin, no se reordena\n");
      break;
    case LPL_FIFO:
      printf("fifo, no se reordena\n");
      break;
    case LPL_SPN:
    	ordenar_listos(data_sistema, LPL_SPN);
      break;
    case LPL_PRI:
    	ordenar_listos(data_sistema, LPL_SPN);
  }
}

void ordenar_listos(data * data_sistema, int algoritmo){
  printf("STS: Reordenando la cola de listos\n");
  
  int i, j; 
  proceso * proceso_j = NULL;
  proceso * proceso_i = NULL;

  pthread_mutex_lock(&data_sistema->listos.mutex);

  int size =  queue_size(data_sistema->listos.procesos);
  t_list * lista = (t_list*) queue_all(data_sistema->listos.procesos);

  for (i = 0; i < (size - 1); i++) 
  { 
    for (j = i + 1; j < size; j++) 
    { 
      proceso_j = (proceso*) list_get(lista, j);
      proceso_i = (proceso*) list_get(lista, i);
      if ( es_menor(proceso_j, proceso_i, algoritmo) ) 
      { 
        list_replace(lista, i, proceso_j);
        list_replace(lista, j, proceso_i);
      } 
    } 
  } 

  pthread_mutex_unlock(&data_sistema->listos.mutex);
}


void agregar_a_listos(proceso * proceso_nuevo, data * data_sistema){
	printf("STS: Agregando a listos\n");
	int algoritmo = data_sistema->algoritmo;
	
	float aux_estimacion = proceso_nuevo->estimacion_q;
	proceso_nuevo->estimacion_q = estimar_rafaga(proceso_nuevo, data_sistema->alpha);
	proceso_nuevo->estimacion_anterior_q = aux_estimacion;

  switch ( algoritmo )
  {
    case LPL_RR:
      cola_push(&data_sistema->listos, proceso_nuevo);
      break;
    case LPL_FIFO:
      cola_push(&data_sistema->listos, proceso_nuevo);
      break;
    case LPL_SPN:
      agregar_a_listos_ordenado(proceso_nuevo, data_sistema, LPL_SPN);
      break;
    case LPL_PRI:
      agregar_a_listos_ordenado(proceso_nuevo, data_sistema, LPL_PRI);
  }
}

void agregar_a_listos_ordenado(proceso * proceso_nuevo, data * data_sistema, int algoritmo){
  int aux_posicion = 0;
  bool fue_agregado = false;
  proceso * aux_proceso = NULL;

  pthread_mutex_lock(&data_sistema->listos.mutex);

  int size = queue_size(data_sistema->listos.procesos);

  while(aux_posicion <= size && ! fue_agregado){
    aux_proceso = list_get(data_sistema->listos.procesos->elements, aux_posicion);
    
    if(aux_proceso == NULL){
      list_add(data_sistema->listos.procesos->elements, proceso_nuevo);
    } else {
      if(es_menor(proceso_nuevo, aux_proceso, algoritmo)){
        list_add_in_index(data_sistema->listos.procesos->elements, aux_posicion, proceso_nuevo);
        fue_agregado = true;
      }
    }

    aux_posicion ++;
  }

  pthread_mutex_unlock(&data_sistema->listos.mutex);
  sem_post(&data_sistema->listos.tamanio);
}

bool es_menor(proceso * proceso1,proceso * proceso2, int algoritmo){
  switch ( algoritmo )
  {
    case LPL_SPN:
      return proceso1->estimacion_q < proceso2->estimacion_q;
      break;
    case LPL_PRI:
      return proceso1->prioridad < proceso2->prioridad;
  }
}

float estimar_rafaga(proceso * proceso, float alpha){
	float estimacion;
	if(proceso->estimacion_anterior_q == 0){
		estimacion = proceso->primera_estimacion_q;
	} else {
		estimacion = proceso->estimacion_anterior_q * alpha + proceso->ultimo_q * (1-alpha);
	}
	return estimacion;
}