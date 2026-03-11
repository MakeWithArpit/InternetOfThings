# HTTP_CONNECTION_between_esp32_and_google_spreadsheet

📡 ESP32 to Google Spreadsheet using HTTP (GET & POST)
This repository demonstrates how to send and receive data between an ESP32 and Google Sheets using the HTTP protocol. With the help of Google Apps Script and ESP32’s WiFi capabilities, you can easily integrate real-time data logging and retrieval with your spreadsheet.

Repository Structure :  
The project is organized into three folders:  

📁 GET_Function

  This folder contains:  
  1. Google Apps Script code (for handling GET requests).  
  2. ESP32 Arduino code.

  Purpose:  
  Fetch data from a Google Spreadsheet and store it into a variable on the ESP32. Useful for reading configuration values or other inputs from the cloud.

📁 POST_Function

  This folder contains:  
  1. Google Apps Script code (for handling POST requests).  
  2. ESP32 Arduino code.  

  Purpose:  
  Send data from the ESP32 to a Google Spreadsheet. Ideal for logging sensor data, device events, or status updates.  

📁 GET_and_POST_Combined  

  This folder contains:  
  1. A single Google Apps Script (capable of handling both GET and POST).  
  2. A single ESP32 code file (combining both GET and POST operations).  
  
  Purpose:  
  Implement both data upload and fetch functionalities within a single ESP32 sketch, reducing complexity and enhancing project scalability. Great for IoT systems that require two-way
  communication with Google Sheets.  

🔧 Key Features  

  1. No third-party platforms required (Google Apps Script only).  
  2. Fully customizable and open-source.  
  3. Simple structure with practical use cases.  
  4. Uses ESP32’s built-in HTTPClient library.  

🚀 Getting Started  

  Each folder includes clearly documented code and instructions to help you:  

  1. Set up Google Apps Script as a web app.  
  2. Configure your ESP32 with WiFi and HTTP functionality.  
  3. Easily test GET and POST interactions with Google Sheets.  
