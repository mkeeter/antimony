#ifndef UTIL_MACROS_H
#define UTIL_MACROS_H

#define BOUND(A, L, H) (((A) >= (L) && (A) <= (H)) ? (A) : ((A >= L) ? (H) : (L)))

#endif
