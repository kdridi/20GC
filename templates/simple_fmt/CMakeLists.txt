tgc_add_template()

find_package(fmt CONFIG REQUIRED)

target_link_libraries("${TGC_TEMPLATE_TARGET}" PRIVATE fmt::fmt)
