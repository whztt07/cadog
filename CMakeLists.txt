cmake_minimum_required(VERSION 2.8)

# Eliminate a warning when building in Windows that relates
# to static linking of Qt executables to qtmain.lib.
# This policy was introduced in CMake version 2.8.11.
# CMake version 2.8.11.2 warns when the policy is not set
# and uses OLD behavior.
if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()

project(cadog)

if(WIN32)
	#set(CMAKE_MODULE_PATH "$ENV{OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
	set(OGRE_SAMPLES_INCLUDEPATH
		$ENV{OGRE_HOME}/Samples/include
	)
endif(WIN32)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Packages")

if(UNIX)
	if(EXISTS "/usr/local/lib/OGRE/cmake")
	  set(CMAKE_MODULE_PATH "/usr/local/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
	  set(OGRE_SAMPLES_INCLUDEPATH "/usr/local/share/OGRE/samples/Common/include/")
	elseif(EXISTS "/usr/lib/OGRE/cmake")
	  set(CMAKE_MODULE_PATH "/usr/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
	  set(OGRE_SAMPLES_INCLUDEPATH "/usr/share/OGRE/samples/Common/include/") # Otherwise, this one
	else ()
	  message(SEND_ERROR "Failed to find module path.")
	endif(EXISTS "/usr/local/lib/OGRE/cmake")
endif(UNIX)

if (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

set(CMAKE_DEBUG_POSTFIX "_d")

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/dist")

find_package(OGRE COMPONENTS Paging Terrain Volume Overlay 
  Plugin_BSPSceneManager Plugin_CgProgramManager Plugin_OctreeSceneManager
  Plugin_OctreeZone Plugin_PCZSceneManager Plugin_ParticleFX
  RenderSystem_Direct3D11 RenderSystem_Direct3D9 RenderSystem_GL RenderSystem_GL3Plus RenderSystem_GLES RenderSystem_GLES2 REQUIRED)
  
set (OGRE_LIBRARIES 
${OGRE_LIBRARIES} 
${OGRE_Overlay_LIBRARIES}
)

#if(NOT "${OGRE_VERSION_NAME}" STREQUAL "Cthugha")
#  message(SEND_ERROR "You need Ogre 1.7 Cthugha to build this.")
#endif()

find_package(OIS REQUIRED)
find_package(irrKlang REQUIRED)
find_package(Freetype REQUIRED)

set(FREETYPE_LIBRARIES ${FREETYPE_LIBRARY_REL})

if(NOT OIS_FOUND)
	message(SEND_ERROR "Failed to find OIS.")
endif()

# Find Boost
if (NOT OGRE_BUILD_PLATFORM_IPHONE)
	if (WIN32 OR APPLE)
		set(Boost_USE_STATIC_LIBS TRUE)
	else ()
		# Statically linking boost to a dynamic Ogre build doesn't work on Linux 64bit
		set(Boost_USE_STATIC_LIBS ${OGRE_STATIC})
	endif ()
	
	set(Boost_USE_STATIC_LIBS ${OGRE_STATIC})
	if (MINGW)
		# this is probably a bug in CMake: the boost find module tries to look for
		# boost libraries with name libboost_*, but CMake already prefixes library
		# search names with "lib". This is the workaround.
		set(CMAKE_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES} "")
	endif ()
	set(Boost_ADDITIONAL_VERSIONS "1.44" "1.44.0" "1.42" "1.42.0" "1.41.0" "1.41" "1.40.0" "1.40" "1.39.0" "1.39" "1.38.0" "1.38" "1.37.0" "1.37" )
	# Components that need linking (NB does not include header-only components like bind)
	set(OGRE_BOOST_COMPONENTS thread date_time thread date_time atomic chrono system)
	find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS})
	if (NOT Boost_FOUND)
		# Try again with the other type of libs
		set(Boost_USE_STATIC_LIBS NOT ${Boost_USE_STATIC_LIBS})
		find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS})
	endif()

	# Set up referencing of Boost
	include_directories(${Boost_INCLUDE_DIR})
	add_definitions(-DBOOST_ALL_NO_LIB)
	set(OGRE_LIBRARIES ${Boost_LIBRARIES} ${OGRE_LIBRARIES} ${ZZip_LIBRARIES} ${ZLIB_LIBRARIES} ${FreeImage_LIBRARIES} ${FREETYPE_LIBRARIES})
	

endif()



set(HDRS
    cadogAnimationControl.h 
    cadogApplication.h
    cadogFrameListener.h 
    cadog.h 
    cadogMainMenu.h
)

set(SRCS
	BaseApplication.cpp 
    cadogApplication.cpp
    cadogFrameListener.cpp
    cadogKeyPressed.cpp 
    main.cpp
    cadogAnimationControl.cpp
    cadog.cpp
    cadogFrameRenderingQueued.cpp
    cadogMainMenu.cpp 
)

include_directories( ${OIS_INCLUDE_DIRS}
	${OGRE_INCLUDE_DIRS}
	${OGRE_SAMPLES_INCLUDEPATH}
    ${IRRKLANG_INCLUDES}
)


if(UNIX)
add_definitions(-DUNIX)
set(Boost_LIBRARIES ${Boost_LIBRARIES} boost_system boost_thread)
endif(UNIX)


add_executable(cadog ${HDRS} ${SRCS})
set_target_properties(cadog PROPERTIES DEBUG_POSTFIX _d)

target_link_libraries(cadog ${OGRE_LIBRARIES} ${OIS_LIBRARIES} ${IRRKLANG_LIBRARIES} ${Boost_LIBRARIES} ${FREETYPE_LIBRARIES})

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/bin)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/media)

# post-build copy for win32
if(WIN32 AND NOT MINGW)
	add_custom_command( TARGET cadog PRE_BUILD
		COMMAND if not exist .\\dist\\bin mkdir .\\dist\\bin )
	add_custom_command( TARGET cadog POST_BUILD
		COMMAND copy \"$(TargetPath)\" .\\dist\\bin )
endif(WIN32 AND NOT MINGW)

if(MINGW OR UNIX)
	set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/dist/bin)
endif(MINGW OR UNIX)

if(WIN32)

	install(TARGETS cadog
		RUNTIME DESTINATION bin
		CONFIGURATIONS All)

	install(DIRECTORY ${CMAKE_SOURCE_DIR}/dist/Media
		DESTINATION ./
		CONFIGURATIONS Release RelWithDebInfo Debug
	)

	install(FILES ${CMAKE_SOURCE_DIR}/dist/bin/plugins.cfg
		${CMAKE_SOURCE_DIR}/dist/bin/resources.cfg
		DESTINATION bin
		CONFIGURATIONS Release RelWithDebInfo
	)

	install(FILES ${CMAKE_SOURCE_DIR}/dist/bin/plugins_d.cfg
		${CMAKE_SOURCE_DIR}/dist/bin/resources_d.cfg
		DESTINATION bin
		CONFIGURATIONS Debug
	)

        # NOTE: for the 1.7.1 sdk the OIS dll is called OIS.dll instead of libOIS.dll
        # so you'll have to change that to make it work with 1.7.1
	install(FILES ${OGRE_PLUGIN_DIR_REL}/OgreMain.dll
		${OGRE_PLUGIN_DIR_REL}/RenderSystem_Direct3D9.dll
		${OGRE_PLUGIN_DIR_REL}/RenderSystem_GL.dll
		${OGRE_PLUGIN_DIR_REL}/libOIS.dll
		DESTINATION bin
		CONFIGURATIONS Release RelWithDebInfo
	)

	install(FILES ${OGRE_PLUGIN_DIR_DBG}/OgreMain_d.dll
		${OGRE_PLUGIN_DIR_DBG}/RenderSystem_Direct3D9_d.dll
		${OGRE_PLUGIN_DIR_DBG}/RenderSystem_GL_d.dll
		${OGRE_PLUGIN_DIR_DBG}/libOIS_d.dll
		DESTINATION bin
		CONFIGURATIONS Debug
	)

   # as of sdk 1.7.2 we need to copy the boost dll's as well
   # because they're not linked statically (it worked with 1.7.1 though)
   install(FILES ${Boost_DATE_TIME_LIBRARY_RELEASE}
      ${Boost_THREAD_LIBRARY_RELEASE}
      DESTINATION bin
      CONFIGURATIONS Release RelWithDebInfo
   )

   install(FILES ${Boost_DATE_TIME_LIBRARY_DEBUG}
      ${Boost_THREAD_LIBRARY_DEBUG}
      DESTINATION bin
      CONFIGURATIONS Debug
   )
endif(WIN32)

if(UNIX)

	install(TARGETS cadog
		RUNTIME DESTINATION bin
		CONFIGURATIONS All)

	install(DIRECTORY ${CMAKE_SOURCE_DIR}/dist/media
		DESTINATION ./
		CONFIGURATIONS Release RelWithDebInfo Debug
	)

	install(FILES ${CMAKE_SOURCE_DIR}/dist/bin/plugins.cfg
		${CMAKE_SOURCE_DIR}/dist/bin/resources.cfg
		DESTINATION bin
		CONFIGURATIONS Release RelWithDebInfo Debug
	)

endif(UNIX)
