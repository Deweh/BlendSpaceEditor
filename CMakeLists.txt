# CMakeList.txt : CMake project for BlendSpaceEditor, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("BlendSpaceEditor")

# Add source to this project's executable.
add_executable (BlendSpaceEditor "BlendSpaceEditor.cpp"  vcpkg.json "BlendSpaceEditor/Main.cpp" "BlendSpaceEditor/Editor.h" "BlendSpaceEditor/Editor.cpp" "BlendSpaceEditor/ImUtil.h" "BlendSpaceEditor/Drawing.h" "BlendSpaceEditor/NodeBuilder.h" "BlendSpaceEditor/NodeBuilder.cpp" "BlendSpaceEditor/Drawing.cpp" "BlendSpaceEditor/ImUtil.cpp" "BlendSpaceEditor/Nodes/NodeDefinitions.h" "BlendSpaceEditor/Nodes/NodeTypes.h" "BlendSpaceEditor/Nodes/NodeDefinitions.cpp" "BlendSpaceEditor/Serialization/JSON.h" "BlendSpaceEditor/Serialization/JSON.cpp" "BlendSpaceEditor/Nodes/NodeTypes.cpp" "BlendSpaceEditor/Win32Util.h")

find_package(unofficial-imgui-node-editor CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
target_link_libraries(BlendSpaceEditor PRIVATE imgui::imgui ${OPENGL_LIBRARIES} unofficial::imgui-node-editor::imgui-node-editor)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET BlendSpaceEditor PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
