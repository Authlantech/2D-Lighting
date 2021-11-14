# 2D_Lighting
2D_Lighting is a program that uses SFML to render dynamic lighting 

![2D Lighting 7 11 2021 18_12_31](https://user-images.githubusercontent.com/80747347/140650844-bd94b6fd-8825-49bd-bc60-58697bb676aa.png)
# Building Prerequisites
- SFML 2.5.1 must be linked
## Linking SFML with Cmake 
In CMakeLists.txt file (file in the same place as other .h and .cpp files) you must set SFML_DIR as path of folder that contains SFMLConfig.cmake file.
### Example Code: 
`set(SFML_DIR "../../SFML-2.5.1/lib/cmake/SFML")`
# After Building 
"white_glow.png" file must be added to folder that contains executable!
# Usage of Program 
- By clicking and releasing left mouse button you can create red background pieces on screen. In order to set a size of a background piece you need to drag your mouse after clicking left mouse button.
- By clicking right mouse button program will automaticly create a 100x100 sized obstacle on screen.
- You can also make changes like changing the color of obstacles and backgrounds, changing the size of obstacles or changing the color of the light source by editing 2D Lighting.cpp file.
