tgc_add_game()

find_package(SDL3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

target_link_libraries("${TGC_GAME_TARGET}" PRIVATE
    SDL3::SDL3
    imgui::imgui
)
