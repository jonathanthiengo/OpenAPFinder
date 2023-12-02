/*****************************************************************************************************************************
 * Author : Firtina Huseyin GOKTAS , fhg971@gmail.com
 * Testing the FileOperations library.
 * 
 * 
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * 
 * 
 * 
 ******************************************************************************************************************************/
#include <FileOperations.h>
#include <SPI.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Crypto.h>
#include <SHA256.h>




#define SERVER_PORT 5000



FileOp File_SSID_Default("/SSID_Default.txt");
FileOp File_SSID_Pass_Default("/SSID_Pass_Default.txt");
FileOp File_SSID_Received("/SSID_Received.txt");
FileOp File_SSID_Pass_Received("/SSID_Pass_Received.txt");
FileOp File_Broken_IP("/Broken_IP.txt");
FileOp File_Device_Name("/Device_Name.txt");
FileOp File_ResetCounter("/ResetCounter.txt");


int tempo;
int tempo_reset = 0;
String randNumber;
WiFiServer wifiServer(SERVER_PORT);

//------------------------------------------ Double Hash Function------------------------------
//------------------------------------------
//------------------------------------------
/* Gera um Double Hash para a string digitada
*/
SHA256 sha256;

String gerarHash256(const String &dataToHash) {
  // Primeiro hash
  uint8_t hash1[SHA256::HASH_SIZE];
  sha256.reset();
  sha256.update(dataToHash.c_str(), dataToHash.length());
  sha256.finalize(hash1, SHA256::HASH_SIZE);

  // Converte o primeiro hash em uma representação hexadecimal
  String hashStr1 = "";
  for (int i = 0; i < SHA256::HASH_SIZE; i++) {
    char hex[3];
    sprintf(hex, "%02X", hash1[i]);
    hashStr1 += hex;
  }
  // Segundo hash
  uint8_t hash2[SHA256::HASH_SIZE];
  sha256.reset();
  sha256.update(hash1, SHA256::HASH_SIZE);
  sha256.finalize(hash2, SHA256::HASH_SIZE);

  // Converte o segundo hash em uma representação hexadecimal
  String hashStr2 = "";
  for (int i = 0; i < SHA256::HASH_SIZE; i++) {
    char hex[3];
    sprintf(hex, "%02X", hash2[i]);
    hashStr2 += hex;
  }

  return hashStr2;
}

//------------------------------------------ timer Function------------------------------
//------------------------------------------
//------------------------------------------
/* Função timer() usada para Adicionar um temporizador.
tempoStr1 = text to show BEFORE the countdown TIME
tempoStr2 = text to show AFTER the countdown TIME
tempo = countdown timer
*/
void timer(String(tempoStr1), int tempo, String(tempoStr2)) {
  Serial.print(tempoStr1);
  //Serial.printf("%d ", tempo);
  Serial.print(tempoStr2);
  while (tempo > 0) {
    Serial.print(".");
    delay(1000);  // seconds countdown to initiate
    tempo--;
  }
  Serial.println("");
}
//------------------------------------------ wifi_Connection Function------------------------------
//------------------------------------------
//------------------------------------------
/* Função wifi_Connection() usada para iniciar modo de conexão WIFI
do ESP. 
*/
void wifi_Connection() {
  timer("", 2, "Starting WIFI Connection");
  int status = WL_IDLE_STATUS;
  int temp = 1;

  if(File_SSID_Received.GetSize() != 0 || File_SSID_Pass_Received.GetSize() != 0){
    Serial.print("\nArquivos para se conectar ao Wifi foram encontrado, iniciando Modo de conexão WIFI\n");
    WiFi.begin(File_SSID_Received.Read(), File_SSID_Pass_Received.Read());
    Serial.println(File_SSID_Received.Read());
    Serial.println(File_SSID_Pass_Received.Read());
  }
  else{
    Serial.print("\nWifi nao encontrado, iniciando Modo HOTSPOT\n");
    wifi_Hotspot();
  }
  
  //Serial.println(status);
  while (WiFi.status() != WL_CONNECTED && temp < 3) {
    Serial.printf("%d attempt to Connect\n", temp);
    // Enquanto não conseguir conectar, tentar conectar novamente.
    Serial.println("Conectando ao WiFi..");
    timer("", 10, ".");
    temp++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Conectado à rede WiFi");
  } else {
    Serial.println("WiFi NAO ENCONTRADO");
    wifi_Hotspot();
    
  }
}
//------------------------------------------ wifi_hotspot_connection Function------------------------------
//------------------------------------------
//------------------------------------------
/* Função wifi_hotspot_connection() usada para iniciar modo server
do ESP, e transferir os dados do cliente conectado(Raspberry) para o ESP. 
    Nessa Função tambem é feita a gravação dos dados transferidos pelo Cliente para os arquivos 
permissivos gerados.
*/
int wifi_hotspot_connection(){
  //Inicia o server
  wifiServer.begin();
  WiFiClient client = wifiServer.available();
  Serial.print(".");
  if (client) {
    Serial.println("\nClient Conectado");
    String mensagem_SSID = "";
    String mensagem_PW = "";
    String mensagem_BROKER = "";
    String mensagem = "";
    unsigned long contadorReset = millis(); // Contador da conexão
    for (int i = 1; i <= 3; i++) {
      Serial.print("Lendo mensagem ");
      Serial.print(i);
      Serial.print(": ");
      
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          //mensagem += c; // Converter o caractere para uma String
          if (c == '\n') {
            break; // Fim da mensagem
          }
          if(i == 1){
            mensagem_SSID += c;
            //Serial.println(mensagem_SSID);
          }
          if(i == 2){
            mensagem_PW += c;
            //Serial.println(mensagem_PW);
          }
          if(i == 3){
            mensagem_BROKER += c;
            //Serial.println(mensagem_BROKER);
          }
          Serial.write(c);
          contadorReset = millis(); // Reseta o contador sempre que receber dados do cliente
        }
         // Verifica o Status da conexão com o cliente
         /* Se a conexão ainda estiver ativa por mais de 20 segundos
            a conexão com o cliente é encerrada, e retorna para a Função
            wifi_Connection().
          */
          if (millis() - contadorReset > 20000) { // Se não houver dados recebidos em 20 segundos, considere a conexão perdida
            client.stop(); // Pare a conexão com o cliente
            Serial.println("\nConexão perdida, reiniciando procedimentos...");
            wifi_Connection();
        }
      }
      
      Serial.println();
    }
    client.stop();
    Serial.println("Client disconnected");

    Serial.println("Atualizando Arquivos.");
    //definedPathes();
    delay(2000);
    File_SSID_Received.Write(mensagem_SSID);
    delay(2000);
    File_SSID_Pass_Received.Write(mensagem_PW);
    delay(2000);
    File_Broken_IP.Write(mensagem_BROKER);
    delay(500);
    // Inserindo as mensagens recebidas pelo Rasp no esp
    while(File_SSID_Received.Read() != mensagem_SSID){ // Verificando se o Arquivo que esta no esp é igual ao que foi recebido pelo Rasp
      delay(4000);
      File_SSID_Received.Write(mensagem_SSID); // adicionando mensagem dentro do esp.
    }
    while(File_SSID_Pass_Received.Read() != mensagem_PW){ // Verificando se o Arquivo que esta no esp é igual ao que foi recebido pelo Rasp
      delay(4000);
      File_SSID_Pass_Received.Write(mensagem_PW); // adicionando mensagem dentro do esp.
    }
    while(File_Broken_IP.Read() != mensagem_BROKER){ // Verificando se o Arquivo que esta no esp é igual ao que foi recebido pelo Rasp
      delay(4000);
      File_Broken_IP.Write(mensagem_BROKER); // adicionando mensagem dentro do esp.
    }
    
    return 0;
  }
  return 1;
}
//------------------------------------------ wifi_hotspot Function------------------------------
//------------------------------------------
//------------------------------------------
/* Função wifi_hotspot() Usada para Iniciar um Ponto de Acesso WIFI
para o Raspberry(Cliente) se conectar*/

void wifi_Hotspot() {
  timer("", 2, "Starting WIFI_HOTSPOT");
  WiFi.mode(WIFI_AP);
  // SSID e SENHA para CLIENTE se conectar
  Serial.print(WiFi.softAP(File_SSID_Default.Read(), File_SSID_Pass_Default.Read()) ? "Ready" : "Failed");
  
  // string = client.readStringUntil('\r');   // FUNCIONANDO
  Serial.println("Starting mobile hotspot");
  Serial.println("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPIP());

  Serial.print("Waiting User WIFI Connection.");
  while(wifi_softap_get_station_num() == 0){
    Serial.print(".");
    delay(1000);
  }
  //timer("\nWaiting Client Connection: ", 1, "");
  Serial.printf("\nWaiting Client Connection ");
  while(wifi_hotspot_connection() == 1){
    delay(500);
  }
  delay(1000);
  WiFi.softAPdisconnect (true);
  wifi_Connection();
}

//-------------------------- DefinedPathes Function------------------------------
void definedPathes() {
  Serial.println("Get current pathes : ");
  File_SSID_Default.definePath("/SSID_Default.txt");
  File_SSID_Pass_Default.definePath("/SSID_Pass_Default.txt");
  File_SSID_Received.definePath("/SSID_Received.txt");
  File_SSID_Pass_Received.definePath("/SSID_Pass_Received.txt");
  File_Broken_IP.definePath("/Broken_IP.txt");
  File_Device_Name.definePath("/Device_Name.txt");
  File_ResetCounter.definePath("/ResetCounter.txt");

  Serial.printf("\r", File_SSID_Default.getCurrentPathName());
  Serial.printf("\r", File_SSID_Pass_Default.getCurrentPathName());
  Serial.printf("\r", File_SSID_Received.getCurrentPathName());
  Serial.printf("\r", File_SSID_Pass_Received.getCurrentPathName());
  Serial.printf("\r", File_Broken_IP.getCurrentPathName());
  Serial.printf("\r\n", File_Device_Name.getCurrentPathName());
  Serial.printf("\r", File_ResetCounter.getCurrentPathName());
}

//-------------------------- First Default WIFI Verification Function------------------------------
void SSID_E_PASS_DevName_Default_Verification() {
  delay(1000);
  if (File_SSID_Default.GetSize() == 0) {      // Verify if exists a SSID.... Ps: Only for first running.
    randNumber = String(random(1, 100000));    // Creating a random number.
    String NewSSID = "";
    NewSSID += "smartZeroconf" + randNumber;
    File_SSID_Default.Write(NewSSID);  // Writing default SSID name.
    Serial.printf("SSID_Default: ");           // Print SSID name.
    Serial.println(File_SSID_Default.Read());  // Print SSID name.
    //  
    //Gerando o Hash a partir do novo SSID criado.
    //
    String NewPASS = gerarHash256(NewSSID);
    delay(1000);
    File_SSID_Pass_Default.Write(NewPASS);           // Writing default SSID name.
    Serial.printf("SSID_Pass_Default: ");                  // Print SSID Password.
    Serial.println(File_SSID_Pass_Default.Read());         // Print SSID Password.
    File_ResetCounter.Write("0");
    Serial.printf("ResetCounterFILE: ");                  // Print SSID Password.
    Serial.println(File_ResetCounter.Read()); 
    delay(1000);
  }else{
    String resetcounterstr = File_ResetCounter.Read(); 
    int resetcounterint = resetcounterstr.toInt();
    //Serial.println(resetcounterint);
    resetcounterint++;
    resetcounterstr = String(resetcounterint);
    if(resetcounterint > 5){
      File_SSID_Default.Delete();
      File_SSID_Pass_Default.Delete();
      SSID_E_PASS_DevName_Default_Verification();
    }
    else{
      // implemantando o SSID existente
      String oldSSID = File_SSID_Default.Read();
      randNumber = 0;
      for(int i = 0; i < oldSSID.length(); i++){
        char currentChar = oldSSID.charAt(i);
        if (isDigit(currentChar)){
          randNumber += currentChar;
        }
      }
      int newrandNumber = randNumber.toInt();
      newrandNumber++;
      randNumber = String(newrandNumber);    // Creating a random number.
      String NewSSID = "";
      NewSSID += "smartZeroconf" + randNumber;
      File_SSID_Default.Write(NewSSID);  // Writing default SSID name.
      Serial.printf("SSID_Default: ");           // Print SSID name.
      Serial.println(File_SSID_Default.Read());  // Print SSID name.
      //  
      //Gerando o Hash a partir do novo SSID criado.
      //
      String NewPASS = gerarHash256(NewSSID);
      delay(1000);
      File_SSID_Pass_Default.Write(NewPASS);           // Writing default SSID name.
      Serial.printf("SSID_Pass_Default: ");                  // Print SSID Password.
      Serial.println(File_SSID_Pass_Default.Read());         // Print SSID Password.
      File_ResetCounter.Write(resetcounterstr); 
      Serial.printf("ResetCounterFILE: ");                  // print ResetCounter
      Serial.println(File_ResetCounter.Read()); 
      
    }

  }
  delay(1000);
  // if (File_SSID_Pass_Default.Read() != "sm@rtCare23") {  // Verify if exists a SSID_Password.... Ps: Only for first running.
  //   File_SSID_Pass_Default.Write("sm@rtCare23");           // Writing default SSID name.
  //   Serial.printf("SSID_Pass_Default: ");                  // Print SSID Password.
  //   Serial.println(File_SSID_Pass_Default.Read());         // Print SSID Password.
  // }
  delay(1000);
  if (File_Device_Name.GetSize() == 0) {      // Verify if exists a SSID.... Ps: Only for first running.
    File_Device_Name.Write(WiFi.hostname());  // Writing default SSID name.
    Serial.printf("Device_Hostname: ");           // Print SSID name.
    Serial.println(File_Device_Name.Read());  // Print SSID name.
  }
  Serial.println("SSID, SSID_Pass, Device_Name successful created.");
}

//-------------------------- Functions------------------------------
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  timer("Waiting ", 3, "seconds to start");
  definedPathes();
  SSID_E_PASS_DevName_Default_Verification();
  wifi_Connection();

  Serial.println("---------------- INICIATING PROGRAM ----------------");
  
}



void loop() {
  Serial.println(tempo_reset);
  if(tempo_reset > 40){
      Serial.println("DELETING");
      File_SSID_Default.Delete();
      File_SSID_Pass_Default.Delete();  // Delete FILE
      File_SSID_Received.Delete();
      File_SSID_Pass_Received.Delete();
      File_Broken_IP.Delete();
      File_Device_Name.Delete();
      Serial.println("ALL FILES WERE DELETED");
      delay(5000);
      tempo_reset = 0;
  }
  
  delay(1000);
  tempo_reset++;
}
