cmake_minimum_required (VERSION 3.8)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project (
  "BlendGraphEditor"
  VERSION 1.0.0
  LANGUAGES CXX
)

file(GLOB_RECURSE SRC_FILES BlendSpaceEditor/*.cpp)

# Add source to this project's executable.
add_executable (${PROJECT_NAME}
 vcpkg.json
 "app_resources.rc"
 "BlendSpaceEditor.cpp"
 ${SRC_FILES})

find_package(unofficial-imgui-node-editor CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(directxtk CONFIG REQUIRED)
target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
    imgui::imgui
    unofficial::imgui-node-editor::imgui-node-editor
    Microsoft::DirectXTK
)

target_compile_options(
	${PROJECT_NAME}
	PRIVATE
		/MD
)

target_link_options(
	${PROJECT_NAME}
	PRIVATE
	  /SUBSYSTEM:windows
    /ENTRY:mainCRTStartup
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/dist
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_CURRENT_SOURCE_DIR}/dist
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PDB_FILE:${PROJECT_NAME}> ${CMAKE_CURRENT_SOURCE_DIR}/dist
  COMMENT "Copying .exe and .pdb to /dist/..."
)
