find_path(FFTW_INCLUDE_DIR fftw3.h)

if (WIN32)
    find_library(FFTW_LIBRARY NAMES fftw3-3) # For pre-compiled binaries
else (WIN32)
    find_library(FFTW_LIBRARY NAMES fftw3)
endif (WIN32)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(FFTW FFTW_INCLUDE_DIR FFTW_LIBRARY)

mark_as_advanced(FFTW_INCLUDE_DIR FFTW_LIBRARY)
