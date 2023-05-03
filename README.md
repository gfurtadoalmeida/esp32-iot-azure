# ESP32 - Project Template

![GitHub Build Status](https://github.com/gfurtadoalmeida/esp32-project-template/actions/workflows/build.yml/badge.svg) [![Bugs](https://sonarcloud.io/api/project_badges/measure?project=esp32_project_template&metric=bugs)](https://sonarcloud.io/summary/new_code?id=esp32_project_template) [![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=esp32_project_template&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=esp32_project_template) [![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=esp32_project_template&metric=sqale_rating)](https://sonarcloud.io/summary/new_code?id=esp32_project_template) [![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=esp32_project_template&metric=security_rating)](https://sonarcloud.io/summary/new_code?id=esp32_project_template)  
ESP32 project template with build pipeline, enhanced VS Code support and more.  

## Characteristics

* ESP-IDF: [v4.4.4](https://docs.espressif.com/projects/esp-idf/en/v4.4.4/esp32/index.html)
* Build pipelines with [Sonar Cloud](https://sonarcloud.io/) integration: 🚀
  * Azure DevOps
  * GitHub Action
* [VS Code task](https://code.visualstudio.com/docs/editor/tasks) for tests (requires [ESP-IDF extension](https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension)): 🧪
  * Build test
  * Flash test
  * Monitor test
  * Build, flash and start a monitor for the tests
* [Newlib nano formating](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/performance/size.html?#newlib-nano-formatting) enabled for `printf/scanf`, with the following advantages:
  * Binary size reduction (25KB~50KB) as they are already in ESP32 ROM.
  * Stack usage reduction for functions that call string formating functions.
  * Increased performance as functions in ROM run faster than functions from flash. 
  * Functions in ROM can run when flash instruction cache is disabled.
* Sample [Kconfig](/components/component_name/Kconfig) file for component configuration through [menuconfig](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/kconfig.html).
* Watchdogs disabled when testing.

## Documentation

Everything is on the [wiki](https://github.com/gfurtadoalmeida/esp32-project-template/wiki).
