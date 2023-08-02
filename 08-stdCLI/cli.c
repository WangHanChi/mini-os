#include "cli.h"

void mkdir(struct File_Structure* fs, const char* new_dir)
{
    struct list_head* iter;
    struct list_head* next;
    struct DirNode* node;

    list_for_each(iter, &fs->dir_tree) {
        node = list_entry(iter, struct DirNode, list);
        if (strcmp(node->dir_name, new_dir) == 0) {
            printf("> %s exists already\n", new_dir);
            return;
        }
    }

    node = (struct DirNode*)malloc(sizeof(struct DirNode));
    node->dir_name = (Dirname)malloc(strlen(new_dir) + 1);
    strcpy(node->dir_name, new_dir);

    if (strcmp(fs->cwd, "/") == 0) {
        list_add(&node->list, &fs->dir_tree);
        (void)next;
    } else {
        list_for_each(iter, &fs->dir_tree) {
            if (strcmp(list_entry(iter, struct DirNode, list)->dir_name, fs->cwd) == 0) {
                break;
            }
        }
        next = iter->next;
        list_add(&node->list, iter);
    }

    printf("> %s created successfully\n", new_dir);
}

void rmdir(struct File_Structure* fs, const char* old_dir)
{
    struct list_head* iter;
    struct list_head* next;
    struct DirNode* node;
    bool found = false;

    list_for_each(iter, &fs->dir_tree) {
        node = list_entry(iter, struct DirNode, list);
        if (strcmp(node->dir_name, old_dir) == 0) {
            found = true;
            break;
        }
    }

    if (!found || strcmp(old_dir, "..") == 0 || strcmp(old_dir, ".") == 0) {
        printf("> %s is not found !\n", old_dir);
        (void)next;
        return;
    }

    if (node->list.next != node->list.prev) {
        printf("> %s is not an empty directory!\n", old_dir);
        (void)next;
        return;
    }

    if (strcmp(old_dir, fs->cwd) == 0) {
        strcpy(fs->cwd, "/");
    }

    next = iter->next;
    list_del(iter);
    free(node->dir_name);
    free(node);

    printf("> %s removed successfully\n", old_dir);
}

void ls(struct File_Structure* fs)
{
    struct list_head* iter;
    struct DirNode* node;
    int col = 5;
    int i = 0;

    list_for_each(iter, &fs->dir_tree) {
        node = list_entry(iter, struct DirNode, list);
        printf("%s  ", node->dir_name);
        if (++i % col == 0)
            printf("\n");
    }
    printf("\n");
}

void cd(struct File_Structure* fs, const char* dir)
{
    if (strcmp(dir, "..") == 0) {
        if (strcmp(fs->cwd, "/") != 0) {
            size_t found = strlen(fs->cwd) - 1;
            while (found > 0 && fs->cwd[found] != '/') {
                found--;
            }
            fs->cwd[found] = '\0';
        }
    } else if (strcmp(dir, ".") != 0) {
        struct list_head* iter;
        struct DirNode* node;
        bool found = false;

        list_for_each(iter, &fs->dir_tree) {
            node = list_entry(iter, struct DirNode, list);
            if (strcmp(node->dir_name, dir) == 0) {
                found = true;
                size_t new_size = strlen(fs->cwd) + strlen(dir) + 2; // +2 for '/' and '\0'
                fs->cwd = (Dirname)realloc(fs->cwd, new_size);
                if (strcmp(fs->cwd, "/") == 0) {
                    strcpy(fs->cwd, dir);
                } else {
                    strcat(fs->cwd, "/");
                    strcat(fs->cwd, dir);
                }
                break;
            }
        }
        if (!found) {
            printf("> no such %s directory !\n", dir);
        }
    }
}
