cmake_minimum_required(VERSION 3.26)
project(Minigin)

if(EMSCRIPTEN)
  message("Using Emscripten!")
endif()

if(EMSCRIPTEN)
  set(ENGINE_NAME minigin_web)
  set(GAME_NAME digdug_web)
  set(TARGET_PARAM )
else()
  set(ENGINE_NAME minigin)
  set(GAME_NAME digdug)
  set(TARGET_PARAM WIN32)
endif()

# added directory variables to i can change the folder stucture more easily later
set(CORE_DIR "Minigin/Core")
set(COMPONENTS_DIR "Minigin/Components")
set(INPUT_DIR "Minigin/Input")
set(INTERFACES_DIR "Minigin/Interfaces")

add_library(${ENGINE_NAME} STATIC
    "${CORE_DIR}/Font.cpp"
    "${CORE_DIR}/GameObject.cpp"
    "${CORE_DIR}/Minigin.cpp"
    "${CORE_DIR}/Renderer.cpp"
    "${CORE_DIR}/ResourceManager.cpp"
    "${CORE_DIR}/Scene.cpp"
    "${CORE_DIR}/SceneManager.cpp"
    "${CORE_DIR}/Texture2D.cpp"
    "${CORE_DIR}/Transform.cpp"
    "${CORE_DIR}/Timer.cpp"
    "${CORE_DIR}/Singleton.cpp"

    "${COMPONENTS_DIR}/Component.cpp"
    "${COMPONENTS_DIR}/TextComponent.cpp"
    "${COMPONENTS_DIR}/RenderComponent.cpp"
    "${COMPONENTS_DIR}/FpsCounterComponent.cpp"
    "${COMPONENTS_DIR}/ScoreDisplay.cpp"
    "${COMPONENTS_DIR}/HealthDisplay.cpp"
    
    "${INPUT_DIR}/InputManager.cpp"
    "${INPUT_DIR}/Gamepad.cpp"
    "${INPUT_DIR}/Command.cpp"

    "${INTERFACES_DIR}/Subject.cpp"
)

target_include_directories(${ENGINE_NAME} PUBLIC 
    ${CORE_DIR}
    ${COMPONENTS_DIR}
    ${INPUT_DIR}
    ${INTERFACES_DIR}
)

# enable c++20 features
target_compile_features(${ENGINE_NAME} PUBLIC cxx_std_20)

# list source files here
add_executable(${GAME_NAME} ${TARGET_PARAM}
    "Game/Main.cpp"
)
target_compile_features(${GAME_NAME} PUBLIC cxx_std_20)

include(FetchContent)
# add glm
find_package(glm CONFIG QUIET)
if(NOT glm_FOUND)
  FetchContent_Declare(
    glm
    URL https://github.com/g-truc/glm/releases/download/1.0.3/glm-1.0.3.zip
    DOWNLOAD_NO_PROGRESS ON
    DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  )
  FetchContent_MakeAvailable(glm)
endif()

# Add SDL3
find_package(SDL3 3.4 CONFIG QUIET)
if(NOT SDL3_FOUND)
  if (WIN32)
    FetchContent_Declare(
      SDL3
      URL https://www.libsdl.org/release/SDL3-devel-3.4.0-VC.zip
      DOWNLOAD_NO_PROGRESS ON
      DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
    )
    FetchContent_MakeAvailable(SDL3)
    list(PREPEND CMAKE_PREFIX_PATH "${sdl3_SOURCE_DIR}")
    find_package(SDL3 CONFIG REQUIRED)
  else()
    FetchContent_Declare(
      SDL3
      GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
      GIT_TAG release-3.4.0
      GIT_SHALLOW TRUE
      GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(SDL3)
  endif()
endif()

# Add SDL3_ttf
find_package(SDL3_ttf 3.2.2 CONFIG QUIET)
if(NOT SDL3_ttf_FOUND)
  if (WIN32)
    FetchContent_Declare(
      SDL3_ttf
      URL https://www.libsdl.org/projects/SDL_ttf/release/SDL3_ttf-devel-3.2.2-VC.zip
      DOWNLOAD_NO_PROGRESS ON
      DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
    )
    FetchContent_MakeAvailable(SDL3_ttf)
    list(PREPEND CMAKE_PREFIX_PATH "${sdl3_ttf_SOURCE_DIR}")
    find_package(SDL3_ttf CONFIG REQUIRED)
  else()
    FetchContent_Declare(
      SDL3_ttf
      GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
      GIT_TAG release-3.2.2
      GIT_SHALLOW TRUE
      GIT_PROGRESS TRUE
    )
    set(SDL3TTF_INSTALL OFF)
    if(EMSCRIPTEN)
      list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/emscripten")
      set(SDL3TTF_USE_SYSTEM_FREETYPE ON)
      set(SDL3TTF_VENDORED OFF)
      set(SDLTTF_PLUTOSVG OFF)
      add_compile_options(-sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1)
      add_link_options(-sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1)
    endif()
    FetchContent_MakeAvailable(SDL3_ttf)
  endif()
endif()

if (WIN32)
  # Add VLD (Visual Leak Detector) for memory leak detection on Windows
  find_package(VLD CONFIG)
endif()

# Dear ImGui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui
  GIT_TAG v1.92.5-docking
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)

target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${sdl3_SOURCE_DIR}/include)
target_link_libraries(imgui PUBLIC SDL3::SDL3)

# Optional: Set IMGUI_INCLUDE_DIR to the path of imgui sources, if different from the default. 
set(IMGUI_INCLUDE_DIR ${imgui_SOURCE_DIR})
FetchContent_Declare(
    imgui_plot
    GIT_REPOSITORY https://github.com/soulthreads/imgui-plot.git
    GIT_TAG master      # this feels kind of dangerous but i don't have other branches
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(imgui_plot)

target_compile_definitions(imgui_plot PUBLIC IMGUI_DEFINE_MATH_OPERATORS)

# enable c++20 features
target_compile_features(imgui PUBLIC cxx_std_20)

target_include_directories("${ENGINE_NAME}" PUBLIC 
  ${CMAKE_SOURCE_DIR}/Minigin/
  ${glm_SOURCE_DIR}
  ${sdl3_SOURCE_DIR}/include
  ${sdl3_image_SOURCE_DIR}/include
  ${sdl3_ttf_SOURCE_DIR}/include
  ${imgui_SOURCE_DIR}
  ${VLD_INCLUDE_DIR}
)

# Add steam if available
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/steam")
find_package(Steamworks QUIET)

target_include_directories("${ENGINE_NAME}" PUBLIC
  ${CMAKE_SOURCE_DIR}/Minigin/
  ${VLD_INCLUDE_DIR}
)

target_link_libraries("${ENGINE_NAME}" PUBLIC
  SDL3::SDL3
  SDL3_ttf::SDL3_ttf
  glm::glm
  imgui
  imgui_plot
  ${VLD_LIBRARIES}
)

if(MSVC)
  # compile at warning level 4 + treat warnings as errors
  target_compile_options(${ENGINE_NAME} PRIVATE /W4 /WX)
  target_compile_options(${GAME_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${ENGINE_NAME} PRIVATE -Wall -Wextra -Werror)
  target_compile_options(${GAME_NAME} PRIVATE -Wall -Wextra -Werror)
endif()

if(EMSCRIPTEN)

  set_target_properties(${GAME_NAME} PROPERTIES
    #OUTPUT_NAME "minigin_web"
    SUFFIX ".html"
  )

  target_link_options(${GAME_NAME} PRIVATE
    -sUSE_FREETYPE=1
    -sUSE_HARFBUZZ=1
    -sALLOW_MEMORY_GROWTH
    --preload-file "${CMAKE_SOURCE_DIR}/Data@/"
  )

  add_custom_command(TARGET ${GAME_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/web"
            "$<TARGET_FILE_DIR:${GAME_NAME}>/"
  )

elseif(WIN32)
  target_link_libraries("${ENGINE_NAME}" PUBLIC Xinput9_1_0)

  # Set minigin as the default startup project
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${ENGINE_NAME})
  # Set working directory to output folder
  set_target_properties(${ENGINE_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${ENGINE_NAME}>")

  # Copy SDL dlls to output folder
  set(SDL_RUNTIME_DLLS
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
  )

  foreach(dllTarget IN LISTS SDL_RUNTIME_DLLS)
    add_custom_command(
      TARGET ${ENGINE_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "$<TARGET_FILE:${dllTarget}>"
          "$<TARGET_FILE_DIR:${ENGINE_NAME}>"
      VERBATIM
    )
  endforeach()

  if(Steamworks_FOUND)
    target_link_libraries("${ENGINE_NAME}" PUBLIC Steamworks::Steamworks)
    target_compile_definitions(${ENGINE_NAME} PRIVATE USE_STEAMWORKS)
    add_custom_command(TARGET ${ENGINE_NAME} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy "${Steamworks_RUNTIME_DLL}" "$<TARGET_FILE_DIR:${ENGINE_NAME}>")
    add_custom_command(TARGET ${ENGINE_NAME} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E echo "480" > "$<TARGET_FILE_DIR:${ENGINE_NAME}>/steam_appid.txt"
      COMMENT "Generating steam_appid.txt"
    )
  endif()

  add_custom_command(TARGET ${ENGINE_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Data" "$<TARGET_FILE_DIR:${ENGINE_NAME}>/Data"
  )
elseif(APPLE)
  set_target_properties(${ENGINE_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    INSTALL_RPATH "@executable_path/../Frameworks"
    BUILD_WITH_INSTALL_RPATH TRUE
  )
  add_custom_command(TARGET ${ENGINE_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_BUNDLE_DIR:${ENGINE_NAME}>/Contents/Frameworks"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3-shared>" "$<TARGET_BUNDLE_DIR:${ENGINE_NAME}>/Contents/Frameworks/"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3_ttf::SDL3_ttf-shared>" "$<TARGET_BUNDLE_DIR:${ENGINE_NAME}>/Contents/Frameworks/"
    COMMAND install_name_tool -change "@rpath/libSDL3_ttf.0.dylib" "@executable_path/../Frameworks/libSDL3_ttf.0.2.2.dylib" "$<TARGET_FILE:${ENGINE_NAME}>"
    COMMAND install_name_tool -change "@rpath/libSDL3.0.dylib" "@executable_path/../Frameworks/libSDL3.0.dylib" "$<TARGET_FILE:${ENGINE_NAME}>"
  )

  if(Steamworks_FOUND)
    target_link_libraries("${ENGINE_NAME}" PUBLIC Steamworks::Steamworks)
    target_compile_definitions(${ENGINE_NAME} PRIVATE USE_STEAMWORKS)
    add_custom_command(TARGET ${ENGINE_NAME} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${Steamworks_LIBRARIES}" "$<TARGET_BUNDLE_DIR:${ENGINE_NAME}>/Contents/Frameworks/"
    )
    add_custom_command(TARGET ${ENGINE_NAME} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E echo "480" > "$<TARGET_FILE_DIR:${ENGINE_NAME}>/steam_appid.txt"
      COMMENT "Generating steam_appid.txt"
    )
  endif()

  # Copy Data folder to output directory after build
  add_custom_command(TARGET ${ENGINE_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Data" "$<TARGET_FILE_DIR:${ENGINE_NAME}>/Data"
  )
else()
 if(Steamworks_FOUND)
    target_link_libraries("${ENGINE_NAME}" PUBLIC Steamworks::Steamworks)
    target_compile_definitions(${ENGINE_NAME} PRIVATE USE_STEAMWORKS)
    add_custom_command(TARGET ${ENGINE_NAME} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${Steamworks_LIBRARIES}" "$<TARGET_FILE_DIR:${ENGINE_NAME}>"
    )
    add_custom_command(TARGET ${ENGINE_NAME} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E echo "480" > "$<TARGET_FILE_DIR:${ENGINE_NAME}>/steam_appid.txt"
      COMMENT "Generating steam_appid.txt"
    )
  endif()

  # Copy Data folder to output directory after build
  add_custom_command(TARGET ${ENGINE_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Data" "$<TARGET_FILE_DIR:${ENGINE_NAME}>/Data"
  )
endif()

add_dependencies(${GAME_NAME} ${ENGINE_NAME})

target_link_libraries(${GAME_NAME} PRIVATE ${ENGINE_NAME})