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
  l.capacity = 30;
  l.index = 0;
  l.l = malloc(l.capacity);
  return l;
}

 void write_to_line(Line* l, char c) {
   l->l[l->index++] = c;
  if (l->index >= l->capacity) {
    l->capacity *= 2;
    char* hold = malloc(l->capacity);
    strcpy(hold, l->l);
    l->l = hold;
  }
}

void clear_line(Line* l) {
  memset(l->l, 0, l->index);
  l->index = 0;
} // don't clear capacity


char buf[512];
void replace_the_with_xyz(int from_fd, int to_fd) {
  char* find = "the";
  char* replace = "xyz";

  uint n, i, j; // j is for inner loop
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
          write(to_fd, find, find_pos); // write all characters of find_pos we've seen so far
          for (j = 0; j < find_pos; ++j) {
           write_to_line(&line, find[j]);
          }

          find_pos = 0;
        }
        printf(to_fd, "%c", buf[i]); // then write the character that failed the match
        write_to_line(&line, buf[i]);
      }

      if (buf[i] == '\n') { // when we hit the end of a line
        if (occured_in_line > 0 && to_fd != 1) {
          printf(1, "%s", line.l);
        }
        clear_line(&line);
        occured_in_line = -1;
      }

      if (find_pos == 3) { // LENGTH OF find string
        ++occurences;
        occured_in_line = 1;
        write(to_fd, replace, 3);
        for (j = 0; j < 3; ++j) {
          write_to_line(&line, replace[j]);
        }
        find_pos = 0;
      }
    }
  }

  free(line.l);

  printf(1, "Found and replaced %d occurences\n", occurences);
}

int main(int argc, char* argv[]) {

  if (argc <= 1) {
    replace_the_with_xyz(0, 1);

    exit();
  }

  // TODO: single argument count "the"
  int from_fd = open(argv[1], 0);
  if (from_fd < 0) {
    printf(2, "sed: Error opening %s\n", argv[1]);
    exit();
  }
  int err = unlink(argv[1]);
  if (err == -1) {
    printf(2, "sed: Error copying %s\n", argv[1]);
  }
  int to_fd = open(argv[1], O_CREATE|O_WRONLY);

  replace_the_with_xyz(from_fd, to_fd);

  close(from_fd);
  close(to_fd);


  exit();
}