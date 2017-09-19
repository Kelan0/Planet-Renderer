# Planet Render
This is a simple rendering engine I made to render a full-scale planet that the player can land one and explore from anywhere. This is a project I worked on for about 3 months in 2016.

### Features
Some of the most notable features that this rendering engine achieves include:
- Chunked level of detail to make distant terrain mesh render with a lower poly-count. The planet is a normalised cube, and so as the player gets close to one of the 6 faces, it subdivides into 4 smaller chunks, which can also be subdivided. There can be up to 20 subdivisions, meaning that for an earth-sized planet (a diameter of 13,000Km) the maximum resolution if a chunk is 12 meters.
- Frustum and horizon culling to make objects that the player cannot see not get uploaded to the video memory and be rendered. Horizon culling stops objects that are behind the planets horizon from being rendered, and frustum culling stops objects that are outside of the players view frustum from being rendered.
- Terrain generation using 3D simplex noise. The terrain colours are calculated per-vertex based on altitude. Vertices below an altitude of 0m will be blue for water, and above a certain height they are white for snow, etc. This is not a very realistic way of colouring the terrain. I plan to implement texture splatting so that the terrain chunks can actually have proper textures rather than vertex colours.
- Logarithmic depth buffer to be able to handle the massive ranges of mesh sizes for an entire planet. When the player is far away from the planet, it is actually about 1 OpenGL unit accross, but as the player gets closer, the planet symultaneously moves further away from the player and gets larger, so that it has the same apparent size to the player. When it gets larger, some depth buffer precision issues start to happen, which a logarithmic buffer solves.
