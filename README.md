# Planet Render
This is a simple rendering engine I made to render a full-scale planet that the player can land one and explore from anywhere. This is a project I worked on for about 3 months in 2016.

### Features
Some of the most notable features that this rendering engine achieves include:
- Chunked level of detail to make distant terrain mesh render with a lower poly-count. The planet is a normalised cube, and so as the player gets close to one of the 6 faces, it subdivides into 4 smaller chunks, which can also be subdivided. There can be up to 20 subdivisions, meaning that for an earth-sized planet
