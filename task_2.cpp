#include <cstdio>

void RemoveDups(char* str) {
    if (str == NULL || str[0] == '\0') return;
    char* read_ptr = str;
    char* write_ptr = str;

    do {
        if (*++read_ptr != *write_ptr) {
            if (++write_ptr != read_ptr) {
                *write_ptr = *read_ptr;
            }
        }
    } while (*read_ptr != '\0');
}

int main() {
    char data[] = "AAA BBB   AAA";
    RemoveDups(data);
    printf("%s\n", data);
}
