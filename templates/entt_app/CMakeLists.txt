tgc_add_template()

find_package(fmt CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)

target_link_libraries("${TGC_TEMPLATE_TARGET}"
    PRIVATE
        fmt::fmt
        SDL3::SDL3
        EnTT::EnTT
)
