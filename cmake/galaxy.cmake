add_library(galaxy SHARED IMPORTED)
set_target_properties(galaxy PROPERTIES
    IMPORTED_IMPLIB "${GALAXY_SDK_DIR}/lib/Galaxy.lib"
    IMPORTED_LOCATION "${GALAXY_SDK_DIR}/lib/Galaxy.dll"
    INTERFACE_INCLUDE_DIRECTORIES "${GALAXY_SDK_DIR}/include"
)
