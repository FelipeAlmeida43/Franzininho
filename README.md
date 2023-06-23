# Franzininho Buttons and LEDs Project

This repository contains a sample project for Franzininho board using ESP-IDF framework, demonstrating how to interface buttons and LEDs with the Franzininho microcontroller. The project provides a basic setup for controlling LEDs based on the state of buttons.

## Hardware Requirements

To run this project, you will need the following components:

- Franzininho development board
- Breadboard
- LEDs (preferably different colors)
- Pushbuttons
- Resistors (for LED current limiting)

## Software Requirements

- ESP-IDF framework (v4.4 or later) installed on your development environment
- Espressif IoT Development Framework (ESP-IDF) toolchain

## Getting Started

Follow the steps below to get started with the project:

1. Clone the repository: `git clone https://github.com/FelipeAlmeida43/franzininho.git`
2. Navigate to the project directory: `cd franzininho-buttons-leds`
3. Configure the project: `idf.py menuconfig`
4. Set up the Wi-Fi credentials, GPIO pins (GPIO4, GPIO5, GPIO21, GPIO33), and other configuration parameters as required.
5. Build the project: `idf.py build`
6. Flash the project to your Franzininho board: `idf.py -p /dev/ttyUSB0 flash`
7. Monitor the project output: `idf.py -p /dev/ttyUSB0 monitor`

## Project Structure

The project structure is organized as follows:

- `main` directory: Contains the main project files
  - `app_main.c`: Entry point of the application
  - `button.c` and `button.h`: Implementations and definitions related to buttons
  - `led.c` and `led.h`: Implementations and definitions related to LEDs

## Usage

Once the project is running on your Franzininho board, you can interact with the buttons and observe the LED state. Here are the basic functionalities:

- Pressing the button connected to GPIO4 will toggle the state of the LED connected to GPIO21.
- Pressing the button connected to GPIO5 will toggle the state of the LED connected to GPIO33.

## Customization

Feel free to customize the project according to your requirements. You can modify the GPIO pins assigned to buttons and LEDs, add more buttons or LEDs, or implement additional functionality based on the button events.

## Contributing

Contributions to this project are welcome! If you find any bugs, have feature suggestions, or want to make improvements, please submit a pull request or open an issue on the GitHub repository.

## License

This project is licensed under the [MIT License](LICENSE.txt).

## Acknowledgments

This project is based on the ESP-IDF framework and was inspired by various Franzininho and ESP32 resources and examples available in the ESP-IDF documentation and community.

## Support

For any questions or support, you can reach out to the project maintainer at felipealmeida43@gmail.com.

We hope you find this project helpful and enjoyable!
