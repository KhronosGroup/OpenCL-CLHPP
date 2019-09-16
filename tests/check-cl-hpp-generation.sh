#!/usr/bin/env bash

# Figure out where we are
TOP=$(dirname "${BASH_SOURCE[0]}")/..

if [[ ${TRAVIS_OS_NAME} != osx ]]; then
    TOP=$(readlink -f "${TOP}")
fi

# Create a temporary output file
TEMP=$(mktemp)

# Generate cl.hpp
python3 "${TOP}"/gen_cl_hpp.py -i "${TOP}"/input_cl.hpp -o "${TEMP}"

# Compare it to the file tracked in version control
diff "${TOP}"/include/CL/cl.hpp "${TEMP}"
RETCODE=$?

# Cleanup and exit
rm "${TEMP}"
exit ${RETCODE}
