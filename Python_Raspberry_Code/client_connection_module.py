import socket  
import time


def client_connection(ssid,PW,broker):
    """Used to communicate with the ESP8266, and after successful 
    communication, it sends the SSID and password of the currently 
    connected Wi-Fi network and sends the IP of the MQTT broker to the ESP. 

    Args:
        ssid (String): SSID of the connected Wi-Fi network.
        PW (String): Password of the connected Wi-Fi network.
        broker (String): IP of the MQTT broker.
    """
    host = "192.168.4.1" #ESP8266 IP in local network
    port = 5000             #ESP8266 Server Port      
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    
    try:
        print("Connecting to ESP")
        # Connect to the ESP Server
        sock.connect((host, port))

        print("Connected successfully.")
        # Messages to be sent
        messages = [ssid + "\n", PW + "\n",broker + "\n"]

        print("Sending data to the ESP")
        # Sending the messages
        for message in messages:
            sock.send(message.encode())
            time.sleep(2)  # Wait for two seconds between messages (PS: Depending on the number of characters, the time may need to be longer.)

        # Indicates to the server that the messages have ended
        sock.send("End\n".encode())

        print("Data sent successfully.")
        # Close the client socket
        sock.close()
    except Exception as exce:
        print(f"Error connecting or sending data: {str(exce)}")
        sock.close()
        return 1
