find_path(FFTW_INCLUDE_DIR fftw3.h)
find_library(FFTW_LIBRARY NAMES fftw3)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(FFTW FFTW_INCLUDE_DIR FFTW_LIBRARY)

mark_as_advanced(FFTW_INCLUDE_DIR FFTW_LIBRARY)
