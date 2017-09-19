# Planet Render
This is a simple rendering engine I made to render a full-scale planet that the player can land one and explore from anywhere. This is a project I worked on for about 3 months in 2016.

### Features
Some of the most notable features that this rendering engine achieves include:
- Chunked level of detail to make distant terrain mesh render with a lower poly-count. The planet is a normalised cube, and so as the player gets close to one of the 6 faces, it subdivides into 4 smaller chunks, which can also be subdivided. There can be up to 20 subdivisions, meaning that for an earth-sized planet (a diameter of 13,000Km) the maximum resolution if a chunk is 12 meters.
- Frustum and horizon culling to make objects that the player cannot see not get uploaded to the video memory and be rendered. Horizon culling stops objects that are behind the planets horizon from being rendered, and frustum culling stops objects that are outside of the players view frustum from being rendered.
- 
