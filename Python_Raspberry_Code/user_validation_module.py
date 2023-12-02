import threading
import sys
import select

def waiting_for_user_answer(SmartCare_SSID):
    """Function to wait for the user's response to proceed with the connection to a new SmartCare device found.

    Args:
        SmartCare_SSID (String): Refers to the SSID of the SmartCare IoT device (ESP8266).

    Returns:
       Return 0: Returns 0 when the user validation time expires, or when the user DOES NOT ACCEPT connecting to the device.
       Return 1: Returns 1 when the user ACCEPTS connecting to the device.
    """

    def wait_or_timeout():
        """Function to wait for the user input or wait for 30 seconds.
        """
        with lock:
            print(f"Enter 1 to connect to {SmartCare_SSID}!\nEnter 0 to not connect!")
        i, o, e = select.select([sys.stdin], [], [], 30)  # Wait for input for 30 seconds
        if i:
            response[0] = sys.stdin.readline().strip()
        else:
            response[0] = None

    # Initialize a response variable as a list so it can be modified by the thread
    response = [None]

    # Initialize a counter
    counter = 0

    # Maximum number of attempts
    max_attempts = 3

    # threading.Lock() is used to ensure that the input function is called
    # within a critical section, avoiding concurrent access conflicts.
    # It prevents synchronization issues when the user types.
    # The lock is used when accessing shared thread resources, preventing
    # RACING CONDITIONS.
    lock = threading.Lock()

    while counter < max_attempts:
        response[0] = None  # Reset the response

        print(f"\n{counter + 1}/{max_attempts} attempts.")

        # Start the thread to wait for the user input or wait for 30 seconds
        thread = threading.Thread(target=wait_or_timeout)
        thread.start()
        thread.join()

        # Check the user's response
        if response[0]:
            if response[0] == "1":
                print(f"You entered: {response[0]}")
                print(f"Connecting to {SmartCare_SSID}.")
                return 0
            else:
                print(f"You entered: {response[0]}")
                print(f"Adding {SmartCare_SSID} to the Exclusion List.")
                return 1

        elif counter < max_attempts - 1:
            print("Timeout. Please try again.")
        else:
            print(f"Timeout. Maximum attempts reached.\nAdding {SmartCare_SSID} to the Exclusion List.")
            return 1

        counter += 1

