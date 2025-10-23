#include <stdio.h>
#include <string.h>

struct Student {
    int id;
    char name[50];
    int age;
};

int main() {
    FILE *fp, *temp;
    struct Student s;
    int idToUpdate;

    fp = fopen("students.txt", "r");
    temp = fopen("temp.txt", "w");

    printf("Enter ID to update: ");
    scanf("%d", &idToUpdate);

    while (fscanf(fp, "%d %s %d", &s.id, s.name, &s.age) != EOF) {
        if (s.id == idToUpdate) {
            printf("Enter new Name and Age: ");
            scanf("%s %d", s.name, &s.age);
        }
        fprintf(temp, "%d %s %d\n", s.id, s.name, s.age);
    }

    fclose(fp);
    fclose(temp);

    remove("students.txt");
    rename("temp.txt", "students.txt");

    return 0;
}
