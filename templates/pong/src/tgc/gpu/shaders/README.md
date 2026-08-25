The HLSL interface and the checked-in generated shader variants follow the
SDL_GPU test shader layout from SDL 3, distributed under the zlib license.
They use vertex uniform slot 0 and vertex attributes TEXCOORD0/TEXCOORD1.
Regenerate the headers with `build-shaders.sh` after changing the HLSL source.
