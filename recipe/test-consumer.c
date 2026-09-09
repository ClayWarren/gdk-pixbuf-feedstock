#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib/gstdio.h>
#include <string.h>

static void roundtrip(GdkPixbuf *source, const char *format, gboolean lossless)
{
    GError *error = NULL;
    char *filename = g_strdup_printf("gdk-pixbuf consumer.%s", format);
    g_assert_true(gdk_pixbuf_save(source, filename, format, &error, NULL));
    g_assert_no_error(error);

    GdkPixbuf *loaded = gdk_pixbuf_new_from_file(filename, &error);
    g_assert_no_error(error);
    g_assert_nonnull(loaded);
    g_assert_cmpint(gdk_pixbuf_get_width(loaded), ==, 3);
    g_assert_cmpint(gdk_pixbuf_get_height(loaded), ==, 2);
    if (lossless) {
        g_assert_cmpint(gdk_pixbuf_get_n_channels(loaded), ==, 3);
        for (int y = 0; y < 2; ++y) {
            const guchar *expected = gdk_pixbuf_read_pixels(source)
                + y * gdk_pixbuf_get_rowstride(source);
            const guchar *actual = gdk_pixbuf_read_pixels(loaded)
                + y * gdk_pixbuf_get_rowstride(loaded);
            g_assert_cmpmem(expected, 9, actual, 9);
        }
    }
    g_print("PASS: installed %s encoder and decoder\n", format);
    g_object_unref(loaded);
    g_assert_cmpint(g_remove(filename), ==, 0);
    g_free(filename);
}

int main(void)
{
    const guchar pixels[2][9] = {
        {255, 0, 0, 0, 255, 0, 0, 0, 255},
        {255, 255, 255, 51, 102, 153, 0, 0, 0}
    };
    GdkPixbuf *source = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, 3, 2);
    g_assert_nonnull(source);
    for (int y = 0; y < 2; ++y)
        memcpy(gdk_pixbuf_get_pixels(source) + y * gdk_pixbuf_get_rowstride(source),
               pixels[y], 9);

    roundtrip(source, "png", TRUE);
    roundtrip(source, "tiff", TRUE);
    roundtrip(source, "jpeg", FALSE);

    GdkPixbuf *scaled = gdk_pixbuf_scale_simple(source, 6, 4, GDK_INTERP_NEAREST);
    g_assert_nonnull(scaled);
    g_assert_cmpint(gdk_pixbuf_get_width(scaled), ==, 6);
    g_assert_cmpint(gdk_pixbuf_get_height(scaled), ==, 4);
    for (int y = 0; y < 4; ++y)
        for (int x = 0; x < 6; ++x) {
            const guchar *actual = gdk_pixbuf_read_pixels(scaled)
                + y * gdk_pixbuf_get_rowstride(scaled) + x * 3;
            g_assert_cmpmem(actual, 3, pixels[y / 2] + (x / 2) * 3, 3);
        }
    g_object_unref(scaled);
    g_object_unref(source);
    g_print("PASS: installed nearest-neighbor scaling\n");
    return 0;
}
