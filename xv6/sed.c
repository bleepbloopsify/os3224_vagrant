#include "types.h"
#include "user.h"
#include "fcntl.h"

struct Line {
  char* l;
  uint index;
  uint capacity;
} ;

typedef struct Line Line;

Line new_line() {
  Line l;
  l.capacity = 128;
  l.index = 0;
  l.l = malloc(l.capacity);
  return l;
}

 void write_to_line(Line* l, char c) {
   l->l[l->index++] = c;
  if (l->index >= l->capacity - 1) {
    l->capacity *= 2;
    char* hold = malloc(l->capacity);
    strcpy(hold, l->l);
    l->l = hold;
  }
}

void write_str_to_line(Line* l, char* str, uint len) {
  if (l->index + len >= l->capacity - 1) {
    l->capacity *= 2;
    l->capacity += len; // just in case its very large
    char* hold = malloc(l->capacity);
    strcpy(hold, l->l);
    l->l = hold;
  }
  memmove(l->l + l->index, str, len); // insert string
  l->index += len;
}

void clear_line(Line* l) {
  memset(l->l, 0, l->index);
  l->index = 0;
} // don't clear capacity


char buf[512];
void replace_the_with_xyz(int from_fd, int to_fd, char* find, char* replace) {
  uint find_len = strlen(find);
  uint replace_len = strlen(replace);

  uint n, i;
  uint occurences, find_pos;
  int occured_in_line = -1;  
  occurences = find_pos = 0;
  Line line = new_line();


  while((n = read(from_fd, buf, sizeof(buf))) > 0) {
    for (i = 0; i < n; ++i) {
      if (buf[i] == find[find_pos]) {
        ++find_pos;
      } else {
        if (find_pos > 0) {
          write_str_to_line(&line, find, find_pos);
          find_pos = 0;
        }
        write_to_line(&line, buf[i]);
      }

      if (buf[i] == '\n') { // when we hit the end of a line
        if (occured_in_line > 0 && to_fd != 1) {
          printf(1, line.l);
        }
        printf(to_fd, line.l);
        clear_line(&line);
        occured_in_line = -1;
      }

      if (find_pos == find_len) { // LENGTH OF find string
        ++occurences;
        occured_in_line = 1;
        write_str_to_line(&line, replace, replace_len);
        find_pos = 0;
      }
    }
  }

  free(line.l);

  printf(1, "Found and replaced %d occurences\n", occurences);
}

// Now sed takes argv[1] as find and argv[2] as replace
int main(int argc, char* argv[]) {

  char* find = argv[1] + 1; // ignore -
  char* replace = argv[2] + 1; // ignore -

  if (argc <= 3) {
    replace_the_with_xyz(0, 1, find, replace);

    exit();
  }

  // TODO: single argument count "the"
  int from_fd = open(argv[3], 0);
  if (from_fd < 0) {
    printf(2, "sed: Error opening %s\n", argv[3]);
    exit();
  }
  int err = unlink(argv[3]);
  if (err == -1) {
    printf(2, "sed: Error copying %s\n", argv[3]);
  }
  int to_fd = open(argv[3], O_CREATE|O_WRONLY);

  replace_the_with_xyz(from_fd, to_fd, find, replace);

  close(from_fd);
  close(to_fd);


  exit();
}