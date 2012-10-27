SHIELD_PATH := /etc/shield

instalar:
	mkdir -p ${SHIELD_PATH}/modulos ; 
	cp -r shield.sh builtin.sh modulos/ ${SHIELD_PATH}; 
	ln -s ${SHIELD_PATH}/shield.sh /usr/bin/shield.sh; 

desinstalar:
	echo desinstalar 

configurar:
	echo el user es ${TARGET}
	usermod -s /usr/bin/shield.sh ${TARGET}
	mkdir -p /home/${TARGET}/.shield/config   

resetear:
	echo resetear