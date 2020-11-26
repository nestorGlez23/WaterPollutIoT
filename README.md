# WaterPollutIoT
Red basada en nodos Arduino y maestro Raspberry Pi para la medición distribuida de sensores de contaminación de aguas. Los nodos se comunican inalámbricamente con el maestro mediante una topología de malla con un alcance de hasta 1Km para reportar los valores de sus sensores a través del módulo RF24. El maestro aporta una interfaz gráfica mediante Grafana y es accesible desde otras redes.


# Instalación en el maestro
1.Grabar el archivo .iso situado en el .zip (https://drive.google.com/file/d/19Pe3scRtVANKhS46gdNJ95ezSyKRv977/view?usp=sharing) en una tarjeta microSD e introducirla en la Raspberry Pi \
2.Conectar la Raspberry Pi a la red con acceso a internet \
3. Acceder por SSH a la dirección: openhab \

Una vez en la terminal y sabiendo que el usuario es openhabian y la contraseña openhabian

1.Introducir configuración de red \
2.Editar el fichero de configuración de OpenHABCloud en ~/openhab-cloud/config.json con el correo y la contraseña de la cuenta pertinente de OpenHAB Cloud \
3.Vincular a través de la web de myopenhab.org el dispositivo con el UUID y Secret \
 

# Instalación de los nodos
1.Cablear los nodos y tener el código que se encargue de de realizar la medida particular que necesiten los sensores conectados \
2.Abrir nodoBasico.ino y añadir el código necesario para los sensores conectados \
3.Configurar los datos de Red que se hayan escogido \
