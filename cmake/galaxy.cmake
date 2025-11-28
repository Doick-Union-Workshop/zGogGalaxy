add_library(galaxy INTERFACE IMPORTED)
set_target_properties(galaxy PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${GALAXY_SDK_DIR}/include"
    INTERFACE_LINK_LIBRARIES      "${GALAXY_SDK_DIR}/lib/Galaxy.lib"
)
