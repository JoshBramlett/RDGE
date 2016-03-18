RDGE - Rainbow Drop Game Engine

If you're reading this, come back when it's done.

1)  Dependencies
    A)  Google Test - Requires "install" so libs and hearders are available
        Linux - http://stackoverflow.com/questions/13513905/how-to-setup-googletest-as-a-shared-library-on-linux
        Mac - http://stackoverflow.com/questions/20746232/how-to-properly-setup-googletest-on-os-x-aside-from-xcode/21045113#21045113
2) OpenGL (GLEW)
    A)  Mac - Install GLEW
        a)  CMake has packaged FindGLEW.cmake.  CMake requires:
        b)  find_package (GLEW REQUIRED)
        c)  find_package (OpenGL REQUIRED)
        d)  include_directories (${GLEW_INCLUDE_DIRS})
        e)  include_directories (${OPENGL_INCLUDE_DIR})
        f)  target_link_libraries (... ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} ...)
