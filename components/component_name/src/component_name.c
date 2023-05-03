#include "component_name/component_name.h"
#include "config.h"
#include "assertion.h"
#include "log.h"

void component_name_do_something()
{
    CMP_LOGI("something was done: %d", CONFIG_COMPONENT_NAME_CONFIG_ONE);
}