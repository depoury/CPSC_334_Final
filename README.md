# CPSC_334_Final

For my final project, I made a wireless lamp with 2-part colour control and speaker functionalities.

Modelled on a Japanese floor lantern, I wanted to give it a modern twist and thus added the functionalities of wirelessly controlled smart lamps (with added sound!)

I also created an app to be run on any Processing-capable device to automatically control the hues and sounds of the lamp, with the option of manual control for more discerning users.

The motivations and results of this project are laid out in more detail in my public blog post found at the following link:

http://201903cpsc33401.canvaspress.yale.edu/2019/12/17/smart-lamp/ 


## My CPSC 334 Final Project Code Repo

Please find 4 folders, each with its corresponding code within this repo.

*Processing_UDP* contains the .pde code and background for it that was used during the live showcase in Leeds.

*RGB_Cathode* contains arduino code for testing the RGB LEDs in the final product.

*Speaker* contains arduino code for testing the speaker in the final product.

*WiFi_Enabled* contains the final arduino code run on the ESP32 for the showcase and a manual override Python script that would grant me control of the lamp over WiFi in the event of failure elsewhere. (Fortunately, this went unused!)

This project requires the following hardware:

- 1 x ESP32 (Pico Model)
- 1 x Battery Pack
- 1 x RGB Cathode LED (Extra Bright)
- 10 x RGB Cathode LED (Regular)
- 10 x 220Ω Resistor
- 1 x 8Ω 1W Speaker


