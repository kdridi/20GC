tgc_add_template()

find_package(SDL3 CONFIG REQUIRED)

target_link_libraries("${TGC_TEMPLATE_TARGET}" PRIVATE SDL3::SDL3)
