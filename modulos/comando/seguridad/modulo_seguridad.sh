#/bin/bash

function iniciar { 
#	log_activity "iniciar seguridad"
	cp seguridad.config seguridad.tmp
	return 0;
}

function detener {
#	log_activity "stop seguridad"
	rm seguridad.tmp
        unset comando
        unset cantidad
	unset word
	return 0;
}

function procesar {
#	log_activity "seguridad procesando: $@"
	comando=""${@}""
	IFS=' '; for word in $comando; do 
		cantidad=0
		grep -c $word seguridad.tmp | while read cantidad
		do 
			if [ $cantidad -gt 0 ]; then
				log_activity "seguridad procesando: comando prohibido ingresado, $word"
				return 1;
			fi
		done
	done
}

function informacion {
#	log_activity "mostrando informacion seguridad"
	echo "Comandos Restringidos al usuario:"
	cat seguridad.tmp
	return 0;
}
