set(CPACK_PACKAGE_VENDOR "khronos")

set(CPACK_DEBIAN_DESCRIPTION "OpenCL API C++ bindings")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")

set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

if(NOT CPACK_PACKAGING_INSTALL_PREFIX)
  set(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
endif()

# DEB packaging configuration
set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_VENDOR})

set(CPACK_DEBIAN_PACKAGE_HOMEPAGE
    "https://github.com/KhronosGroup/OpenCL-CLHPP")

# Version number [epoch:]upstream_version[-debian_revision]
set(LATEST_RELEASE_VERSION "2023.04.17")
set(CPACK_DEBIAN_PACKAGE_VERSION "${PROJECT_VERSION}~${LATEST_RELEASE_VERSION}")  # upstream_version
set(CPACK_DEBIAN_PACKAGE_RELEASE "1") # debian_revision (because this is a
                                        # non-native pkg)

set(DEBIAN_PACKAGE_NAME "opencl-clhpp-headers")
set(CPACK_DEBIAN_PACKAGE_NAME
    "${DEBIAN_PACKAGE_NAME}"
    CACHE STRING "Package name" FORCE)

# Package file name in deb format:
# <PackageName>_<VersionNumber>-<DebianRevisionNumber>_<DebianArchitecture>.deb
set(CPACK_DEBIAN_FILE_NAME "${CPACK_PACKAGE_VENDOR}-${DEBIAN_PACKAGE_NAME}_${CPACK_DEBIAN_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")

# Dependencies
set(CPACK_DEBIAN_PACKAGE_DEPENDS "opencl-c-headers (>= 3.0~2021.04.29)")

set(CPACK_DEBIAN_PACKAGE_DEBUG ON)

include(CPack)
