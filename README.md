# Human Health Monitor

An AI-enabled Human Health Monitor using Arduino, health sensors, Machine Learning, Gemini, and a Single Tool AI Agent.

## Project Overview

This project monitors important health parameters and provides intelligent health-status analysis using hardware sensors, Machine Learning, and Generative AI.

## Hardware Components

- Arduino UNO R3
- Temperature Sensor
- Pulse Oximeter Sensor
- MPU6050
- OLED Display
- LED Indicators
- Buzzer

## Key Features

- Temperature monitoring
- Heart-rate monitoring
- SpO2 monitoring
- Movement/activity monitoring
- Abnormal-condition alert using LED and buzzer
- ML-based health-status prediction
- Gemini-based health explanation
- Single Tool AI Agent

## Machine Learning

The collected health data contains:

- BPM
- SpO2
- Temperature

Multiple ML models were trained and compared using:

- Accuracy
- Precision
- Recall
- F1 Score

The trained model was saved as a `.pkl` file.

## AI Module

The ML prediction is combined with Gemini to provide a simple explanation of the predicted health status.

The Single Tool AI Agent workflow allows the AI model to decide when the available tool is required and use its result to generate the final response.

## Project Files

- `Human_Health_Monitor.ino` – Arduino hardware code
- `Human_Health_Monitor_AI.ipynb` – ML, Gemini and AI-agent implementation

## Future Scope

- Cloud-based health monitoring
- Mobile application integration
- Real-time patient monitoring
- Improved ML models and larger datasets
