# Raylib MPEG Video Player: Converting YCbCr to RGB using Shaders

This project extends an example provided by [raysan](https://github.com/raysan5) on running [pl_mpeg](https://github.com/phoboslab/pl_mpeg) within the [raylib](https://github.com/raysan5/raylib) framework.
For the original raysan implementation check [this issue comment](https://github.com/phoboslab/pl_mpeg/issues/7#issuecomment-586723926).

The example demonstrates the use of shaders to optimize video decoding by converting YCbCr to RGB on the GPU. While this improves performance compared to CPU-based conversion, certain limitations still exist. Specifically, on low-spec systems, the video and audio may exhibit glitches and breaks, particularly at resolutions above 1080p, even when using the shader.

![](./docs/example.gif)

## Features

- GPU-accelerated YCbCr to RGB conversion via shaders.
- Support for MPEG-1 video decoding using pl_mpeg.
- Audio and video playback integrated with raylib.

## Limitations

- On lower-spec systems, the GPU optimizations may still result in undesirable video quality, especially at high resolutions (e.g., 1080p or above).
- Audio and video may exhibit glitches or breaks at higher resolutions.

## Building and Running

To build the project, run: All dependencies, are automatically downloaded and compiled (if needed).

    make build

To play the video, use the following command:

    make play

## Encoding MPEG Video for PL_MPEG

To encode a video in a compatible MPEG format for this player, you can use ffmpeg. The following command will convert an input video into MPEG-1 format with MP2 audio:

    ffmpeg -i input.mp4 -c:v mpeg1video -q:v 0 -c:a mp2 -f mpeg output.mpg

Make sure to replace input.mp4 with your video file, and output.mpg with the desired output file name.

## Dependencies

- raylib
- pl_mpeg
- cmake
