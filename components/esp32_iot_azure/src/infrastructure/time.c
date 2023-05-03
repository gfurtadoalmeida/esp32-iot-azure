#include "infrastructure/time.h"
#include <time.h>
#include "assertion.h"

static const char TAG_TIME[] = "AZ_TIME";

uint64_t time_get_unix()
{
    time_t now = time(NULL);

    CMP_CHECK(TAG_TIME, (now > (time_t)(-1)), "failure obtaining time", 0)

    return now;
}