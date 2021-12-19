set(torch_dir ${CMAKE_CURRENT_BINARY_DIR}/torch)

set(torch_lib_name torch)

set(torch_dir ${CMAKE_CURRENT_BINARY_DIR}/torch)
set(torch_lib_name torch)
find_library(torch_lib
  NAMES ${torch_lib_name}
  PATHS ${torch_dir}/libtorch/lib
)

if (NOT torch_lib)
  message(STATUS "Downloading torch C API pre-built")

  # Download
  if (UNIX AND NOT APPLE)  # Linux
    set(torch_url
        "https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.10.1%2Bcpu.zip")
  elseif (UNIX AND APPLE)  # OSX
    set(torch_url
        "https://download.pytorch.org/libtorch/cpu/libtorch-macos-1.10.1.zip")
  else()                   # Windows
    set(torch_url
        "https://download.pytorch.org/libtorch/cpu/libtorch-win-shared-with-deps-1.10.1%2Bcpu.zip")
  endif()

  file(DOWNLOAD
    ${torch_url}
    ${torch_dir}/torch_cc.zip
    SHOW_PROGRESS
  )
  execute_process(COMMAND ${CMAKE_COMMAND} -E tar -xf torch_cc.zip
                  WORKING_DIRECTORY ${torch_dir})
  
  file(REMOVE ${torch_dir}/torch_cc.zip)

endif()

# Find the libraries again
find_library(torch_lib
  NAMES ${torch_lib_name}
  PATHS ${torch_dir}/libtorch/lib
)
if (NOT torch_lib)
  message(FATAL_ERROR "torch could not be included")
endif()
