cmake_minimum_required(VERSION 3.8)
project(pkg_template_multi_language)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

# to be added by user
add_executable(node_template_cpp src/node_template_cpp.cpp) # add for each node
ament_target_dependencies(node_template_cpp rclcpp) # add for each node

install(TARGETS
  node_template_cpp # add for each node
  DESTINATION lib/${PROJECT_NAME}
)

install(PROGRAMS
  scripts/node_template_py.py # add for each node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
