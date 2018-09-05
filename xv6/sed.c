#include "types.h"
#include "user.h"

char buf[512];

void count_the(int fd) {
  char* word = "the";

  uint i, n;
  uint count = 0;
  uint word_pos = 0;

  // This is basic buffered reading with no backseeks
  while((n = read(fd, buf, sizeof(buf))) > 0) {
    for (i = 0; i < n; ++i) {
      if (buf[i] == word[word_pos]) {
        ++word_pos;
      } else {
        word_pos = 0;
      }

      if (word_pos == 3) {
        ++count;
        word_pos = 0;
      }
    }
  }

  printf(0, "%d\n", count);
}


int main(int argc, char* argv[]) {

  if (argc <= 1) {
    count_the(0);

    exit();
  }

  // TODO: single argument count "the"
  int fd = open(argv[1], 0);
  if (fd < 0) {
    printf(2, "sed: Error opening %s\n", argv[1]);
    exit();
  }

  count_the(fd);

  // TODO: find all occurences of sequence in file

  exit();
}