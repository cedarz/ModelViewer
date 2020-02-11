cmake_minimum_required (VERSION 3.12)

set(PROJECT "Zero")
project(${PROJECT})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Includes)

file(GLOB_RECURSE PTSOURCES "ModelViewer/*.cpp" "ModelViewer/*.c" "ModelViewer/*.h")

add_executable(${PROJECT} WIN32 ${PTSOURCES})

set_property (DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT})

if(WIN32)
   set_target_properties(${PROJECT} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:CONSOLE")
   set_target_properties(${PROJECT} PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
endif(WIN32)

target_link_libraries(${PROJECT} $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/Libs/Debug/assimp-vc140-mt.lib>)
target_link_libraries(${PROJECT} $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/Libs/Debug/FreeImaged.lib>)
target_link_libraries(${PROJECT} $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/Libs/Debug/FreeImagePlusd.lib>)
target_link_libraries(${PROJECT} $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/Libs/Debug/freetyped.lib>)
target_link_libraries(${PROJECT} $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/Libs/Debug/SDL2.lib>)
target_link_libraries(${PROJECT} $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/Libs/Debug/SDL2_mixer.lib>)
target_link_libraries(${PROJECT} $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/Libs/Debug/SDL2main.lib>)

# copy dlls
add_custom_command(
	TARGET ${PROJECT} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/Dlls/assimp-vc140-mt.dll> $<TARGET_FILE_DIR:${PROJECT}>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/Dlls/freeglutd.dll> $<TARGET_FILE_DIR:${PROJECT}>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/Dlls/FreeImaged.dll> $<TARGET_FILE_DIR:${PROJECT}>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/Dlls/FreeImagePlusd.dll> $<TARGET_FILE_DIR:${PROJECT}>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/Dlls/SDL2.dll> $<TARGET_FILE_DIR:${PROJECT}>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/Dlls/SDL2_mixer.dll> $<TARGET_FILE_DIR:${PROJECT}>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/Dlls/SDL2main.dll> $<TARGET_FILE_DIR:${PROJECT}>		
)