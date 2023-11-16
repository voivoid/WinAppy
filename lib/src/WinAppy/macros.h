#pragma once

#define WINAPPY_CONCAT_INNER(a, b) a##b
#define WINAPPY_CONCAT(a, b) WINAPPY_CONCAT_INNER(a, b)

#define WINAPPY_UNIQUE_NAME(base) WINAPPY_CONCAT(base, __COUNTER__)