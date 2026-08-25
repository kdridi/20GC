# `games/pong` already owns the `pong` target name in this monorepo.
tgc_add_template(TARGET pong_gpu NO_AUTO_SOURCES)

find_package(SDL3 CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# Keep the game bootstrap as a direct/object source: an unreferenced static-library
# object containing only registration side effects may be discarded by the linker.
set(pong_sources
    src/main.cpp
    src/tgc/core/Application.cpp
    src/tgc/core/GameBootstrap.cpp
    src/tgc/core/IGame.cpp
    src/tgc/games/pong/Bootstrap.cpp
    src/tgc/games/pong/Pong.cpp
    src/tgc/games/pong/systems/CollisionSystem.cpp
    src/tgc/gpu/GpuRenderer.cpp
    src/tgc/systems/RenderSystem.cpp
)

set(pong_headers
    src/tgc/components/Rectangle.h
    src/tgc/components/Transform2D.h
    src/tgc/components/Velocity2D.h
    src/tgc/core/Application.h
    src/tgc/core/GameBootstrap.h
    src/tgc/core/GameConfig.h
    src/tgc/core/GameRegistrationStatus.h
    src/tgc/core/IGame.h
    src/tgc/games/pong/Arena.h
    src/tgc/games/pong/InputState.h
    src/tgc/games/pong/Pong.h
    src/tgc/games/pong/Score.h
    src/tgc/games/pong/components/Ball.h
    src/tgc/games/pong/components/Decoration.h
    src/tgc/games/pong/components/Paddle.h
    src/tgc/games/pong/components/ScoreMarker.h
    src/tgc/games/pong/components/Side.h
    src/tgc/games/pong/systems/CollisionSystem.h
    src/tgc/gpu/GpuRenderer.h
    src/tgc/gpu/ShaderBinary.h
    src/tgc/gpu/SolidVertex.h
    src/tgc/systems/RenderSystem.h
)

set(pong_shader_sources
    src/tgc/gpu/shaders/solid.hlsli
    src/tgc/gpu/shaders/solid.vert.hlsl
    src/tgc/gpu/shaders/solid.frag.hlsl
)

set(pong_generated_shaders
    src/tgc/gpu/shaders/generated/solid.frag.dxil.h
    src/tgc/gpu/shaders/generated/solid.frag.msl.h
    src/tgc/gpu/shaders/generated/solid.frag.spv.h
    src/tgc/gpu/shaders/generated/solid.vert.dxil.h
    src/tgc/gpu/shaders/generated/solid.vert.msl.h
    src/tgc/gpu/shaders/generated/solid.vert.spv.h
)

set_source_files_properties(
    ${pong_shader_sources}
    ${pong_generated_shaders}
    PROPERTIES HEADER_FILE_ONLY TRUE
)

target_sources("${TGC_TEMPLATE_TARGET}"
    PRIVATE
        ${pong_sources}
        ${pong_shader_sources}
        ${pong_generated_shaders}
    PRIVATE
        FILE_SET private_headers
        TYPE HEADERS
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src"
        FILES ${pong_headers}
)

target_link_libraries("${TGC_TEMPLATE_TARGET}"
    PRIVATE
        SDL3::SDL3
        EnTT::EnTT
        glm::glm
)
