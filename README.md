# Minecraft-Cat-V1
Code (Joystick transmitter &amp; Cat reciever) + 3D Models (Cat parts)
TinkerCAD: https://www.tinkercad.com/things/lH2LceW6PMJ-minecraft-cat-v1

# Preface
The legs and the inner mechanism might look a bit odd, but I designed it this way so that it reduces the need for too many servos. Since it doesn't use that many servos, it's ability to move is quite limited compared to other quadruped robots, but Minecraft cats have relatively simple walking animations. 
It can walk forward + backward, along with turning and a special trick (you can edit it to be whatever you want that trick to be!)

# Recommendations
1.  This cat uses around 7 Motors in total. You could use SG90's for the servos connected to the legs, but I really recommend using atleast 1 MG90 for the Rack & Pinion. The rack and Pinion handles a lot of weight (lifting the legs up and pushing the body down) and an SG90 might not provide enough torque or the inner plastic gears will strip. 
  Personally, I used MG90 for the whole thing
2.  Use a PCA9685 (servo driver) for all the servos. Originally, I didn't intend to use one and I saw my legs were glitching a lot (some would move to random angles spontaneously), but after using a PCA9685, it removed that issue and helped the servos move more smoothly.
3.  Please be careful to not short circuit :,) I don't have a PCB designed for this (yet)
4.  Everything is meant to be put together with M2 screws (8mm), except for the servos & the rack and pinion, use the screws that come with the servos.

# Final Notes
I don't have any set ideas on where to screw the PCA9685 and the ESP32 into. The butt, infront of the rack, and head have space for extra components but it's also important to keep in mind the center of gravity so this doesn't tip over. 

V1 was to mainly see how the cat would walk since that was my biggest worry, I didn't see a lot of guides on making cheap 4 legged robots (especially ones that move like a Minecraft cat) so it was a big struggle to design this. I'm not a robotics, mechatronics, electrical, or mechanical student. 

The body and head might not attach correctly or it'll tip over due to uneven weight distribution, I don't know yet. I don't have the money or time to print them out currently :P

If you have any tips or advice, please let me know! I really want to improve this into a real functioning Minecraft cat.
