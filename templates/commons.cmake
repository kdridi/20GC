include_guard(GLOBAL)

set(TGC_CODESIGN_IDENTITY "" CACHE STRING
  "Apple code-signing identity used for local macOS executables"
)
set(TGC_CODESIGN_IDENTIFIER_PREFIX "com.kdridi.20gc" CACHE STRING
  "Code-signing identifier prefix used for local macOS executables"
)

function(tgc_add_template)
  if(ARGC GREATER 0)
    message(FATAL_ERROR "tgc_add_template() does not accept arguments")
  endif()

  get_filename_component(target_name "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

  if(TARGET "${target_name}")
    message(FATAL_ERROR "A target named '${target_name}' already exists")
  endif()

  set(platform_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/platform/${CMAKE_SYSTEM_NAME}"
  )

  file(
    GLOB_RECURSE common_sources
    CONFIGURE_DEPENDS
    LIST_DIRECTORIES false
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
  )

  file(
    GLOB_RECURSE platform_sources
    CONFIGURE_DEPENDS
    LIST_DIRECTORIES false
    "${platform_directory}/*.cpp"
    "${platform_directory}/*.cc"
    "${platform_directory}/*.cxx"
  )

  list(SORT common_sources)
  list(SORT platform_sources)

  if(NOT common_sources AND NOT platform_sources)
    message(FATAL_ERROR
      "Template '${target_name}' has no C++ source for platform '${CMAKE_SYSTEM_NAME}'"
    )
  endif()

  add_executable("${target_name}")
  set(TGC_TEMPLATE_TARGET "${target_name}")
  set(TGC_TEMPLATE_TARGET "${target_name}" PARENT_SCOPE)

  target_sources("${target_name}" PRIVATE ${common_sources} ${platform_sources})

  target_include_directories("${target_name}" PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
  )
  if(IS_DIRECTORY "${platform_directory}")
    target_include_directories("${target_name}" PRIVATE "${platform_directory}")
  endif()

  set_target_properties("${target_name}" PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
  )

  target_compile_definitions("${target_name}" PRIVATE
    "TEMPLATE_NAME=\"${target_name}\""
    "TGC_PLATFORM_NAME=\"${CMAKE_SYSTEM_NAME}\""
  )

  if(CMAKE_SYSTEM_NAME STREQUAL "Darwin" AND TGC_CODESIGN_IDENTITY)
    string(REGEX REPLACE "[^A-Za-z0-9.-]" "-"
      code_signing_target_name "${target_name}"
    )
    set(code_signing_identifier
      "${TGC_CODESIGN_IDENTIFIER_PREFIX}.${code_signing_target_name}"
    )

    if(CMAKE_GENERATOR STREQUAL "Xcode")
      set_target_properties("${target_name}" PROPERTIES
        XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Manual"
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "${TGC_CODESIGN_IDENTITY}"
        XCODE_ATTRIBUTE_OTHER_CODE_SIGN_FLAGS
          "--identifier ${code_signing_identifier}"
      )
    else()
      find_program(codesign_executable
        NAMES codesign
        PATHS /usr/bin
        REQUIRED
        NO_CACHE
      )
      add_custom_command(TARGET "${target_name}" POST_BUILD
        COMMAND "${codesign_executable}"
          --force
          --sign "${TGC_CODESIGN_IDENTITY}"
          --identifier "${code_signing_identifier}"
          "$<TARGET_FILE:${target_name}>"
        VERBATIM
        COMMENT "Signing ${target_name} as ${code_signing_identifier}"
      )
    endif()
  endif()

  set(platform_configuration "${platform_directory}/platform.cmake")
  if(EXISTS "${platform_configuration}")
    include("${platform_configuration}")
  endif()
endfunction()
