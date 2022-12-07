#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

#include "../include/day6.h"
#include "../include/utils.h"

static const size_t max_name_len = 128;
static const size_t max_line_len = 65536;

typedef struct entry entry;
typedef struct entry {
  size_t sz;
  entry * parent;

//  size_t files_len;
//  entry * files;

  size_t dirs_len;
  entry * dirs;

  char * name;
} entry;

/*
--- Day 7: No Space Left On Device ---

You can hear birds chirping and raindrops hitting leaves as the expedition
proceeds. Occasionally, you can even hear much louder sounds in the distance;
how big do the animals get out here, anyway?

The device the Elves gave you has problems with more than just its
communication system. You try to run a system update:

$ system-update --please --pretty-please-with-sugar-on-top
Error: No space left on device

Perhaps you can delete some files to make space for the update?

You browse around the filesystem to assess the situation and save the resulting
terminal output (your puzzle input). For example:

$ cd /
$ ls
dir a
14848514 b.txt
8504156 c.dat
dir d
$ cd a
$ ls
dir e
29116 f
2557 g
62596 h.lst
$ cd e
$ ls
584 i
$ cd ..
$ cd ..
$ cd d
$ ls
4060174 j
8033020 d.log
5626152 d.ext
7214296 k

The filesystem consists of a tree of files (plain data) and directories (which
can contain other directories or files). The outermost directory is called /.
You can navigate around the filesystem, moving into or out of directories and
listing the contents of the directory you're currently in.

Within the terminal output, lines that begin with $ are commands you executed,
very much like some modern computers:

    cd means change directory. This changes which directory is the current
    directory, but the specific result depends on the argument:

        cd x moves in one level: it looks in the current directory for the
        directory named x and makes it the current directory.

        cd .. moves out one level: it finds the directory that contains the
        current directory, then makes that directory the current directory.

        cd / switches the current directory to the outermost directory, /.

    ls means list. It prints out all of the files and directories immediately
    contained by the current directory:

        123 abc means that the current directory contains a file named abc with
        size 123.

        dir xyz means that the current directory contains a directory named
        xyz.

Given the commands and output in the example above, you can determine that the
filesystem looks visually like this:

- / (dir)
  - a (dir)
    - e (dir)
      - i (file, size=584)
    - f (file, size=29116)
    - g (file, size=2557)
    - h.lst (file, size=62596)
  - b.txt (file, size=14848514)
  - c.dat (file, size=8504156)
  - d (dir)
    - j (file, size=4060174)
    - d.log (file, size=8033020)
    - d.ext (file, size=5626152)
    - k (file, size=7214296)

Here, there are four directories: / (the outermost directory), a and d (which
are in /), and e (which is in a). These directories also contain files of
various sizes.

Since the disk is full, your first step should probably be to find directories
that are good candidates for deletion. To do this, you need to determine the
total size of each directory. The total size of a directory is the sum of the
sizes of the files it contains, directly or indirectly. (Directories themselves
do not count as having any intrinsic size.)

The total sizes of the directories above can be found as follows:

    The total size of directory e is 584 because it contains a single file i of
    size 584 and no other directories.

    The directory a has total size 94853 because it contains files f (size
    29116), g (size 2557), and h.lst (size 62596), plus file i indirectly
    (a contains e which contains i).

    Directory d has total size 24933642.

    As the outermost directory, / contains every file. Its total size is
    48381165, the sum of the size of every file.

To begin, find all of the directories with a total size of at most 100000, then
calculate the sum of their total sizes. In the example above, these directories
are a and e; the sum of their total sizes is 95437 (94853 + 584). (As in this
example, this process can count files more than once!)

Find all of the directories with a total size of at most 100000. What is the
sum of the total sizes of those directories?
*/

static char * text = NULL;
static char root_name[2] = "/";
static entry root = {
  .name = &root_name[0]
};

static entry * current = &root;

static inline void ws(const char **s) {
  while(isspace(**s)) { ++(*s); }
}

static void parse_name(const char **s, char **name) {
  ws(s);

  const char * start = *s, * end = *s;
  while(end++) {
    if(isspace(*end)) { break; }
  }

  if(name) {
    *name = NULL;
    ptrdiff_t len = end - start;
    if(len > 0) {
      char * temp = calloc((size_t)len + 1, sizeof * temp);
      if(!temp) { abort(); }
      memcpy(temp, start, len);
      temp[len] = '\0';
      *name = temp;
    }
  }

  *s = end;
}

static void exec_cd(const char ** cmd) {
  ws(cmd);
  if(**cmd == '/') {
    current = &root;
    (*cmd)++;
  } else if(strncmp("..", *cmd, 2) == 0) {
    if(current != &root) {
      current = current->parent;
      *cmd += 2;
    }
  } else {
    char * name = NULL;
    parse_name(cmd, &name);
    entry * first = current->dirs;
    const entry * last = current->dirs + current->dirs_len;
    while(first < last) {
      if(strncmp(name, first->name, strnlen(first->name, max_name_len)) == 0) {
        current = first;
        return;
      }
      first++;
    }
  }
}

static void create_entry(entry * parent, entry ** node, size_t * len, entry ** new_node) {
  *len += 1;
  entry * temp = realloc(*node, *len * sizeof * temp);
  if(!temp) { abort(); }
  *node = temp;
  *new_node = *node + (*len - 1);
  (*new_node)->parent = parent;
}

static void parse_ls(const char ** cmd) {
  while(**cmd != '$' && **cmd != '\0') {
    ws(cmd);
    if(strncmp(*cmd, "dir ", 4) == 0) {
      *cmd += 4;
      entry * dir = NULL;
      create_entry(current, &current->dirs, &current->dirs_len, &dir);
      parse_name(cmd, &(dir->name));
    } else if(**cmd >= '0' && **cmd <= '9') {
      uint32_t sz = read_uint(cmd);
      parse_name(cmd, NULL);
      current->sz += sz;
      entry * P = current->parent;
      while(P) {
        P->sz += sz;
        P = P->parent;
      }
    }
  }
}

static void eat_mun(const char **cmd) {
  if(**cmd == '$') (*cmd)++;
}

static void parse_command(const char ** cmd) {
  eat_mun(cmd);
  ws(cmd);
  if(strncmp("cd", *cmd, 2) == 0) {
    *cmd += 2; exec_cd(cmd);
  } else if(strncmp("ls", *cmd, 2) == 0) {
    *cmd += 2; parse_ls(cmd);
  }
}

static void calculate_total(const entry * e, size_t * total) {
  if(e->sz <= 100000) { *total += e->sz; }
  if(e->dirs_len > 0) {
    for(size_t i = 0; i < e->dirs_len; i++) {
      calculate_total(&(e->dirs[i]), total);
    }
  }
}

static void parse_lines(const char * line) {
  const char * s = line;
  do {
    if(*s == '$') {
      parse_command(&s);
    } else {
      s++;
    }
  } while(*s != '\0');
}

static void aoc_day6_p0_text_reader(const char * line, ssize_t read, void * state) {
  (void)state;
  static size_t sz = 0;

  if(read <= 1) { return; }
  if(!line) { return; }

  size_t line_len = strnlen(line, max_line_len);
  if(line_len > 0) {
    char * temp = realloc(text, (sz + line_len) + 1 * sizeof * temp);
    if(!temp) { abort(); }
    text = temp;
    memmove(text + sz, line, line_len);
    sz += line_len;
    text[sz] = '\0';
  }
}

int aoc_day6_p0(int argc, char **argv) {
  int result = read_lines(argc, argv, NULL, &aoc_day6_p0_text_reader);
  parse_lines(text);
  size_t total = 0;
  calculate_total(&root, &total);
  fprintf(stdout, "Total size: %zu\n", total);
  return result;
}

/*
--- Part Two ---

Now, you're ready to choose a directory to delete.

The total disk space available to the filesystem is 70000000. To run the
update, you need unused space of at least 30000000. You need to find a
directory you can delete that will free up enough space to run the update.

In the example above, the total size of the outermost directory (and thus the
total amount of used space) is 48381165; this means that the size of the unused
space must currently be 21618835, which isn't quite the 30000000 required by
the update. Therefore, the update still requires a directory with total size of
at least 8381165 to be deleted before it can run.

To achieve this, you have the following options:

    Delete directory e, which would increase unused space by 584.
    Delete directory a, which would increase unused space by 94853.
    Delete directory d, which would increase unused space by 24933642.
    Delete directory /, which would increase unused space by 48381165.

Directories e and a are both too small; deleting them would not free up enough
space. However, directories d and / are both big enough! Between these, choose
the smallest: d, increasing unused space by 24933642.

Find the smallest directory that, if deleted, would free up enough space on the
filesystem to run the update. What is the total size of that directory?
*/

void find_min(const entry * e, size_t required, size_t stack[], size_t *stack_len) {
  if(e->sz > required) {
    stack[(*stack_len)++] = e->sz;
  }
  if(e->dirs_len > 0) {
    for(size_t i = 0; i < e->dirs_len; i++) {
      find_min(&(e->dirs[i]), required, stack, stack_len);
    }
  }
}

int aoc_day6_p1(int argc, char **argv) {
  static const size_t min_free = 30000000;
  static const size_t max_avail = 70000000;

  int result = read_lines(argc, argv, NULL, &aoc_day6_p0_text_reader);
  parse_lines(text);

  size_t unused = max_avail - root.sz;

  size_t stack[65536] = { 0 };
  size_t stack_len = 0;
  size_t required = min_free - unused;
  find_min(&root, required, stack, &stack_len);

  size_t min = stack[0];
  for(size_t i = 1; i < stack_len; i++) {
    min = min < stack[i] ? min : stack[i];
  }

  fprintf(stdout, "Total size of min folder to delete: %zu\n", min);

  return result;
}

