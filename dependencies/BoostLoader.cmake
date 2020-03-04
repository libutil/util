function(BoostLoader_extract_and_build BOOST_PACKAGE_PATH BOOST_PACKAGE_NAME)
    if (NOT EXISTS ${BOOST_PACKAGE_PATH}/${BOOST_PACKAGE_NAME}.tar.gz)
        message(FATAL_ERROR "${BOOST_PACKAGE_PATH}/${BOOST_PACKAGE_NAME}.tar.gz not found")
    else ()
        # Remove extracted folder
        file(REMOVE_RECURSE ${BOOST_PACKAGE_PATH}/${BOOST_PACKAGE_NAME})
        message(STATUS "Extracting ${BOOST_PACKAGE_NAME}.tar.gz")
        execute_process(COMMAND ${CMAKE_COMMAND} -E
                        tar xzf ${BOOST_PACKAGE_NAME}.tar.gz
                        WORKING_DIRECTORY ${BOOST_PACKAGE_PATH}
                        RESULT_VARIABLE extractResult)
        if (NOT extractResult EQUAL "0")
            file(REMOVE_RECURSE ${BOOST_PACKAGE_PATH}/${BOOST_PACKAGE_NAME})
            message(FATAL_ERROR "Failed to extract ${BOOST_PACKAGE_NAME}.tar.gz")
        else ()
            message(STATUS "Extraction Succeed")
            # Build b2 when if not built
            unset(b2binary CACHE)
            find_program(b2binary NAMES b2
                         PATHS ${BOOST_PACKAGE_PATH}/${BOOST_PACKAGE_NAME}
                         NO_DEFAULT_PATH)
            if (NOT b2binary)
                message(STATUS "Building b2")
                if (MSVC)
                    set(b2Bootstrap "bootstrap.bat")
                else ()
                    set(b2Bootstrap "./bootstrap.sh")
                    if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
                        list(APPEND b2Bootstrap --with-toolset=gcc)
                    elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
                        list(APPEND b2Bootstrap --with-toolset=clang)
                    elseif (${CMAKE_CXX_COMPILER_ID} MATCHES "^(Apple)?Clang$")
                        list(APPEND b2Bootstrap --with-toolset=clang)
                    endif ()
                endif ()
                execute_process(COMMAND ${b2Bootstrap}
                                WORKING_DIRECTORY ${BOOST_PACKAGE_PATH}/${BOOST_PACKAGE_NAME}
                                RESULT_VARIABLE bootstrapResult
                                OUTPUT_VARIABLE bootstrapOutput
                                ERROR_VARIABLE bootstrapError)
                if (NOT bootstrapResult EQUAL "0")
                    message(FATAL_ERROR
                            "Failed running ${b2Bootstrap}:\n${bootstrapOutput}\n${bootstrapError}\n")
                endif ()
                # Check whether b2 binary exists
                unset(b2binary CACHE)
                find_program(b2binary NAMES b2
                             PATHS ${BOOST_PACKAGE_PATH}/${BOOST_PACKAGE_NAME}
                             NO_DEFAULT_PATH)
                if (NOT b2binary)
                    message(FATAL_ERROR "b2 binary not exists after trying to build")
                else ()
                    message(STATUS "b2 build succeed")
                    unset(b2binary CACHE)
                endif ()
            else ()
                message(STATUS "b2 binary exists")
            endif ()
            # Create a directory and
            # b2 binary will build in this location
            # instead of building within the distribution tree.
            execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory
                            ${BOOST_PACKAGE_PATH}/${BOOST_PACKAGE_NAME}/build)
            if (MSVC)
                set(b2Command "b2.exe")
            else ()
                set(b2Command "./b2")
            endif ()
            # https://boostorg.github.io/build/manual/master/index.html#bbv2.overview.invocation.properties
            list(APPEND b2Command --without-mpi)
            list(APPEND b2Command --build-dir=build)
            # Build in this location instead of building within the distribution tree. Recommended!
            list(APPEND b2Command variant=release)
            # Determines if Boost.Build creates shared or static libraries
            list(APPEND b2Command threading=multi)
            # Build single-threaded or multi-threaded binaries
            list(APPEND b2Command address-model=64)
            # Explicitly request either 32-bit or 64-bit code generation
            list(APPEND b2Command link=static)
            # Build static and shared libraries
            list(APPEND b2Command runtime-link=static)
            # Determines if shared or static version of C and C++ runtimes should be used.
            list(APPEND b2Command -a) # Rebuild everything
            list(APPEND b2Command -q) # Stop at first error. Quit as soon as a build failure is encountered
            # list(APPEND b2Command -d2) # Debug Level. See ./b2 --help-options
            list(APPEND b2Command stage)
            message(STATUS "Building Boost Library")
            execute_process(COMMAND ${b2Command}
                            WORKING_DIRECTORY ${BOOST_PACKAGE_PATH}/${BOOST_PACKAGE_NAME}
                            RESULT_VARIABLE buildResult
                            OUTPUT_VARIABLE buildOutput
                            ERROR_VARIABLE buildError)
            if (NOT buildResult EQUAL "0")
                message(WARNING "Build Result: ${buildResult}")
                message(WARNING "Build Output: ${buildOutput}")
                message(WARNING "Build Error: ${buildError}")
                message(FATAL_ERROR "Failed to build Boost.")
            else ()
                message(STATUS "Boost build succeed")
                message(STATUS "Build Output: ${buildOutput}")
            endif ()
        endif ()
    endif ()
endfunction()


function(BoostLoader_load BOOST_PACKAGE_PATH BOOST_PACKAGE_NAME)
    if (EXISTS ${BOOST_PACKAGE_PATH}/${BOOST_PACKAGE_NAME}.tar.gz)
        message(STATUS "${BOOST_PACKAGE_PATH}/${BOOST_PACKAGE_NAME}.tar.gz found")
    else ()
        message(FATAL_ERROR "Error: ${BOOST_PACKAGE_PATH}/${BOOST_PACKAGE_NAME}.tar.gz not found")
    endif ()
    
    set(NO_DEFAULT_PATH true)   # Set flag for find_package
    # see https://cmake.org/cmake/help/v3.0/command/find_package.html
    
    set(Boost_USE_STATIC_LIBS ON)    # Set Flags for BoostConfig.cmake
    set(Boost_USE_STATIC_RUNTIME ON) # See stage/lib/cmake/Boost-{version}/BoostConfig.cmake
    
    if (NOT EXISTS ${BOOST_PACKAGE_PATH}/${BOOST_PACKAGE_NAME}/stage/lib/cmake)
        message(STATUS "Extract and Build Boost Library")
        BoostLoader_extract_and_build(${BOOST_PACKAGE_PATH} ${BOOST_PACKAGE_NAME})
    else ()
        message(STATUS "${BOOST_PACKAGE_PATH}/${BOOST_PACKAGE_NAME}/stage/lib/cmake found")
    endif ()
    
    if (NOT Boost_FOUND)
        # https://cmake.org/cmake/help/v3.0/command/find_package.html
        # https://zhuanlan.zhihu.com/p/50829542
        find_package(Boost
                     REQUIRED COMPONENTS atomic
                     chrono container context contract coroutine
                     date_time exception fiber filesystem graph
                     headers iostreams locale log
                     math_c99 math_c99f math_c99l math_tr1 math_tr1f math_tr1l
                     program_options python
                     random regex serialization
                     thread timer type_erasure wave
                     PATHS ${BOOST_PACKAGE_PATH}/${BOOST_PACKAGE_NAME}/stage)
    endif ()
    
    if (Boost_FOUND)
        message(STATUS "Boost Library Found And Registered, the following cmake normal variables have been set")
        message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
        # message(STATUS "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
        message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
        message(STATUS "Boost_VERSION: ${Boost_VERSION}")
        # message(STATUS "Boost_LIB_VERSION: ${Boost_LIB_VERSION}")
        set(Boost_FOUND ${Boost_FOUND} PARENT_SCOPE)
        # 头文件搜索路径
        set(Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} PARENT_SCOPE)
        # 链接路径
        # set(Boost_LIBRARY_DIRS ${Boost_LIBRARY_DIRS} PARENT_SCOPE)
        # Boost库模块名，用于链接生成目标程序
        set(Boost_LIBRARIES ${Boost_LIBRARIES} PARENT_SCOPE)
        set(Boost_VERSION ${Boost_VERSION} PARENT_SCOPE)
        # set(Boost_LIB_VERSION ${Boost_LIB_VERSION} PARENT_SCOPE)
    else ()
        message(FATAL_ERROR
                "${BOOST_PACKAGE_PATH}/${BOOST_PACKAGE_NAME}/stage/lib/cmake exists but find_package() failed.\n
                 Try to remove extracted Boost Library folder and run again.\n
                 So that Boost Library will be extracted and built in next run.")
    endif ()
endfunction()
