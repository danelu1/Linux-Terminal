typedef struct FileList
{
    struct File *file;
    struct FileList *next;
} FileList;

typedef struct DirectoryList
{
    struct Directory *directory;
    struct DirectoryList *next;
} DirectoryList;

typedef struct File
{
    char *contents;
    int size;
    char *name;
} File;

typedef struct Directory
{
    char *name;
    FileList *files;
    DirectoryList *directories;
    struct Directory *parent;
} Directory;

void add_file(FileList **files, File *file)
{
    FileList *temp = (FileList *)malloc(sizeof(FileList));
    temp->file = file;
    temp->next = *files;
    *files = temp;
}

void add_directory(DirectoryList **directories, Directory *directory)
{
    DirectoryList *temp = (DirectoryList *)malloc(sizeof(DirectoryList));
    temp->directory = directory;
    temp->next = *directories;
    *directories = temp;
}

void print_path(Directory *directory)
{
    if (!directory->parent)
    {
        printf("%s", directory->name);
        return;
    }

    print_path(directory->parent);
    printf("%s/", directory->name);
}

void remove_file(FileList **files, char *filename)
{
    FileList *temp = *files;
    FileList *prev;

    if (temp && strcmp(temp->file->name, filename) == 0)
    {
        *files = temp->next;
        free(temp);
        return;
    }

    while (temp && strcmp(temp->file->name, filename) != 0)
    {
        prev = temp;
        temp = temp->next;
    }

    if (!temp)
    {
        return;
    }

    prev->next = temp->next;
    free(temp);
}