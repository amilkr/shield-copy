#/bin/bash

function iniciar { 
  log_activity "iniciar limitacion de red"
  return 0;
}

function detener {
  log_activity "stop limitacion de red"
  return 0;
}

function procesar {
  	log_activity "procesando limitacion de red"
	USER_WHO=$(who i am)
	USER=($USER_WHO)
	log_activity "procesando limitacion $USER"
	TRAFFIC=$(tcpdump not dst host 127.0.0.1 -i eth0 -Z $USER > /dev/null -G 2)
	return 0;
}


