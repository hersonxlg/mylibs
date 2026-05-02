#ifndef RASTER2D_H
#define RASTER2D_H

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ============================================================================
// 1. ESTRUCTURAS DE DATOS
// ============================================================================

typedef struct {
    float x;
    float y;
} r2d_vec2;

typedef struct {
    float u;
    float v;
} r2d_vec2_uv;

// Sistema de Matrices para Transformaciones 2D
typedef struct { 
    float m[9]; 
} r2d_mat3;

// Sistema de Cámara
typedef struct {
    r2d_vec2 target;
    r2d_vec2 offset;
    float rotation;
    float zoom;
} r2d_camera;

typedef struct {
    uint32_t *pixels;
    int width;
    int height;
} r2d_texture;

// NUEVO: Motor Tipográfico (Texture Atlas)
typedef struct {
    uint8_t *atlas_pixels;  // La imagen gigante pre-renderizada (Canal Alpha)
    int atlas_width;        // Ancho de la imagen (ej. 512)
    int atlas_height;       // Alto de la imagen (ej. 512)
    void *char_data;        // Diccionario: coordenadas UV y métricas de cada letra
    float size;             // Tamaño con el que fue horneada
} r2d_font;

// ACTUALIZADO: El lienzo ya no depende de la fuente (Stateless)
typedef struct {
    uint32_t *buffer;
    int width;
    int height;
    int clip_x0;
    int clip_y0;
    int clip_x1;
    int clip_y1;
} r2d_canvas;

// ============================================================================
// 2. DECLARACIONES DE FUNCIONES (API) Y UTILIDADES GLOBALES
// ============================================================================

// Utilidad matemática global y segura
static inline float r2d_clamp(float val, float min, float max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

// -- Gestión del Lienzo --
r2d_canvas r2d_init_canvas(uint32_t *buffer, int width, int height);
void r2d_clear_background(r2d_canvas *canvas, uint32_t color);
void r2d_push_scissor(r2d_canvas *canvas, int x, int y, int w, int h);
void r2d_pop_scissor(r2d_canvas *canvas);

// Alias directos para Scissor (Compatibilidad)
void r2d_set_scissor(r2d_canvas *canvas, int x, int y, int w, int h);
void r2d_reset_scissor(r2d_canvas *canvas);
void r2d_push_scissor_v(r2d_canvas *canvas, r2d_vec2 pos, r2d_vec2 size);
void r2d_set_scissor_v(r2d_canvas *canvas, r2d_vec2 pos, r2d_vec2 size);

// -- Primitivas Geométricas (Coordenadas Crudas) --
void r2d_fill_rect(r2d_canvas *canvas, int x, int y, int w, int h, uint32_t color);
void r2d_draw_rect(r2d_canvas *canvas, int x, int y, int w, int h, uint32_t color);
void r2d_fill_circle(r2d_canvas *canvas, int cx, int cy, int r, uint32_t color);
void r2d_draw_circle(r2d_canvas *canvas, int cx, int cy, int r, uint32_t color);
void r2d_fill_sector(r2d_canvas *canvas, int cx, int cy, int r, float start_angle, float end_angle, uint32_t color);
void r2d_draw_arc(r2d_canvas *canvas, int cx, int cy, int r, float start_angle, float end_angle, uint32_t color);
void r2d_draw_line(r2d_canvas *canvas, int x0, int y0, int x1, int y1, uint32_t color);
void r2d_fill_triangle(r2d_canvas *canvas, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);
void r2d_draw_triangle(r2d_canvas *canvas, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

// -- Efectos Avanzados por Píxel --
void r2d_draw_pixel_blend(r2d_canvas *canvas, int x, int y, uint32_t color, float alpha);

// -- Primitivas Geométricas (Sobrecarga con r2d_vec2) --
void r2d_fill_rect_v(r2d_canvas *canvas, r2d_vec2 pos, r2d_vec2 size, uint32_t color);
void r2d_draw_rect_v(r2d_canvas *canvas, r2d_vec2 pos, r2d_vec2 size, uint32_t color);
void r2d_fill_circle_v(r2d_canvas *canvas, r2d_vec2 center, int r, uint32_t color);
void r2d_draw_circle_v(r2d_canvas *canvas, r2d_vec2 center, int r, uint32_t color);
void r2d_fill_sector_v(r2d_canvas *canvas, r2d_vec2 center, int r, float start_angle, float end_angle, uint32_t color);
void r2d_draw_line_v(r2d_canvas *canvas, r2d_vec2 p0, r2d_vec2 p1, uint32_t color);
void r2d_fill_triangle_v(r2d_canvas *canvas, r2d_vec2 p1, r2d_vec2 p2, r2d_vec2 p3, uint32_t color);
void r2d_draw_triangle_v(r2d_canvas *canvas, r2d_vec2 p0, r2d_vec2 p1, r2d_vec2 p2, uint32_t color);
void r2d_draw_arc_v(r2d_canvas *canvas, r2d_vec2 center, int r, float start_angle, float end_angle, uint32_t color);
void r2d_draw_pixel_blend_v(r2d_canvas *canvas, r2d_vec2 pos, uint32_t color, float alpha);
void r2d_draw_sprite_ex_v(r2d_canvas *canvas, r2d_texture *tex, r2d_vec2 pos, float scale, float rotation, uint8_t alpha);

// -- Curvas --
void r2d_draw_spline_catmull_rom(r2d_canvas *canvas, r2d_vec2 p0, r2d_vec2 p1, r2d_vec2 p2, r2d_vec2 p3, uint32_t color, int segments);

// -- Matrices y Cámara (Transformaciones) --
r2d_mat3 r2d_mat3_translate(float x, float y);
r2d_mat3 r2d_mat3_scale(float sx, float sy);
r2d_mat3 r2d_mat3_mul(r2d_mat3 a, r2d_mat3 b);
r2d_vec2 r2d_vec2_transform(r2d_vec2 v, r2d_mat3 mat);
r2d_mat3 r2d_camera_get_matrix(r2d_camera *cam);

// -- Texturas y UVs --
void r2d_fill_triangle_uv(r2d_canvas *canvas, r2d_vec2 p1, r2d_vec2 p2, r2d_vec2 p3, r2d_vec2_uv uv1, r2d_vec2_uv uv2, r2d_vec2_uv uv3, r2d_texture *tex);
void r2d_draw_sprite_ex(r2d_canvas *canvas, r2d_texture *tex, int x, int y, float scale, float rotation, uint8_t alpha);
void r2d_free_texture(r2d_texture *tex);

// -- Motor de Tipografía (Basado en Texture Atlas) --

// NUEVAS FIRMAS LIMPIAS
r2d_font r2d_load_font(const char *filename, float size);
r2d_vec2 r2d_measure_text(r2d_font font, const char *text);
void r2d_draw_text(r2d_canvas *canvas, r2d_font font, const char *text, int x, int y, uint32_t color);
void r2d_draw_text_v(r2d_canvas *canvas, r2d_font font, const char *text, r2d_vec2 pos, uint32_t color);
void r2d_free_font(r2d_font *font);

#endif // RASTER2D_H


// ============================================================================
// 3. IMPLEMENTACIÓN (Se activa con #define RASTER2D_IMPLEMENTATION)
// ============================================================================

#ifdef RASTER2D_IMPLEMENTATION

#include <stdio.h>
#include <string.h>

// Utilidades Internas
static inline uint32_t r2d_blend_color(uint32_t fg, uint32_t bg, uint8_t alpha) {
    if (alpha == 0) return bg;
    if (alpha == 255) return fg;
    uint32_t rb = (bg & 0xFF00FF) + ((((fg & 0xFF00FF) - (bg & 0xFF00FF)) * alpha) >> 8);
    uint32_t g = (bg & 0x00FF00) + ((((fg & 0x00FF00) - (bg & 0x00FF00)) * alpha) >> 8);
    return 0xFF000000 | (rb & 0xFF00FF) | (g & 0x00FF00); // Canal Alpha intacto
}

static inline void r2d_draw_pixel(r2d_canvas *canvas, int x, int y, uint32_t color) {
    if (x >= canvas->clip_x0 && x < canvas->clip_x1 && y >= canvas->clip_y0 && y < canvas->clip_y1) {
        uint8_t alpha = (color >> 24) & 0xFF;
        if (alpha == 255) {
            canvas->buffer[y * canvas->width + x] = color;
        } else if (alpha > 0) {
            uint32_t bg = canvas->buffer[y * canvas->width + x];
            canvas->buffer[y * canvas->width + x] = r2d_blend_color(color, bg, alpha);
        }
    }
}

// --- Implementación: Lienzo ---
r2d_canvas r2d_init_canvas(uint32_t *buffer, int width, int height) {
    r2d_canvas c;
    c.buffer = buffer;
    c.width = width;
    c.height = height;
    c.clip_x0 = 0;
    c.clip_y0 = 0;
    c.clip_x1 = width;
    c.clip_y1 = height;
    return c;
}

void r2d_clear_background(r2d_canvas *canvas, uint32_t color) {
    for (int i = 0; i < canvas->width * canvas->height; i++) {
        canvas->buffer[i] = color;
    }
}

void r2d_push_scissor(r2d_canvas *canvas, int x, int y, int w, int h) {
    canvas->clip_x0 = x < 0 ? 0 : x;
    canvas->clip_y0 = y < 0 ? 0 : y;
    canvas->clip_x1 = x + w > canvas->width ? canvas->width : x + w;
    canvas->clip_y1 = y + h > canvas->height ? canvas->height : y + h;
}

void r2d_pop_scissor(r2d_canvas *canvas) {
    canvas->clip_x0 = 0;
    canvas->clip_y0 = 0;
    canvas->clip_x1 = canvas->width;
    canvas->clip_y1 = canvas->height;
}

void r2d_set_scissor(r2d_canvas *canvas, int x, int y, int w, int h) { r2d_push_scissor(canvas, x, y, w, h); }
void r2d_reset_scissor(r2d_canvas *canvas) { r2d_pop_scissor(canvas); }
void r2d_push_scissor_v(r2d_canvas *canvas, r2d_vec2 pos, r2d_vec2 size) { 
    r2d_push_scissor(canvas, (int)pos.x, (int)pos.y, (int)size.x, (int)size.y); 
}
void r2d_set_scissor_v(r2d_canvas *canvas, r2d_vec2 pos, r2d_vec2 size) { 
    r2d_push_scissor_v(canvas, pos, size); 
}

// --- Implementación: Geometría Cruda ---
void r2d_fill_rect(r2d_canvas *canvas, int x, int y, int w, int h, uint32_t color) {
    int start_x = x < canvas->clip_x0 ? canvas->clip_x0 : x;
    int start_y = y < canvas->clip_y0 ? canvas->clip_y0 : y;
    int end_x = x + w > canvas->clip_x1 ? canvas->clip_x1 : x + w;
    int end_y = y + h > canvas->clip_y1 ? canvas->clip_y1 : y + h;

    for (int iy = start_y; iy < end_y; iy++) {
        for (int ix = start_x; ix < end_x; ix++) {
            r2d_draw_pixel(canvas, ix, iy, color);
        }
    }
}

void r2d_draw_rect(r2d_canvas *canvas, int x, int y, int w, int h, uint32_t color) {
    r2d_draw_line(canvas, x, y, x + w, y, color);
    r2d_draw_line(canvas, x, y + h, x + w, y + h, color);
    r2d_draw_line(canvas, x, y, x, y + h, color);
    r2d_draw_line(canvas, x + w, y, x + w, y + h, color);
}

void r2d_fill_circle(r2d_canvas *canvas, int cx, int cy, int r, uint32_t color) {
    int start_x = (cx - r < canvas->clip_x0) ? canvas->clip_x0 : cx - r;
    int end_x = (cx + r > canvas->clip_x1) ? canvas->clip_x1 : cx + r;
    int start_y = (cy - r < canvas->clip_y0) ? canvas->clip_y0 : cy - r;
    int end_y = (cy + r > canvas->clip_y1) ? canvas->clip_y1 : cy + r;

    int r2 = r * r;
    for (int y = start_y; y < end_y; y++) {
        for (int x = start_x; x < end_x; x++) {
            int dx = x - cx; int dy = y - cy;
            if (dx * dx + dy * dy <= r2) r2d_draw_pixel(canvas, x, y, color);
        }
    }
}

void r2d_draw_circle(r2d_canvas *canvas, int cx, int cy, int r, uint32_t color) {
    int x = r, y = 0, p = 1 - r;
    while (x >= y) {
        r2d_draw_pixel(canvas, cx + x, cy + y, color); r2d_draw_pixel(canvas, cx + y, cy + x, color);
        r2d_draw_pixel(canvas, cx - y, cy + x, color); r2d_draw_pixel(canvas, cx - x, cy + y, color);
        r2d_draw_pixel(canvas, cx - x, cy - y, color); r2d_draw_pixel(canvas, cx - y, cy - x, color);
        r2d_draw_pixel(canvas, cx + y, cy - x, color); r2d_draw_pixel(canvas, cx + x, cy - y, color);
        y++;
        if (p <= 0) p = p + 2 * y + 1;
        else { x--; p = p + 2 * y - 2 * x + 1; }
    }
}

void r2d_fill_sector(r2d_canvas *canvas, int cx, int cy, int r, float start_angle, float end_angle, uint32_t color) {
    start_angle = fmodf(start_angle, 2 * M_PI); if (start_angle < 0) start_angle += 2 * M_PI;
    end_angle = fmodf(end_angle, 2 * M_PI); if (end_angle < 0) end_angle += 2 * M_PI;
    if (end_angle < start_angle) end_angle += 2 * M_PI;

    int r2 = r * r;
    int min_y = (cy - r < canvas->clip_y0) ? canvas->clip_y0 : cy - r;
    int max_y = (cy + r > canvas->clip_y1) ? canvas->clip_y1 : cy + r;
    int min_x = (cx - r < canvas->clip_x0) ? canvas->clip_x0 : cx - r;
    int max_x = (cx + r > canvas->clip_x1) ? canvas->clip_x1 : cx + r;

    for (int y = min_y; y < max_y; y++) {
        for (int x = min_x; x < max_x; x++) {
            int dx = x - cx; int dy = y - cy;
            if (dx * dx + dy * dy <= r2) {
                float angle = atan2f((float)dy, (float)dx);
                if (angle < 0) angle += 2 * M_PI;
                if ((angle >= start_angle && angle <= end_angle) || 
                    (angle + 2 * M_PI >= start_angle && angle + 2 * M_PI <= end_angle)) {
                    r2d_draw_pixel(canvas, x, y, color);
                }
            }
        }
    }
}

void r2d_draw_arc(r2d_canvas *canvas, int cx, int cy, int r, float start_angle, float end_angle, uint32_t color) {
    start_angle = fmodf(start_angle, 2 * M_PI); if (start_angle < 0) start_angle += 2 * M_PI;
    end_angle = fmodf(end_angle, 2 * M_PI); if (end_angle < 0) end_angle += 2 * M_PI;
    if (end_angle < start_angle) end_angle += 2 * M_PI;

    int r2_outer = r * r;
    int r2_inner = (r - 1) * (r - 1);

    int min_y = (cy - r < canvas->clip_y0) ? canvas->clip_y0 : cy - r;
    int max_y = (cy + r > canvas->clip_y1) ? canvas->clip_y1 : cy + r;
    int min_x = (cx - r < canvas->clip_x0) ? canvas->clip_x0 : cx - r;
    int max_x = (cx + r > canvas->clip_x1) ? canvas->clip_x1 : cx + r;

    for (int y = min_y; y < max_y; y++) {
        for (int x = min_x; x < max_x; x++) {
            int dx = x - cx; int dy = y - cy;
            int dist2 = dx * dx + dy * dy;
            if (dist2 <= r2_outer && dist2 >= r2_inner) {
                float angle = atan2f((float)dy, (float)dx);
                if (angle < 0) angle += 2 * M_PI;
                if ((angle >= start_angle && angle <= end_angle) || 
                    (angle + 2 * M_PI >= start_angle && angle + 2 * M_PI <= end_angle)) {
                    r2d_draw_pixel(canvas, x, y, color);
                }
            }
        }
    }
}

void r2d_draw_line(r2d_canvas *canvas, int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    for (;;) {
        r2d_draw_pixel(canvas, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void r2d_draw_triangle(r2d_canvas *canvas, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    r2d_draw_line(canvas, x0, y0, x1, y1, color);
    r2d_draw_line(canvas, x1, y1, x2, y2, color);
    r2d_draw_line(canvas, x2, y2, x0, y0, color);
}

void r2d_fill_triangle(r2d_canvas *canvas, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color) {
    int min_x = x1 < x2 ? (x1 < x3 ? x1 : x3) : (x2 < x3 ? x2 : x3);
    int max_x = x1 > x2 ? (x1 > x3 ? x1 : x3) : (x2 > x3 ? x2 : x3);
    int min_y = y1 < y2 ? (y1 < y3 ? y1 : y3) : (y2 < y3 ? y2 : y3);
    int max_y = y1 > y2 ? (y1 > y3 ? y1 : y3) : (y2 > y3 ? y2 : y3);

    min_x = min_x < canvas->clip_x0 ? canvas->clip_x0 : min_x;
    max_x = max_x > canvas->clip_x1 - 1 ? canvas->clip_x1 - 1 : max_x;
    min_y = min_y < canvas->clip_y0 ? canvas->clip_y0 : min_y;
    max_y = max_y > canvas->clip_y1 - 1 ? canvas->clip_y1 - 1 : max_y;

    float det = (y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3);
    if (det == 0) return;

    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            float l1 = ((y2 - y3) * (x - x3) + (x3 - x2) * (y - y3)) / det;
            float l2 = ((y3 - y1) * (x - x3) + (x1 - x3) * (y - y3)) / det;
            float l3 = 1.0f - l1 - l2;
            if (l1 >= -0.001f && l2 >= -0.001f && l3 >= -0.001f) {
                r2d_draw_pixel(canvas, x, y, color);
            }
        }
    }
}

void r2d_draw_pixel_blend(r2d_canvas *canvas, int x, int y, uint32_t color, float alpha) {
    if (x >= canvas->clip_x0 && x < canvas->clip_x1 && y >= canvas->clip_y0 && y < canvas->clip_y1) {
        uint8_t alpha_u8 = (uint8_t)(r2d_clamp(alpha, 0.0f, 1.0f) * 255.0f);
        uint32_t bg = canvas->buffer[y * canvas->width + x];
        canvas->buffer[y * canvas->width + x] = r2d_blend_color(color, bg, alpha_u8);
    }
}

// --- Implementación: Geometría con r2d_vec2 (Wrappers) ---
void r2d_fill_rect_v(r2d_canvas *canvas, r2d_vec2 pos, r2d_vec2 size, uint32_t color) { r2d_fill_rect(canvas, (int)pos.x, (int)pos.y, (int)size.x, (int)size.y, color); }
void r2d_draw_rect_v(r2d_canvas *canvas, r2d_vec2 pos, r2d_vec2 size, uint32_t color) { r2d_draw_rect(canvas, (int)pos.x, (int)pos.y, (int)size.x, (int)size.y, color); }
void r2d_fill_circle_v(r2d_canvas *canvas, r2d_vec2 center, int r, uint32_t color) { r2d_fill_circle(canvas, (int)center.x, (int)center.y, r, color); }
void r2d_draw_circle_v(r2d_canvas *canvas, r2d_vec2 center, int r, uint32_t color) { r2d_draw_circle(canvas, (int)center.x, (int)center.y, r, color); }
void r2d_fill_sector_v(r2d_canvas *canvas, r2d_vec2 center, int r, float start_angle, float end_angle, uint32_t color) { r2d_fill_sector(canvas, (int)center.x, (int)center.y, r, start_angle, end_angle, color); }
void r2d_draw_line_v(r2d_canvas *canvas, r2d_vec2 p0, r2d_vec2 p1, uint32_t color) { r2d_draw_line(canvas, (int)p0.x, (int)p0.y, (int)p1.x, (int)p1.y, color); }
void r2d_fill_triangle_v(r2d_canvas *canvas, r2d_vec2 p1, r2d_vec2 p2, r2d_vec2 p3, uint32_t color) { r2d_fill_triangle(canvas, (int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, (int)p3.x, (int)p3.y, color); }
void r2d_draw_triangle_v(r2d_canvas *canvas, r2d_vec2 p0, r2d_vec2 p1, r2d_vec2 p2, uint32_t color) { r2d_draw_triangle(canvas, (int)p0.x, (int)p0.y, (int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, color); }
void r2d_draw_arc_v(r2d_canvas *canvas, r2d_vec2 center, int r, float start_angle, float end_angle, uint32_t color) { 
    r2d_draw_arc(canvas, (int)center.x, (int)center.y, r, start_angle, end_angle, color); 
}

void r2d_draw_pixel_blend_v(r2d_canvas *canvas, r2d_vec2 pos, uint32_t color, float alpha) { 
    r2d_draw_pixel_blend(canvas, (int)pos.x, (int)pos.y, color, alpha); 
}

void r2d_draw_sprite_ex_v(r2d_canvas *canvas, r2d_texture *tex, r2d_vec2 pos, float scale, float rotation, uint8_t alpha) { 
    r2d_draw_sprite_ex(canvas, tex, (int)pos.x, (int)pos.y, scale, rotation, alpha); 
}

// --- Implementación: Matrices y Cámara ---
r2d_mat3 r2d_mat3_translate(float x, float y) { r2d_mat3 res = { {1, 0, x,  0, 1, y,  0, 0, 1} }; return res; }
r2d_mat3 r2d_mat3_scale(float sx, float sy) { r2d_mat3 res = { {sx, 0, 0,  0, sy, 0,  0, 0, 1} }; return res; }
r2d_mat3 r2d_mat3_mul(r2d_mat3 a, r2d_mat3 b) {
    r2d_mat3 res = {0};
    for(int i=0; i<3; i++) for(int j=0; j<3; j++) for(int k=0; k<3; k++)
        res.m[i*3+j] += a.m[i*3+k] * b.m[k*3+j];
    return res;
}
r2d_vec2 r2d_vec2_transform(r2d_vec2 v, r2d_mat3 mat) {
    r2d_vec2 res;
    res.x = v.x * mat.m[0] + v.y * mat.m[1] + mat.m[2];
    res.y = v.x * mat.m[3] + v.y * mat.m[4] + mat.m[5];
    return res;
}
r2d_mat3 r2d_camera_get_matrix(r2d_camera *cam) {
    r2d_mat3 mat = r2d_mat3_translate(-cam->target.x, -cam->target.y);
    float c = cosf(cam->rotation), s = sinf(cam->rotation);
    r2d_mat3 rot = { {c, -s, 0,  s, c, 0,  0, 0, 1} };
    mat = r2d_mat3_mul(rot, mat);
    mat = r2d_mat3_mul(r2d_mat3_scale(cam->zoom, cam->zoom), mat);
    return r2d_mat3_mul(r2d_mat3_translate(cam->offset.x, cam->offset.y), mat);
}

// --- Implementación: Curvas ---
void r2d_draw_spline_catmull_rom(r2d_canvas *canvas, r2d_vec2 p0, r2d_vec2 p1, r2d_vec2 p2, r2d_vec2 p3, uint32_t color, int segments) {
    if (segments <= 0) segments = 1;
    r2d_vec2 last_point = p1;
    for (int i = 1; i <= segments; i++) {
        float t = (float)i / segments;
        float t2 = t * t; float t3 = t2 * t;
        float x = 0.5f * ((2.0f * p1.x) + (-p0.x + p2.x) * t + (2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t2 + (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t3);
        float y = 0.5f * ((2.0f * p1.y) + (-p0.y + p2.y) * t + (2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * t2 + (-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t3);
        r2d_draw_line(canvas, (int)last_point.x, (int)last_point.y, (int)x, (int)y, color);
        last_point.x = x; last_point.y = y;
    }
}

// --- Implementación: Texturas y UVs ---
void r2d_fill_triangle_uv(r2d_canvas *canvas, r2d_vec2 p1, r2d_vec2 p2, r2d_vec2 p3, r2d_vec2_uv uv1, r2d_vec2_uv uv2, r2d_vec2_uv uv3, r2d_texture *tex) {
    int min_x = (int)fminf(p1.x, fminf(p2.x, p3.x));
    int max_x = (int)fmaxf(p1.x, fmaxf(p2.x, p3.x));
    int min_y = (int)fminf(p1.y, fminf(p2.y, p3.y));
    int max_y = (int)fmaxf(p1.y, fmaxf(p2.y, p3.y));

    min_x = r2d_clamp(min_x, canvas->clip_x0, canvas->clip_x1 - 1);
    max_x = r2d_clamp(max_x, canvas->clip_x0, canvas->clip_x1 - 1);
    min_y = r2d_clamp(min_y, canvas->clip_y0, canvas->clip_y1 - 1);
    max_y = r2d_clamp(max_y, canvas->clip_y0, canvas->clip_y1 - 1);

    float det = (p2.y - p3.y) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.y - p3.y);
    if (det == 0) return;

    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            float l1 = ((p2.y - p3.y) * (x - p3.x) + (p3.x - p2.x) * (y - p3.y)) / det;
            float l2 = ((p3.y - p1.y) * (x - p3.x) + (p1.x - p3.x) * (y - p3.y)) / det;
            float l3 = 1.0f - l1 - l2;

            if (l1 >= -0.001f && l2 >= -0.001f && l3 >= -0.001f) {
                float u = r2d_clamp(l1 * uv1.u + l2 * uv2.u + l3 * uv3.u, 0.0f, 1.0f);
                float v = r2d_clamp(l1 * uv1.v + l2 * uv2.v + l3 * uv3.v, 0.0f, 1.0f);

                int tx = (int)(u * (tex->width - 1));
                int ty = (int)(v * (tex->height - 1));

                if (tx < 0) tx = 0; if (tx >= tex->width) tx = tex->width - 1;
                if (ty < 0) ty = 0; if (ty >= tex->height) ty = tex->height - 1;

                uint32_t tex_color = tex->pixels[ty * tex->width + tx];
                r2d_draw_pixel(canvas, x, y, tex_color);
            }
        }
    }
}

void r2d_draw_sprite_ex(r2d_canvas *canvas, r2d_texture *tex, int x, int y, float scale, float rotation, uint8_t alpha) {
    if (scale <= 0) return;
    float cos_rot = cosf(rotation);
    float sin_rot = sinf(rotation);
    int hw = tex->width / 2;
    int hh = tex->height / 2;

    int r_width = (int)(tex->width * scale);
    int r_height = (int)(tex->height * scale);
    int max_radius = (int)sqrtf(r_width*r_width + r_height*r_height);

    int start_x = r2d_clamp(x - max_radius, canvas->clip_x0, canvas->clip_x1);
    int end_x = r2d_clamp(x + max_radius, canvas->clip_x0, canvas->clip_x1);
    int start_y = r2d_clamp(y - max_radius, canvas->clip_y0, canvas->clip_y1);
    int end_y = r2d_clamp(y + max_radius, canvas->clip_y0, canvas->clip_y1);

    for (int iy = start_y; iy < end_y; iy++) {
        for (int ix = start_x; ix < end_x; ix++) {
            float dx = (ix - x) / scale;
            float dy = (iy - y) / scale;

            int tx = (int)(dx * cos_rot + dy * sin_rot) + hw;
            int ty = (int)(-dx * sin_rot + dy * cos_rot) + hh;

            if (tx >= 0 && tx < tex->width && ty >= 0 && ty < tex->height) {
                uint32_t color = tex->pixels[ty * tex->width + tx];
                uint8_t tex_alpha = (color >> 24) & 0xFF;
                uint8_t final_alpha = (tex_alpha * alpha) / 255;
                if (final_alpha > 0) {
                    color = (color & 0x00FFFFFF) | (final_alpha << 24);
                    r2d_draw_pixel(canvas, ix, iy, color);
                }
            }
        }
    }
}

void r2d_free_texture(r2d_texture *tex) {
    if (tex && tex->pixels) {
        free(tex->pixels);
        tex->pixels = NULL;
    }
}

// --- Implementación: Motor de Tipografía (Texture Atlas Refactor) ---
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

// Retorna la estructura lista. Usa un Atlas estándar de 512x512
r2d_font r2d_load_font(const char *filename, float size) {
    r2d_font font = {0};
    
    FILE *file = fopen(filename, "rb");
    if (!file) return font;

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char *ttf_buffer = (unsigned char *)malloc(file_size);
    if (!ttf_buffer) {
        fclose(file);
        return font;
    }
    
    fread(ttf_buffer, 1, file_size, file);
    fclose(file);

    font.atlas_width = 512;
    font.atlas_height = 512;
    font.atlas_pixels = (uint8_t *)malloc(font.atlas_width * font.atlas_height);
    font.char_data = malloc(sizeof(stbtt_bakedchar) * 96); // ASCII 32 a 126
    font.size = size;

    if (font.atlas_pixels && font.char_data) {
        // Hornear 96 caracteres en el mapa de bits (Desde el espacio ' ' hasta '~')
        stbtt_BakeFontBitmap(ttf_buffer, 0, size, font.atlas_pixels, font.atlas_width, font.atlas_height, 32, 96, (stbtt_bakedchar *)font.char_data);
    } else {
        if (font.atlas_pixels) free(font.atlas_pixels);
        if (font.char_data) free(font.char_data);
        font.atlas_pixels = NULL;
        font.char_data = NULL;
    }

    // ELIMINACIÓN DE MEMORY LEAK: ¡Ya no guardamos el TTF original en RAM!
    free(ttf_buffer); 
    return font;
}

// Limpieza de memoria RAM del Atlas
void r2d_free_font(r2d_font *font) {
    if (font) {
        if (font->atlas_pixels) { free(font->atlas_pixels); font->atlas_pixels = NULL; }
        if (font->char_data) { free(font->char_data); font->char_data = NULL; }
    }
}

// Recorre las métricas de stb_truetype sin dibujar para calcular el ancho total
r2d_vec2 r2d_measure_text(r2d_font font, const char *text) {
    r2d_vec2 size = {0.0f, 0.0f};
    if (!font.char_data || !text) return size;

    stbtt_bakedchar *cdata = (stbtt_bakedchar *)font.char_data;
    float x = 0.0f;
    float y = 0.0f;
    float min_y = 10000.0f;
    float max_y = -10000.0f;

    while (*text) {
        if (*text >= 32 && *text < 128) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(cdata, font.atlas_width, font.atlas_height, *text - 32, &x, &y, &q, 1);
            if (q.y0 < min_y) min_y = q.y0;
            if (q.y1 > max_y) max_y = q.y1;
        }
        text++;
    }
    
    size.x = x; 
    size.y = (max_y > min_y) ? (max_y - min_y) : font.size;
    return size;
}

// Renderizado Ultrarrápido: Solo recorta píxeles del Atlas, cero cálculos de curvas.
void r2d_draw_text(r2d_canvas *canvas, r2d_font font, const char *text, int x, int y, uint32_t color) {
    if (!font.char_data || !font.atlas_pixels || !text) return;

    stbtt_bakedchar *cdata = (stbtt_bakedchar *)font.char_data;
    float cursor_x = (float)x;
    float cursor_y = (float)y;

    while (*text) {
        if (*text >= 32 && *text < 128) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(cdata, font.atlas_width, font.atlas_height, *text - 32, &cursor_x, &cursor_y, &q, 1);

            int start_x = (int)q.x0;
            int end_x = (int)q.x1;
            int start_y = (int)q.y0;
            int end_y = (int)q.y1;

            int tex_start_x = (int)(q.s0 * font.atlas_width);
            int tex_start_y = (int)(q.t0 * font.atlas_height);

            for (int iy = start_y, ty = tex_start_y; iy < end_y; iy++, ty++) {
                for (int ix = start_x, tx = tex_start_x; ix < end_x; ix++, tx++) {
                    // Verificación de recorte del lienzo (Scissor)
                    if (ix >= canvas->clip_x0 && ix < canvas->clip_x1 && iy >= canvas->clip_y0 && iy < canvas->clip_y1) {
                        // Extracción segura del atlas
                        if (tx >= 0 && tx < font.atlas_width && ty >= 0 && ty < font.atlas_height) {
                            uint8_t alpha = font.atlas_pixels[ty * font.atlas_width + tx];
                            
                            if (alpha > 0) {
                                uint32_t bg = canvas->buffer[iy * canvas->width + ix];
                                uint8_t global_alpha = (color >> 24) & 0xFF;
                                uint8_t final_alpha = (alpha * global_alpha) / 255;
                                
                                uint32_t final_color = (color & 0x00FFFFFF) | (final_alpha << 24);
                                canvas->buffer[iy * canvas->width + ix] = r2d_blend_color(final_color, bg, final_alpha);
                            }
                        }
                    }
                }
            }
        }
        text++;
    }
}
// Wrapper de r2d_draw_text usando r2d_vec2
void r2d_draw_text_v(r2d_canvas *canvas, r2d_font font, const char *text, r2d_vec2 pos, uint32_t color) {
    r2d_draw_text(canvas, font, text, (int)pos.x, (int)pos.y, color);
}


#endif // RASTER2D_IMPLEMENTATION
