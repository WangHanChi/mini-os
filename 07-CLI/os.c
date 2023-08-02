#include <stdint.h>
#include "libc.h"
#include "malloc.h"
#include "cli.h"



int main(void)
{
    usart_init();
    struct File_Structure foo;
    foo.cwd = (Dirname)malloc(2); // Initialize cwd to "/"
    strcpy(foo.cwd, "/");

    INIT_LIST_HEAD(&foo.dir_tree); // Initialize the list head
    struct DirNode* root_dir = (struct DirNode*)malloc(sizeof(struct DirNode));
    root_dir->dir_name = (Dirname)malloc(2);
    strcpy(root_dir->dir_name, "/");
    list_add(&root_dir->list, &foo.dir_tree); // Add the root directory node

    char line[256];
    char cmd[10];
    char arg[100];

    int count = 1;

    while (1) {
        myprintf("\r%d>%s-> ", count++, foo.cwd);

        scan_str(line);
        flush();
        myprintf("%s\n\r", line);
        sscanf_str(line, "%s", cmd);
        if (strcmp(cmd, "ls") == 0 || strcmp(cmd, "dir") == 0) {
            ls(&foo);
        } else if (strcmp(cmd, "cd") == 0) {
            sscanf_str(line, "%s %s", cmd, arg);
            cd(&foo, arg);
        } else if (strcmp(cmd, "mkdir") == 0) {
            sscanf_str(line, "%s %s", cmd, arg);
            mkdir(&foo, arg);
        } else if (strcmp(cmd, "rmdir") == 0) {
            sscanf_str(line, "%s %s", cmd, arg);
            rmdir(&foo, arg);
        } else if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "exit") == 0) {
            free(foo.cwd);
            struct list_head* iter, * next;
            struct DirNode* node;
            list_for_each_safe(iter, next, &foo.dir_tree) {
                node = list_entry(iter, struct DirNode, list);
                free(node->dir_name);
                free(node);
            }
            return 0;
        }
    }
	return 0;
}