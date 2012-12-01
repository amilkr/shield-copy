/*

	main.c

*/

#include "main.h"

int main(int argc,char* argv[]){
	struct DATA_SISTEMA* data;
	data =(struct DATA_SISTEMA*) iniciar();	
	iniciarHilos(data);
	pthread_exit(NULL);
}

struct DATA_SISTEMA* iniciar(){
	struct DATA_SISTEMA* data;
	data = (struct DATA_SISTEMA*) malloc(sizeof( struct DATA_SISTEMA ));
	t_config * config = config_create("procer.config");
	t_log* log = log_create("procer.log", "PROCER", true, LOG_LEVEL_DEBUG);

// Valores que pueden estar en la config
	data->max_procesos_en_sitema = get_config("MPS", config, MPS);
	data->max_valor_multiprogramacion = get_config("MMP", config, MMP);
	data->algoritmo = get_algoritmo(config);
	data->quantum = get_config("QUANTUM", config, QUANTUM);
	data->prioridad_nuevos = get_config("PLPN", config, PLPN);
	data->prioridad_reanudados = get_config("PLPR", config, PLPR);
	data->prioridad_fin_q = get_config("PLPFQ", config, PLPFQ);
	data->prioridad_fin_io = get_config("PLPFIO", config, PLPFIO);
	data->sleep = get_config("SLEEP", config, SLEEP);
	data->sleep_io = get_config("SLEEP_IO", config, SLEEP_IO);
	data->iot_sistema = get_config("INSTANCIAS_IOT", config, INSTANCIAS_IOT);
	data->puertoEscucha = get_config("PUERTO", config, PUERTO);
	data->alpha = get_float_config("ALPHA", config, ALPHA);
// Fin valores que pueden estar en la config

	data->log=log;
	data->estadoSistema=1;
	data->valor_multiprogramacion=0;
	data->procesos_en_sitema=0;
	data->actualizacion_algoritmo = 0;
	data->actualizacion_prioridad = 0;
	data->reanudados.procesos = queue_create();
	sem_init(&data->reanudados.tamanio, 0, 0);
	data->nuevos.procesos = queue_create();
	sem_init(&data->nuevos.tamanio, 0, 0);
	data->fin_q.procesos = queue_create();
	sem_init(&data->fin_q.tamanio, 0, 0);
	data->fin_io.procesos = queue_create();
	sem_init(&data->fin_io.tamanio, 0, 0);
	data->listos.procesos = queue_create();
	sem_init(&data->listos.tamanio, 0, 0);
	data->bloqueados.procesos = queue_create();
	sem_init(&data->bloqueados.tamanio, 0, 0);
	data->suspendidos.elementos = list_create();
	pthread_mutex_lock(&data->mutex_sts);

	return data;
}

int iniciarHilos(struct DATA_SISTEMA* data){
	iniciarLTS(data);
	iniciarSTS(data);
	iniciarPROCER(data);

	int i;
	for (i = 0; i < INSTANCIAS_IOT; ++i)
	{
		iniciarIOT(data);	
	}
	return CONTINUAR;
}