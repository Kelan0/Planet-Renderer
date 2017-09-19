# Planet Render
This is a simple rendering engine I made to render a full-scale planet that the player can land one and explore from anywhere. This is a project I worked on for about 3 months in 2016. The project was named `Voxel Renderer` because I originally planned for the terrain to be made of voxels and to be editable, but this ended up not happening due to complications with storing voxels on a spherical planet.

### Features
Some of the most notable features that this rendering engine achieves include:
- Chunked level of detail to make distant terrain mesh render with a lower poly-count. The planet is a normalised cube, and so as the player gets close to one of the 6 faces, it subdivides into 4 smaller chunks, which can also be subdivided. There can be up to 20 subdivisions, meaning that for an earth-sized planet (a diameter of 13,000Km) the maximum resolution if a chunk is 12 meters.
- Realistic atmospheric scattering using rayleigh and mei scattering in the fragment shader. The sky is a sphere around the planet with a slightly larger radius. The scattering shader is applied to the sky and also the terrain to make distant terrain fade into the sky a bit.
- Frustum and horizon culling to make objects that the player cannot see not get uploaded to the video memory and be rendered. Horizon culling stops objects that are behind the planets horizon from being rendered, and frustum culling stops objects that are outside of the players view frustum from being rendered.
- Terrain generation using 3D simplex noise. I plan to make the noise 4D to allow for ridges, overhangs and other such structures that would be impossible with just a heightmap. The terrain colours are calculated per-vertex based on altitude. Vertices below an altitude of 0m will be blue for water, and above a certain height they are white for snow, etc. This is not a very realistic way of colouring the terrain. I plan to implement texture splatting so that the terrain chunks can actually have proper textures rather than vertex colours.
- Logarithmic depth buffer to be able to handle the massive ranges of mesh sizes for an entire planet. When the player is far away from the planet, it is actually about 1 OpenGL unit accross, but as the player gets closer, the planet symultaneously moves further away from the player and gets larger, so that it has the same apparent size to the player. When it gets larger, some depth buffer precision issues start to happen, which a logarithmic buffer solves.

## Images
Some screenshots of all of this in action.
![Planet from space](https://i.imgur.com/LmLxO4c.png)
![High altitude atmospheric scattering](https://i.imgur.com/NZerOcx.png)
![Terrain mesh and LOD](https://i.imgur.com/u5rmT3a.png)
!Frustum culling](https://i.imgur.com/LSc0Oyc.png)
![More tmospheric scattering](https://i.imgur.com/4OHl4lr.png)
![Terrain generation](https://i.imgur.com/dkzZRQw.png)
![Atmospheric scattering on terrain](https://i.imgur.com/qYBPh5P.png)

## Installing
To run this, you can either clone the entire repository and run the visual studio project files (which I haven't tested. This may not work propery), or you can run the executable in the `VoxelRenderer (Runnable)` folder.
