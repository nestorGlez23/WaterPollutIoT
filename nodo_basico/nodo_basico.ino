/*
 *************************************************************************
   RF24Ethernet Arduino library by TMRh20 - 2014-2015

   Automated (mesh) wireless networking and TCP/IP communication stack for RF24 radio modules

   RF24 -> RF24Network -> UIP(TCP/IP) -> RF24Ethernet
                       -> RF24Mesh

        Documentation: http://tmrh20.github.io/RF24Ethernet/

 *************************************************************************
 *
 **** EXAMPLE REQUIRES: PubSub MQTT library: https://github.com/knolleary/pubsubclient ***
 * 
 * Install using the Arduino library manager
 * 
 *************************************************************************
  Modificación para el proyecto abierto de la Red de monitorización de contaminación de aguas
  
  Este sketch de Arduino está basado en el ejemplo básico de MQTT dado por la librería, y ha sido editado para enviar los mensajes y tomar las medidas de la forma requerida tanto por la red como por el sensor encargado.
  
  Gracias a este programa, el nodo se conectará a un servidor MQTT y comenzará a publicar tantos mensajes como se le hayan configurado. Este proceso se hará con una frecuencia definida por el usuario, y cada vez que se pierda la conexión, el dispositivo se reconectará al broker.
 
*/

#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include <RF24Mesh.h>
#include <RF24Ethernet.h>
#include <PubSubClient.h>

//*****************************************************Configuración del nodo
int pinTemp = A0;   //Se definen los pines analógicos de entrada que se van a usar
int pinTurb = A1;
char usr[]={"mqtt_openhab"}; //Usuario y contraseña del broker MQTT
char pwd[]={"mqtt_openhab"};

int tiempoActualizacion=1000; //Se define el número de milisegundos entre envío de mensajes
//*****************************************************************************
RF24 radio(7,8);
RF24Network network(radio);
RF24Mesh mesh(radio,network);
RF24EthernetClass RF24Ethernet(radio,network,mesh);
//****************************************************Configuración de red
IPAddress ip(10,10,10,10);     // Es necesario definir la IP que va a tener el nodo. El último octeto será el identificador
IPAddress gateway(10,10,10,1); // Se definen las IP tanto del dispositivo con la RF24 que tiene conectividad con el broker
IPAddress server(10,10,10,1);  // Como la IP del broker en cuestión
//******************************************************************************
char clientID[] = {"arduinoClient   "};

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

EthernetClient ethClient;
PubSubClient client(ethClient);

void reconnect() {
  // Loop until we're reconnected
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientID, usr, pwd)) {       //Se intenta establecer la conexión con el broker
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic","hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      uint32_t recTimer = millis();
      while(millis()-recTimer < 5000){ Ethernet.update(); }
    }
  }
}

void setup()
{
  Serial.begin(115200);

  client.setServer(server, 1883);
  client.setCallback(callback);

  Ethernet.begin(ip);
  Ethernet.set_gateway(gateway);
  if (mesh.begin()) {
    Serial.println(" OK");
  } else {
    Serial.println(" Failed");
  }

   //Convertir el último octeto de la dirección IP en el identificador de la red
   char str[4];
   int test = ip[3];
   itoa(ip[3],str,10);   
   memcpy(&clientID[13],&str,strlen(str));
   Serial.println(clientID);
  
}

uint32_t mesh_timer = 0;
uint32_t pub_timer = 0;

void loop()
{
  Ethernet.update();
  
  if(millis()-mesh_timer > 30000){ //Cada 30 segundos, comprueba la conectividad con la malla RF24
    mesh_timer = millis();
    if( ! mesh.checkConnection() ){
        if(!mesh.renewAddress()){
          mesh.begin(MESH_DEFAULT_CHANNEL,RF24_1MBPS,5000);
        }
     }
  }  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Cada tiempoActualizacion
  if(client.connected() && millis() - pub_timer > tiempoActualizacion){
    pub_timer = millis();

    
    //*********************************SENSOR 1*********************************
    char msgTemp[7];
    //Sección donde se realiza la medida
    int sensorTemp = analogRead(pinTemp);
    float valorTemp = sensorTemp*5/1024; //Y la conversión necesaria
    dtostrf(valorTemp, 3, 3, msgTemp); //Se convierte el resultado en una string para poder enviarla al broker
    Serial.println(msgTemp);

    
    //Se publica el resultado en el servidor MQTT
    
    client.publish("localizacion/nodo10/temp",msgTemp);  //Importante definir el topic correctamente
    
    //*********************************SENSOR 2*********************************
    char msgTurb[7];
    int sensorTurb = analogRead(pinTurb);
    float valorTurb = sensorTurb; //Aqui la conversión necesaria
    dtostrf(valorTurb, 3, 3, msgTurb);
    Serial.println(msgTurb);

    //Se publican los resultados en el servidor MQTT
        
    client.publish("localizacion/nodo10/turb",msgTurb);  //Importante definir el topic correctamente, siempre tiene que ser el primero el de la localización del maestro
  }
}
