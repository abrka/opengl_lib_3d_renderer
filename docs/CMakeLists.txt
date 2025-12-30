find_package(Doxygen)

if(Doxygen_FOUND)
  configure_file(Doxyfile.in Doxyfile)

  doxygen_add_docs(
    abrka_game_engine_docs
    USE_STAMP_FILE
    COMMENT "Generating project documentation with custom Doxyfile"
    CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  )
endif()