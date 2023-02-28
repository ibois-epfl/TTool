file(REMOVE_RECURSE
  "libtsegment_lib.pdb"
  "libtsegment_lib.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/tsegment_lib.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
