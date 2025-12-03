cmake_minimum_required(VERSION 3.13)

project("Rendering Application")

add_executable(app main.cpp)

target_include_directories(app
    PRIVATE ${CMAKE_SOURCE_DIR}/Renderer/include/
)

find_package(Vulkan REQUIRED COMPONENTS glslc)

function(add_shaders TARGET_NAME)
  set(SHADER_SOURCE_FILES ${ARGN}) # the rest of arguments to this function will be assigned as shader source files
  
  # Validate that source files have been passed
  list(LENGTH SHADER_SOURCE_FILES FILE_COUNT)
  if(FILE_COUNT EQUAL 0)
    message(FATAL_ERROR "Cannot create a shaders target without any source files")
  endif()

  set(SHADER_COMMANDS)
  set(SHADER_PRODUCTS)

  foreach(SHADER_SOURCE IN LISTS SHADER_SOURCE_FILES)
      cmake_path(ABSOLUTE_PATH SHADER_SOURCE NORMALIZE)
    cmake_path(GET SHADER_SOURCE FILENAME SHADER_NAME)
    
    # Build command
    list(APPEND SHADER_COMMANDS COMMAND)
    list(APPEND SHADER_COMMANDS Vulkan::glslc)
    list(APPEND SHADER_COMMANDS "${SHADER_SOURCE}")
    list(APPEND SHADER_COMMANDS "-o")
    list(APPEND SHADER_COMMANDS "${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.spv")

    # Add product
    list(APPEND SHADER_PRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.spv")

  endforeach()

  add_custom_target(${TARGET_NAME} ALL
    ${SHADER_COMMANDS}
    COMMENT "Compiling Shaders [${TARGET_NAME}]"
    SOURCES ${SHADER_SOURCE_FILES}
    BYPRODUCTS ${SHADER_PRODUCTS}
  )
endfunction()

add_shaders(shaders 
    ${CMAKE_SOURCE_DIR}/Renderer/basic_shaders/simple_shader.vert 
    ${CMAKE_SOURCE_DIR}/Renderer/basic_shaders/simple_shader.frag
)

target_link_directories(app PRIVATE ${CMAKE_SOURCE_DIR}/build)

target_link_libraries(app Renderer)
