///usr/bin/env \
    [ -n "${PATHEXT}" ] && ext='.exe'; \
    bin="$(dirname $0)/$(basename ${0%.*})$ext"; \
    c++ -std=c++11 -Werror -o $bin $0 \
    && \
    $bin "$@"; \
    status=$?; \
    rm -f $bin; \
    exit $status

#include "gpuc.c"