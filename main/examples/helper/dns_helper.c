#include "dns_helper.h"
#include "lwip/dns.h"
#include "lwip/sockets.h"
#include "esp_log.h"

static const char TAG_HELPER_DNS[] = "HELPER_DNS";

bool helper_dns_set_servers()
{
    const char *IP_1 = "1.1.1.1";
    const char *IP_2 = "1.0.0.1";
    ip_addr_t address = {0};

    if (ipaddr_aton(IP_1, &address) == 1)
    {
        dns_setserver(0, &address);
    }
    else
    {
        ESP_LOGE(TAG_HELPER_DNS, "invalid dns ip %d: %s", 0, IP_1);

        return false;
    }

    if (ipaddr_aton(IP_2, &address) == 1)
    {
        dns_setserver(1, &address);
    }
    else
    {
        ESP_LOGE(TAG_HELPER_DNS, "invalid dns ip %d: %s", 1, IP_2);

        return false;
    }

    return true;
}