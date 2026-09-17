# reSpeaker Smart Home AI Assistant
Building our own AI-powered smart-home assistant with Seeed Studio's reSpeaker Voice AI Development Kit.

<img src="/Images/m2e-reSpeaker-AI-project.jpg" height="200">
  
**Joy** is our DIY AI-powered smart home assistant, built around the **Seeed Studio reSpeaker Voice AI Development Kit**, featuring the **XMOS XVF3800** and an integrated **XIAO ESP32-S3**. We deployed the **Xiaozhi AI MCP chatbot** on the XIAO ESP32-S3, turning the reSpeaker into the voice interface for our smart home. Instead of relying only on predefined commands, Joy can understand natural-language requests and translate them into actions through the MCP-based interaction.

The smart-home control system uses another **XIAO ESP32-S3 connected to a four-channel relay module**, which acts as the **home controller** and controls four devices in our demonstration setup. We also built a **7-inch CrowPanel Advance touchscreen dashboard**, based on ESP32-S3, providing a visual interface to monitor and control the connected devices, along with environmental information and controller status.

The project brings **voice AI, embedded hardware, physical device control, and a touchscreen interface** together into one system. The goal is to explore how an open and customizable AI voice interface can be integrated with embedded hardware to create a practical smart-home platform that we can continue to extend and experiment with.
  
<img src="/Images/respeaker-banner.jpg" height="200">

## Features
- Chip upgrade : From XVF3000 to XVF3800
- Quad Microphone Array : **4 high-performance mics** in circular pattern for 360° far-field voice capture up to 5 meters
- Advanced Audio Processing : Powered by XVF3800 with **AEC**, **multi-beamforming**, **de-reverberation**, **DoA detection**, **dynamic noise suppression**, **60dB AGC range**
- Unique Device Serial Number : **Built-in SN** enables multi-device deployments and advanced device management
- Dual Operation Modes : **USB plug-and-play mode** for instant PC connectivity and INT-Device (I2S) mode for integration with embedded systems—configurable via USB or I2C commands by switching the firmware accordingly
- Open Source Compatible : Works with USB hosts (Windows, macOS, Raspberry Pi OS) and I2S hosts (XIAO Series, ESP32, Arduino).
- Visual Feedback : **Programmable RGB LEDs** and status indicators show device states and voice activity
- Equal or better audio quality : compared to previous model

  

## Documentation

- Refer the [Official Wiki Documentation](https://wiki.seeedstudio.com/respeaker_xvf3800_xiao_getting_started/) for more information.  
- Refer [SeeedStudio Announcement Blog](https://www.seeedstudio.com/blog/2025/08/08/new-release-respeaker-xmos-xvf3800-ai-powered-4-microphone-array-for-clear-voice-even-in-noise/)  
- Technical Support [SeeedStudio Support Forum](https://forum.seeedstudio.com/)  
- ReSpeaker Product [Document PDF](https://files.seeedstudio.com/Bazaar/product_pdf/114993702.pdf)  
- Buy reSpeaker XVF3800 🔗 [Product page](https://www.seeedstudio.com/ReSpeaker-XVF3800-With-Case-XIAO-ESP32S3-p-6628.html?utm_source=kol&utm_medium=make2explore&utm_campaign=3800+With+Xiao) 

**Software**
- Arduino IDE [Download](https://www.arduino.cc/en/software/)  
- Arduino-ESP32 Core [Instructions](https://wiki.seeedstudio.com/respeaker_xvf3800_xiao_getting_started/#software-preparation)  
- Arduino Audio Tools library [Download](https://github.com/pschatzmann/arduino-audio-tools)  
  
**Hardware**
- [**reSpeaker XMOS XVF3800 with XIAO ESP32S3**](https://www.seeedstudio.com/ReSpeaker-XVF3800-With-Case-XIAO-ESP32S3-p-6628.html?utm_source=kol&utm_medium=make2explore&utm_campaign=3800+With+Xiao)  
- USB Type C to Type A cable  
 

------------------------------------------------------------------------------------------------------

📕 **YouTube Video Links**  

▶️ Inside the ReSpeaker XVF3800 Voice AI Dev Kit✨  #ai #esp32 🔗  https://youtube.com/shorts/igvbQ5YoHng  

▶️ DIY Project on ReSpeaker XVF3800 🔗  https://youtu.be/  
  

-------------------------------------------------------------------------------------------------------
📒 **Important Links**  
 
📖 reSpeaker XVF3800 WiKi :🔗 https://wiki.seeedstudio.com/respeaker_xvf3800_xiao_getting_started  
  
💾 SeeedStudio Community Projects : 🔗 https://www.hackster.io/seeed    
  
📞 SeeedStudio Support Forums : 🔗 https://forum.seeedstudio.com/    
  

🛒  Purchase  -   
  
🟥 Buy reSpeaker XVF3800 🔗 [Product page](https://www.seeedstudio.com/ReSpeaker-XVF3800-With-Case-XIAO-ESP32S3-p-6628.html?utm_source=kol&utm_medium=make2explore&utm_campaign=3800+With+Xiao)  

🌐 SeeedStudio Official Website : 🔗 https://www.seeedstudio.com  

  
🚀 Projects - 
  - Refer Project [reSpeaker XVF3800 + Agora Conversational AI](https://wiki.seeedstudio.com/respeaker_xvf3800_agora_convo_client/)  
  - Refer Project [reSpeaker Xiaozhi AI voice assistant](https://wiki.seeedstudio.com/respeaker_xvf_3800_xiaozhi/)  


------------------------------------------------------------------------------------------------------

📜 Source Code, Circuit Diagrams and Documentation : 

🌐 GitHub Repository - 🔗 https://github.com/make2explore/reSpeaker-XVF3800    
  
🌐 Hackster Blog - 🔗 https://www.hackster.io/make2explore  
  
🌐 Instructable Blog - 🔗 https://www.instructables.com/make2explore  
  

------------------------------------------------------------------------------------------  

[![CC BY-NC-SA 4.0][cc-by-nc-sa-shield]][cc-by-nc-sa]

This work is licensed under a
[Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License][cc-by-nc-sa].

[![CC BY-NC-SA 4.0][cc-by-nc-sa-image]][cc-by-nc-sa]

[cc-by-nc-sa]: http://creativecommons.org/licenses/by-nc-sa/4.0/
[cc-by-nc-sa-image]: https://licensebuttons.net/l/by-nc-sa/4.0/88x31.png
[cc-by-nc-sa-shield]: https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg