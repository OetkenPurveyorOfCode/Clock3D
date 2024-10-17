 #include <stdlib.h>
#include "raylib.h"
#include "raymath.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"
#include <memory.h>
#include <stdio.h>
#include "rcamera.h"
#include "camera.h"
#include <time.h>

#define ESCAPEMENT_ACCELERATION_DEG_PER_SEC_SQ 1
#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define MIN(a, b) ((a) <= (b) ? (a) : (b))

Matrix rotate_deg(float alpha, float beta, float gamma) {
    return MatrixRotateXYZ((Vector3){alpha*DEG2RAD, beta*DEG2RAD, gamma*DEG2RAD});
}
#ifdef _MSC_VER
int WinMain(void)
#else
int main(void)
#endif
{
    InitWindow(800, 600, "Clock");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowState(FLAG_WINDOW_MAXIMIZED);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitAudioDevice(); 
#if BAKE
    #include "../baked/tick.h"
    #include "../baked/tock.h"
    Wave tick_wave = LoadWaveFromMemory(".wav", ___assets_sounds_tick_wav, (int)sizeof(___assets_sounds_tick_wav));
    Wave tock_wave = LoadWaveFromMemory(".wav", ___assets_sounds_tock_wav, (int)sizeof(___assets_sounds_tock_wav));
    Sound tick = LoadSoundFromWave(tick_wave);
    Sound tock = LoadSoundFromWave(tock_wave);
    #ifdef PLATFORM_DESKTOP
        #include "../baked/lighting.vs.h"
        #include "../baked/lighting.fs.h"
        Shader shader = LoadShaderFromMemory(___assets_shaders_lighting_vs, ___assets_shaders_lighting_fs);
    #else
         #include "../baked/lighting_es.vs.h"
        #include "../baked/lighting_es.fs.h"
        Shader shader = LoadShaderFromMemory(___assets_shaders_lighting_es_vs, ___assets_shaders_lighting_es_fs);
    #endif
#else
    Sound tick = LoadSound("./assets_sounds/tick.wav");
    Sound tock = LoadSound("./assets_sounds/tock.wav");
    Shader shader = LoadShader("./assets_shaders/lighting.vs", "./assets_shaders/lighting.fs");
#endif
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    int ambient_loc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambient_loc, (float[4]){0.05f, 0.05f, 0.05f, 0.1f}, SHADER_UNIFORM_VEC4);
    Light lights[MAX_LIGHTS] = {
        [0] = CreateLight(LIGHT_POINT, (Vector3){-120, 110, -120}, Vector3Zero(), WHITE, shader),
        [1] = CreateLight(LIGHT_POINT, (Vector3){40, 40, 40}, Vector3Zero(), WHITE, shader),
        [2] = CreateLight(LIGHT_POINT, (Vector3){0, 100, 50}, Vector3Zero(), GRAY, shader),
    };
    (void)lights;

    int texel_color_loc = GetShaderLocation(shader, "texelColor");
    Vector4 random_color = {
        0.1,
        0.7,
        0.2,
        1.0,
    };
    SetShaderValue(shader, texel_color_loc, &random_color, SHADER_UNIFORM_VEC4);

    SetTargetFPS(60);
    Camera camera = {
        .position = (Vector3){0.0f, 0.0f, 200.0f }, // Camera position
        .target = (Vector3){ 0.0f, 0.0f, 0.0f },      // Camera looking at point
        .up = (Vector3){ 1.0f, 0.0f, 0.0f },          // Camera up vector (rotation towards target)
        .fovy = 100.0f,                               // Camera field-of-view Y
        .projection = CAMERA_PERSPECTIVE,             // Camera projection type
    };
#if BAKE
    #include "../baked/back_frame.h"
    #include "../baked/front_frame.h"
    #include "../baked/pallet.h"
    #include "../baked/escapement.h"
    #include "../baked/gear2_54_12.h"
    #include "../baked/gear3_54_12.h"
    #include "../baked/gear4_54_18.h"
    #include "../baked/gear4_18.h"
    #include "../baked/gear7_24.h"
    #include "../baked/gear7_48.h"
    #include "../baked/gear7_clicks.h"
    #include "../baked/gear8_54.h"
    #include "../baked/gear5_48_12.h"
    #include "../baked/gear6_54.h"
    #include "../baked/minute_hand.h"
    #include "../baked/hour_hand.h"
    Model gear2 = gear2_54_12;
    Model gear3 = gear3_54_12;
    Model gear4_58_12 = gear4_54_18;
#else
    // import("C:/Users/alexa/Coding_Projects/C/mechanical_clock/assets/back_frame.stl");
    Model back_frame = LoadModel("./assets_obj/back_frame.obj");

    // import("C:/Users/alexa/Coding_Projects/C/mechanical_clock/assets/front_frame.stl");
    Model front_frame = LoadModel("./assets_obj/front_frame.obj");

    //import("C:/Users/alexa/Coding_Projects/C/mechanical_clock/assets/pallet.stl");
    Model pallet = LoadModel("./assets_obj/pallet.obj");

    // import("C:/Users/alexa/Coding_Projects/C/mechanical_clock/assets/escapement.stl");
    Model escapement = LoadModel("./assets_obj/escapement.obj");

    // import("C:/Users/alexa/Coding_Projects/C/mechanical_clock/assets/gear2_54_12.stl");
    Model gear2 = LoadModel("./assets_obj/gear2_54_12.obj");

    // import("C:/Users/alexa/Coding_Projects/C/mechanical_clock/assets/gear3_54_12.stl");
    Model gear3 = LoadModel("./assets_obj/gear3_54_12.obj");

    // import("C:/Users/alexa/Coding_Projects/C/mechanical_clock/assets/gear4_54_18.stl");
    Model gear4_58_12 = LoadModel("./assets_obj/gear4_54_18.obj");

    // import("C:/Users/alexa/Coding_Projects/C/mechanical_clock/assets/gear4_18.stl");
    Model gear4_18 = LoadModel("./assets_obj/gear4_18.obj");

    // import("C:/Users/alexa/Coding_Projects/C/mechanical_clock/assets/gear7_24.stl");
    Model gear7_24 = LoadModel("./assets_obj/gear7_24.obj");

    // import("C:/Users/alexa/Coding_Projects/C/mechanical_clock/assets/gear7_48.stl");
    Model gear7_48 = LoadModel("./assets_obj/gear7_48.obj");

    // import("C:/Users/alexa/Coding_Projects/C/mechanical_clock/assets/gear7_clicks.stl");
    Model gear7_clicks = LoadModel("./assets_obj/gear7_clicks.obj");

    // import("C:/Users/alexa/Coding_Projects/C/mechanical_clock/assets/gear8_54.stl");
    Model gear8_54 = LoadModel("./assets_obj/gear8_54.obj");

    // import("C:/Users/alexa/Coding_Projects/C/mechanical_clock/assets/gear5_48_12.stl");
    Model gear5_48_12 = LoadModel("./assets_obj/gear5_48_12.obj");

    // import("C:/Users/alexa/Coding_Projects/C/mechanical_clock/assets/gear6_54.stl");
    Model gear6_54 = LoadModel("./assets_obj/gear6_54.obj");

    // import("C:/Users/alexa/Coding_Projects/C/mechanical_clock/assets/minute_hand.stl");
    Model minute_hand = LoadModel("./assets_obj/minute_hand.obj");

    // import("C:/Users/alexa/Coding_Projects/C/mechanical_clock/assets/hour_hand.stl");
    Model hour_hand = LoadModel("./assets_obj/hour_hand.obj");

#endif
    //rotate([0, 0, -35]) translate([0, -10, 0]) 
    back_frame.transform = MatrixMultiply(
        MatrixTranslate(0, -10, 0),
        MatrixRotateXYZ((Vector3){ DEG2RAD*(0), DEG2RAD*0, DEG2RAD*(-35)})
    );
    back_frame.materials[0].shader = shader;

    // rotate([0, 0, -35]) translate([7, 4, 34])
    front_frame.transform = MatrixMultiply(
        MatrixTranslate(7, 4, 34),
        MatrixRotateXYZ((Vector3){ DEG2RAD*(0), DEG2RAD*0, DEG2RAD*(-35)})
    );
    front_frame.materials[0].shader = shader;
    pallet.materials[0].shader = shader;
    escapement.materials[0].shader = shader;
    gear2.materials[0].shader = shader;
    gear3.materials[0].shader = shader;
    gear4_58_12.materials[0].shader = shader;
    gear4_18.materials[0].shader = shader;
    gear7_24.materials[0].shader = shader;
    gear7_48.materials[0].shader = shader;
    gear7_clicks.materials[0].shader = shader;
    Model gear7_clicks2 = gear7_clicks;
    Model gear7_clicks3 = gear7_clicks;
    gear8_54.materials[0].shader = shader;
    gear5_48_12.materials[0].shader = shader;
    gear6_54.materials[0].shader = shader;
    minute_hand.materials[0].shader = shader;
    hour_hand.materials[0].shader = shader;
    
    bool draw_grid = false;
    float escapement_angle_deg = 0.0;
    float escapement_desired_angle_deg = 0.0;
    float last_time = 0.0;
    float gear2_angle_deg = 0.0;
    bool update_camera = false;
    bool render_front = true;
    float hour_hand_offset = -90;
    float minute_hand_offset = -90;
    float gear4_angle_deg = 0;
    float gear6_54_angle_deg = 0;
    bool turbo_mode = false;
    bool has_tocked = true;
    bool audio = true;
    bool first = true;
    while (!WindowShouldClose())
    {
        if (update_camera) {
            Update_Camera(&camera, CAMERA_ORBITAL);
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            update_camera = !update_camera;
        }
        if (IsKeyPressed(KEY_B)) {
            render_front = !render_front;
        }   
        if (IsKeyPressed(KEY_R)) {
            int texel_color_loc = GetShaderLocation(shader, "texelColor");
            Vector4 random_color = {
                (float)GetRandomValue(0, 255)/255,
                (float)GetRandomValue(0, 255)/255,
                (float)GetRandomValue(0, 255)/255,
                1.0,
            };
            SetShaderValue(shader, texel_color_loc, &random_color, SHADER_UNIFORM_VEC4);
        }
        if (IsKeyPressed(KEY_S) || first) {
            time_t now = time(0);
            struct tm* tm = localtime(&now);
            TraceLog(LOG_INFO, TextFormat("Current time %d:%d", tm->tm_hour, tm->tm_min));
            float minute_hand_desired_angle = -90-tm->tm_min*6; // 60 mins * 6 = 360 degrees
            float hour_hand_desired_angle = -90-tm->tm_hour*360/12-tm->tm_min*360/(12*60); // 
            TraceLog(LOG_INFO, TextFormat("New desired angles %f:%f", hour_hand_desired_angle, minute_hand_desired_angle));
            hour_hand_offset = hour_hand_desired_angle - gear6_54_angle_deg;
            minute_hand_offset = minute_hand_desired_angle + gear4_angle_deg;
        }
        if (IsKeyPressed(KEY_T)) {
            turbo_mode = !turbo_mode;
        }
        if (IsKeyPressed(KEY_U)) {
            audio = !audio;
        }
        float camera_pos[3] = {camera.position.x, camera.position.y, camera.position.z};
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], camera_pos, SHADER_UNIFORM_VEC3);
        // update clock

        float t_in_sec = GetTime();
        float pallet_amplitude_angle_deg = 3;
        float pallet_period_in_sec = 1.316;
        if (turbo_mode) {
            pallet_period_in_sec = 0.1316;
        }
        float pallet_angle_deg = pallet_amplitude_angle_deg * sin(2 * PI * (t_in_sec/pallet_period_in_sec));
        pallet.transform = MatrixMultiply(
            rotate_deg(0, 0, pallet_angle_deg),
            MatrixMultiply(
                MatrixMultiply(
                    rotate_deg(0, 0, 90),
                    MatrixTranslate(-70, 0, 0)
                ),
                rotate_deg(0, 180, 0)
            )
        );
        if (turbo_mode) {
            escapement_angle_deg = -10000 * t_in_sec;
            escapement_desired_angle_deg = escapement_angle_deg;
        } else {
            if (t_in_sec - last_time > pallet_period_in_sec / 2 && !has_tocked) {
                if (audio) {
                    PlaySound(tock);
                }
                has_tocked = true;
            }
            if (t_in_sec - last_time > pallet_period_in_sec) {
                // tick
                escapement_desired_angle_deg -= 360 / 30; // thirty teeth
                last_time = t_in_sec;
                TraceLog(LOG_INFO, "tick");
                if (audio) {
                    PlaySound(tick);
                }
                has_tocked = false;
            }
            if (escapement_angle_deg > escapement_desired_angle_deg) {
                float t = GetTime() - last_time; 
                float escapement_accel = 20 / (pallet_period_in_sec*pallet_period_in_sec);
                escapement_angle_deg -= escapement_accel* t *t;
                if (escapement_angle_deg <= escapement_desired_angle_deg) {
                    escapement_angle_deg = escapement_desired_angle_deg;
                }
            }
        }
        // rotate([0, 180, 0]) translate([-30, 0, 0])
        escapement.transform = MatrixMultiply(
            rotate_deg(0, 0, escapement_angle_deg),
            MatrixMultiply(
                MatrixTranslate(-30, 0, 0),
                rotate_deg(0, 180, 0)
            )
        );
        // rotate([0, 180, 0]) translate([5, -22, 10])
        const float ESCAPEMENT_GEAR2_RATIO = 12.0f / 54.0f;
        gear2_angle_deg = -ESCAPEMENT_GEAR2_RATIO * escapement_angle_deg;
        gear2.transform = MatrixMultiply(
            rotate_deg(0, 0, gear2_angle_deg),
            MatrixMultiply(
                MatrixTranslate(5, -22, 10),
                rotate_deg(0, 180, 0)
            )
        );
        // rotate([0, 180, 0])  translate([5, 22, 12]) 
        const float GEAR2_GEAR3_RATIO = 12.0f / 54.0f;
        float gear3_angle_deg = -GEAR2_GEAR3_RATIO * gear2_angle_deg;
        gear3.transform = MatrixMultiply(
            rotate_deg(0, 0, gear3_angle_deg),
            MatrixMultiply(
                MatrixTranslate(5, 22, 12),
                rotate_deg(0, 180, 0)
            )
        );

        //translate([-40, 0, -6])
        const float GEAR3_GEAR4_RATIO = 12.0f / 54.0f;
        gear4_angle_deg = -GEAR3_GEAR4_RATIO*gear3_angle_deg;
        gear4_58_12.transform = MatrixMultiply(
            rotate_deg(0, 0, -gear4_angle_deg), // Not flipped
            MatrixTranslate(-40, 0, -6)
        );

        // gear7_48: translate([-63, -37, -8])
        const float GEAR4_GEAR7_48_RATIO = 18.0f/48.0f;
        float gear7_48_angle_deg = -GEAR4_GEAR7_48_RATIO*gear4_angle_deg;
        gear7_48.transform = MatrixMultiply(
            rotate_deg(0, 0, -gear7_48_angle_deg), // Not flipped
            MatrixTranslate(-63, -37, -8)
        );

        // gear7_24 translate([-63, -37, 0]) rotate([0, 180, 0]) 
        const float gear7_24_angle_deg = gear7_48_angle_deg;
        gear7_24.transform = MatrixMultiply(
            rotate_deg(0, 0, gear7_24_angle_deg),
            MatrixMultiply(
                rotate_deg(0, 180, 0),
                MatrixTranslate(-63, -37, 0)
            )
        );

        // translate([-67, -28, -4]) rotate([0, 180, 100])
        gear7_clicks.transform = MatrixMultiply(
            MatrixMultiply(
                MatrixTranslate(-8.47, 6.48, 0.88), // find rotation for piece
                rotate_deg(0, 180, gear7_48_angle_deg) // rotate around center
                
            ),
            MatrixTranslate(-64, -37, -4) // move to center 
        );

        // translate([-72, -44, -4])rotate([0, 180, 200]) 
        gear7_clicks2.transform = MatrixMultiply(
            MatrixMultiply(
                MatrixTranslate(-8.47, 6.48, 0.88), // find rotation for piece
                rotate_deg(0, 180, gear7_48_angle_deg+120) // rotate around center
                
            ),
            MatrixTranslate(-64, -37, -4) // move to center 
        );

        gear7_clicks3.transform = MatrixMultiply(
            MatrixMultiply(
                MatrixTranslate(-8.47, 6.48, 0.88), // find rotation for piece
                rotate_deg(0, 180, gear7_48_angle_deg+240) // rotate around center
                
            ),
            MatrixTranslate(-64, -37, -4) // move to center 
        );


        // rotate([0, 180, 0]) translate([95, 0, 7]) // values so weird because rotate translate order swapped
        const float GEAR7_24_GEAR8_54_RATIO = 24.0f/54.0f;
        float gear8_54_angle_deg = -GEAR7_24_GEAR8_54_RATIO*gear7_24_angle_deg;
        gear8_54.transform = MatrixMultiply(
            rotate_deg(0, 0, gear8_54_angle_deg),
            MatrixMultiply(
                MatrixTranslate(95, 0, 7),
                rotate_deg(0, 180, 0)
            )
        );

        // gear4_18 translate([-40, 0, 20])
        gear4_18.transform = MatrixMultiply(
            rotate_deg(0, 0, -gear4_angle_deg),
            MatrixTranslate(-40, 0, 20)
        );

        // gear5_48_12 translate([-63, 36, 20])
        const float GEAR4_18_GEAR5_48_12_RATIO = 18.0f/48.0f;
        float gear5_48_12_angle_deg = -GEAR4_18_GEAR5_48_12_RATIO*-gear4_angle_deg;
        gear5_48_12.transform = MatrixMultiply(
            rotate_deg(0, 0, gear5_48_12_angle_deg),
            MatrixTranslate(-63, 36, 20)
        );

        // gear6_54 (hour hand) translate([-40, 0, 33]) 
        const float GEAR6_54_GEAR5_48_12_RATIO = 12.0f/54.0f;
        gear6_54_angle_deg = -GEAR6_54_GEAR5_48_12_RATIO*gear5_48_12_angle_deg;
        gear6_54.transform = MatrixMultiply(
            rotate_deg(0, 0, gear6_54_angle_deg),
            MatrixTranslate(-40, 0, 33)
        );

        // minute hand translate([-40, 0, 53]) rotate([0, 0, -360*$t]) translate([0, 29, 0])
        minute_hand.transform = MatrixMultiply(
            MatrixTranslate(0, 29, 0),
            MatrixMultiply(
                rotate_deg(0, 0, minute_hand_offset-gear4_angle_deg),
                MatrixTranslate(-40, 0, 53)
            )
        );

        // hour hand translate([-40, 0, 53]) rotate([0, 0, -60*$t]) translate([0, 19.5, 0])
        hour_hand.transform = MatrixMultiply(
            MatrixTranslate(0, 19.5, 0),
            MatrixMultiply(
                rotate_deg(0, 0, hour_hand_offset+gear6_54_angle_deg),
                MatrixTranslate(-40, 0, 53)
            )
        );
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);
        BeginShaderMode(shader); 
        DrawModel(back_frame, (Vector3){.x=0.0, .y=0.0, .z=0.0}, 1.0f, WHITE);
        if (render_front) {
            DrawModel(front_frame, (Vector3){.x=0.0, .y=0.0, .z=0.0}, 1.0f, WHITE);
        }
        DrawModel(pallet, (Vector3){.x=0.0, .y=0.0, .z=0.0}, 1.0f, WHITE);
        DrawModel(escapement, (Vector3){.x=0.0, .y=0.0, .z=0.0}, 1.0f, WHITE); 
        DrawModel(gear2, (Vector3){.x=0.0, .y=0.0, .z=0.0}, 1.0f, WHITE);
        DrawModel(gear3, (Vector3){.x=0.0, .y=0.0, .z=0.0}, 1.0f, WHITE);
        DrawModel(gear4_58_12, (Vector3){.x=0.0, .y=0.0, .z=0.0}, 1.0f, WHITE);
        DrawModel(gear7_48, (Vector3){.x=0.0, .y=0.0, .z=0.0}, 1.0f, WHITE);
        DrawModel(gear7_24, (Vector3){.x=0.0, .y=0.0, .z=0.0}, 1.0f, WHITE);
        DrawModel(gear7_clicks, (Vector3){.x=0.0, .y=0.0, .z=0.0}, 1.0f, WHITE);
        DrawModel(gear7_clicks2, (Vector3){.x=0.0, .y=0.0, .z=0.0}, 1.0f, WHITE);
        DrawModel(gear7_clicks3, (Vector3){.x=0.0, .y=0.0, .z=0.0}, 1.0f, WHITE);
        DrawModel(gear8_54, (Vector3){.x=0.0, .y=0.0, .z=0.0}, 1.0f, WHITE);
        DrawModel(gear5_48_12, (Vector3){.x=0.0, .y=0.0, .z=0.0}, 1.0f, WHITE);
        DrawModel(gear4_18, (Vector3){.x=0.0, .y=0.0, .z=0.0}, 1.0f, WHITE);
        DrawModel(gear6_54, (Vector3){.x=0.0, .y=0.0, .z=0.0}, 1.0f, WHITE);
        DrawModel(minute_hand, (Vector3){.x=0.0, .y=0.0, .z=0.0}, 1.0f, WHITE);
        DrawModel(hour_hand, (Vector3){.x=0.0, .y=0.0, .z=0.0}, 1.0f, WHITE);
        EndShaderMode();
        if (draw_grid) {
            DrawGrid(100, 10.0f);
        }
        EndMode3D();
        EndDrawing();
        first = false;
    }
    CloseWindow();
    return 0;
}
