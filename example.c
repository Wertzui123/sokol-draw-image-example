#define SOKOL_IMPL
#define SOKOL_GLCORE33
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_app.h"
#include "sokol/sokol_log.h"
#include "sokol/sokol_glue.h"
#include "sokol/util/sokol_gl.h"

#include <stdlib.h>

typedef struct StreamingImage {
    sg_image image;
    sg_sampler sampler;
} StreamingImage;

StreamingImage canvas_image;

StreamingImage new_streaming_image(int width, int height, int channels) {
    sg_image_desc desc = {
        .width = width,
        .height = height,
        .pixel_format = SG_PIXELFORMAT_RGBA8,
        .num_slices = 1,
        .num_mipmaps = 1,
        .usage = SG_USAGE_STREAM,
        .label = "Example Image"
    };
    desc.data.subimage[0][0].ptr = 0;
    desc.data.subimage[0][0].size = 0;
    sg_sampler_desc sampler_desc = {
        .min_filter = SG_FILTER_LINEAR,
        .mag_filter = SG_FILTER_LINEAR,
        .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
    };
    return (StreamingImage) { .image = sg_make_image(&desc), .sampler = sg_make_sampler(&sampler_desc) };
}

void init(void) {
    sg_setup(&(sg_desc) {
        .context = sapp_sgcontext(),
            .logger.func = slog_func,
    });
    sgl_setup(&(sgl_desc_t) {
        .logger.func = slog_func,
    });

    canvas_image = new_streaming_image(400, 300, 4);
}

void frame(void) {
    unsigned char* ptr = calloc(sapp_width() * sapp_height() * 4, sizeof(unsigned char));
    // The following for-loop is just an example of how to update the image data
    for (int i = 0; i < sapp_width() * sapp_height() * 4; i += 4) {
        ptr[i] = 0;
        ptr[i + 1] = 0;
        if (sapp_frame_count() % 512 < 256) {
            ptr[i + 2] = sapp_frame_count() % 256;
        }
        else {
            ptr[i + 2] = 255 - sapp_frame_count() % 256;
        }
        ptr[i + 3] = 255;
    }

    if (sg_query_image_desc(canvas_image.image).width != sapp_width() || sg_query_image_desc(canvas_image.image).height != sapp_height()) {
        sg_destroy_image(canvas_image.image);
        sg_destroy_sampler(canvas_image.sampler);
        canvas_image = new_streaming_image(sapp_width(), sapp_height(), 4);
    }

    sg_image_data data = {
        .subimage[0][0] = {
            .ptr = ptr,
            .size = sapp_width() * sapp_height() * 4,
        }
    };
    sg_update_image(canvas_image.image, &data);

    sgl_defaults();

    sgl_ortho(0.0f, sapp_width(), sapp_height(), 0.0f, -1.0f, 1.0f);

    sgl_enable_texture();
    sgl_texture(canvas_image.image, canvas_image.sampler);
    sgl_begin_quads();
    sgl_c4b(255, 255, 255, 255);
    sgl_v3f_t2f(0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    sgl_v3f_t2f(sapp_width(), 0.0f, 0.0f, 1.0f, 0.0f);
    sgl_v3f_t2f(sapp_width(), sapp_height(), 0.0f, 1.0f, 1.0f);
    sgl_v3f_t2f(0.0f, sapp_height(), 0.0f, 0.0f, 1.0f);
    sgl_end();
    sgl_disable_texture();

    sg_begin_default_pass(&(sg_pass_action) {
        .colors[0] = { .load_action = SG_LOADACTION_CLEAR, .clear_value = { 0.0f, 0.0f, 0.0f, 1.0f } }
    }, sapp_width(), sapp_height());

    sgl_draw();

    sg_end_pass();
    sg_commit();
}

void cleanup(void) {
    sg_shutdown();
}

sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    return (sapp_desc) {
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .width = 400,
        .height = 300,
        .window_title = "Sokol Draw Image Example",
        .logger.func = slog_func,
    };
}