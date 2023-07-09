#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"

/*
    Functie prin intermediul careia este creeat "root"-ul sistemului de fisiere.
    Cu ajutorul comenzii "start" este pornit terminalul efectiv si creeata radacina,
    prin initializarea fiecarui camp specific unui director.
*/
void create_filesystem(Directory **directory)
{
    char command[20];
    scanf("%s", command);

    if (strcmp(command, "start") == 0)
    {
        *directory = (Directory *)malloc(sizeof(Directory));
        (*directory)->name = (char *)malloc(2 * sizeof(char));
        strcpy((*directory)->name, "/");
        (*directory)->parent = NULL;
        (*directory)->files = NULL;
        (*directory)->directories = NULL;
    }
    else
    {
        printf("Try typing another command:\n");
        return;
    }
}

/*
    Functie prin intermediul careia este sters sistemul de fisiere si dezalocat
    directorul radacina, in conditiile in care deja au fost sterse toate fisierele
    si directoarele continute de sistem.
*/
void delete_filesystem(Directory **directory)
{
    if ((*directory)->parent == NULL)
    {
        free((*directory)->name);
        free(*directory);
    }
}

/*
    Functie prin care se creeaza un nou fisier caruia ii este atribuit un nume,
    o dimensiune si un continut, dupa care acesta este adaugat in lista de
    a directorului curent.
*/
void touch(Directory **directory)
{
    char filename[127];
    char contents[127];
    scanf("%s%s", filename, contents);

    File *file = (File *)malloc(sizeof(File));

    file->size = strlen(contents);
    file->name = malloc(strlen(filename) + 1);
    file->contents = malloc(strlen(contents) + 1);

    strcpy(file->name, filename);
    strcpy(file->contents, contents);

    add_file(&((*directory)->files), file);
}

/*
    Similar cu functia "touch" de mai sus.
*/
void mkdir(Directory **directory)
{
    char name[127];
    scanf("%s", name);

    Directory *dir = (Directory *)malloc(sizeof(Directory));

    dir->directories = NULL;
    dir->files = NULL;
    dir->parent = *directory;
    dir->name = malloc(strlen(name) + 1);
    strcpy(dir->name, name);

    add_directory(&((*directory)->directories), dir);
}

/*
    Functie ce listeaza continutul directorului curent(toate fisierele
    + toate directorarele).
*/
void ls(Directory *directory)
{
    FileList *files = directory->files;
    DirectoryList *directories = directory->directories;

    while (files)
    {
        printf("%s ", files->file->name);
        files = files->next;
    }

    while (directories)
    {
        printf("%s ", directories->directory->name);
        directories = directories->next;
    }

    printf("\n");
}

/*
    Functie ce afiseaza continutul unui fisier din direcotrul curent(daca acesta exista).
*/
void cat(Directory *directory)
{
    char name[127];
    scanf("%s", name);

    FileList *files = directory->files;

    while (files)
    {
        if (strcmp(files->file->name, name) == 0)
        {
            printf("%s\n", files->file->contents);
            break;
        }

        files = files->next;
    }

    if (!files)
    {
        printf("File does not exist\n");
        return;
    }
}

/*
    Functie ce afiseaza calea absoluta de la directorul radacina al sistemului
    de fisiere pana la cel curent.
*/
void pwd(Directory *directory)
{
    if (!directory->parent)
    {
        printf("/");
        return;
    }

    pwd(directory->parent);
    printf("%s/", directory->name);
}

/*
    Functie prin intermediul careia schimbam directorul curent cu unul aflat in acesta, sau cu
    cel parinte al directorului curent. Functia cauta in functie de numele citit de la "stdin",
    numele directorului in care vrea sa se schimbe. In cazul in care introducem "..", atunci ne
    vom intoarce in directorul parinte al celui curent. In caz ca nu introducem un nume valid,
    lista de directoare a directorului curent este reinitializata cu lista de dinainte de
    parcurgerea tuturor directoarelor si este intors tot directorul curent. In caz contrar,
    daca totul merge bine, va fi intors directorul in care doream sa ne mutam.
*/
Directory *cd(Directory *directory)
{
    char name[127];
    scanf("%s", name);

    if (strcmp(name, "..") == 0 && directory->parent)
    {
        return directory->parent;
    }

    DirectoryList *directories = directory->directories;
    Directory *dir = NULL;

    while (directory->directories)
    {
        dir = directory->directories->directory;

        if (strcmp(dir->name, name) == 0)
        {
            break;
        }

        directory->directories = directory->directories->next;
    }

    if (!directory->directories)
    {
        printf("mkdir : Cannot move to %s: No such directory\n", name);
        directory->directories = directories;
        return directory;
    }

    directory->directories = directories;
    return dir;
}

/*
    Afiseaza in mod recursiv, sub forma arborescenta, continutul directorului curent
    (toate fisierele + toate directoarele aflate pe orice nivel de imbricare) aflat
    la un anumit nivel de adancime.
*/
void tree(Directory *directory, int depth)
{
    FileList *files = directory->files;
    DirectoryList *directories = directory->directories;

    while (files)
    {
        printf("%*s%s\n", depth * 4, "", files->file->name);
        files = files->next;
    }

    while (directories)
    {
        printf("%*s%s\n", depth * 4, "", directories->directory->name);
        tree(directories->directory, depth + 1);
        directories = directories->next;
    }
}

/*
    Sterge un fisier din directorul curent(in cazul in care acesta exista),
    cu ajutorul functiei "remove_file".
*/
void rm(Directory **directory)
{
    char name[127];
    scanf("%s", name);

    FileList *files = (*directory)->files;

    while (files)
    {
        if (strcmp(files->file->name, name) == 0)
        {
            break;
        }

        files = files->next;
    }

    if (!files)
    {
        printf("rm : File %s does not exist in the current directory\n", name);
        return;
    }

    remove_file(&((*directory)->files), name);
}

/*
    Sterge recursiv intreg continutul unui director, cat si directorul in sine. Parcurgem toate
    directoarele aflate in directorul curent pana cand il gasim pe cel introdus la "stdin".
    Stergem succesiv fiecare fisier din directorul respectiv, dupa care incepem sa parcurgem
    directoarele si stergem recursiv continutul fiecaruia in parte.
*/
void rmdir(Directory **directory)
{
    char name[127];
    scanf("%s", name);

    DirectoryList *directories = (*directory)->directories;
    Directory *dir = NULL;

    while (directories)
    {
        dir = directories->directory;

        if (strcmp(dir->name, name) == 0)
        {
            while (dir->files)
            {
                FileList *fs = dir->files;
                dir->files = dir->files->next;
                remove_file(&fs, fs->file->name);
            }

            while (dir->directories)
            {
                DirectoryList *dirs = dir->directories;
                dir->directories = dir->directories->next;
                rmdir(&(dirs->directory));
                free(dirs);
            }

            if (dir->parent)
            {
                dir->parent->directories = directories->next;
            }
            else
            {
                (*directory)->directories = directories->next;
            }

            free(dir);
            free(directories);
            return;
        }

        dir->parent->directories = directories;
        directories = directories->next;
    }

    if (!directories)
    {
        printf("rmdir : Directory %s does not exist\n", name);
        return;
    }
}

/*
    In functia "main" este tratata functionalitatea fiecarei comenzi in parte primite de la "stdin".
*/
int main(int argc, char *argv[])
{
    Directory *directory = NULL;
    create_filesystem(&directory);

    while (1)
    {
        printf("root@user: ~");
        print_path(directory);
        printf("\n$ ");

        char *command = (char *)malloc(1001 * sizeof(char));
        scanf("%s", command);

        if (strcmp(command, "touch") == 0)
        {
            touch(&directory);
        }
        else if (strcmp(command, "mkdir") == 0)
        {
            mkdir(&directory);
        }
        else if (strcmp(command, "ls") == 0)
        {
            ls(directory);
        }
        else if (strcmp(command, "cat") == 0)
        {
            cat(directory);
        }
        else if (strcmp(command, "cd") == 0)
        {
            directory = cd(directory);
        }
        else if (strcmp(command, "pwd") == 0)
        {
            pwd(directory);
            printf("\n");
        }
        else if (strcmp(command, "tree") == 0)
        {
            tree(directory, 0);
        }
        else if (strcmp(command, "rm") == 0)
        {
            rm(&directory);
        }
        else if (strcmp(command, "rmdir") == 0)
        {
            rmdir(&directory);
        }
        else if (strcmp(command, "exit") == 0)
        {
            printf("Leaving...");
            delete_filesystem(&directory);
            break;
        }
        else
        {
            printf("%s: Command does not exist\n", command);
        }

        printf("\n");
    }
}