#/bin/bash
COMMAND_CONFIG="./config/modulos_comando.config"
PERIODIC_CONFIG="./config/modulos_periodicos.config"
REGISTERED_COMMAND_MODS_FILE="./tmp/modulos_comando_registrados.tmp"
REGISTERED_PERIODIC_MODS_FILE="./tmp/modulos_periodicos_registrados.tmp"
BUILTIN_FILE="./builtin.sh"
BUILTIN_LIST_FILE="./config/builtin.list"
LOG_PATH="./log"
TMP_PATH="./tmp"
VERIFY_CONFIG_DELAY=60 # 1 minuto
PERIODIC_MODULES_EXEC_DELAY=60 # 1 minuto
    
function inicializar_modulos {
  detener_modulos_en_ejecucion
  inicializar_modulos_activos
}

function detener_modulos_en_ejecucion {
  if [ -f $REGISTERED_COMMAND_MODS_FILE ]; then
    ejecucion_masiva detener $REGISTERED_COMMAND_MODS_FILE
    ## Al borrar los archivos de modulos registrados, evito detener
    ## modulos que ya han sido detenidos.
    rm $REGISTERED_COMMAND_MODS_FILE
  fi
  
  if [ -f $REGISTERED_PERIODIC_MODS_FILE ]; then
    ejecucion_masiva detener $REGISTERED_PERIODIC_MODS_FILE
    ## Al borrar los archivos de modulos registrados, evito detener
    ## modulos que ya han sido detenidos.
    rm $REGISTERED_PERIODIC_MODS_FILE
  fi
}

function inicializar_modulos_activos {
  # Registrando modulos activos
  grep ":on" $COMMAND_CONFIG  | cut -d ":" -f 1 > $REGISTERED_COMMAND_MODS_FILE
  grep ":on" $PERIODIC_CONFIG | cut -d ":" -f 1 > $REGISTERED_PERIODIC_MODS_FILE
  
  # Iniciando modulos
  ejecucion_masiva iniciar $REGISTERED_COMMAND_MODS_FILE
  ejecucion_masiva iniciar $REGISTERED_PERIODIC_MODS_FILE
}

## ejecucion_masiva iniciar|detener|procesar ruta/hacia/archivo/de/modulos/registrados
function ejecucion_masiva {
  while read modulo; do 
    . $modulo
    if  ! $1 ; then
      log_error "error en funcion $1. modulo: $modulo"
      exit 1
    fi
  done < $2 
}

function verificar_cambios_de_configuracion {
  last_edit_time_for_command_config=`ls -la ${COMMAND_CONFIG} | awk '{print $8}'`
  last_edit_time_for_periodic_config=`ls -la ${PERIODIC_CONFIG} | awk '{print $8}'`

  echo last_edit_time_for_command_config $last_edit_time_for_command_config
  echo last_edit_time_for_periodic_config $last_edit_time_for_periodic_config
  while true; do
    sleep $VERIFY_CONFIG_DELAY
    log_activity "verificando cambios en la configuracion"

    if [$last_edit_time_for_command_config  != `ls -la ${COMMAND_CONFIG} | awk '{print $8}'` || 
       $last_edit_time_for_periodic_config != `ls -la ${PERIODIC_CONFIG} | awk '{print $8}'` ] ; then
      echo "Se han modificado los archivos de configuracion"
      echo "Reiniciando modulos..."
      cp $COMMAND_CONFIG $COMMAND_CONFIG_TMP
      cp $PERIODIC_CONFIG $PERIODIC_CONFIG_TMP
      inicializar_modulos
    fi
  done
}

function administrador_de_mod_periodicos {
  while true; do
    sleep $PERIODIC_MODULES_EXEC_DELAY
    log_activity "ejecutando modulos periodicos"
    ejecucion_masiva procesar $REGISTERED_PERIODIC_MODS_FILE
  done
}

function administrador_de_comandos_de_usuario {  
  while true; do
    ## imprime el "PS1" de shield
    echo -ne "\033[31mST\033[0m" $USER:$PWD$ ""
    ## espera y lee el comando del usuario
    read comando
    log_activity "nuevo comando: $comando"

    ## los modulos de comando procesan el comando
    if procesar_comando; then
      ## es un comando permitido
      ## evaluo si es un built-in
      if es_builtin $comando; then
        ## es built-in, lo ejecuto internamente
        log_activity "comando built-in $comando"
        $comando
      else
        ## no es buil-in, se lo doy a bash para que lo ejecute
        eval $comando
      fi
    else
      ## es un comando NO permitido
      echo "[error] vo' no pode' hacer eso, viteh?: $comando"
    fi
    
  done
}

function es_builtin {
  while read builtin_name; do
    if [ $1 = $builtin_name ]; then
      return 0
    fi
  done < $BUILTIN_LIST_FILE
  return 1
}

function procesar_comando {
  while read modulo; do
    . $modulo
    if ! procesar $comando; then
      return 1
    fi
  done < $REGISTERED_COMMAND_MODS_FILE
  return 0
}

function log_error {
  echo $1
  echo `date +%x-%X` "[error]" $1 >> $LOG_PATH/shell.log
}

function log_activity {
  echo `date +%x-%X` "[activity]" $1 >> $LOG_PATH/shell.log
}

. $BUILTIN_FILE
mkdir -p $LOG_PATH
mkdir -p $TMP_PATH
inicializar_modulos

## TODO:
## hay que encontrar la forma de matar a estos procesos ejecutados en segundo plano
verificar_cambios_de_configuracion &
administrador_de_mod_periodicos &
##
##

administrador_de_comandos_de_usuario

exit 0