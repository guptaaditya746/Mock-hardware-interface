cmake_minimum_required(VERSION 3.8)
project(mock_hardware_interface)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(hardware_interface REQUIRED)
find_package(pluginlib REQUIRED)

# Add your source file
add_library(mock_hardware SHARED src/mock_hardware.cpp)

# Link dependencies
ament_target_dependencies(mock_hardware
  hardware_interface
  rclcpp
  pluginlib
)

# Add include directories
target_include_directories(mock_hardware PRIVATE include)

# Install the plugin XML file
install(
  FILES mock_hardware_plugin.xml
  DESTINATION share/${PROJECT_NAME}
)

# Install launch, config, and description directories
install(
  DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

install(
  DIRECTORY description
  DESTINATION share/${PROJECT_NAME}
)

install(
  DIRECTORY config
  DESTINATION share/${PROJECT_NAME}
)

# Install the library
install(
  TARGETS mock_hardware
  LIBRARY DESTINATION lib
)

# Register the plugin
pluginlib_export_plugin_description_file(hardware_interface mock_hardware_plugin.xml)

ament_package()
