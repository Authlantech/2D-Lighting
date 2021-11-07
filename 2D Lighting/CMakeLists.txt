cmake_minimum_required (VERSION 3.8)

add_executable (2D_Lighting "2D Lighting.cpp" "2D Lighting.h" "GameEngine.h" "GameEngine.cpp")

# Here you have to set SFML_DIR as the path of folder that contains SFMLConfig.cmake file
# Example :
# set(SFML_DIR "../SFML/lib/cmake/SFML")

 find_package(SFML 2.5.1 COMPONENTS graphics system window REQUIRED)
 target_link_libraries(2D_Lighting sfml-graphics sfml-system sfml-window)