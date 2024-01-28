# SmartHomeSystem

The Smart Home System is a home automation project designed to provide centralized control over various smart devices within a household. The project includes a central server hosted on a mother device, which serves a website to enable user interaction and control over connected smart devices.

## Features:

1. **RGB Controller:**
   - Allows users to control the color and state (on/off) of RGB lighting systems in their home.
   - Provides real-time updates on the current state of the RGB controller.

2. **Light Controller:**
   - Enables users to manage the state of lighting fixtures, supporting options such as turning lights on, off, or setting them to auto mode.
   - Offers a user-friendly interface for controlling lighting within the home.

3. **Door Lock:**
   - Provides control over the state of the door lock, allowing users to open, close, or set the door to auto mode.
   - Enhances security and convenience by integrating door lock control into the centralized system.

4. **User Registration:**
   - Allows users to register accounts with the Smart Home System, ensuring personalized access to device control features.

5. **Device Registration:**
   - Enables users to register new smart devices to be controlled by the system, including RGB controllers, light controllers, and door locks.
   - Generates unique secret keys for each registered device to ensure secure communication.

## Issues:

- **Wemos Mini TFT Errors:**
  - Currently facing issues with Wemos Mini TFT due to potential incompatibility or errors related to the GFX library.
  - Further investigation and troubleshooting are required to address these errors and ensure proper functionality.

## Future Development:

- Integration of additional smart devices (e.g., thermostats, cameras) into the Smart Home System.
- Implementation of advanced automation scenarios and scheduling features.
- Enhancements to the user interface for improved user experience.

The Smart Home System is designed to provide users with a comprehensive platform for managing and controlling various smart devices within their home, promoting convenience, energy efficiency, and security. Ongoing development and troubleshooting efforts are aimed at refining and expanding the capabilities of the system.

Todo:
usuń z register rodzaj urządzenia i zmien tylko na ip 
w get device secret wysyłasz ip z rodzajem i to tworzy wiersz w bd i zwraca klucz
