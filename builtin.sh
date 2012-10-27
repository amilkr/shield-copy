inicializar_modulos(){
#ejecuta en background registrar e inicializar modulos
source /etc/shield/modulos/registrar_e_inicializar_modulos.sh procesar &
}

ayuda(){
#si no entra ningun parametro, muestra todos los builtins, tanto de shield como de bash
if [ -z "$*" ]; then
_ayuda
_info_modulos 
_listar_modulos 
_actualizar_modulos
_mostrar 
_salir 
_apagar 
#muestra los builtins de bash
help -d | tail -n 38
fi 

#muestra por cada paramatro(builtin) que entro la ayuda del mismo. Tambien es posible ingresar builtins de bash.
for i in $@; do
     case $i in
     *ayuda) _ayuda ;;
     *info_modulos) _info_modulos ;;
     *listar_modulos) _listar_modulos ;;
     *actualizar_modulos) _actualizar_modulos;;
     *mostrar) _mostrar ;;
     *salir) _salir ;;
     *apagar) _apagar ;;
     *) _otros_builtins ;;
     esac
done
}

_ayuda() {
    echo "ayuda - Presenta en pantalla una breve ayuda del builtin indicado. Si no recibe parámentro presentará la ayuda de todos los builtins.";
}

_info_modulos() {
    echo "info_modulos - Muestra informacion del modulo indicado. Si no recibe ningun parametro presentara la informacion de todos los modulos activos.";
}

_listar_modulos() {
    echo "listar_modulos - Muestra en pantalla los path absolutos de los modulos que tiene activo el usuario.";
}

_actualizar_modulos() {
    echo "actualizar_modulos - Invoca a la funcion del nucleo "Registrar e Inicializar Módulos"";
}

_mostrar() {
    echo "mostrar - Muestra el nombre de la variable indicada.";
}

_salir() {
    echo "salir - Termina la sesion actual del usuario.";
}

_apagar() {
#para poder ejecutar este builtin es necesario ejecutar en modo root el comando visudo y agregar debajo de todo la siguiente linea: "%users ALL=(ALL) NOPASSWD: /sbin/poweroff"
    echo "apagar - Apaga la PC.";
}

_otros_builtins() {
    help -d $i
}

listar_modulos(){
#obtiene los modulos activos actualmente e imprime su path
ps h | awk '{print $5}'  | sort -n | uniq | while read comando 
do
path_comando= type -path $comando
echo -e $path_comando'\c'
done
}

apagar(){
#apaga el equipo, sin necesidad de contraseña, para ello es necesario tener modificado el fichero sudoers del usuario por el root
sudo shutdown -h now;	
}

info_modulos(){
#si no ingreso ningun modulo, busco los modulos activos e imprimo por pantalla su informacion
if [ -z "$*" ]; then
  ps h | awk '{print $5}'  | sort -n | uniq | while read comando 
  do
     $comando informacion
  done
fi
#muestro por pantalla la informacion de cada modulo requerido ingresado
for i in $@; do
     $i informacion
done
}

mostrar(){
#Muestro el contenido de cada parametro ingresado
echo -e "$@"
}

salir(){
#cierro la sesion actual
exit
}
