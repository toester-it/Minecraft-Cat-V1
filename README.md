# Minecraft-Cat-V1
Code (Joystick transmitter &amp; Cat reciever) + 3D Models (Cat parts)

# Intro
I've had this idea for about a year now, so since about grade 11. I can't have a pet cat so I've always wanted to make a robotic cat, and Minecraft cats are my 
absolute favourite in-game animal across ALL games >U<

I never had the proper parts before but I've been saving up money so that I can 3D print and buy the servo motors, overall I think it was a good investment since I learned a lot about ESP32's, 3D modelling, and C++ ! 
The legs and the inner mechanism might look a bit odd, but I designed it this way so that it reduces the need for too many servos. It can walk forward + backward, along with turning and a special trick (you can edit it to be whatever you want that trick to be!)

# Recommendations
1.  This cat uses around 7 Motors in total. You could use SG90's for the servos connected to the legs, but I really recommend using atleast 1 MG90 for the Rack & Pinion. The rack and Pinion handles a lot of weight (lifting the legs up and pushing the body down) and an SG90 might not provide enough torque or the inner plastic gears will strip. 
  Personally, I used MG90 for the whole thing
2.  Use a PCA9685 (servo driver) for all the servos. Originally, I didn't intend to use one and I saw my legs were glitching a lot (some would move to random angles spontaneously), but after using a PCA9685, it removed that issue and helped the servos move more smoothly.
3.  Please be careful to not short circuit :,) I don't have a PCB designed for this (yet)
4.  Everything is meant to be put together with M2 screws (8mm), except for the servos, use the screws that come with the servos to screw on any add ons.

# Final Notes
This was to mainly see how the cat would walk since that was my biggest worry, I didn't see a lot of guides on making cheap 4 legged robots so it was a big struggle to design this. I'm not a robotics, mechatronics, electrical, or mechanical student. The body and head might not attach correctly or it'll tip over due to uneven weight distribution, I don't know yet :P

If you have any tips or advice, please let me know! I really want to improve this into a real functioning cat
