# GLSL Extractor
Generates GLSL shader definitions in JSON format from shader source files.

[![Ubuntu](https://github.com/j-junckes/glsl_extractor/actions/workflows/ubuntu-build.yml/badge.svg)](https://github.com/j-junckes/glsl_extractor/actions/workflows/ubuntu-build.yml)

## Usage
```bash
glslextractor [--help] [--version] vertex_file fragment_file
```

## Example
```bash
glslextractor samples/simple.vsh samples/simple.fsh
```

Output:
```json
{
  "fragment": {
    "uniforms": [
      {
        "name": "shaderTexture",
        "type": "sampler2D"
      }
    ]
  },
  "vertex": {
    "attributes": [
      {
        "location": 0,
        "name": "aPos",
        "type": "vec3"
      },
      {
        "location": 1,
        "name": "aTexCoords",
        "type": "vec2"
      }
    ],
    "uniforms": [
      {
        "name": "model",
        "type": "mat4"
      },
      {
        "name": "view",
        "type": "mat4"
      },
      {
        "name": "projection",
        "type": "mat4"
      }
    ]
  }
}
```

## Building
```bash
mkdir build && cd build
cmake ..
make
```