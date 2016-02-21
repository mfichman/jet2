# jet2

A simple C++ game framework/engine with high-perf networking support. The engine uses the rendering engine 
from another of my projects, the [Simple, Fast Renderer](https://github.com/mfihcman/sfr.git)

The engine uses the component-entity-process design. Each game entity consists of multiple data components. 
Components are updated and processed in each frame/render pass by "processes," which functions
that are applied to each component once per frame.

