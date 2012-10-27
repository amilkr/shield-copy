#/bin/bash

function iniciar { 
	log_activity "iniciar control sesiones"
	exec 6<&0 # Enlaza el descriptor 6 a stdin. Esto se hace para salvar stdin
	exec < control_sesiones.config # Reemplaza stdin por control_sesiones.config
	read 
	read cantidadMaximaSesiones
	exec 0<&6 6<&- # Restaura stdin
        USUARIO=whoami
        cantidadSesionesAbiertas=w | awk '{print $1}' | grep $USUARIO -c
	if [ $cantidadSesionesAbiertas > $cantidadMaximaSesiones ]; then
		echo "cantidad maxima de sesiones abiertas alcanzada." #sacar despues de probar
		return 1;
	fi
}

function detener {
	log_activity "stop control sesiones"
	unset cantidadMaximaSesiones
	unset cantidadSesionesAbiertas
	return 0;
}

function procesar {
  log_activity "procesando control sesiones"
  return 0;
}

function informacion {
	log_activity "mostrando informacion control sesiones"
	echo "cantidad maxima de sesiones abiertas:" $cantidadMaximaSesiones
	echo "cantidad de sesiones abiertas:" $cantidadSesionesAbiertas
	return 0;
}
