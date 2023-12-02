import subprocess
from array import array
import numpy as np
import re
#import time

#---------------------------------------- FUNCTIONS -------------------------------------------
def what_wifi_pass():
    """Checks the password of the connected Wi-Fi network on the device.

    Returns:
        String: Returns the password if a network is connected.
    """
    process = subprocess.run(['nmcli', 'device', 'wifi', 'show-password'], stdout=subprocess.PIPE)
    result = process.stdout.decode('utf-8').strip()
    result = re.split('[\n :]', result) # Remove ALL : and \n
    #print(result[8])
    if process.returncode == 0:
        return result[8]
    else:
        return ''
#-----------------------
def what_wifi():
    """Checks which SSID is connected to the computer.

    Returns:
        String: Returns the SSID if a network is connected.
    """
    process = subprocess.run(['nmcli', 'device', 'wifi', 'show-password'], stdout=subprocess.PIPE)
    result = process.stdout.decode('utf-8').strip()
    result = re.split('[\n :]', result) # Remove ALL : and \n

    if process.returncode == 0:
        return result[2]
    else:
        return ''
#-----------------------
def is_connected_to(ssid: str):
    process = subprocess.run(['nmcli', '-t', '-f', 'name', 'connection', 'show', '--active'], stdout=subprocess.PIPE)
    result = process.stdout.decode('utf-8').strip()
    result = re.split('\n', result)
    return result
#-----------------------
def scan_wifi():
    """Performs a scan for new Wi-Fi devices.

    Returns:
        List: Returns a list of Wi-Fi devices found on the network.
    """
    process = subprocess.run(['nmcli', '-t', '-f', 'SSID,SECURITY,SIGNAL', 'dev', 'wifi'], stdout=subprocess.PIPE)
    result = process.stdout.decode('utf-8').strip()
    if process.returncode == 0:
        print(result)
        return process.stdout.decode('utf-8').strip().split('\n')
    else:
        return []
#-----------------------
# Scan for a parts or pieces of wifi ssid

def scan_ssid_wifi(ssid):
    """Checks if a Wi-Fi network exists using a String.
        Example: Variable ssid = "MyWifiNetwork".
        When this function is executed, a search on the Wi-Fi network will be performed to check if any Wi-Fi network contains the provided string "MyWifiNetwork".
        If the string is found, the function returns the complete name of the Wi-Fi network.
        Example: "MyWifiNetwork987653"
        If the suggested string is not found, the function returns 0.

    Args:
        ssid String: A part of an SSID to search for in the network.

    Returns:
        Return SSID_FOUND: Returns the found SSID.
        Return 0: Returns 0 if the SSID is not found.
    """
    process = subprocess.run(['nmcli', '-t', '-f', 'SSID', 'dev', 'wifi'], stdout=subprocess.PIPE)
    thislist = process.stdout.decode('utf_8').strip().split('\n')

    for index in range(len(thislist)):
     if thislist[index].count(ssid) > 0:
         return thislist[index]

    return 0
#-----------------------
def is_wifi_available(ssid: str):
    return ssid in [x.split(':')[0] for x in scan_wifi()]

#-----------------------MODDED
def connect_to_wifi(ssid: str, password: str):
    if not scan_ssid_wifi(ssid):
        return False
    subprocess.run(['nmcli', 'd', 'wifi', 'connect', ssid, 'password', password])
    return is_connected_to(ssid)

#-----------------------
def connect_to_saved(ssid: str):
    if not is_wifi_available(ssid):
        return False
    subprocess.call(['nmcli', 'c', 'up', ssid])
    return is_connected_to(ssid)
#-----------------------
def wifi_remover(ssid):
    """Removes the saved SSID from the device.

    Args:
        ssid String: The SSID to be deleted.
    """
    subprocess.run(['nmcli', 'con', 'delete', ssid])

####-----------------------------------------------------------
