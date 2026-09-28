# Human Health Monitor

An AI-enabled Human Health Monitor using Arduino, health sensors, Machine Learning, Gemini, and a Single Tool AI Agent.

## Project Overview

This project is designed to monitor important health parameters using sensors connected to an Arduino UNO R3. The collected health data is processed using Machine Learning, and Gemini is used to provide a simple explanation of the predicted health status.

## Hardware Components

- Arduino UNO R3
- Temperature Sensor (DS18B20)
- Pulse Oximeter Sensor
- I2C Display
- LED Indicators
- Buzzer
- Li-ion Battery
- Li-ion Battery Holder
- HW Battery / Battery Module for Board Power

## Key Features

- Temperature monitoring
- Heart-rate monitoring
- SpO2 monitoring
- Abnormal-condition alert using LED and buzzer
- ML-based health-status prediction
- Gemini-based health explanation
- Single Tool AI Agent

## Temperature Monitoring

The DS18B20 temperature sensor is used to monitor temperature. An abnormal temperature condition can activate the red LED and buzzer, while the normal condition is indicated using the green LED.

## Health Monitoring

The pulse oximeter sensor is used to obtain heart-rate and SpO2 values. The monitored values can be displayed through the I2C display.

## Machine Learning

The collected health data contains:

- BPM
- SpO2
- Temperature

Machine Learning models were trained and evaluated using the collected health data.

The models were compared using:

- Accuracy
- Precision
- Recall
- F1 Score

The trained model was saved as a `.pkl` file for further prediction.

## Gemini AI Module

The Machine Learning prediction is combined with Gemini to provide a simple and understandable explanation based on the sensor values and predicted health status.

## Single Tool AI Agent

The project also includes a Single Tool AI Agent workflow.

The workflow includes:

1. Providing health data to the AI model
2. Providing information about the available tool
3. Allowing the AI model to decide whether the tool is required
4. Executing the tool when required
5. Sending the tool result back to the AI model
6. Generating the final response

## Project Files

- `Human_Health_Monitor.ino` – Arduino hardware and sensor code
- `Human_Health_Monitor_AI.ipynb` – Machine Learning, Gemini and Single Tool AI Agent implementation

## Power Supply

The project uses a Li-ion battery with a battery holder for portable power. A battery/module is also used to power the Arduino board.

## Future Scope

- Real-time remote health monitoring
- Mobile application integration
- Cloud-based health data storage
- Larger health datasets for improved ML performance
- Advanced AI-based health analysis
