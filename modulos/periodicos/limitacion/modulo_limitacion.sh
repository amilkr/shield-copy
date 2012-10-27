#/bin/bash

function log_activity {
	echo $1
}

function iniciar { 
  log_activity "iniciar limitacion"
  return 0;
}

function detener {
  log_activity "stop limitacion"
  return 0;
}

function procesar {
  log_activity "procesando limitacion"
  for i in  $(ps -fea | grep "pts/" | awk '$1=="utnso" {print $2}')
  	do
  		DATA=($(top -n 1 -p $i | awk '$2=="'$i'" {print $6,$10,$11;}'))
  		if [ "${#DATA[*]}" -gt 0 ];then
  			echo "Proceso $i Memoria ${DATA[0]} Proceso ${DATA[2]}"
  		fi
  	done
  return 0;
}


procesar

exit 0
