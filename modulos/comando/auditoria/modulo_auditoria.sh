function iniciar { 
#	log_activity "iniciar auditoria"
	exec 6<&0 # Enlaza el descriptor 6 a stdin. Esto se hace para salvar stdin
	exec < auditoria.config # Reemplaza stdin por auditoria.config
	read 
	read TamanoMaximoLocal
	read 
	read direccionIP
	exec 0<&6 6<&- # Restaura stdin
        usuario=whoami
	return 0;
}

function detener {
#	log_activity "stop auditoria"
#Borrar ficheros de Log??
	unset TamanoMaximoLocal
	unset direccionIP
	return 0;
}

function procesar {
#	log_activity "procesando auditoria, comando: $@"
	comando=""${@}""
        TamanoActual=stat -c %s auditoria.log
        echo $tamanoActual
        if [ $TamanoActual > $TamanoMaximoLocal ]; then
#En la configuracion de los modulos se debera realizar lo siguiente para que cada vez que se pasen datos al servidor remoto no nos pida contraseña:
#"ssh-keygen -b 4096 -t rsa", con esto generaremos la clave publica, le damos a todo enter para crear todo por default
#"ssh-copy-id usuario@IP" con esto compartimos la clave publica recien creada a nuestro usuario en la IP indicada, nos requerira la contraseña, por unica vez.
                ssh $usuario@$direccionIP echo $comando >> auditoria.log

	else
		echo $comando >> auditoria.log
	fi
}

function informacion {
#	log_activity "mostrando informacion auditoria"
	echo "IP de Servidor Remoto:" $direccionIP
	echo "Tamaño Maximo Archivo Log Local:" $TamanoMaximoLocal "bytes"
        TamanoActual=stat -c %s auditoria.log 
	echo "Tamaño Actual Archivo Log Local:" $TamanoActual
	return 0
}
