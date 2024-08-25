cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("BlendGraphEditor")

# Add source to this project's executable.
add_executable (${PROJECT_NAME}
 vcpkg.json
 "app_resources.rc"
 "BlendSpaceEditor.cpp"
 "BlendSpaceEditor/Main.cpp"
 "BlendSpaceEditor/Editor.cpp"
 "BlendSpaceEditor/NodeBuilder.cpp"
 "BlendSpaceEditor/Drawing.cpp"
 "BlendSpaceEditor/ImUtil.cpp"
 "BlendSpaceEditor/Nodes/NodeDefinitions.cpp"
 "BlendSpaceEditor/Nodes/NodeTypes.cpp")

find_package(unofficial-imgui-node-editor CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui ${OPENGL_LIBRARIES} unofficial::imgui-node-editor::imgui-node-editor)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()
