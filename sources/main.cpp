#include "raylib.h"

#include <iostream>

#define PL_MPEG_IMPLEMENTATION
#include <pl_mpeg.h>

int main() {
  const int screenWidth = 960;
  const int screenHeight = 540;

  InitWindow(screenWidth, screenHeight, "raylib MPEG video player");

  InitAudioDevice();

  plm_t *plm = nullptr;
  plm_frame_t *frame = nullptr;
  plm_samples_t *sample = nullptr;
  double framerate = 0.0;
  int samplerate = 0;

  Image imFrameY = {0};
  Texture textureY = {0};
  Image imFrameCb = {0};
  Texture textureCb = {0};
  Image imFrameCr = {0};
  Texture textureCr = {0};

  AudioStream stream = {0};

  double baseTime = 0.0;
  double timeExcess = 0.0;
  bool pause = false;

  int currentVideoFrame = 0;
  int totalVideoFrames = 0;

  int currentAudioFrame = 0;
  int totalAudioFrames = 0;

  std::string shaderPath =
      SHADERS_PATH "" + std::string("convert_ycbcr_to_rgb.fs");
  Shader convertShader = LoadShader(0, shaderPath.c_str());

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // Update
    //-----------------------------------------------------
    if (IsFileDropped()) {
      FilePathList files = LoadDroppedFiles();
      char **droppedFiles = files.paths;
      std::cout << droppedFiles[0];
      int dropsCount = files.count;

      if ((dropsCount == 1) && IsFileExtension(droppedFiles[0], ".mpg")) {
        if (plm) {
          // If a video file is already loaded,
          // unload and reset everything
          plm_destroy(plm);
          frame = nullptr;
          sample = nullptr;
          framerate = 0.0;
          samplerate = 0;

          UnloadImage(imFrameY);
          UnloadTexture(textureY);
          UnloadImage(imFrameCb);
          UnloadTexture(textureCb);
          UnloadImage(imFrameCr);
          UnloadTexture(textureCr);

          StopAudioStream(stream);
          baseTime = 0.0;
          timeExcess = 0.0;
          pause = false;
        }

        plm = plm_create_with_filename(droppedFiles[0]);

        if (plm) {
          plm_set_loop(plm, false);

          framerate = plm_get_framerate(plm);
          samplerate = plm_get_samplerate(plm);

          TraceLog(LOG_INFO,
                   "[%s] Loaded succesfully. Framerate: %f - Samplerate: %i",
                   droppedFiles[0], (float)framerate, samplerate);

          totalVideoFrames = plm_get_duration(plm) * framerate;

          // Init video settings
          int videoWidth = plm_get_width(plm);
          int videoHeight = plm_get_height(plm);

          SetWindowSize(videoWidth, videoHeight);

          imFrameY.width = videoWidth;
          imFrameY.height = videoHeight;
          imFrameY.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
          imFrameY.mipmaps = 1;
          imFrameY.data = new unsigned char[imFrameY.width * imFrameY.height];

          textureY = LoadTextureFromImage(imFrameY);

          imFrameCb.width = videoWidth / 2;
          imFrameCb.height = videoHeight / 2;
          imFrameCb.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
          imFrameCb.mipmaps = 1;
          imFrameCb.data =
              new unsigned char[imFrameCb.width * imFrameCb.height];

          textureCb = LoadTextureFromImage(imFrameCb);

          imFrameCr.width = videoWidth / 2;
          imFrameCr.height = videoHeight / 2;
          imFrameCr.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
          imFrameCr.mipmaps = 1;
          imFrameCr.data =
              new unsigned char[imFrameCr.width * imFrameCr.height];

          textureCr = LoadTextureFromImage(imFrameCr);

          if (plm_get_num_audio_streams(plm) > 0) {
            plm_set_audio_enabled(plm, true);

            SetAudioStreamBufferSizeDefault(PLM_AUDIO_SAMPLES_PER_FRAME);
            stream = LoadAudioStream(samplerate, 32, 2);

            PlayAudioStream(stream);
            // plm_set_audio_lead_time(
            //     plm, static_cast<double>(PLM_AUDIO_SAMPLES_PER_FRAME) /
            //                              static_cast<double>(samplerate));
          }
        }
      }
      UnloadDroppedFiles(files);
    }

    if (IsKeyPressed(KEY_SPACE)) {
      pause = !pause;
    }

    if (IsKeyPressed(KEY_R)) {
      if (plm) {
        plm_destroy(plm);
      }
      plm = nullptr;
      frame = nullptr;
      sample = nullptr;
      baseTime = 0.0;
      timeExcess = 0.0;
    }

    if (plm && !pause) {
      double time = (GetTime() - baseTime);

      if (time >= (1.0 / framerate)) {
        timeExcess += (time - 0.040);
        baseTime = GetTime();

        frame = plm_decode_video(plm);
        currentVideoFrame++;

        if (timeExcess >= 0.040) {
          frame = plm_decode_video(plm);
          currentVideoFrame++;
          timeExcess = 0;
        }

        if (frame) {
          UpdateTexture(textureY, frame->y.data);
          UpdateTexture(textureCb, frame->cb.data);
          UpdateTexture(textureCr, frame->cr.data);
        }
      }

      while (IsAudioStreamProcessed(stream)) {
        sample = plm_decode_audio(plm);
        currentAudioFrame++;

        if (sample) {
          UpdateAudioStream(stream, sample->interleaved,
                            PLM_AUDIO_SAMPLES_PER_FRAME);
        }
      }

      if (plm_has_ended(plm)) {
        if (plm) {
          plm_destroy(plm);
        }
        plm = nullptr;
        frame = nullptr;
        sample = nullptr;
        baseTime = 0.0;
        timeExcess = 0.0;
      }
    }

    // Draw
    //-----------------------------------------------------
    BeginDrawing();

    ClearBackground(RAYWHITE);
    if (plm) {
      BeginShaderMode(convertShader);
      SetShaderValueTexture(convertShader,
                            GetShaderLocation(convertShader, "textureY"),
                            textureY);
      SetShaderValueTexture(convertShader,
                            GetShaderLocation(convertShader, "textureCb"),
                            textureCb);
      SetShaderValueTexture(convertShader,
                            GetShaderLocation(convertShader, "textureCr"),
                            textureCr);
      DrawTexturePro(textureY,
                     {0, 0, (float)textureY.width, (float)textureY.height},
                     {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
                     {0, 0}, 0.0, WHITE);
      EndShaderMode();
    } else {
      DrawText("MPEG Video Player", 320, 180, 30, LIGHTGRAY);
      DrawText("Drag and drop your MPEG file", 310, 240, 20, LIGHTGRAY);
    }
    if (pause) {
      DrawRectangle(GetScreenWidth() / 2 - 40, GetScreenHeight() / 2 - 40, 20,
                    80, RAYWHITE);
      DrawRectangle(GetScreenWidth() / 2 + 10, GetScreenHeight() / 2 - 40, 20,
                    80, RAYWHITE);
    }
    DrawFPS(0, 0);
    EndDrawing();
    //-----------------------------------------------------
  }

  // De-Initialization
  //---------------------------------------------------------
  if (plm) {
    plm_destroy(plm);
  }

  UnloadImage(imFrameY);
  UnloadTexture(textureY);

  UnloadImage(imFrameCb);
  UnloadTexture(textureCb);

  UnloadImage(imFrameCr);
  UnloadTexture(textureCr);

  StopAudioStream(stream);
  CloseAudioDevice();

  CloseWindow();
  //----------------------------------------------------------

  return 0;
}
