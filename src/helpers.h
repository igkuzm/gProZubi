/**
 * File              : helpers.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 15.11.2023
 * Last Modified Date: 15.11.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include <gtk/gtk.h>

static gboolean
copy_recursive(GFile *src, GFile *dest, GFileCopyFlags flags, GCancellable *cancellable, GError **error) {
    GFileType src_type = g_file_query_file_type(src, G_FILE_QUERY_INFO_NONE, cancellable);
    if (src_type == G_FILE_TYPE_DIRECTORY) {
        g_file_make_directory(dest, cancellable, error);
        g_file_copy_attributes(src, dest, flags, cancellable, error);

        GFileEnumerator *enumerator = g_file_enumerate_children(src, G_FILE_ATTRIBUTE_STANDARD_NAME, G_FILE_QUERY_INFO_NONE, cancellable, error);
        for (GFileInfo *info = g_file_enumerator_next_file(enumerator, cancellable, error); info != NULL; info = g_file_enumerator_next_file(enumerator, cancellable, error)) {
            const char *relative_path = g_file_info_get_name(info);
            copy_recursive(
                g_file_resolve_relative_path(src, relative_path),
                g_file_resolve_relative_path(dest, relative_path),
                flags, cancellable, error);
        }
    } else if (src_type == G_FILE_TYPE_REGULAR) {
        g_file_copy(src, dest, flags, cancellable, NULL, NULL, error);
    }

    return TRUE;
}

static gboolean
delete_recursive(GFile *file, GCancellable *cancellable, GError **error) {
    GFileType file_type = g_file_query_file_type(file, G_FILE_QUERY_INFO_NONE, cancellable);
    if (file_type == G_FILE_TYPE_DIRECTORY) {
        GFileEnumerator *enumerator = g_file_enumerate_children(file, G_FILE_ATTRIBUTE_STANDARD_NAME, G_FILE_QUERY_INFO_NONE, cancellable, error);
        for (GFileInfo *info = g_file_enumerator_next_file(enumerator, cancellable, error); info != NULL; info = g_file_enumerator_next_file(enumerator, cancellable, error)) {
            delete_recursive(
                g_file_resolve_relative_path(file, g_file_info_get_name(info)),
                cancellable, error);
        }
        g_file_delete(file, cancellable, error);
    } else if (file_type == G_FILE_TYPE_REGULAR) {
        g_file_delete(file, cancellable, error);
    }

    return TRUE;
}

static int 
fstrrep(FILE *in, FILE *out, const char *needle, const char *replace) {
  int nlen = strlen(needle);
  int rlen = strlen(replace);

  char buf[nlen];

  int i, cnt = 0;

  // clear buf
  for (int i = 0; i < nlen; i++)
    buf[i] = 0;

  i = 0;

  while (1) {
    int ch = fgetc(in);
    if (ch == EOF) {
      break;
    }

    if (ch == needle[i]) {
      buf[i] = ch;
      i++;
      if (i == nlen) {
        // found string matches
        cnt++;

        // replace string
        for (i = 0; i < rlen; ++i) {
          fputc(replace[i], out);
        }
        i = 0;
      }
    } else {
      if (i > 0) {
        // print buff
        int c = i;
        for (i = 0; i < c; ++i) {
          fputc(buf[i], out);
          // clear buf
          buf[i] = 0;
        }
      }
      i = 0;
      fputc(ch, out);
    }
  }

  return cnt;
}

static int 
fpstrrep(const char *file_in, const char *file_out, const char *needle,
             const char *replace) {
  FILE *in = fopen(file_in, "r");
  if (!in)
    return -1;

  FILE *out = fopen(file_out, "w");
  if (!out) {
    fclose(in);
    return -2;
  }

  int cnt = fstrrep(in, out, needle, replace);

  fclose(in);
  fclose(out);

  return cnt;
}


