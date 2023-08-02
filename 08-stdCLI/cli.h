#ifndef __CLI_H_
#define __CLI_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

typedef char* Dirname;

struct DirNode {
    Dirname dir_name;
    struct list_head list;
};

struct File_Structure {
    Dirname cwd;
    struct list_head dir_tree; // Head of the doubly-linked list
};


void mkdir(struct File_Structure* fs, const char* new_dir);
void rmdir(struct File_Structure* fs, const char* old_dir);
void ls(struct File_Structure* fs);
void cd(struct File_Structure* fs, const char* dir);

#endif