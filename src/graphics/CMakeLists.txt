add_library (Graphics
	Graphics.c
	GLUtilities.c
	VectorUtils3.c
	LoadObject.c
	LoadTGA.c
	shaders/main.vert
	shaders/main.frag)
target_link_libraries (Graphics
	${OPENGL_LIBRARIES}
	${GLUT_LIBRARIES})



add_custom_command(TARGET Graphics POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/graphics/shaders
        $<TARGET_FILE_DIR:Graphics>/shaders)

add_custom_command(TARGET Graphics POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/models
        $<TARGET_FILE_DIR:Graphics>/models)

add_custom_command(TARGET Graphics POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/textures
        $<TARGET_FILE_DIR:Graphics>/textures)
